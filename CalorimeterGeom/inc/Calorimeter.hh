#ifndef CalorimeterGeom_Calorimeter_hh
#define CalorimeterGeom_Calorimeter_hh
//
// Interface to the disk calorimeter
// Original author B. Echenard
//


#include "Mu2eInterfaces/inc/Detector.hh"
#include "CalorimeterGeom/inc/Calorimeter.hh"
#include "CalorimeterGeom/inc/CaloGeomInfo.hh"
#include "CalorimeterGeom/inc/CaloGeomUtil.hh"
#include "CalorimeterGeom/inc/Disk.hh"
#include "CalorimeterGeom/inc/Crystal.hh"

#include "CLHEP/Vector/ThreeVector.h"
#include <vector>


namespace mu2e {
    
    class Calorimeter: virtual public Detector {

	public:

	   //no constructor for this interface
	   virtual ~Calorimeter(){};


           // calo sections
	   virtual unsigned int             nDisk()     const = 0;  
	   virtual const Disk&              disk(int i) const = 0;  


  	   // crystal / readout section
           virtual unsigned int             nCrystal()     const = 0; 
	   virtual unsigned int             nRO()          const = 0; 
           virtual const Crystal&           crystal(int i) const = 0; 

	   virtual int                      crystalByRO(int roid)          const = 0;  
	   virtual int                      ROBaseByCrystal(int crystalId) const = 0; 


            // calorimeter geometry information 
	   virtual const CaloInfo&          caloInfo() const = 0;
	   virtual const CaloGeomInfo&      geomInfo() const = 0;  	    
	   virtual const CaloGeomUtil&      geomUtil() const = 0; 



  	   // neighbors, indexing 
           virtual const std::vector<int>&  neighbors(int crystalId)                             const = 0;
           virtual const std::vector<int>&  nextNeighbors(int crystalId)                         const = 0;
           virtual       std::vector<int>   neighborsByLevel(int crystalId, int level)           const = 0; 
           virtual int                      crystalIdxFromPosition(const CLHEP::Hep3Vector &pos) const = 0;


           // get to know me!
           virtual void                     print(std::ostream &os = std::cout)  const = 0;
    };

}

#endif 
