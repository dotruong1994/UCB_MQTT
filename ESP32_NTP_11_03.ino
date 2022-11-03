/*
     $Id: ESP32_NTP.ino,v 1.8 2019/04/04 04:48:23 gaijin Exp $

    UDP NTP client example program.

    Get the time from a Network Time Protocol (NTP) time server
    Demonstrates use of UDP sendPacket and ReceivePacket

    Created:  04 Sep 2010 by Michael Margolis
    Modified: 09 Apr 2012 by Tom Igoe
    Modified: 02 Sep 2015 by Arturo Guadalupi
    Munged:   04 Apr 2019 by PuceBaboon (for the ESP32 with a W5500 module)

*/
#include <WiFi.h>
#include <SPI.h>
#include <Ethernet.h>
#include "local_config.h"	// <--- Change settings for YOUR network here.
#include "ModbusClientTCP.h"
#include <AsyncTCP.h>
EthernetClient theClient;
ModbusClientTCP MB(theClient);
#include <AsyncMqttClient.h>
#include <ArduinoJson.h>
#include "time.h"
#define WIFI_TIMEOUT_MS 10000 // 20 second WiFi connection timeout
#define WIFI_RECOVER_TIME_MS 10000 // Wait 30 seconds after a failed connection attempt
extern "C" {
  #include "freertos/FreeRTOS.h"
  #include "freertos/timers.h"
}
const char* ntpServer = "172.21.129.8";
const long  gmtOffset_sec = 25200;
const int   daylightOffset_sec = 0;
char timeHour[11];
char timeHour2[9];
const char* ssid = "Factory-K1-1F-2";
//const char* ssid = "factory-A17-1F-2";
//const char* ssid = "Factory-A3-2F-1";
const char* password = "qwerasdf";

const char* mqtt_server = "172.21.149.102";
const int ledPin = 27 ;
int dem=0;
int ledState = LOW;
String timeHour1;
String timeHour3;

byte IEStandardCycleTime_1;
byte IEStandardCycleTime_2;

byte MachineStatus_1;
byte MachineStatus_2;

byte TodayTotalOutput_1;
byte TodayTotalOutput_2;

byte MachineStartUpTime_1;
byte MachineStartUpTime_2;

byte MachineRunTime_1;
byte MachineRunTime_2;

byte MachineIdleTime_1;
byte MachineIdleTime_2;

byte MachineAlarmTime_1;
byte MachineAlarmTime_2;

byte TodayDefectiveCount_1;
byte TodayDefectiveCount_2;




String sendContent;
String test[]="";
#define MQTT_PORT 8001
AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;

unsigned long previousMillis = 0;   // Stores last time temperature was published
const long interval = 2000;        // Interval at which to publish sensor readings

long lastMsg = 0;
uint16_t addr, words;
/*
   Wiz W5500 reset function.  Change this for the specific reset
   sequence required for your particular board or module.
*/

void WizReset() {
  Serial.print("Resetting Wiz W5500 Ethernet Board...  ");
  pinMode(RESET_P, OUTPUT);
  digitalWrite(RESET_P, HIGH);
  delay(250);
  digitalWrite(RESET_P, LOW);
  delay(50);
  digitalWrite(RESET_P, HIGH);
  delay(350);
  Serial.println("Done.");
}


/*
   This is a crock. It's here in an effort
   to help people debug hardware problems with
   their W5100 ~ W5500 board setups.  It's
   a copy of the Ethernet library enums and
   should, at the very least, be regenerated
   from Ethernet.h automatically before the
   compile starts (that's a TODO item).

*/
/*
   Print the result of the hardware status enum
   as a string.
   Ethernet.h currently contains these values:-

    enum EthernetHardwareStatus {
   	EthernetNoHardware,
   	EthernetW5100,
   	EthernetW5200,
   	EthernetW5500
    };

*/
void prt_hwval(uint8_t refval) {
  switch (refval) {
    case 0:
      Serial.println("No hardware detected.");
      break;
    case 1:
      Serial.println("WizNet W5100 detected.");
      break;
    case 2:
      Serial.println("WizNet W5200 detected.");
      break;
    case 3:
      Serial.println("WizNet W5500 detected.");
      break;
    default:
      Serial.println
      ("UNKNOWN - Update espnow_gw.ino to match Ethernet.h");
  }
}


