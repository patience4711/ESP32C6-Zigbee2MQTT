//#include <AsyncJson.h>
#include <ArduinoJson.h>

#include "esp_log.h"
#include <string.h>
#include "Zigbee.h" 
#include "esp_zigbee_core.h"
// #include "esp_coexist.h" ? ? ? see setupespasynctype:AllTopic:All
#include "esp_coexist.h"

#include <ESP32wifiConnect.h>
ESP32wifiConnect my_wifiConnect;

#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h" // De nieuwe async bibliotheek

#include <AsyncEventSource.h>

#include <WiFi.h>
#include <esp_wifi.h>
#include <DNSServer.h>

#include "OTA.h"
#include <Update.h>
//#include <Hash.h>
#include "PSACrypto.h"
#define VERSION "ESP32-ZIGBEE_v1_4"

#include <TimeLib.h>
#include <time.h>
#include <sunMoon.h>

#include "soc/soc.h"           // ha for brownout
//#include "soc/rtc_cntl_reg.h"  // ha for brownout
#include <esp_task_wdt.h>      // ha
#include <rtc_wdt.h>
#include "lwip/tcpip.h"

#include "SPIFFS.h"
#include "FS.h"

//#include "AsyncJson.h"
#include <Arduino.h>

#include <Preferences.h>  //#include <AsyncTCP.h>
Preferences prefs;

AsyncWebServer server(80);
AsyncEventSource events("/events");
AsyncWebSocket ws("/ws");

#include <PubSubClient.h>

#include <NTPClient.h>
#include <WiFiUdp.h>
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
const byte DNS_PORT = 53;
DNSServer dnsServer;

//#include ZigbeeCore.h

#include "HTML.h"
#include "AAA_MENUPAGE.h"
//#include "PORTAL_HTML.h"
//#include "DEVICES.ino"
#include "eventSource.h"
/*
 * if we define TEST we have a special environment to test the decoding of a string
 * for the different invertertypes. We only test inv0. If we set this up as a DS3
 * a ds3 string will be analyzed.
 * - no automatic polling
 * - no automatic healthcheck
 * - no start of zigbee at boot\
 * - in the console an option to test inv 0 '10;TESTINV'
 * - more debug information in the console
 */
//bool uartBusy = true;
uint8_t normalOps = 0;  //means dicard all zigbee trafic

typedef struct {
  char devName[20];
  esp_zb_ieee_addr_t devIeee;
  uint16_t shortAddr; // 16-bit netwerkadres
  int devType;
  int devIdx;
  uint8_t endpoint;
  bool toMqtt;
  bool isActive;
  } devices;
devices Dev_Prop[9];
  
 enum ColorIdx { R = 0, G = 1, B = 2, D = 3};
 typedef struct { 
  uint16_t profileId;
  uint16_t deviceId;
  uint8_t inputClusterCount;
  uint16_t inputClusters[20];
  uint8_t outputClusterCount;
  uint16_t outputClusters[20];
  float values[4];
  uint8_t preset[4];
} deviceData;
deviceData Dev_Data[9];

uint8_t devCount = 0;

//bool readZBFF(char *inMess);
//bool waitForPairAndQuery(char* s_d, uint16_t &shortAddr, uint8_t ieeeAddr[8]);
//#define TEST // prevents healthcheck and polling
String debugLog = "";  // the debug info see consoleOut

uint8_t procesId = 1;  //determines the please wait time
//bool USB_serial = true;
char ssid[33] = "";  // was 33
char pass[64] = "";  // was 40
bool tryConnectFlag = false;
bool wifiConnected = false;
// variables concerning time and timers
WiFiUDP Udp;
//time_t daybeginTime = 0;
//time_t switchoffTime = 0;
//time_t switchonTime = 0;
//bool dayTime=true;

//  byte mDay = 0;
//  String  maan = "";
uint8_t actionFlag = 0;
//uint8_t midnightFlag = 0;
// variables wificonfig
char pswd[11] = "";
char userPwd[11] = "";
float longi = 0;
float lati = 0;
char gmtOffset[5] = "";  //+5.30 GMT
bool zomerTijd = true;
//char static_ip[16] = "";
uint8_t securityLevel = 6;
bool ledSignal=false;

