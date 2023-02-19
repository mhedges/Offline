#include "Offline/TrkHitReco/inc/StrawHitRecoUtils.hh"
#include "Offline/TrkHitReco/inc/PeakFit.hh"
#include "Offline/TrkHitReco/inc/PeakFitRoot.hh"
#include "Offline/TrkHitReco/inc/PeakFitFunction.hh"
#include "Offline/TrkHitReco/inc/ComboPeakFitRoot.hh"

#include "Offline/DataProducts/inc/StrawEnd.hh"

#include <numeric>


namespace mu2e {

  StrawHitRecoUtils::StrawHitRecoUtils(TrkHitReco::FitType fittype, int diagLevel,
      StrawIdMask mask, bool writesh,
      float minT, float maxT, float minE, float maxE, float minR, float maxR,
      bool filter,
      float ctE, float ctMinT, float ctMaxT, bool usecc, float clusterDt) :
    _fittype(fittype), _diagLevel(diagLevel),
    _mask(mask), _writesh(writesh),
    _minT(minT), _maxT(maxT), _minE(minE), _maxE(maxE), _minR(minR), _maxR(maxR),
    _filter(filter), _ctE(ctE), _ctMinT(ctMinT), _ctMaxT(ctMaxT), _usecc(usecc), _clusterDt(clusterDt) {
      // Detailed histogram-based waveform fits are no longer supported TODO!
      if (_fittype != TrkHitReco::FitType::peakminusped && _fittype != TrkHitReco::FitType::peakminuspedavg && _fittype != TrkHitReco::FitType::firmwarepmp)
        throw cet::exception("RECO")<<"TrkHitReco: Peak fit " << _fittype << " not implemented " <<  std::endl;
    }


  void StrawHitRecoUtils::flagCrossTalk(std::unique_ptr<StrawHitCollection> const& shCol, std::unique_ptr<ComboHitCollection> const& chCol) const {
//    size_t numDigis = shCol->size();
//    hits_by_panel = std::vector<std::vector<size_t> >(StrawId::_nupanels,std::vector<size_t>());
//    largeHits.clear();
//    largeHitPanels.clear();
//    largeHits.reserve(numDigis);
//    largeHitPanels.reserve(numDigis);
//
//    //buffer large hit for cross-talk analysis
//    size_t upanel = straw.id().uniquePanel();
//      hits_by_panel[upanel].push_back(shCol->size());
//      if (energy >= _ctE) {largeHits.push_back(shCol->size()); largeHitPanels.push_back(upanel);}
//
//      for (size_t ilarge=0; ilarge < largeHits.size();++ilarge)
//    {
//      const StrawHit& sh = (*shCol)[largeHits[ilarge]];
//      for (size_t jsh : hits_by_panel[largeHitPanels[ilarge]])
//      {
//        if (jsh==largeHits[ilarge]) continue;
//        const StrawHit& sh2 = (*shCol)[jsh];
//        if (sh2.time()-sh.time() > _ctMinT && sh2.time()-sh.time() < _ctMaxT)
//        {
//          if (sh.strawId().samePreamp(sh2.strawId())) (*chCol)[jsh]._flag.merge(StrawHitFlag::elecxtalk);
//          if (sh.strawId().nearestNeighbor(sh2.strawId())) (*chCol)[jsh]._flag.merge(StrawHitFlag::strawxtalk);
//        }
//      }
//    }
  }

  double StrawHitRecoUtils::peakMinusPedWF(TrkTypes::ADCWaveform const& adcData, StrawResponse const& srep, ADCWFIter& maxiter) const {
    auto wfstart = adcData.begin() +  srep.nADCPreSamples();
    auto pedestal = std::accumulate(adcData.begin(), wfstart, 0)/static_cast<float>(srep.nADCPreSamples());
    maxiter = wfstart;
    auto nextIter = maxiter; nextIter++;
    while(nextIter != adcData.end()){
      if (*nextIter > *maxiter)maxiter = nextIter;
      ++nextIter;
    }
    auto peak = *maxiter;
    return (peak-pedestal);
  }

