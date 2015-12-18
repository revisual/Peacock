/*

*/

#include "WayPointData.h"
#include "Arduino.h"
#include "StringUtil.h"

WayPointData::WayPointData() {}

WayPointData::WayPointData(String data, char delimiter) {
  deserialise( data, delimiter);
}

void WayPointData::setCoords( double lat, double lon )
{
  _lat = lat;
  _lon = lon;
}

double WayPointData::getLat(  )
{
  return _lat;
}

double WayPointData::getLon(  )
{
  return _lon;
}

void WayPointData::setArrivalTolerance( unsigned int tolerance )
{
  _arrivalTolerance = tolerance;
}

void WayPointData::setAction( String action )
{
  _action = action;
}

void WayPointData::deserialise( String data, char delimiter )
{
  StringUtil util;
  _lat = (double)util.getSubstring(data, delimiter, 0).toFloat();
  _lon = (double)util.getSubstring(data, delimiter, 1).toFloat();
  _arrivalTolerance = (int)util.getSubstring(data, delimiter, 2).toInt();
  _action = util.getSubstring(data, delimiter, 3);
}


bool WayPointData::hasArrived( unsigned int distance )
{
  return distance <= _arrivalTolerance;
}
