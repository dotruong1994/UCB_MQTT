void sendSample() {
  dem++;
  StaticJsonDocument<128> doc;
  doc.clear();
  doc["Machine name"] = "UCC";
  doc["Product name"] = "ABCXYZ";
  doc["Output"] = sendContent;
  doc["Date"] = timeHour1;
  doc["Time"] = timeHour3;
  char buffersend[128];

  //  serializeJson(doc, Serial);
  serializeJson(doc, buffersend);


  uint16_t packetIdPub1 = mqttClient.publish("Nothing", 1, true, buffersend);
  Serial.printf("Publishing on topic %s at QoS 1, packetId: %i", "Nothing", packetIdPub1);
  //  Serial.printf("Message: %.2f \n", content);
}

void sendNew() {
  StaticJsonDocument<512> doc;

  doc["Machine_name"] = "UCB_01";
  doc["Product_name"] = "1351824120";
  doc["Date"] = "12";
  doc["Time"] = "10";

  JsonObject To_Mqtt = doc.createNestedObject("To_Mqtt");

  JsonArray To_Mqtt_IEStandardCycleTime = To_Mqtt.createNestedArray("IEStandardCycleTime");
  To_Mqtt_IEStandardCycleTime.add(80);
  To_Mqtt_IEStandardCycleTime.add(443);

  JsonArray To_Mqtt_MachineStatus = To_Mqtt.createNestedArray("MachineStatus");
  To_Mqtt_MachineStatus.add(80);
  To_Mqtt_MachineStatus.add(443);

  JsonArray To_Mqtt_MachineStartUpTime = To_Mqtt.createNestedArray("MachineStartUpTime");
  To_Mqtt_MachineStartUpTime.add(80);
  To_Mqtt_MachineStartUpTime.add(443);

  JsonArray To_Mqtt_MachineRunTime = To_Mqtt.createNestedArray("MachineRunTime");
  To_Mqtt_MachineRunTime.add(80);
  To_Mqtt_MachineRunTime.add(443);

  JsonArray To_Mqtt_MachineIdleTime = To_Mqtt.createNestedArray("MachineIdleTime");
  To_Mqtt_MachineIdleTime.add(80);
  To_Mqtt_MachineIdleTime.add(443);

  JsonArray To_Mqtt_TodayDefectiveCount = To_Mqtt.createNestedArray("TodayDefectiveCount");
  To_Mqtt_TodayDefectiveCount.add(80);
  To_Mqtt_TodayDefectiveCount.add(443);

  JsonArray To_Mqtt_MachineAlarmTime = To_Mqtt.createNestedArray("MachineAlarmTime");
  To_Mqtt_MachineAlarmTime.add(80);
  To_Mqtt_MachineAlarmTime.add(443);

  char buffersendnew[400];
  serializeJson(doc, buffersendnew);

  //  serializeJson(doc, Serial);


  uint16_t packetIdPub1 = mqttClient.publish("Nothing", 1, true, buffersendnew);
  //  Serial.printf("Publishing on topic %s at QoS 1, packetId: %i", "Nothing", packetIdPub1);
}