  bool StrawHitRecoUtils::createComboHit(size_t isd, std::unique_ptr<ComboHitCollection> const& chCol,
      std::unique_ptr<StrawHitCollection> const& shCol, const CaloClusterCollection* caloClusters,
      double pbtOffset,
      StrawId const& sid, TrkTypes::TDCValues const& tdc, TrkTypes::TOTValues const& tot,
      double pmp,
      TrackerStatus const& trackerStatus, StrawResponse const& srep, Tracker const& tt) const {

    // flag digis that shouldn't be here or we don't want
    StrawHitFlag flag;
    if (trackerStatus.noSignal(sid) || trackerStatus.suppress(sid)) {
      flag.merge(StrawHitFlag::dead); // hits from these straws will not be used in track reconstruction
    } else if ( trackerStatus.noisy(sid)) {
      flag.merge(StrawHitFlag::noisy); // these hits may be used in track reconstruction but not pattern recognition
    }

    //extract energy from waveform
    double charge(0.0);
    auto invgainAvg = srep.adcLSB()/srep.strawGain();
    switch (_fittype) {
      case TrkHitReco::FitType::peakminuspedavg: default:
        charge = pmp*invgainAvg*srep.peakMinusPedestalEnergyScale();
        break;
      case TrkHitReco::FitType::peakminusped: case TrkHitReco::FitType::firmwarepmp:
        charge = pmp*invgainAvg*srep.peakMinusPedestalEnergyScale(sid);
        break;
    }
    float energy = srep.ionizationEnergy(charge);

    // energy selection
    // filter on specific ionization TODO!
    // filter based on composite e/P separation TODO!
    if( energy > _maxE || energy < _minE ) {
      if(_filter) return false;
    } else
      flag.merge(StrawHitFlag::energysel);

    // reconstruct the times
    TrkTypes::TDCTimes times;
    srep.calibrateTimes(tdc,times,sid);
    // subtract proton bunch time
    for(auto iend=0;iend<2;++iend)times[iend] -= pbtOffset;
    // find the end with the earliest time
    StrawEnd eend = (times[StrawEnd::hv] < times[StrawEnd::cal]) ?
      StrawEnd(StrawEnd::hv) : StrawEnd(StrawEnd::cal);
    // time-over-threshold
    TrkTypes::TOTTimes tots{0.0,0.0};
    for(size_t iend=0;iend<2;++iend){
      tots[iend] = tot[iend]*srep.totLSB();
    }
    double dt = times[StrawEnd::hv] - times[StrawEnd::cal];
    double halfpv;
    // get distance along wire from the straw center and it's estimated error
    auto const& straw  = tt.getStraw( sid );
    double dw, dwerr;
    bool td = srep.wireDistance(straw,energy,dt, dw,dwerr,halfpv);
    float propd = straw.halfLength()+eend.endSign()*dw;
    float ptime = propd/(2*halfpv); // propagation time to the early end
    XYZVectorF pos = XYZVectorF(straw.getMidPoint()+dw*straw.getDirection());
    // select based on radial position
    auto rho = pos.Rho();
    if( rho < _minR || rho > _maxR) {
      if(_filter)return false;
    } else
      flag.merge(StrawHitFlag::radsel);

    // choose earliest end TOT: maybe average later?
    float selected_tot = tots[eend.end()];
    //    float selected_tot = 0.5*(tots[0] + tots[1]);
    // compute corrected time, and use that for selections.  eventually average the times from both ends TODO
    double dtime = srep.TOTdriftTime(straw,selected_tot,energy);
    double tres = srep.TOTdriftTimeError(straw,selected_tot,energy);
    double time = times[eend.end()] - ptime - dtime;
    if (time < _minT || time > _maxT ){
      if(_filter) return false;
    } else
      flag.merge(StrawHitFlag::timesel);
    double plen = srep.pathLength(straw,selected_tot);

    //calorimeter filtering
    if (_usecc && caloClusters) {
      bool outsideCaloTime(true);
      for (auto const& cluster : *caloClusters)
        if (std::abs(time-cluster.time())<_clusterDt) {outsideCaloTime=false; break;}
      if (outsideCaloTime){
        if(_filter) return false;
      } else
        flag.merge(StrawHitFlag::calosel);
    }

    // create combo hit
    static const XYZVectorF _zdir(0.0,0.0,1.0);
    ComboHit ch;
    ch._nsh = 1; // 'combo' of 1 hit
    ch._pos = pos;
    ch._wdir = straw.getDirection();
    ch._sdir = _zdir.Cross(ch._wdir);
    ch._wdist = dw;
    ch._wres = dwerr;
    ch._time = time;
    ch._timeres = tres;
    ch._dedx = energy/plen;
    ch._sid = straw.id();
    ch._dtime = dtime;
    ch._tot[0] = tots[0];
    ch._tot[1] = tots[1];
    ch._ttdc[0] = times[0];
    ch._ttdc[1] = times[1];
    ch._ptime = ptime;
    ch._pathlength = plen;
    ch.addIndex(isd);
    // initial estimate of the transverse error is the straw diameter/sqrt(12)
    static const float invsqrt3 = 1.0/sqrt(3.0);
    ch._tres = tt.strawOuterRadius()*invsqrt3;
    // set flags
    ch._mask = _mask;
    ch._flag = flag;
    if (td) ch._flag.merge(StrawHitFlag::tdiv);
    ch._eend = eend;
    chCol->push_back(std::move(ch));
    // optionally create legacy straw hit (for diagnostics and calibration)
    if(_writesh){
      StrawHit hit(sid,times,tots,energy);
      shCol->push_back(std::move(hit));
    }

    return true;
  }

}
