void wifi_Task(void* pvParameters){
  if (WiFi.status() == WL_NO_SHIELD) {
    xSemaphoreTake(binSemaphore_Console, portMAX_DELAY);
    Serial.println("WiFi shield not present. Wifi service halted.");
    xSemaphoreGive(binSemaphore_Console);
    while (true);
  }
  String fv = WiFi.firmwareVersion();
  if (fv != "1.1.0") {
    xSemaphoreTake(binSemaphore_Console, portMAX_DELAY);
    Serial.println("Please upgrade the firmware");
    xSemaphoreGive(binSemaphore_Console);
  }
  srvr_or_ap();
  while(1){
    if(status == WL_CONNECTED)
      runWifiClient();

    else
      srvr_or_ap();
  }
}

void runWifiClient(){
  WiFiClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    String response = "";
    while (client.connected()) {
      
      if (client.available()) {
        char c = client.read();
        //Serial.write(c);
        response = response + c;
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          
          
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          //client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          servePage(client);
          
          break;
          }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
      
    }
    Serial.println("Request: "+response);

    // give the web browser time to receive the data
    vTaskDelay(1);

    // close the connection:
    client.stop();
    Serial.println("client disonnected");

    //if we get WiFi creds
    if(getCreds(response)){
      //save to NVM
      writeCredNVM();

      //start STA
      //1st disconnect, 2nd begin
    }
  }
}
void srvr_or_ap(){
  pinMode(13, INPUT);
  vTaskDelay(500);
  if(digitalRead(13) == HIGH){
    // attempt to start AP:
    while (status != WL_CONNECTED) {
      xSemaphoreTake(binSemaphore_Console, portMAX_DELAY);
      Serial.print("Attempting to start AP with SSID: ");
      Serial.println(ssid);
      xSemaphoreGive(binSemaphore_Console);

      xSemaphoreTake(binSemaphore_WifiStatus, portMAX_DELAY);
      status = WiFi.apbegin(ssid, pass, channel);
      xSemaphoreGive(binSemaphore_WifiStatus);
      vTaskDelay(10000);
    }
    //AP MODE already started:
    xSemaphoreTake(binSemaphore_Console, portMAX_DELAY);
    Serial.println("AP mode started");
    Serial.println();
    xSemaphoreGive(binSemaphore_Console);
  }
  else{
    xSemaphoreTake(binSemaphore_Console, portMAX_DELAY);
    Serial.println("Will start in STA... ");
    xSemaphoreGive(binSemaphore_Console);
    if(readCredNVM()){
      while (status != WL_CONNECTED){
        xSemaphoreTake(binSemaphore_Console, portMAX_DELAY);
        Serial.print("Attempting to connect to ssid: "); Serial.println(ssid);
        xSemaphoreGive(binSemaphore_Console);

        xSemaphoreTake(binSemaphore_WifiStatus, portMAX_DELAY);
        status = WiFi.begin(ssid, pass);
        xSemaphoreGive(binSemaphore_WifiStatus);
        vTaskDelay(5000);
      }
      isSTA = true;
      xSemaphoreTake(binSemaphore_Console, portMAX_DELAY);
      Serial.print("Connected to: ");
      Serial.println(ssid);
      xSemaphoreGive(binSemaphore_Console);
      return;
    }
    xSemaphoreTake(binSemaphore_Console, portMAX_DELAY);
    Serial.println("Could not connect... halting");
    xSemaphoreGive(binSemaphore_Console);
    while(true);
  }
  
}

bool readCredNVM(){
  
  xSemaphoreTake(binSemaphore_Console, portMAX_DELAY);
  Serial.println("Reading from NVM: ");
  xSemaphoreGive(binSemaphore_Console);
  
  FlashMemory.read();
  byte len = FlashMemory.buf[0];
  byte plen = FlashMemory.buf[len+1];
  ssid[len] = '\0';
  pass[plen] = '\0';

  //read next i char beginning at pos
  byte pos = 1;
  byte i = len;
  while(i > 0) {
    ssid[pos - 1] = FlashMemory.buf[pos];
    pos++;
    i--;
  }

  pos = len + 2;
  i = plen;
  while(i > 0){
    pass[plen-i] = FlashMemory.buf[pos];
    i--;
    pos++;
  }
  xSemaphoreTake(binSemaphore_Console, portMAX_DELAY);
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("pass: ");
  Serial.print(pass);
  xSemaphoreGive(binSemaphore_Console);
  return true;
}

bool writeCredNVM(){
  FlashMemory.read();

  int len = strlen(ssid);
  FlashMemory.buf[0] = len;

  xSemaphoreTake(binSemaphore_Console, portMAX_DELAY);
  Serial.print("FlashMemory.buf[0] = ");
  Serial.println(len);
  
  
  for(int i = 0; i < len; i++) {
    FlashMemory.buf[i+1] = ssid[i];
    Serial.print("FlashMemory.buf[");
    Serial.print(i+1);
    Serial.print("] = ");
    Serial.println(ssid[i]);
    
  }

  int pw_len = strlen(pass);
  FlashMemory.buf[len+1] = pw_len;
  Serial.print("pw_len = ");
  Serial.print(pw_len);
  Serial.print("stored at: ");
  Serial.println(len + 1);
  
  for(int i = 0; i < pw_len; i++) {
    FlashMemory.buf[i+len+2] = pass[i];
    Serial.print("FlashMemory.buf[");
    Serial.print(i+len+2);
    Serial.print("] = ");
    Serial.println(pass[i]);
  }
  Serial.println("Writing to NVM");
  xSemaphoreGive(binSemaphore_Console);
  FlashMemory.update();

  FlashMemory.read();
  Serial.print("SSID len: "); Serial.println(FlashMemory.buf[0]);
  Serial.print("Pass len: "); Serial.println(FlashMemory.buf[(FlashMemory.buf[0] + 1)]);
  
  return true;
}
void servePage(WiFiClient cl){
   char page[] = "<html><body><Form action=\"submitscript\" name =\"login\"><Input type=\"text\" name=\"txtssid\" /> <br /><Input type=\"password\" name=\"txtpass\" /><br /><Input type=\"submit\" name=\"btnSubmit\" value=\"Submit\" /></Form></body></html>";
   cl.println(page);
}

int find_text(String needle, String haystack) {
  int foundpos = -1;
  for (int i = 0; i <= haystack.length() - needle.length(); i++) {
    if (haystack.substring(i,needle.length()+i) == needle) {
      foundpos = i;
    }
  }
  return foundpos;
}

bool getCreds(String str){
  int pos = find_text("ssid=", str);
  if(pos == -1){
    Serial.println("SSID not found");
    return false;
  }
  //ssid = str.substring(pos+5, str.indexOf('&')).c_str();
  strcpy(ssid, str.substring(pos+5, str.indexOf('&')).c_str());
  Serial.print("SSID entered: ");
  Serial.println(ssid);

  pos = find_text("pass=", str);  
  if(pos == -1){
    Serial.println("PASS not found");
    return false;
  }
  //pass = str.substring(pos+5, (find_text("&btn", str))).c_str();
  strcpy(pass, str.substring(pos+5, (find_text("&btn", str))).c_str());
  Serial.print("PASS entered: ");
  Serial.println(pass);
  return true;
}

