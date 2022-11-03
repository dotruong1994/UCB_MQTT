void handleData(ModbusMessage response, uint32_t token) 
{
  //Serial.printf("Response: serverID=%d, FC=%d, Token=%08X, length=%d:\n", response.getServerID(), response.getFunctionCode(), token, response.size());
  //Serial.println(response);
  for (auto& byte : response) {
    Serial.printf("%02X ", byte);
  }
  IEStandardCycleTime_1 = response[3];
  IEStandardCycleTime_2 = response[4];
  //-----------------------
  MachineStatus_1 = response[5];
  MachineStatus_2 = response[6];  
  //-----------------------
  TodayTotalOutput_1 = response[7];
  TodayTotalOutput_2 = response[8];
  //-----------------------
  MachineStartUpTime_1 = response[9];
  MachineStartUpTime_2 = response[10]; 
  //-----------------------
  MachineRunTime_1 = response[11];
  MachineRunTime_2 = response[12];   
  //-----------------------
  MachineIdleTime_1 = response[13];
  MachineIdleTime_2 = response[14];
  //-----------------------
  TodayDefectiveCount_1 = response[15];
  TodayDefectiveCount_2 = response[16];
}

// Define an onError handler function to receive error responses
// Arguments are the error code returned and a user-supplied token to identify the causing request
void handleError(Error error, uint32_t token) 
{
  // ModbusError wraps the error code and provides a readable error message for it
  ModbusError me(error);
  Serial.printf("Error response: %02X - %s\n", (int)me, (const char *)me);
}
