#include "Offline/CaloConditions/inc/CalEnergyCalibMaker.hh"
#include "Offline/DataProducts/inc/CaloId.hh"
#include <vector>

/*
Written by S. Middleton
*/
namespace mu2e {
  typedef std::shared_ptr<CalCalib> ptr_t;

  ptr_t CalEnergyCalibMaker::fromFcl() {
  CalCalibPar nominal(_config.ADC2MeV(), _config.timeoffset(), _config.algName());

  size_t nChan = 1; //FIXME

  CalCalib::CalibVec cvec(nChan, nominal);
  auto ptr = std::make_shared<CalCalib>(cvec);
  return ptr;

  } // end fromFcl

  ptr_t CalEnergyCalibMaker::fromDb(CalEnergyCalib::cptr_t) {
    // initially fill from fcl to get all the constants

    auto ptr = fromFcl();
    return ptr;

  } // end fromDb

}