// ***************************************************************************
//                             variables ECU
// ***************************************************************************
// ZIGBEE_globals
//#define CC2530_MAX_SERIAL_BUFFER_SIZE 512
//#define MAX_FRAME_SIZE 128
//#define YC600_MAX_NUMBER_OF_INVERTERS 9  // 0 1 2 3 4 5 6 7 8
//char inMessage[CC2530_MAX_SERIAL_BUFFER_SIZE] = {0};
int readCounter = 0;
//char messageHead[5];
int diagNose = 2;  // initial 2 all to serial;
                   //bool Polling = false; // when true we have automatic polling
                   //int errorCode=10;
                   //int recovered = 0;
char txBuffer[90];
char InputBuffer_Serial[50];  // need to be global


uint8_t zigbeeUp = 11;  // initial allways initializing, this changes to 1 = up or 0 not up after initial healthcheck
bool zigbeeInitialized = false;
int pollOffset = 0;
char requestUrl[15] = { "/" };  // to remember from which webpage we came
 
// variables mqtt ********************************************
char Mqtt_Broker[30] = { "" };
char Mqtt_outTopic[26] = { "" };
char Mqtt_inTopic[26] = { "" };  // was 26
char Mqtt_Username[26] = { "" };
char Mqtt_Password[26] = { "" };
char Mqtt_Port[5] = { "" };
int Mqtt_Format = 0;

int event = 0;
long mqtt_lastConnect = 0;

int dst;
//int iKeuze;
uint8_t devChoice;
uint8_t toJoin;
bool joinWithBind = false;
bool joinSuccess = false;
bool timeRetrieved = false;
int networksFound = 0;  // used in the portal
int datum = 0;          //

typedef struct {
  bool onoff = false;
  uint8_t mode = 0;
  uint8_t red = 0;
  uint8_t gre = 0;
  uint8_t blu = 0;
  uint8_t dim = 0;
  uint8_t present[4];
  uint8_t colorTemp = 0;
} lamps;
lamps lampState[9];
uint8_t pendingFlag = 0;


unsigned long previousMillis = 0;        // will store last temp was read
static unsigned long laatsteMeting = 0;  //wordt ook bij OTA gebruikt en bij wifiportal
static unsigned long lastCheck = 0;      //wordt ook bij OTA gebruikt en bij wifiportal

//#define LED_AAN HIGH  //sinc
//#define LED_UIT LOW
#define knop 0      //
//#define led_onb 8   // onboard led was 2
//#define RGB_LED_PIN 8   // onboard led was 2
#define ZB_RESET 5  // D5
//#define ZB_TX             17 // D8

// we use this string only to send webpages
String toSend = "";

int value = 0;

//int aantal = 0;
//int resetCounter=0;
//bool apFlag=false;
// *******************************  log *************************************
//// variables To record and display last events on the home page.
//struct logEvent {
//  String    Log_date ;
//  String    Log_kind ;
//  String    Log_message;
//};
// *******************************  log *************************************
// variables To record and display last events on the home page.
#define Log_MaxEvents 18

typedef struct {
  char date[14];
  int kind;  // zigbee, system, mqtt, pairing
  char message[13];
} logEvent;
logEvent Log_Events[Log_MaxEvents];
bool Log_MaxReached = false;
byte logNr = 0;

WiFiClient espClient;
PubSubClient MQTT_Client(espClient);

#include <Ticker.h>
Ticker resetTicker;
//AsyncWebServerRequest* pendingRequest = nullptr;
char PAN_ID[5];

int interviewFlag = 20;
int mqttFlag = 0;
unsigned long triggerTime = 0;  // when shouldDo was set true
//const unsigned long delayDuration = 60000UL; // 60 seconds in ms
bool joinAbort = false;  // predeclare

//#define THERMOSTAT_ENDPOINT 1
//ZigbeeThermostat zbThermostat(THERMOSTAT_ENDPOINT);
//#define SENSOR_ENDPOINT     2
//ZigbeeTempSensor zbTempSensor(SENSOR_ENDPOINT);
bool zbDebug = false;
bool networkOpen = false;       // Staat de poort open of dicht?
uint32_t networkOpenTime = 0;        // Het milliseconde-tijdstip van openen
uint32_t pairingWindowDurationSecs = 0; // Hoeveel seconden mocht de poort openblijven?
