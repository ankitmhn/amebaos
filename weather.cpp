#ifndef WEATHER_H
#define WEATHER_H
#include "weather.h"
void Weather::appendParams(String param, String value){
  kPath = kPath + "&" + param + "=" + value;
}

bool Weather::getData(){
  HttpClient http(c);
  bool success = true;
  response = "";
  int err =0;  
  err = http.get(kHostname.c_str(), kPath.c_str());
  if (err == 0)
  {
    /*xSemaphoreTake(binSemaphore_Console, portMAX_DELAY);
    Serial.println("startedRequest ok");
    xSemaphoreGive(binSemaphore_Console);
    */
    err = http.responseStatusCode();
    if (err >= 0)
    {
      /*xSemaphoreTake(binSemaphore_Console, portMAX_DELAY);
      Serial.print("Got status code: ");
      Serial.println(err);
      xSemaphoreGive(binSemaphore_Console);
      */

      // Usually you'd check that the response code is 200 or a
      // similar "success" code (200-299) before carrying on,
      // but we'll print out whatever response we get

      err = http.skipResponseHeaders();
      
      if (err >= 0)
      {
        int bodyLen = http.contentLength();
        /*
        xSemaphoreTake(binSemaphore_Console, portMAX_DELAY);
        Serial.print("Content length is: ");
        Serial.println(bodyLen);
        Serial.println();
        Serial.println("Body returned follows:");
        xSemaphoreGive(binSemaphore_Console);
       */
        // Now we've got to the body, so we can print it out
        unsigned long timeoutStart = millis();
        char c;
        
        // Whilst we haven't timed out & haven't reached the end of the body
        while ( (http.connected() || http.available()) &&
               ((millis() - timeoutStart) < kNetworkTimeout) )
        {
            if (http.available())
            {
                c = http.read();
                response = response + c;
                // Print out this character
                /*xSemaphoreTake(binSemaphore_Console, portMAX_DELAY);           
                Serial.print(c);
                xSemaphoreGive(binSemaphore_Console);
               */
                bodyLen--;
                // We read something, reset the timeout counter
                timeoutStart = millis();
            }
            else
            {
                // We haven't got any data, so let's pause to allow some to
                // arrive
                delay(kNetworkDelay);
            }
        }
      }
      else
      {
        /*xSemaphoreTake(binSemaphore_Console, portMAX_DELAY);
        Serial.print("Failed to skip response headers: ");
        Serial.println(err);
        xSemaphoreGive(binSemaphore_Console);
        */
        success = false;
      }
    }
    else
    {
      /*xSemaphoreTake(binSemaphore_Console, portMAX_DELAY);    
      Serial.print("Getting response failed: ");
      Serial.println(err);
      xSemaphoreGive(binSemaphore_Console);
      */
      success = false;
    }
  }
  else
  {
    /*xSemaphoreTake(binSemaphore_Console, portMAX_DELAY);
    Serial.print("Connect failed: ");
    Serial.println(err);    
    xSemaphoreGive(binSemaphore_Console);
    */
    success = false;
  }
  //xSemaphoreGive(binSemaphore_Console);
  http.stop();

  return success;

}
#endif
