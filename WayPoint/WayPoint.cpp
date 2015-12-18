/*

*/

#include "WayPoint.h"

WayPoint::WayPoint() {}

bool WayPoint::add( WayPointData waypoint)
{
  if ( _numbItems == MAX_ITEMS) return false;
  _data[_numbItems] = waypoint;
  _numbItems++;
  return true;  
}

 bool WayPoint::hasArrived(unsigned int dist)
 {
    return( _data[_currentItem].hasArrived(dist)); 
 }

 bool WayPoint::isComplete()
 {
    return( _currentItem >= _numbItems);    
 }

WayPointData WayPoint::next()
{
    _currentItem++;
    return _data[_currentItem];
}

WayPointData WayPoint::current()
{
    return _data[_currentItem];
}

