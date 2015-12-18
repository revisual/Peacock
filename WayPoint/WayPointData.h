/*
 
*/
#ifndef WayPointData_h
#define WayPointData_h

#include "Arduino.h"

class WayPointData
{
  public:
    WayPointData();
    WayPointData(String data, char delimiter);
    void setCoords( double lat, double lon );
    void setArrivalTolerance( unsigned int tolerance );
    void setAction( String action );
    void deserialise( String data, char delimiter );
    bool hasArrived( unsigned int distance );
    double getLat(  );
    double getLon(  );
    
  private:
    double _lat;
    double _lon;
    int _arrivalTolerance;
    String _action;
};

#endif
