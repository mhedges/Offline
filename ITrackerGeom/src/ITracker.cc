#include "ITrackerGeom/inc/ITracker.hh"

using namespace std;

namespace mu2e { 

ITracker::ITracker() {
        _r0              =0.0;
        _z0              =0.0;
        _nSWire          =0;
        _nSDeltaWire     =0;
        _nRing           =0;
        _rOut            =0.0;
        _extFile         ="";
        _isExternal      =false;
        _nSuperLayers    =0;
        _nWalls          =0;
        _zHalfLength     =0.0;
        _max_EndCap_dim  =0.0;
        _geomType        =ITracker::Hexagonal;
        _endcapType      =ITracker::Plane;
        _displayGasLayer =false;
        _displayWires    =false;
}

SuperLayer* ITracker::getSuperLayer(int n) const throw(cms::Exception) {
        if (n>=0 && n< _nSuperLayers){
                return &(_sprlr[n]);
        }
        else throw cms::Exception("GEOM")<< "Super Layer number: "<< n <<" not present";
}

//        const boost::shared_ptr<Wall> ITracker::getWall(int n) throw(cms::Exception) {
//                if (n>=0 && n< _nWalls){
//                        //_walls_it --;//+= (n-_lastSeenWall);
//                        advance (_walls_it,n-_lastSeenWall);
//                        _lastSeenWall = n;
//                        return _walls_it->second;
//                }
//                else throw cms::Exception("GEOM")<< "Wall number: "<< n <<" not present";
//        }

void ITracker::addWall(Wall *wall){
        if (_nWalls==0) _walls.reset(new std::multimap<Wall::Walltype,boost::shared_ptr<Wall> >() );
        _walls->insert(std::pair<Wall::Walltype,boost::shared_ptr<Wall> >(wall->getType(),boost::shared_ptr<Wall>(wall)) );
        _nWalls++;
        if (_nWalls==1){
                _walls_it=_walls->begin();
                _lastSeenWall=0;
        }
}

} // namespace mu2e