/*
   Print the result of the ethernet connection
   status enum as a string.
   Ethernet.h currently contains these values:-

    enum EthernetLinkStatus {
       Unknown,
       LinkON,
       LinkOFF
    };

*/
void prt_ethval(uint8_t refval) {
  switch (refval) {
    case 0:
      Serial.println("Uknown status.");
      break;
    case 1:
      Serial.println("Link flagged as UP.");
      break;
    case 2:
      Serial.println("Link flagged as DOWN. Check cable connection.");
      break;
    default:
      Serial.println
      ("UNKNOWN - Update espnow_gw.ino to match Ethernet.h");
  }
}


void setup() {
  Serial.begin(115200);
  delay(500);
  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  WiFi.onEvent(WiFiEvent);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  //mqttClient.onSubscribe(onMqttSubscribe);
  //mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(mqtt_server, MQTT_PORT);
  // If your broker requires authentication (username and password), set them below
  //mqttClient.setCredentials("REPlACE_WITH_YOUR_USER", "REPLACE_WITH_YOUR_PASSWORD");
  connectToWifi();

  // Use Ethernet.init(pin) to configure the CS pin.
  Ethernet.init(5);           // GPIO5 on the ESP32.
  WizReset();
  
  /*
     Network configuration - all except the MAC are optional.

     IMPORTANT NOTE - The mass-produced W5500 boards do -not-
                      have a built-in MAC address (depite
                      comments to the contrary elsewhere). You
                      -must- supply a MAC address here.
  */
  Serial.println("Starting ETHERNET connection...");
  Ethernet.begin(eth_MAC, eth_IP, eth_DNS, eth_GW, eth_MASK);

  delay(200);

  Serial.print("Ethernet IP is: ");
  Serial.println(Ethernet.localIP());

  /*
     Sanity checks for W5500 and cable connection.
  */
  Serial.print("Checking connection.");
  bool rdy_flag = false;
  for (uint8_t i = 0; i <= 20; i++) {
    if ((Ethernet.hardwareStatus() == EthernetNoHardware)
        || (Ethernet.linkStatus() == LinkOFF)) {
      Serial.print(".");
      rdy_flag = false;
      delay(80);
    } else {
      rdy_flag = true;
      break;
    }
  }
  if (rdy_flag == false) {
    Serial.println
    ("\n\r\tHardware fault, or cable problem... cannot continue.");
    Serial.print("Hardware Status: ");
    prt_hwval(Ethernet.hardwareStatus());
    Serial.print("   Cable Status: ");
    prt_ethval(Ethernet.linkStatus());
    while (true) {
      delay(10);          // Halt.
    }
  } else {
    Serial.println(Ethernet.localIP());
    Serial.println(" OK");
  }

  // Set up ModbusTCP client.
  // - provide onData handler function
  MB.onDataHandler(&handleData);
  // - provide onError handler function
  MB.onErrorHandler(&handleError);
  // Set message timeout to 2000ms and interval between requests to the same host to 200ms
  MB.setTimeout(2000, 200);
  // Start ModbusTCP background task
  MB.begin();

  // Issue a request
  // Set Modbus TCP server address and port number
  // (Fill in your data here!)
  MB.setTarget(IPAddress(192, 168, 40, 10), 502);

  // Create request for
  // (Fill in your data here!)
  // - token to match the response with the request. We take the current millis() value for it.
  // - server ID = 4
  // - function code = 0x03 (read holding register)
  // - start address to read = word 2
  // - number of words to read = 6
  //
  // If something is missing or wrong with the call parameters, we will immediately get an error code
  // and the request will not be issued

  //run 1st time
  /*  
    Error err = MB.addRequest((uint32_t)millis(), 1, READ_INPUT_REGISTER, 1056, 1);
    if (err != SUCCESS) {
      ModbusError e(err);
      Serial.printf("Error creating request: %02X - %s\n", (int)e, (const char *)e);
    }
  */   
}

void loop() {
long now = millis();
  if (now - lastMsg > 3000) {
    lastMsg = now;
    printLocalTime();  
    Error err = MB.addRequest((uint32_t)millis(), 1, READ_INPUT_REGISTER, 1041, 10);
    if (err != SUCCESS) {
      ModbusError e(err);
      Serial.printf("Error creating request: %02X - %s\n", (int)e, (const char *)e);
    }
    sendNew();
  }
}
