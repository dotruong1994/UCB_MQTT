void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
}
void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void WiFiEvent(WiFiEvent_t event) {
  Serial.printf("[WiFi-event] event: %d\n", event);
  switch(event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      connectToMqtt();
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("WiFi lost connection");
      xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
      xTimerStart(wifiReconnectTimer, 0);
      break;
  }
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");
  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
}
void onMqttPublish(uint16_t packetId) {
  Serial.println("");
  Serial.print("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}


void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  strftime(timeHour,11, "%F", &timeinfo);
  strftime(timeHour2,9, "%T", &timeinfo);
  timeHour1 = String(timeHour);
  timeHour3 = String(timeHour2);
  String timeresetShift1 = "07:00:00"; //06:00:00
//  String timeresetShift2 = "14:00:00"; //14:00:00
//  String timeresetShift3 = "22:00:00";
  if (timeHour3 == timeresetShift1){Serial.println("ESP reset chứ còn cái nịt gì nữa");ESP.restart();}
//  if (timeHour3 == timeresetShift2){Serial.println("ESP reset chứ còn cái nịt gì nữa");ESP.restart();}
//  if (timeHour3 == timeresetShift3){Serial.println("ESP reset chứ còn cái nịt gì nữa");ESP.restart();}
  Serial.println(timeHour3);
}
