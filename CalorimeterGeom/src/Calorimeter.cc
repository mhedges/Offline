//
// Geometry and identifier info about the Calorimeter.
//
//
// $Id: Calorimeter.cc,v 1.3 2010/05/18 21:16:49 kutschke Exp $
// $Author: kutschke $ 
// $Date: 2010/05/18 21:16:49 $
//
// Original author R. Bernstein and Rob Kutschke
//

#include "CalorimeterGeom/inc/Calorimeter.hh"

using namespace std;

namespace mu2e { 
  namespace calorimeter{

    // On readback from persistency, recursively recompute mutable members.
    void Calorimeter::fillPointers() const{
      for ( size_t i=0; i<_vanes.size(); ++i ){
        _vanes[i].fillPointers(*this);
      }
    }

    // Given the _indices member in each Layer, fill the _straws member.
    void Calorimeter::FillPointers1(){

      for ( vector<Vane>::iterator ivane = _vanes.begin(),
              edev = _vanes.end(); 
            ivane != edev;  ++ivane ){
  
        for ( vector<ZSlice>::iterator izsl = ivane->_zslices.begin(),
                ezsl = ivane->_zslices.end();
              izsl != ezsl; ++izsl ){

          for ( vector<RSlice>::iterator irsl = izsl->_rslices.begin(),
                  ersl = izsl->_rslices.end();
                irsl != ersl; ++irsl ){
            irsl->_crystals.clear();
            for ( vector<CrystalIndex>::iterator icrys = irsl->_indices.begin(),
                    ecrys = irsl->_indices.end();
                  icrys != ecrys ; ++icrys ){
              const Crystal& crys = _allCrystals[(*icrys).asInt()];
              irsl->_crystals.push_back( &crys );
            }
          }
        }
      }

    }

    void Calorimeter::FillPointers2(){

      // Fill nearest neighbour indices and pointers from the NN Ids.
      for ( deque<Crystal>::iterator i= _allCrystals.begin(), 
              e= _allCrystals.end();
            i!=e; 
            ++i){
        //vector<const Straw *>& byPtr = i->_nearest;
        vector<CrystalId>& byId        = i->_nearestById;
        vector<CrystalIndex>& byIndex  = i->_nearestByIndex;
    
        //    byPtr.clear();
        byIndex.clear();
    
        for ( vector<CrystalId>::iterator j=byId.begin(), je=byId.end();
              j != je; ++j){
          const CrystalId& id = *j;
          const Crystal& crystal = getCrystal(id);
          //      byPtr.push_back( &straw);
          byIndex.push_back( crystal.Index() );
        }
      }

    }
  } //namespace calorimeter
} // namespace mu2e
