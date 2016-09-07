void kernel_Task(void* pvParameters){
  //set output pins for zones
  for(int i = 1; i <= 4; i++){
    pinMode(i, OUTPUT);
    delay(100);
    digitalWrite(i, LOW);
  }
  xSemaphoreTake(binSemaphore_Console, portMAX_DELAY);
  Serial.println(F("Zone pins init complete"));
  xSemaphoreGive(binSemaphore_Console);

  Weather wx;

  //TODO: dynamic query
  //locationid=ZIP:28801&startdate=2016-08-04&enddate=2016-08-05
  wx.appendParams("locationid", "ZIP:28801");
  wx.appendParams("startdate", "2016-09-04");
  wx.appendParams("enddate", "2016-09-05");

  if(wx.getData()){
    //TODO: parse response and process
    Serial.println(F("Got data"));
  }
  else{
    //could not get response
    Serial.println(F("Error getting WX data"));
  }
  while(1){
    Serial.println(F("Kernal Task waiting..."));
    delay(200);
    vTaskDelay(500/portTICK_PERIOD_MS);
  }
}

