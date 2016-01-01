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
  return ( _data[_currentItem].hasArrived(dist));
}

bool WayPoint::isComplete()
{
  return ( _currentItem >= (_numbItems));
}

void WayPoint::next()
{
  _currentItem++;
}

void WayPoint::reset()
{
  _currentItem = 0;
}

double WayPoint::currentLat()
{
  return _data[_currentItem].getLat();
}

double WayPoint::currentLon()
{
  return _data[_currentItem].getLon();
}

String WayPoint::currentAction()
{
  return  _data[_currentItem].getAction();
}

String WayPoint::currentName()
{
  _data[_currentItem].getName();
}

