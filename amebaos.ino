#include <config.h>
#include <ds3231.h>

#include <FreeRTOS.h>
#include <croutine.h>
#include <event_groups.h>
//#include <FreeRTOSConfig.h>
//#include <FreeRTOSVariant.h>
#include <list.h>
#include <mpu_wrappers.h>
#include <portable.h>
#include <portmacro.h>
#include <projdefs.h>
#include <queue.h>
#include <semphr.h>
#include <StackMacros.h>
#include <task.h>
#include <timers.h>

#include <HttpClient.h>

#include <WiFi.h>
#include <Wire.h>

#include <FlashMemory.h>
#include "weather.h"
#include "rtc_ds3231.h"

TaskHandle_t RTC_TaskHandle;
TaskHandle_t kernel_TaskHandle;
TaskHandle_t wifi_TaskHandle;

SemaphoreHandle_t binSemaphore_RTC = NULL;
SemaphoreHandle_t binSemaphore_Console = NULL;
SemaphoreHandle_t binSemaphore_WifiStatus = NULL;

#define printMsg(taskhandle,str)  {Serial.print(F("Priority "));Serial.print(uxTaskPriorityGet(taskhandle));Serial.print(F(" : "));Serial.println(F(str));}

struct ts t; //struct to hold RTC time -> is continuously updated
bool isSTA = false;
int status = WL_IDLE_STATUS;     // the Wifi radio's status

char ssid[50] = "ameba";  //Set the AP's SSID
char pass[50] = "amebaiot";     //Set the AP's password
char channel[] = "1";         //Set the AP's channel

// Name of the server we want to connect to
WiFiServer server(80);

void setup()
{  
    Serial.begin(9600);
    Serial.println(F("In Setup function, Creating Binary Semaphore"));

    vSemaphoreCreateBinary(binSemaphore_RTC);  /* Create binary semaphore */
    vSemaphoreCreateBinary(binSemaphore_Console);
    vSemaphoreCreateBinary(binSemaphore_WifiStatus);

    if(binSemaphore_RTC != NULL && binSemaphore_RTC != NULL && binSemaphore_WifiStatus != NULL)
    {
        Serial.println(F("Creating RTC task"));
        xTaskCreate(RTC_Task, "RTC_Task", 1000, NULL, 1, &RTC_TaskHandle);

        Serial.println(F("Creating kernel task"));
        xTaskCreate(kernel_Task, "Krnl_Task", 1000, NULL, 2, &kernel_TaskHandle);

        Serial.println(F("Creating WIFI task"));
        xTaskCreate(wifi_Task, "WIFI_Task", 1000, NULL, 3, &wifi_TaskHandle);

        vTaskStartScheduler();
    }
    else
    {
        Serial.println(F("Failed to create Semaphore"));
    }
}


void loop()
{ // Hooked to Idle Task, will run when CPU is Idle
    Serial.println(F("Loop function"));
    delay(50);
}

void RTC_Task(void* pvParameters){
  Wire.begin();
  DS3231_init(DS3231_INTCN);
  while(1){
    //take control
    
    xSemaphoreTake(binSemaphore_RTC,portMAX_DELAY);
    getLatestTime();
    xSemaphoreGive(binSemaphore_RTC);
    Serial.println(F("RTC Task waiting..."));
    delay(200);
    vTaskDelay(10000/portTICK_PERIOD_MS); //wait 10 sec before next update
  }
}
