
#define BUFF_MAX 2048
void getLatestTime()
{
  
  // struct to hold time 
  
  
  // retrieve data from DS3231
  DS3231_get(&t);
  
  
  char buff[BUFF_MAX];
  
#ifdef CONFIG_UNIXTIME
        snprintf(buff, BUFF_MAX, "%d.%02d.%02d %02d:%02d:%02d %ld", t.year,
             t.mon, t.mday, t.hour, t.min, t.sec, t.unixtime);
#else
        snprintf(buff, BUFF_MAX, "%d.%02d.%02d %02d:%02d:%02d", t.year,
             t.mon, t.mday, t.hour, t.min, t.sec);
#endif
  //wait for console 
  xSemaphoreTake(binSemaphore_Console,portMAX_DELAY);
  
  Serial.print(buff);
  Serial.print(" which is a ");
  switch(t.wday){
  case 1:
    Serial.println("Sunday.");
    break;
  case 2:
    Serial.println("Monday.");
    break;
  case 3:
    Serial.println("Tuesday.");
    break;
  case 4:
    Serial.println("Wednesday.");
    break;
  case 5:
    Serial.println("Thursday.");
    break;
  case 6:
    Serial.println("Friday.");
    break;
  case 7:
    Serial.println("Saturday.");
    break;
  }
  xSemaphoreGive(binSemaphore_Console);
}


