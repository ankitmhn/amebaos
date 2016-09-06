#include <WiFi.h>
#include <HttpClient.h>

class Weather{
  private:
  // Number of milliseconds to wait without receiving any data before we give up
  const int kNetworkTimeout = 30*1000;
  // Number of milliseconds to wait if no data is available before trying again
  const int kNetworkDelay = 1000;
  
  String kHostname;// = "www.ncdc.noaa.gov";
  
  //const char kPath[] = "/cdo-web/api/v2/data?datasetid=GHCND&locationid=ZIP:28801&startdate=2016-08-04&enddate=2016-08-05";
  String kPath;// = "/cdo-web/api/v2/data?datasetid=GHCND&locationid=ZIP:28801&startdate=2016-08-04&enddate=2016-08-05";
  String response; 
  
  WiFiClient c;

  
  public:

  Weather(String host = "www.ncdc.noaa.gov", String path = "/cdo-web/api/v2/data?datasetid=GHCND"){
    kHostname = host;
    kPath = path;    
  }
  
  void appendParams(String, String);

  bool getData();
};


