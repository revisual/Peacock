/*

*/
#ifndef WayPoint_h
#define WayPoint_h

#define MAX_ITEMS 20

#include "WayPointData.h"

class WayPoint
{
  public:
    WayPoint();
    bool add( WayPointData waypoint);
    void reset( );
    void next();
    double currentLat();
    double currentLon();
    String currentAction();
    String currentName();
    bool hasArrived(unsigned int dist);
    bool isComplete();


  private:
    int _numbItems = 0;
    int _currentItem = 0;
    WayPointData _data[MAX_ITEMS];
};

#endif
