// The intent of this class is to handle per-job tasks supporting
// handling of input and output data products, like configuration
// parsing and the procuces/consumes delcarations.  It is used by both
// single- and multithreaded geant modules.
//
// The class exposes parsed configuration results for use by
// per-thread code that handles the actual data product collections.
//
// Andrei Gaponenko, 2021

#ifndef Mu2eG4_Mu2eG4IOConfigHelper_hh
#define Mu2eG4_Mu2eG4IOConfigHelper_hh

#include "Mu2eG4/inc/Mu2eG4MultiStageParameters.hh"
#include "Mu2eG4/inc/Mu2eG4ResourceLimits.hh"
#include "fhiclcpp/ParameterSet.h"

namespace art { class Event; }
namespace art { class ProducesCollector; }
namespace art { class ConsumesCollector; }

namespace mu2e { namespace Mu2eG4Config { class Top; } }

namespace mu2e {

  class Mu2eG4IOConfigHelper {

    art::InputTag generatorModuleLabel_;
    Mu2eG4MultiStageParameters multiStagePars_;
    bool timeVD_enabled_;
    bool produceMCTrajectories_;
    bool extMonPixelsEnabled_;

    Mu2eG4ResourceLimits mu2elimits_;
    fhicl::ParameterSet stackingCutsConf_;
    fhicl::ParameterSet steppingCutsConf_;
    fhicl::ParameterSet commonCutsConf_;

  public:

    Mu2eG4IOConfigHelper(const Mu2eG4Config::Top& conf, art::ProducesCollector& pc, art::ConsumesCollector& cc);

    const art::InputTag& generatorModuleLabel() const { return generatorModuleLabel_; }
    const Mu2eG4MultiStageParameters& multiStagePars() const { return multiStagePars_; }
    bool timeVD_enabled() const { return timeVD_enabled_; }
    bool produceMCTrajectories() const { return produceMCTrajectories_; }
    bool extMonPixelsEnabled() const { return extMonPixelsEnabled_; }

    const Mu2eG4ResourceLimits& mu2elimits() const { return mu2elimits_; }
    const fhicl::ParameterSet& stackingCutsConf() const { return stackingCutsConf_; }
    const fhicl::ParameterSet& steppingCutsConf() const { return steppingCutsConf_; }
    const fhicl::ParameterSet& commonCutsConf() const { return commonCutsConf_; }
  };

} // end namespace mu2e

#endif /* Mu2eG4_Mu2eG4IOConfigHelper_hh */
