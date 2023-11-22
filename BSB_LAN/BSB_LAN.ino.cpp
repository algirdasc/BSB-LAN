# 1 "/tmp/tmptuj3h1i2"
#include <Arduino.h>
# 1 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
# 452 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
#include "platformio.h"

#if defined(__AVR__)
#error "Sorry, Arduino Mega not supported since BSB-LAN 2.1"
#endif

#define LOGTELEGRAM_OFF 0
#define LOGTELEGRAM_ON 1
#define LOGTELEGRAM_UNKNOWN_ONLY 2
#define LOGTELEGRAM_BROADCAST_ONLY 4
#define LOGTELEGRAM_UNKNOWNBROADCAST_ONLY 6

#define HTTP_AUTH 1
#define HTTP_GZIP 2
#define HTTP_HEAD_REQ 4
#define HTTP_ETAG 8
#define HTTP_GET_ROOT 16
#define HTTP_FRAG 128

#define HTTP_FILE_NOT_GZIPPED false
#define HTTP_FILE_GZIPPED true

#define HTTP_NO_DOWNLOAD false
#define HTTP_IS_DOWNLOAD true

#define HTTP_ADD_CHARSET_TO_HEADER true
#define HTTP_DO_NOT_ADD_CHARSET_TO_HEADER false
#define HTTP_DO_NOT_CACHE -1
#define HTTP_AUTO_CACHE_AGE 0

#define HTTP_OK 200
#define HTTP_NOT_MODIFIED 304
#define HTTP_AUTH_REQUIRED 401
#define HTTP_NOT_FOUND 404

#define MIME_TYPE_TEXT_HTML 1
#define MIME_TYPE_TEXT_CSS 2
#define MIME_TYPE_APP_JS 3
#define MIME_TYPE_APP_XML 4
#define MIME_TYPE_TEXT_TEXT 5
#define MIME_TYPE_APP_JSON 6
#define MIME_TYPE_TEXT_PLAIN 7
#define MIME_TYPE_IMAGE_JPEG 101
#define MIME_TYPE_IMAGE_GIF 102
#define MIME_TYPE_IMAGE_SVG 103
#define MIME_TYPE_IMAGE_PNG 104
#define MIME_TYPE_IMAGE_ICON 105
#define MIME_TYPE_APP_GZ 201
#define MIME_TYPE_FORCE_DOWNLOAD 202

#define DO_NOT_PRINT_DISABLED_VALUE false
#define PRINT_DISABLED_VALUE true
#define PRINT_NOTHING 0
#define PRINT_VALUE 1
#define PRINT_DESCRIPTION 2
#define PRINT_VALUE_FIRST 4
#define PRINT_DESCRIPTION_FIRST 8
#define PRINT_ONLY_VALUE_LINE 16
#define PRINT_ENUM_AS_DT_BITS 32

#if defined(ESP32)
void loop();
int set(float line, const char *val, bool setcmd);
#endif

typedef struct {
  float number;
  short int dest_addr;
} parameter;

#include <Arduino.h>
#include "src/Base64/src/Base64.h"


#include "src/BSB/bsb.h"
#include "BSB_LAN_config.h"
#include "BSB_LAN_defs.h"

#define REQUIRED_CONFIG_VERSION 33
#if CONFIG_VERSION < REQUIRED_CONFIG_VERSION
  #error "Your BSB_LAN_config.h is not up to date! Please use the most recent BSB_LAN_config.h.default, rename it to BSB_LAN_config.h and make the necessary changes to this new one."
#endif

#define EEPROM_SIZE 0x1000
#if !defined(EEPROM_ERASING_PIN)
  #if defined(ESP32)
    #if defined(RX1)
#undef EEPROM_ERASING_PIN
#define EEPROM_ERASING_PIN 34
    #else
#undef EEPROM_ERASING_PIN
#define EEPROM_ERASING_PIN 18
    #endif
  #else
#undef EEPROM_ERASING_PIN
#define EEPROM_ERASING_PIN 31
  #endif
#endif
#if !defined(EEPROM_ERASING_GND_PIN) && !defined(ESP32)
  #define EEPROM_ERASING_GND_PIN 33
#endif
#if !defined(LED_BUILTIN)
  #define LED_BUILTIN 2
#endif

#if defined(__arm__)
  #include <SPI.h>
  #include <Wire.h>
  #include "src/I2C_EEPROM/I2C_EEPROM.h"
template<uint8_t I2CADDRESS=0x50> class UserDefinedEEP : public eephandler<I2CADDRESS, 4096U,2,32>{};


UserDefinedEEP<> EEPROM;
#endif

#if defined(ESP32)
  #include <esp_task_wdt.h>
  #include <EEPROM.h>
  #include <WiFiUdp.h>
  #include <esp_wifi.h>
WiFiUDP udp, udp_log;
  #if defined(ENABLE_ESP32_OTA)
    #include <WebServer.h>
    #include <Update.h>
WebServer update_server(8080);
  #endif

EEPROMClass EEPROM_ESP((const char *)PSTR("nvs"));
  #define EEPROM EEPROM_ESP

  #define strcpy_PF strcpy
  #define strcat_PF strcat
  #define strchr_P strchr
#else
  #ifdef WIFI
    #include "src/WiFiSpi/src/WiFiSpiUdp.h"
WiFiSpiUdp udp, udp_log;
  #else
    #include <EthernetUdp.h>
EthernetUDP udp, udp_log;
  #endif
#endif



#include "src/CRC32/CRC32.h"

#if defined(ESP32)
  #include "src/esp32_time.h"
#else
  #include "src/Time/TimeLib.h"
#endif

#ifdef MQTT
  #include "src/PubSubClient/src/PubSubClient.h"
#endif
#include "html_strings.h"

#ifdef BME280




  #include <Wire.h>
  #include "src/BlueDot_BME280/BlueDot_BME280.h"
BlueDot_BME280 *bme;

  #define TCA9548A_ADDR 0x70
#endif

bool client_flag = false;
#ifdef WIFI
  #ifdef ESP32
    #include <WiFi.h>
bool localAP = false;
unsigned long localAPtimeout = millis();
  #else
    #include "src/WiFiSpi/src/WiFiSpi.h"
using ComServer = WiFiSpiServer;
using ComClient = WiFiSpiClient;
    #define WiFi WiFiSpi
  #endif
#else
  #ifdef ESP32


    #include <ETH.h>

class Eth : public ETHClass {
public:
    int maintain(void) const { return 0;} ;
    void begin(uint8_t *mac, IPAddress ip, IPAddress dnsserver, IPAddress gateway, IPAddress subnet) {
      begin(mac);
      config(ip, gateway, subnet, dnsserver, dnsserver);
    }
    void begin(uint8_t *mac) {
      ETHClass::begin(ETH_PHY_ADDR, ETH_PHY_POWER, ETH_PHY_MDC, ETH_PHY_MDIO, ETH_PHY_TYPE, ETH_CLK_MODE);
    }
};

Eth Ethernet;
  #else
    #include <Ethernet.h>
using ComServer = EthernetServer;
using ComClient = EthernetClient;
  #endif
#endif

#ifdef ESP32
using ComServer = WiFiServer;
using ComClient = WiFiClient;
#endif

#if defined(MDNS_SUPPORT)
  #if !defined(ESP32)
    #include "src/ArduinoMDNS/ArduinoMDNS.h"
MDNS mdns(udp);
  #else
    #include <ESPmDNS.h>
  #endif
#endif

bool EEPROM_ready = true;
byte programWriteMode = 0;

ComServer *server;
ComServer *telnetServer;
Stream *SerialOutput;


BSB *bus;



#define OUTBUF_LEN 450
char outBuf[OUTBUF_LEN] = { 0 };


#undef OUTBUF_USEFUL_LEN
#define OUTBUF_USEFUL_LEN (OUTBUF_LEN * 2)

char bigBuff[OUTBUF_USEFUL_LEN + OUTBUF_LEN] = { 0 };
int bigBuffPos=0;


char DebugBuff[OUTBUF_LEN] = { 0 };

#if defined(__SAM3X8E__)
const char averagesFileName[] PROGMEM = "averages.txt";
const char datalogFileName[] PROGMEM = "datalog.txt";
const char datalogIndexFileName[] PROGMEM = "datalog.idx";
const char journalFileName[] PROGMEM = "journal.txt";
const char datalogTemporaryFileName[] PROGMEM = "datalog.tmp";
const char datalogIndexTemporaryFileName[] PROGMEM = "dataidx.tmp";
#elif defined(ESP32)
const char averagesFileName[] PROGMEM = "/averages.txt";
const char datalogFileName[] PROGMEM = "/datalog.txt";
const char datalogIndexFileName[] PROGMEM = "/datalog.idx";
const char journalFileName[] PROGMEM = "/journal.txt";
const char datalogTemporaryFileName[] PROGMEM = "/datalog.tmp";
const char datalogIndexTemporaryFileName[] PROGMEM = "/dataidx.tmp";
#endif





typedef union {
  struct { uint8_t day, month; uint16_t year; } elements;
  uint32_t combined;
} compactDate_t;
#if BYTE_ORDER != LITTLE_ENDIAN
#error "Unexpected endian, please contact DE-cr on github"
#endif
#define datalogIndexEntrySize (sizeof(compactDate_t)+sizeof(uint32_t))
compactDate_t previousDatalogDate, firstDatalogDate, currentDate;

ComClient client;
#ifdef MQTT
ComClient *mqtt_client;
#endif
#ifdef VERSION_CHECK
ComClient httpclient;
#endif
ComClient telnetClient;

#ifdef MAX_CUL
ComClient *max_cul;
#endif

#ifdef MQTT
PubSubClient *MQTTPubSubClient;
#include "MQTTSensors.h"
#endif
bool haveTelnetClient = false;

#define MAX_CUL_DEVICES (sizeof(max_device_list)/sizeof(max_device_list[0]))
#ifdef MAX_CUL
int32_t max_devices[MAX_CUL_DEVICES] = { 0 };
uint16_t max_cur_temp[MAX_CUL_DEVICES] = { 0 };
uint8_t max_dst_temp[MAX_CUL_DEVICES] = { 0 };
int8_t max_valve[MAX_CUL_DEVICES] = { -1 };
#endif





#if defined LOGGER || defined WEBSERVER
  #if defined(ESP32)
    #if defined(ESP32_USE_SD)
      #include "FS.h"






      #include "SD.h"

      #define MINIMUM_FREE_SPACE_ON_SD 100000
    #else
      #include <LittleFS.h>
      #define SD LittleFS

      #define MINIMUM_FREE_SPACE_ON_SD 10000
    #endif
  #else
    #define FILE_APPEND FILE_WRITE

    #define MINIMUM_FREE_SPACE_ON_SD 100


    #include "src/SdFat/SdFat.h"
SdFat SD;
  #endif
#endif

#ifdef ONE_WIRE_BUS


  #include "src/OneWireNg/OneWire.h"
  #include "src/DallasTemperature/DallasTemperature.h"
  #ifndef TEMPERATURE_PRECISION
    #define TEMPERATURE_PRECISION 9

  #endif
OneWire *oneWire;
DallasTemperature *sensors;
uint8_t numSensors;
unsigned long lastOneWireRequestTime = 0;
  #ifndef ONE_WIRE_REQUESTS_PERIOD
    #define ONE_WIRE_REQUESTS_PERIOD 25000
  #endif
#endif

#ifdef DHT_BUS
  #include "src/DHTesp/DHTesp.h"
DHTesp dht;

unsigned long DHT_Timer = 0;
int last_DHT_State = 0;
uint8_t last_DHT_pin = 0;
#endif

unsigned long maintenance_timer = millis();
unsigned long lastAvgTime = 0;
unsigned long lastLogTime = millis();
#ifdef MQTT
unsigned long lastMQTTTime = millis();
#endif
unsigned long custom_timer = millis();
unsigned long custom_timer_compare = 0;
float custom_floats[20] = { 0 };
long custom_longs[20] = { 0 };

#ifdef RGT_EMULATOR
byte newMinuteValue = 99;
#endif

#ifdef BUTTONS
volatile byte PressedButtons = 0;
#define TWW_PUSH_BUTTON_PRESSED 1
#define ROOM1_PRESENCE_BUTTON_PRESSED 2
#define ROOM2_PRESENCE_BUTTON_PRESSED 4
#define ROOM3_PRESENCE_BUTTON_PRESSED 8
#endif

static const int numLogValues = sizeof(log_parameters) / sizeof(log_parameters[0]);
static const int numCustomFloats = sizeof(custom_floats) / sizeof(custom_floats[0]);
static const int numCustomLongs = sizeof(custom_longs) / sizeof(custom_longs[0]);

#ifdef AVERAGES
static const int numAverages = (sizeof(avg_parameters) / sizeof(avg_parameters[0]));
float avgValues_Old[numAverages] = {0};
float avgValues[numAverages] = {0};
float avgValues_Current[numAverages] = {0};
int avgCounter = 1;
#endif
int loopCount = 0;

#if defined(JSONCONFIG) || defined(WEBCONFIG)
byte config_level = 0;
#endif

struct decodedTelegram_t {

int cat;
float prognr;
uint_farptr_t catdescaddr;
uint_farptr_t prognrdescaddr;
uint_farptr_t enumdescaddr;
uint_farptr_t enumstr;
uint_farptr_t progtypedescaddr;
uint_farptr_t data_type_descaddr;
uint16_t enumstr_len;
uint16_t error;
uint8_t msg_type;
uint8_t tlg_addr;
uint8_t readwrite;
uint8_t isswitch;
uint8_t type;
uint8_t data_type;
uint8_t precision;
uint8_t enable_byte;
uint8_t payload_length;
uint8_t sensorid;

float operand;
char value[64];
char unit[32];
char *telegramDump;
} decodedTelegram;

uint8_t my_dev_fam = DEV_FAM(DEV_NONE);
uint8_t my_dev_var = DEV_VAR(DEV_NONE);
uint32_t my_dev_id = 0;
uint8_t default_flag = DEFAULT_FLAG;



int brenner_stufe = 0;
unsigned long brenner_start = 0;
unsigned long brenner_start_2 = 0;
unsigned long brenner_duration= 0;
unsigned long brenner_duration_2= 0;
unsigned long brenner_count = 0;
unsigned long brenner_count_2 = 0;
unsigned long TWW_start = 0;
unsigned long TWW_duration= 0;
unsigned long TWW_count = 0;


uint8_t msg_cycle = 0;
uint8_t saved_msg_cycle = 0;
int16_t pps_values[PPS_ANZ] = { 0 };
uint8_t allow_write_pps_values[PPS_ANZ/8 + 1] = { 0 };
bool pps_time_received = false;
bool pps_time_set = false;
bool pps_wday_set = false;
uint8_t current_switchday = 0;
unsigned long pps_mcba_timer = millis();

#include "BSB_LAN_EEPROMconfig.h"

static uint16_t baseConfigAddrInEEPROM = 0;
void mqtt_callback(char* topic, byte* payload, unsigned int length);

#ifdef BUTTONS
void interruptHandlerTWWPush();
void interruptHandlerPresenceROOM1();
void interruptHandlerPresenceROOM2();
void interruptHandlerPresenceROOM3();
void ShowSockStatus();
void checkSockStatus();
int char2int(char input);
parameter parsingStringToParameter(char *data);
void printHTTPheader(uint16_t code, int mimetype, bool addcharset, bool isGzip, bool isDownload, long cachingTime);
int recognize_mime(char *str);
uint_farptr_t calc_enum_offset(uint_farptr_t enum_addr, uint16_t enumstr_len, int shift);
inline uint_farptr_t calc_enum_offset(uint_farptr_t enum_addr, uint16_t enumstr_len);
void setBusType();
int dayofweek(uint8_t day, uint8_t month, uint16_t year);
void listEnumValues(uint_farptr_t enumstr, uint16_t enumstr_len, const char *prefix, const char *delimiter, const char *alt_delimiter, const char *suffix, const char *string_delimiter, uint16_t value, uint8_t print_mode, bool canBeDisabled);
void printcantalloc(void);
void set_temp_destination(short destAddr);
void return_to_default_destination(int destAddr);
uint8_t recognizeVirtualFunctionGroup(float nr);
int findLine(float line
           , uint16_t start_idx
           , uint32_t *cmd);
int freeRam ();
void SerialPrintHex(byte val);
void SerialPrintHex32(uint32_t val);
void SerialPrintData(byte* msg);
void SerialPrintRAW(byte* msg, byte len);
void EEPROM_dump();
void switchPresenceState(uint16_t set_mode, uint16_t current_state);
bool programIsreadOnly(uint8_t param_len);
void loadPrognrElementsFromTable(float nr, int i);
void resetDecodedTelegram();
void SerialPrintAddr(byte addr);
int bin2hex(char *toBuffer, byte *fromAddr, int len, char delimiter);
void SerialPrintType(byte type);
void prepareToPrintHumanReadableTelegram(byte *msg, byte data_len, int shift);
int undefinedValueToBuffer(char *p);
void printPassKey(void);
void printyesno(bool i);
void remove_char(char* str, char c);
void resetDurations();
void UpdateMaxDeviceList();
void print_bus_send_failed(void);
void printPStr(uint_farptr_t outstr, uint16_t outstr_len);
void init_ota_update();
void printDeviceArchToWebClient();
void generateConfigPage(void);
void startBLEScan();
uint8_t takeNewConfigValueFromUI_andWriteToRAM(int option_id, char *buf);
bool SaveConfigFromRAMtoEEPROM();
int returnENUMID4ConfigOption(uint8_t id);
void printMAClistToWebClient(byte *variable, uint16_t size);
void printDHTlistToWebClient(byte *variable, uint16_t size);
void printProglistToWebClient(parameter *variable, uint16_t size);
void printMAXlistToWebClient(byte *variable, uint16_t size);
void applyingConfig();
void printConfigWebPossibleValues(int i, uint16_t temp_value, bool printCurrentSelectionOnly);
void generateWebConfigPage(bool printOnly);
void printConfigJSONPossibleValues(int i, bool its_a_bits_enum);
void generateJSONwithConfig();
void LogTelegram(byte* msg);
void LogTelegram(byte* msg);
int set(float line
      , const char *val
      , bool setcmd);
int queryDefaultValue(float line, byte *msg, byte *tx_msg);
const char* printError(uint16_t error);
void query_program_and_print_result(int line, const char* prefix, const char* suffix);
void query_printHTML();
void tcaselect(uint8_t i);
void queryVirtualPrognr(float line, int table_line);
void query(float line);
void query(float line_start
          , float line_end
          , bool no_print);
void GetDevId();
void SetDevId();
void SetDateTime();
uint16_t setPPS(uint8_t pps_index, int16_t value);
void transmitFile(File dataFile);
void resetBoard();
void resetAverageCalculation();
void readFirstAndPreviousDatalogDateFromFile();
void createDatalogIndexFile();
bool createdatalogFileAndWriteHeader();
void connectToMaxCul();
void clearEEPROM(void);
void internalLEDBlinking(uint16_t period, uint16_t count);
String scanAndConnectToStrongestNetwork();
void printWifiStatus();
void dateTime(uint16_t* date, uint16_t* time);
void setup();
#line 921 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
void interruptHandlerTWWPush() {
  PressedButtons |= TWW_PUSH_BUTTON_PRESSED;
}
void interruptHandlerPresenceROOM1() {
  PressedButtons |= ROOM1_PRESENCE_BUTTON_PRESSED;
}
void interruptHandlerPresenceROOM2() {
  PressedButtons |= ROOM2_PRESENCE_BUTTON_PRESSED;
}
void interruptHandlerPresenceROOM3() {
  PressedButtons |= ROOM3_PRESENCE_BUTTON_PRESSED;
}
#endif

#include "include/eeprom_io.h"

#include "json-api-version.h"
#include "bsb-version.h"
const char BSB_VERSION[] PROGMEM = MAJOR "." MINOR "." PATCH "-" COMPILETIME;

#ifdef CUSTOM_COMMANDS
#include "BSB_LAN_custom_global.h"
#endif





#ifdef WATCH_SOCKETS

byte socketStat[MAX_SOCK_NUM];
unsigned long connectTime[MAX_SOCK_NUM];

#include <utility/w5100.h>


void ShowSockStatus()
{
  for (int i = 0; i < MAX_SOCK_NUM; i++) {
    printFmtToDebug(PSTR("Socket#%d"), i);
    SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
    uint8_t s = W5100.readSnSR(i);
    SPI.endTransaction();
    socketStat[i] = s;
    printFmtToDebug(PSTR(":0x%02X %d D:"), s, W5100.readSnPORT(i));
    uint8_t dip[4];
    SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
    W5100.readSnDIPR(i, dip);
    SPI.endTransaction();
    SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
    printFmtToDebug(PSTR("%d.%d.%d.%d(%d)\r\n"), dip[0], dip[1], dip[2], dip[3], W5100.readSnDPORT(i));
    SPI.endTransaction();
  }
}

void checkSockStatus()
{
  unsigned long thisTime = millis();

  for (int i = 0; i < MAX_SOCK_NUM; i++) {
    SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
    uint8_t s = W5100.readSnSR(i);
    SPI.endTransaction();

    if ((s == 0x14) || (s == 0x1C)) {
        if (thisTime - connectTime[i] > 30000UL) {
          printFmtToDebug(PSTR("\r\nSocket frozen: %d\r\n"), i);
          SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
          W5100.execCmdSn(s, Sock_DISCON);
          SPI.endTransaction();

          printlnToDebug(PSTR("Socket freed."));
          ShowSockStatus();
        }
    } else {
      connectTime[i] = thisTime;
    }
    SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
    socketStat[i] = W5100.readSnSR(i);
    SPI.endTransaction();
  }
}

#endif

int char2int(char input)
{
  if(input >= '0' && input <= '9')
    return input - '0';
  input |= 0x20;
  if(input >= 'a' && input <= 'f')
    return input - 'a' + 10;
  return 0;
}


#include "include/print2debug.h"

#include "include/print2webclient.h"

#if defined(BLE_SENSORS) && defined(ESP32)
#include "include/BLESensors.h"
#endif


parameter parsingStringToParameter(char *data){
  parameter param;
  param.number = atof(data);
  param.dest_addr = -1;
  char* token = strchr(data, '!');
  if (token != NULL) {
    token++;
    if (token[0] > 0) {
      param.dest_addr = atoi(token);
    }
  }
  return param;
}

void printHTTPheader(uint16_t code, int mimetype, bool addcharset, bool isGzip, bool isDownload, long cachingTime) {
  const char *getfarstrings;
  long autoDetectCachingTime = 590800;
  printFmtToWebClient(PSTR("HTTP/1.1 %d "), code);
  switch (code) {
    case HTTP_OK: printToWebClient(PSTR("OK")); break;
    case HTTP_NOT_MODIFIED: printToWebClient(PSTR("Not Modified")); break;
    case HTTP_AUTH_REQUIRED: printToWebClient(PSTR("Authorization Required")); break;
    case HTTP_NOT_FOUND: printToWebClient(PSTR("Not Found")); break;
    default: break;
  }
  printToWebClient(PSTR("\r\nContent-Type: "));
  switch (mimetype) {
    case MIME_TYPE_TEXT_HTML: getfarstrings = PSTR("text/html"); break;
    case MIME_TYPE_TEXT_CSS: getfarstrings = PSTR("text/css"); break;
    case MIME_TYPE_APP_JS: getfarstrings = PSTR("application/x-javascript"); break;
    case MIME_TYPE_APP_XML: getfarstrings = PSTR("application/xml"); autoDetectCachingTime = HTTP_DO_NOT_CACHE; break;

    case MIME_TYPE_APP_JSON: getfarstrings = PSTR("application/json"); autoDetectCachingTime = HTTP_DO_NOT_CACHE; break;
    case MIME_TYPE_TEXT_PLAIN: getfarstrings = PSTR("text/plain"); autoDetectCachingTime = 60; break;
    case MIME_TYPE_IMAGE_JPEG: getfarstrings = PSTR("image/jpeg"); break;
    case MIME_TYPE_IMAGE_GIF: getfarstrings = PSTR("image/gif"); break;
    case MIME_TYPE_IMAGE_SVG: getfarstrings = PSTR("image/svg+xml"); break;
    case MIME_TYPE_IMAGE_PNG: getfarstrings = PSTR("image/png"); break;
    case MIME_TYPE_IMAGE_ICON: getfarstrings = PSTR("image/x-icon"); autoDetectCachingTime = 2592000; break;
    case MIME_TYPE_APP_GZ: getfarstrings = PSTR("application/x-gzip"); break;
    case MIME_TYPE_FORCE_DOWNLOAD: getfarstrings = PSTR("application/force-download"); break;
    case MIME_TYPE_TEXT_TEXT:
    default: getfarstrings = PSTR("text"); autoDetectCachingTime = 60; break;
  }
  printToWebClient(getfarstrings);
  if (cachingTime == HTTP_AUTO_CACHE_AGE)cachingTime = autoDetectCachingTime;
  if (addcharset)printToWebClient(PSTR("; charset=utf-8"));
  printToWebClient(PSTR("\r\n"));
  if (isGzip) printToWebClient(PSTR("Content-Encoding: gzip\r\n"));
  printToWebClient(PSTR("Cache-Control: "));
  if (cachingTime > 0) {

    printFmtToWebClient(PSTR("max-age=%ld, public"), cachingTime);
  } else {
    printToWebClient(PSTR("no-cache, no-store, must-revalidate\r\nPragma: no-cache\r\nExpires: 0"));
  }
  printToWebClient(PSTR("\r\n"));
  if (isDownload) {
    printToWebClient(PSTR("Content-Disposition: attachment; filename=\"BSB-LAN-"));
    printFmtToWebClient("%03u-%03u-%u.txt\"\r\n", my_dev_fam, my_dev_var, my_dev_id);
  }
}


int recognize_mime(char *str) {

  char mimebuf[32];
  int i = 0;
  int mimetype = 0;
  if (!str) return mimetype;

  strncpy(mimebuf, str, sizeof(mimebuf));

  while (mimebuf[i]) {
    mimebuf[i] |= 0x20;
    i++;
  }

  if (!strcmp_P(mimebuf, PSTR("html")) || !strcmp_P(mimebuf, PSTR("htm"))) mimetype = MIME_TYPE_TEXT_HTML;
  else if (!strcmp_P(mimebuf, PSTR("css"))) mimetype = MIME_TYPE_TEXT_CSS;
  else if (!strcmp_P(mimebuf, PSTR("js"))) mimetype = MIME_TYPE_APP_JS;
  else if (!strcmp_P(mimebuf, PSTR("xml"))) mimetype = MIME_TYPE_APP_XML;
  else if (!strcmp_P(mimebuf, PSTR("txt"))) mimetype = MIME_TYPE_TEXT_TEXT;
  else if (!strcmp_P(mimebuf, PSTR("jpg"))) mimetype = MIME_TYPE_IMAGE_JPEG;
  else if (!strcmp_P(mimebuf, PSTR("gif"))) mimetype = MIME_TYPE_IMAGE_GIF;
  else if (!strcmp_P(mimebuf, PSTR("svg"))) mimetype = MIME_TYPE_IMAGE_SVG;
  else if (!strcmp_P(mimebuf, PSTR("png"))) mimetype = MIME_TYPE_IMAGE_PNG;
  else if (!strcmp_P(mimebuf, PSTR("ico"))) mimetype = MIME_TYPE_IMAGE_ICON;
  else if (!strcmp_P(mimebuf, PSTR("gz"))) mimetype = MIME_TYPE_APP_GZ;


  return mimetype;
  }
# 1135 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
uint_farptr_t calc_enum_offset(uint_farptr_t enum_addr, uint16_t enumstr_len, int shift) {
  return enum_addr;
}

inline uint_farptr_t calc_enum_offset(uint_farptr_t enum_addr, uint16_t enumstr_len) {
  return calc_enum_offset(enum_addr, enumstr_len, 0);
}

void setBusType() {
  switch (bus_type) {
    default:
    case BUS_BSB:
    case BUS_LPB: bus->setBusType(bus_type, own_address, dest_address); break;
    case BUS_PPS: bus->setBusType(bus_type, pps_write); break;
  }
}
# 1170 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
int dayofweek(uint8_t day, uint8_t month, uint16_t year)
{


   if (month < 3) {
      month += 12;
      year--;
   }
   return (((13*month+3)/5 + day + year + year/4 - year/100 + year/400) % 7) + 1;
}
# 1203 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
void listEnumValues(uint_farptr_t enumstr, uint16_t enumstr_len, const char *prefix, const char *delimiter, const char *alt_delimiter, const char *suffix, const char *string_delimiter, uint16_t value, uint8_t print_mode, bool canBeDisabled) {
  uint16_t val = 0;
  uint16_t c=0;
  uint8_t bitmask=0;
  bool isFirst = true;
  if (decodedTelegram.type == VT_CUSTOM_BIT) c++;

  while (c + 2 < enumstr_len) {
    if ((byte)(pgm_read_byte_far(enumstr+c+2))==' ') {
      val = uint16_t(pgm_read_byte_far(enumstr+c+1));
      if (decodedTelegram.type != VT_CUSTOM_ENUM) val |= uint16_t((pgm_read_byte_far(enumstr+c) << 8));
      if (print_mode & PRINT_ENUM_AS_DT_BITS) {
        bitmask = val & 0xff;
        val = val >> 8 & 0xff;
      }
      c++;
    } else if ((byte)(pgm_read_byte_far(enumstr+c+1))==' ') {
      val=uint16_t(pgm_read_byte_far(enumstr+c));
    }

    c+=2;
    if ((print_mode & PRINT_ONLY_VALUE_LINE) && val != value) {
      while(c < enumstr_len){
        if ((byte)(pgm_read_byte_far(enumstr+c)) == '\0'){
          break;
        }
        c++;
      }
      continue;
    }
    if (isFirst) {isFirst = false;} else {if (string_delimiter) printToWebClient(string_delimiter);}
    if (prefix) printToWebClient(prefix);
    uint_farptr_t descAddr;
    if (canBeDisabled) {
      val = 65535;
      descAddr = STR_DISABLED;
    } else {
      descAddr = enumstr + c;
    }
    if (print_mode & PRINT_DESCRIPTION && print_mode & PRINT_DESCRIPTION_FIRST) {
      c += printToWebClient(descAddr) + 1;

      if (alt_delimiter && ((value == val && !(print_mode & PRINT_ENUM_AS_DT_BITS)) || ((value & bitmask) == (val & bitmask) && (print_mode & PRINT_ENUM_AS_DT_BITS)))) {
        printToWebClient(alt_delimiter);
      } else {
        if (delimiter) printToWebClient(delimiter);
      }
    }
    if (print_mode & PRINT_VALUE) printFmtToWebClient(PSTR("%u"), val);
    if (print_mode & PRINT_DESCRIPTION && print_mode & PRINT_VALUE_FIRST) {

      if (alt_delimiter && ((value == val && !(print_mode & PRINT_ENUM_AS_DT_BITS)) || ((value & bitmask) == (val & bitmask) && (print_mode & PRINT_ENUM_AS_DT_BITS)))) {
        printToWebClient(alt_delimiter);
      } else {
        if (delimiter) printToWebClient(delimiter);
      }
      c += printToWebClient(descAddr) + 1;
    }
    if (suffix) printToWebClient(suffix);
    if (canBeDisabled) {
      canBeDisabled = false;
      c = 0;
    }
  }
}

#include "include/get_cmdtbl_values.h"

void printcantalloc(void) {
  printlnToDebug(PSTR("Can't alloc memory"));
}

void set_temp_destination(short destAddr){
  printFmtToDebug(PSTR("Setting temporary destination to %d\r\n"), destAddr);
  bus->setBusType(bus->getBusType(), bus->getBusAddr(), destAddr);
  GetDevId();
}


void return_to_default_destination(int destAddr){
  printFmtToDebug(PSTR("Returning to default destination %d\r\n"), destAddr);
  bus->setBusType(bus->getBusType(), bus->getBusAddr(), destAddr);
}
# 1300 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
uint8_t recognizeVirtualFunctionGroup(float nr) {
  if (nr >= BSP_INTERNAL && nr < BSP_INTERNAL+7) { return 1;}
#ifdef AVERAGES
  else if (nr >= BSP_AVERAGES && nr < BSP_AVERAGES + numAverages) {return 2;}
#endif
#ifdef DHT_BUS
  else if (nr >= BSP_DHT22 && nr < BSP_DHT22 + sizeof(DHT_Pins) / sizeof(DHT_Pins[0])) {return 3;}
#endif
#ifdef BME280
  else if (nr >= BSP_BME280 && nr < BSP_BME280 + BME_Sensors) {return 8;}
#endif
#ifdef ONE_WIRE_BUS
  else if (nr >= BSP_ONEWIRE && nr < BSP_ONEWIRE + (uint16_t)numSensors) {return 4;}
#endif
  else if (nr >= BSP_MAX && nr < BSP_MAX + MAX_CUL_DEVICES) {return 5;}
  else if (nr >= BSP_FLOAT && nr < BSP_FLOAT + numCustomFloats) {return 6;}
  else if (nr >= BSP_LONG && nr < BSP_LONG + numCustomLongs) {return 7;}
  #if defined(BLE_SENSORS) && defined(ESP32)
  else if (nr >= BSP_BLE && nr < BSP_BLE + BLESensors_num_of_sensors) {
    return 9;
  }
  else if (nr >= BSP_MQTT_TEMP && nr < BSP_MQTT_TEMP + numMQTTTemps) {return 10;}
#endif
  return 0;
}
# 1343 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
int findLine(float line
           , uint16_t start_idx
           , uint32_t *cmd)
{
  uint8_t found = 0;
  int i = -1;
  int save_i = 0;
  uint32_t c, save_c = 0;
  float l;
#ifdef DEVELOPER_DEBUG
  printFmtToDebug(PSTR("line = %.1f\r\n"), line);
#endif


  if (line >= BSP_INTERNAL && line < BSP_END) {
    switch (recognizeVirtualFunctionGroup(line)) {
      case 1: break;
      case 2: line = avg_parameters[(((uint16_t)line) - BSP_AVERAGES)].number; if (line == 0) return -1; else break;
      case 3: {
        if (DHT_Pins[(((uint16_t)line) - BSP_DHT22)] == 0) {
          return -1;
        } else {
#if defined(__SAM3X8E__)
          double intpart;
#else
          float intpart;
#endif
          line = BSP_DHT22 + modf(line, &intpart);
        }
        break;
      }
      case 4: {
#if defined(__SAM3X8E__)
        double intpart;
#else
        float intpart;
#endif
        line = BSP_ONEWIRE + modf(line, &intpart);
        break;
      }
      case 5:{
        if (max_device_list[((uint16_t)line) - BSP_MAX][0] == 0) {
          return -1;
        } else {
#if defined(__SAM3X8E__)
          double intpart;
#else
          float intpart;
#endif
          line = BSP_MAX + modf(line, &intpart);
        }
        break;
      }
      case 6: line = BSP_FLOAT; break;
      case 7: line = BSP_LONG; break;
      case 8: {
#ifdef BME280
        if ((int)roundf(line - BSP_BME280) < BME_Sensors) {
#if defined(__SAM3X8E__)
          double intpart;
#else
          float intpart;
#endif
          line = BSP_BME280 + modf(line, &intpart);
        } else {
          return -1;
        }
#else
        return -1;
#endif
        break;
      }
       case 9: {
#if defined(BLE_SENSORS) && defined(ESP32)
        if ((int)roundf(line - BSP_BLE) < BLESensors_num_of_sensors) {
          float intpart;
          line = BSP_BLE + modf(line, &intpart);
        } else {
          return -1;
        }
#else
        return -1;
#endif
        break;
      }
      case 10: {
        if ((int)roundf(line - BSP_MQTT_TEMP) < numMQTTTemps) {
          float intpart;
          line = BSP_MQTT_TEMP + modf(line, &intpart);
        } else {
          return -1;
        }
      }
      break;
            default: return -1;
    }
  }



  int left = start_idx;
  int right = (int)(sizeof(cmdtbl)/sizeof(cmdtbl[0]) - 1);
  int mid = 0;
  int line_dd = roundf(line * 10);
  while (!(left >= right))
    {
#ifdef DEVELOPER_DEBUG
    printFmtToDebug(PSTR("get_cmdtbl_line: left = %f, line = %f\r\n"), get_cmdtbl_line(left), line);
#endif
    if (roundf(get_cmdtbl_line(left) * 10) == line_dd){ i = left; break; }
    mid = left + (right - left) / 2;
    int temp_dd = roundf(get_cmdtbl_line(mid) * 10);
#ifdef DEVELOPER_DEBUG
    printFmtToDebug(PSTR("get_cmdtbl_line integer: temp = %d, line = %d\r\n"), temp_dd, line_dd);
    printFmtToDebug(PSTR("get_cmdtbl_line: left = %.1f, mid = %.1f\r\n"), get_cmdtbl_line(left), get_cmdtbl_line(mid));
#endif
    if (temp_dd == line_dd) {
      if (mid == left + 1) {
            i = mid; break;
      } else {
        right = mid + 1;
      }
    } else if (temp_dd > line_dd) {
      right = mid;
    } else {
      left = mid + 1;
    }
#ifdef DEVELOPER_DEBUG
    printFmtToDebug(PSTR("left = %d, mid = %d, right = %d\r\n"), left, mid, right);
#endif
  }
#ifdef DEVELOPER_DEBUG
  printFmtToDebug(PSTR("i = %d\r\n"), i);
#endif
  if (i == -1) return i;

  l = get_cmdtbl_line(i);
  while (l == line) {
    c = get_cmdtbl_cmd(i);
    uint8_t dev_fam = get_cmdtbl_dev_fam(i);
    uint8_t dev_var = get_cmdtbl_dev_var(i);
    uint8_t dev_flags = get_cmdtbl_flags(i);
#ifdef DEVELOPER_DEBUG
    printFmtToDebug(PSTR("l = %.1f, dev_fam = %d,  dev_var = %d, dev_flags = %d\r\n"), l, dev_fam, dev_var, dev_flags);
#endif

    if ((dev_fam == my_dev_fam || dev_fam == DEV_FAM(DEV_ALL)) && (dev_var == my_dev_var || dev_var == DEV_VAR(DEV_ALL))) {
      if (dev_fam == my_dev_fam && dev_var == my_dev_var) {
        if ((dev_flags & FL_NO_CMD) == FL_NO_CMD) {
          while (c==get_cmdtbl_cmd(i)) {
            i++;
          }
          found=0;
          i--;
        } else {
          found=1;
          save_i=i;
          save_c=c;
          break;
        }
      } else if ((!found && dev_fam!=my_dev_fam) || (dev_fam==my_dev_fam)) {
        if ((dev_flags & FL_NO_CMD) == FL_NO_CMD) {
          while (c==get_cmdtbl_cmd(i)) {
            i++;
          }
          found=0;
          i--;
        } else {
          found=1;
          save_i=i;
          save_c=c;
        }
      }
    }
    i++;
    l = get_cmdtbl_line(i);
  }

  if (!found) {
    return -1;
  }
  if (cmd!=NULL) *cmd=save_c;
  return save_i;
}







#if defined(__arm__)
extern "C" char* sbrk(int incr);
#endif
int freeRam () {
#ifdef ESP32
 return (int)ESP.getFreeHeap();
#else
  char top;
  return &top - reinterpret_cast<char*>(sbrk(0));
#endif
}
# 1561 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
void SerialPrintHex(byte val) {
  printFmtToDebug(PSTR("%02X"), val);
}
# 1579 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
void SerialPrintHex32(uint32_t val) {
  printFmtToDebug(PSTR("%08lX"), val);
}
# 1598 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
void SerialPrintData(byte* msg) {

  int data_len=0;
  byte offset = 0;
  byte msg_type = msg[4+(bus->getBusType()*4)];
  if (bus_type != BUS_PPS) {
    if (msg_type >= 0x12) {
      offset = 4;
    }
  }
  switch (bus->getBusType()) {
  case BUS_BSB: data_len=msg[bus->getLen_idx()]-11+offset; break;
  case BUS_LPB: data_len=msg[bus->getLen_idx()]-14+offset; break;
  case BUS_PPS: data_len=9; break;
  }

  SerialPrintRAW(msg + bus->getPl_start() - offset, data_len);
}
# 1634 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
void SerialPrintRAW(byte* msg, byte len) {
  int outBufLen = strlen(outBuf);
  bin2hex(outBuf + outBufLen, msg, len, ' ');
  printToDebug(outBuf + outBufLen);
  outBuf[outBufLen] = 0;
}

void EEPROM_dump() {
  if ((debug_mode == 1 || haveTelnetClient) && EEPROM_ready) {
    printlnToDebug(PSTR("EEPROM dump:"));
    for (uint16_t x=0; x<EEPROM_SIZE; x++) {
      printFmtToDebug(PSTR("%02X "), EEPROM.read(x));
    }
  }
}

#ifdef BUTTONS
void switchPresenceState(uint16_t set_mode, uint16_t current_state) {



  int state = 0;
  char buf[9];
  unsigned int i0, i1;
  query(current_state);
  strcpy_P(buf, PSTR("%02x%02x"));
  if (2 != sscanf(decodedTelegram.value, buf, &i0, &i1)) return;
  if (i0 != 0x01) return;
  switch (i1) {
    case 0x01: state = 0x02; break;
    case 0x02: state = 0x01; break;
    case 0x03: state = 0x02; break;
    case 0x04: state = 0x01; break;
    default: return;
  }
  sprintf_P(buf, PSTR("%d"), state);
  set(set_mode, buf, true);
}
#endif

bool programIsreadOnly(uint8_t param_len) {
  if ((default_flag & FL_SW_CTL_RONLY) == FL_SW_CTL_RONLY) {
    switch (programWriteMode) {
      case 0: return true;
      case 1: if ((param_len & FL_OEM) == FL_OEM || ((param_len & FL_RONLY) == FL_RONLY && (default_flag & FL_RONLY) != FL_RONLY)) return true; else return false;
      case 2: if ((param_len & FL_RONLY) == FL_RONLY) return true; else return false;
    }
  } else {
    if ((param_len & FL_RONLY) == FL_RONLY) return true;
  }
  return false;
}
# 1702 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
void loadPrognrElementsFromTable(float nr, int i) {
  if (i<0) i = findLine(19999,0,NULL);
  decodedTelegram.prognrdescaddr = get_cmdtbl_desc(i);
  decodedTelegram.type = get_cmdtbl_type(i);
  decodedTelegram.cat=get_cmdtbl_category(i);
  decodedTelegram.enumstr_len=get_cmdtbl_enumstr_len(i);

  decodedTelegram.enumstr = calc_enum_offset(get_cmdtbl_enumstr(i), decodedTelegram.enumstr_len, decodedTelegram.type == VT_CUSTOM_BIT?1:0);
  uint8_t flags=get_cmdtbl_flags(i);
  if (programIsreadOnly(flags)) {
    decodedTelegram.readwrite = FL_RONLY;
  } else if ((flags & FL_WONLY) == FL_WONLY) {
    decodedTelegram.readwrite = FL_WONLY;
    decodedTelegram.prognr = nr;
  } else {
    decodedTelegram.readwrite = FL_WRITEABLE;
  }
  decodedTelegram.data_type=optbl[decodedTelegram.type].data_type;
  decodedTelegram.operand=optbl[decodedTelegram.type].operand;
  decodedTelegram.precision=optbl[decodedTelegram.type].precision;
  decodedTelegram.enable_byte=optbl[decodedTelegram.type].enable_byte;
  decodedTelegram.payload_length=optbl[decodedTelegram.type].payload_length;
  memcpy(decodedTelegram.unit, optbl[decodedTelegram.type].unit, optbl[decodedTelegram.type].unit_len);
  decodedTelegram.progtypedescaddr = optbl[decodedTelegram.type].type_text;
  decodedTelegram.data_type_descaddr = dt_types_text[decodedTelegram.data_type].type_text;

  if (decodedTelegram.type == VT_BINARY_ENUM || decodedTelegram.type == VT_ONOFF || decodedTelegram.type == VT_YESNO|| decodedTelegram.type == VT_CLOSEDOPEN || decodedTelegram.type == VT_VOLTAGEONOFF) {
    decodedTelegram.isswitch = 1;
  } else {
    decodedTelegram.isswitch = 0;
  }

  decodedTelegram.sensorid = 0;
  if (nr >= BSP_INTERNAL) {
    decodedTelegram.prognr = nr;
    switch (recognizeVirtualFunctionGroup(nr)) {
      case 1: break;
      case 2: decodedTelegram.cat = CAT_USERSENSORS; decodedTelegram.readwrite = FL_RONLY; break;
      case 3: decodedTelegram.sensorid = nr - BSP_DHT22 + 1; break;
      case 4: decodedTelegram.sensorid = nr - BSP_ONEWIRE + 1; break;
      case 5: decodedTelegram.sensorid = nr - BSP_MAX + 1; break;
      case 6: decodedTelegram.sensorid = nr - BSP_FLOAT + 1; break;
      case 7: decodedTelegram.sensorid = nr - BSP_LONG + 1; break;
      case 8: decodedTelegram.sensorid = nr - BSP_BME280 + 1; break;
      case 9: decodedTelegram.sensorid = nr - BSP_BLE + 1; break;
      case 10: decodedTelegram.sensorid = nr - BSP_MQTT_TEMP + 1; break;
    }
  }
}


void resetDecodedTelegram() {
  decodedTelegram.prognr = -1;
  decodedTelegram.cat = -1;
  decodedTelegram.catdescaddr = 0;
  decodedTelegram.prognrdescaddr = 0;
  decodedTelegram.enumdescaddr = 0;
  decodedTelegram.progtypedescaddr = 0;
  decodedTelegram.type = 0;
  decodedTelegram.data_type = 0;
  decodedTelegram.data_type_descaddr = 0;

  decodedTelegram.precision = 1;
  decodedTelegram.enable_byte = 0;
  decodedTelegram.payload_length = 0;
  decodedTelegram.error = 0;
  decodedTelegram.readwrite = FL_WRITEABLE;
  decodedTelegram.isswitch = 0;
  decodedTelegram.value[0] = 0;
  decodedTelegram.unit[0] = 0;
  decodedTelegram.enumstr_len = 0;
  decodedTelegram.enumstr = 0;
  decodedTelegram.msg_type = 0;
  decodedTelegram.tlg_addr = 0;
  decodedTelegram.operand = 1;
  decodedTelegram.sensorid = 0;
  if (decodedTelegram.telegramDump) {free(decodedTelegram.telegramDump); decodedTelegram.telegramDump = NULL;}
}
# 1797 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
char *TranslateAddr(byte addr, char *device) {
  const char *p = NULL;
  switch (addr & 0x7F) {
    case ADDR_HEIZ: p = PSTR("HEIZ"); break;
    case ADDR_EM1: p = PSTR("EM1"); break;
    case ADDR_EM2: p = PSTR("EM2"); break;
    case ADDR_RGT1: p = PSTR("RGT1"); break;
    case ADDR_RGT2: p = PSTR("RGT2"); break;
    case ADDR_RGT3: p = PSTR("RGT3"); break;
    case ADDR_SRVC: p = PSTR("SRVC"); break;
    case ADDR_DSP1: p = PSTR("DSP1"); break;
    case ADDR_DSP2: p = PSTR("DSP2"); break;
    case ADDR_DSP3: p = PSTR("DSP3"); break;
    case ADDR_LAN: p = PSTR("LAN"); break;
    case ADDR_OZW: p = PSTR("OZW"); break;
    case ADDR_FUNK: p = PSTR("FUNK"); break;
    case ADDR_RC: p = PSTR("REMO"); break;
    case ADDR_ALL: p = PSTR("ALL "); break;
    default: bin2hex(device, &addr, 1, 0); break;
  }
  if (p) strcpy_P(device, p);
  device[4] = 0;
  return device;
}
# 1839 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
void SerialPrintAddr(byte addr) {
  char device[5];
  printToDebug(TranslateAddr(addr, device));
}
# 1860 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
int bin2hex(char *toBuffer, byte *fromAddr, int len, char delimiter){
  int resultLen = 0;
  bool isNotFirst = false;
  for (int i = 0; i < len; i++) {
    if(isNotFirst) {
      if(delimiter != 0) {
        toBuffer[resultLen] = delimiter;
        resultLen++;
      }
    } else {
      isNotFirst = true;
    }
    resultLen += sprintf_P(toBuffer + resultLen, PSTR("%02X"), fromAddr[i]);
  }
  return resultLen;
}
# 1889 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
char *TranslateType(byte type, char *mtype) {
  const char *p = NULL;
  if (type > 0x20) {
    type = type & 0x0F;
  }
  switch (type) {
    case TYPE_QINF: p = PSTR("QINF"); break;
    case TYPE_INF: p = PSTR("INF"); break;
    case TYPE_SET: p = PSTR("SET"); break;
    case TYPE_ACK: p = PSTR("ACK"); break;
    case TYPE_NACK: p = PSTR("NACK"); break;
    case TYPE_QUR: p = PSTR("QUR"); break;
    case TYPE_ANS: p = PSTR("ANS"); break;
    case TYPE_QRV: p = PSTR("QRV"); break;
    case TYPE_ARV: p = PSTR("ARV"); break;
    case TYPE_ERR: p = PSTR("ERR"); break;
    case TYPE_QRE: p = PSTR("QRE"); break;
    case TYPE_IQ1: p = PSTR("IQ1"); break;
    case TYPE_IA1: p = PSTR("IA1"); break;
    case TYPE_IQ2: p = PSTR("IQ2"); break;
    case TYPE_IA2: p = PSTR("IA2"); break;

    default: bin2hex(mtype, &type, 1, 0); break;
  }
  if (p) strcpy_P(mtype, p);
  mtype[4] = 0;
  return mtype;
}
# 1930 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
void SerialPrintType(byte type) {
  char device[5];
  printToDebug(TranslateType(type, device));
}
# 1947 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
void prepareToPrintHumanReadableTelegram(byte *msg, byte data_len, int shift) {
  SerialPrintData(msg);
  decodedTelegram.telegramDump = (char *) malloc(data_len * 2 + 1);
  if (decodedTelegram.telegramDump) {
    bin2hex(decodedTelegram.telegramDump, msg + shift, data_len, 0);
  } else {
    printcantalloc();
  }
}
# 1969 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
int undefinedValueToBuffer(char *p) {
  strcpy(p, STR_DISABLED);
  return 3;
}

void printPassKey(void) {
  if (PASSKEY[0]) {
    printToWebClient(PASSKEY);
    printToWebClient(PSTR("/"));
  }
}
# 1992 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
void printyesno(bool i) {
  if (i) {
    printToWebClient(PSTR(MENU_TEXT_YES "<BR>"));
  } else {
    printToWebClient(PSTR(MENU_TEXT_NO "<BR>"));
  }
  printToWebClient(PSTR("\r\n"));
}
# 2013 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
 void printDebugValueAndUnit(char *p1, char *p2) {
   printToDebug(p1);
   if (p2[0] != 0) {
     printFmtToDebug(PSTR(" %s"), p2);
   }
}
# 2033 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
void remove_char(char* str, char c) {
  char *pr = str, *pw = str;
  while (*pr) {
    *pw = *pr++;
    pw += (*pw != c);
  }
  *pw = '\0';
}

void resetDurations() {
  brenner_duration=0;
  brenner_count=0;
  brenner_duration_2=0;
  brenner_count_2=0;
  TWW_duration=0;
  TWW_count=0;
}

#include "include/print_telegram.h"
#include "include/mqtt_handler.h"
# 2068 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
#ifdef MAX_CUL

void UpdateMaxDeviceList() {
  char max_id_eeprom[sizeof(max_device_list[0])] = { 0 };
  int32_t max_addr = 0;
  for (uint16_t z = 0; z < MAX_CUL_DEVICES; z++) {
    max_devices[z] = 0;
  }

  for (uint16_t z = 0; z < MAX_CUL_DEVICES; z++) {
    if (EEPROM_ready) {
      for (uint16_t i = 0; i < sizeof(max_id_eeprom); i++) {
        max_id_eeprom[i] = EEPROM.read(getEEPROMaddress(CF_MAX_DEVICES) + sizeof(max_id_eeprom) * z + i);
      }
    }
    for (uint16_t x = 0; x < MAX_CUL_DEVICES; x++) {
      if (!memcmp(max_device_list[x], max_id_eeprom, sizeof(max_id_eeprom))) {
        if (EEPROM_ready) {
          for (uint16_t i = 0; i < sizeof(max_addr); i++) {
           ((char *)&max_addr)[i] = EEPROM.read(getEEPROMaddress(CF_MAX_DEVADDR) + sizeof(max_devices[0]) * z + i);
          }
          max_devices[x] = max_addr;
          printFmtToDebug(PSTR("Adding known Max ID to list: %08lX\r\n"), max_devices[x]);
        }
        break;
      }
    }
  }
  writeToEEPROM(CF_MAX_DEVICES);
  writeToEEPROM(CF_MAX_DEVADDR);
  }
#endif

void print_bus_send_failed(void) {
  printlnToDebug(PSTR("bus send failed"));
}

void printPStr(uint_farptr_t outstr, uint16_t outstr_len) {
  for (uint16_t x=0;x<outstr_len-1;x++) {
    bigBuff[bigBuffPos] = pgm_read_byte_far(outstr+x);
    bigBuffPos++;
    if (bigBuffPos >= OUTBUF_USEFUL_LEN + OUTBUF_LEN) {
      flushToWebClient();
    }
  }


}

#include "include/print_webpage.h"

#if defined(ESP32) && defined(ENABLE_ESP32_OTA)
void init_ota_update(){
  if(enable_ota_update) {
    update_server.on("/", HTTP_GET, []() {
      char temp_user_pass[64] = { 0 };
      strncpy(temp_user_pass, USER_PASS, 64);
      if (USER_PASS[0]) {
        if (!update_server.authenticate(strtok(temp_user_pass,":"),strtok(NULL,":"))) {
          return update_server.requestAuthentication();
        }
      }
      update_server.sendHeader("Connection", "close");
      update_server.send(200, "text/html", serverIndex);
    });
    update_server.on("/update", HTTP_POST, []() {
      char temp_user_pass[64] = { 0 };
      strncpy(temp_user_pass, USER_PASS, 64);
      if (USER_PASS[0]) {
        if (!update_server.authenticate(strtok(temp_user_pass,":"),strtok(NULL,":"))) {
          return update_server.requestAuthentication();
        }
      }
      update_server.sendHeader("Connection", "close");
      update_server.send(200, "text/plain", (Update.hasError()) ? "Failed" : "Success");
      delay(1000);
      ESP.restart();
    }, []() {
      HTTPUpload& upload = update_server.upload();
      if (upload.status == UPLOAD_FILE_START) {
        printlnToDebug(PSTR("Updating ESP32 firmware..."));
        uint32_t maxSketchSpace = 0x140000;
        if (!Update.begin(maxSketchSpace)) {
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) {
          printlnToDebug(PSTR("Update success, rebooting..."));
        } else {
          Update.printError(Serial);
        }
      }
      yield();
    });
    update_server.begin();
    printlnToDebug(PSTR("Update Server started on port 8080."));
  }
}
#endif

char *lookup_descr(float line) {
  int i=findLine(line,0,NULL);
  if (i<0) {
    strcpy_PF(outBuf, get_cmdtbl_desc(findLine(19999,0,NULL)));
  } else {
    strcpy_PF(outBuf, get_cmdtbl_desc(i));
  }
  return outBuf;
}

void printDeviceArchToWebClient(){
  #if defined(ESP32)
    printToWebClient(PSTR("ESP32"));
  #elif defined(__SAM3X8E__)
    printToWebClient(PSTR("Due"));
  #else
    printToWebClient(PSTR("Unknown"));
  #endif
}
# 2205 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
char *GetDateTime(char *date) {
#if defined(ESP32)
  struct tm now;
  getLocalTime(&now,100);
  sprintf_P(date,PSTR("%02d.%02d.%d %02d:%02d:%02d"),
            currentDate.elements.day = now.tm_mday,
            currentDate.elements.month = now.tm_mon + 1,
            currentDate.elements.year = now.tm_year + 1900,
            now.tm_hour,now.tm_min,now.tm_sec);
#else
  sprintf_P(date,PSTR("%02d.%02d.%d %02d:%02d:%02d"),
            currentDate.elements.day = day(),
            currentDate.elements.month = month(),
            currentDate.elements.year = year(),
            hour(),minute(),second());
#endif
  date[19] = 0;
  return date;
}

void generateConfigPage(void) {
#if !defined(WEBCONFIG)
  printlnToWebClient(PSTR(MENU_TEXT_CFG "<BR>"));
#endif
  printToWebClient(PSTR("<BR>" MENU_TEXT_MCU ": "));
  printDeviceArchToWebClient();
  printToWebClient(PSTR("<BR>\r\n" MENU_TEXT_VER ": "));
  printToWebClient(BSB_VERSION);
  printFmtToWebClient(PSTR("<BR>\r\n" MENU_TEXT_RAM ": %d " MENU_TEXT_BYT "<BR>\r\n"), freeRam());
  unsigned long ms = millis();
  unsigned long s = ms / 1000;
  unsigned long m = s / 60;
  unsigned long h = m / 60;
  unsigned d = h / 24;
  printFmtToWebClient(PSTR(MENU_TEXT_UPT ": %lu\r\nms = %ud+%02lu:%02lu:%02lu.%03lu<BR>\r\n"), ms, d, h%24, m%60, s%60, ms%1000);
  char tmp_date[20];
  printFmtToWebClient(PSTR(ENUM_CAT_00_TEXT ": %s<BR>\r\n"), GetDateTime(tmp_date));
#ifndef WEBCONFIG
  printlnToWebClient(PSTR(MENU_TEXT_BUS ": "));
  int bustype = bus->getBusType();

  switch (bustype) {
    case 0: printToWebClient(PSTR("BSB")); break;
    case 1: printToWebClient(PSTR("LPB")); break;
    case 2: printToWebClient(PSTR("PPS")); break;
  }
  if (bustype != BUS_PPS) {
    printFmtToWebClient(PSTR(" (%d, %d) "), bus->getBusAddr(), bus->getBusDest());

    if ((default_flag & FL_RONLY) == FL_RONLY || ((default_flag & FL_SW_CTL_RONLY) == FL_SW_CTL_RONLY && !programWriteMode)) {
      printToWebClient(PSTR(MENU_TEXT_BRO));
    } else {
      printToWebClient(PSTR(MENU_TEXT_BRW));
    }
  } else {
    if (pps_write == 1) {
      printToWebClient(PSTR(" (" MENU_TEXT_BRW ")"));
    } else {
      printToWebClient(PSTR(" (" MENU_TEXT_BRO ")"));
    }
  }
  printFmtToWebClient(PSTR("<BR>\r\n" MENU_TEXT_MMD ": %d"), monitor);
  printFmtToWebClient(PSTR("<BR>\r\n" MENU_TEXT_VBL ": %d<BR>\r\n"), verbose);

  printToWebClient(PSTR(MENU_TEXT_MAC ": \r\n"));
  bin2hex(outBuf, mac, 6, ':');
  printToWebClient(outBuf);
  outBuf[0] = 0;
  printToWebClient(PSTR("<BR>\r\n"));

  #ifdef DHT_BUS
  printlnToWebClient(PSTR(MENU_TEXT_DHP ": "));
  bool not_first = false;
  int numDHTSensors = sizeof(DHT_Pins) / sizeof(DHT_Pins[0]);
  for (int i=0;i<numDHTSensors;i++) {
    if (DHT_Pins[i]) {
      if (not_first) {
        printToWebClient(PSTR(", "));
      } else {
        not_first = true;
      }
      printFmtToWebClient(PSTR("%d"), DHT_Pins[i]);
    }
  }
  printToWebClient(PSTR("\r\n<BR>\r\n"));
  #endif
  #endif

  #ifdef ONE_WIRE_BUS
  printFmtToWebClient(PSTR(MENU_TEXT_OWP ": \r\n%d, "), One_Wire_Pin);
  printToWebClient(STR_TEXT_SNS);
  printFmtToWebClient(PSTR(": %d\r\n<BR>\r\n"), numSensors);
  #endif

  printToWebClient(PSTR("<BR><BR>\r\n"));


  printToWebClient(PSTR(MENU_TEXT_MOD ": <BR>\r\n"

  #ifdef AVERAGES
  #ifdef ANY_MODULE_COMPILED
  ", "
  #else
  #define ANY_MODULE_COMPILED
  #endif
  "AVERAGES"
  #endif

  #ifdef BME280
  #ifdef ANY_MODULE_COMPILED
  ", "
  #else
  #define ANY_MODULE_COMPILED
  #endif
  "BME280"
  #endif

  #ifdef BUTTONS
  #ifdef ANY_MODULE_COMPILED
  ", "
  #else
  #define ANY_MODULE_COMPILED
  #endif
  "BUTTONS"
  #endif

  #ifdef CONFIG_IN_EEPROM
  #ifdef ANY_MODULE_COMPILED
  ", "
  #else
  #define ANY_MODULE_COMPILED
  #endif
  "CONFIG_IN_EEPROM"
  #endif

  #ifdef CUSTOM_COMMANDS
  #ifdef ANY_MODULE_COMPILED
  ", "
  #else
  #define ANY_MODULE_COMPILED
  #endif
  "CUSTOM_COMMANDS"
  #endif

  #ifdef DEBUG
  #ifdef ANY_MODULE_COMPILED
  ", "
  #else
  #define ANY_MODULE_COMPILED
  #endif
  "DEBUG"
  #endif

  #ifdef DEVELOPER_DEBUG
  #ifdef ANY_MODULE_COMPILED
  ", "
  #else
  #define ANY_MODULE_COMPILED
  #endif
  "DEVELOPER_DEBUG"
  #endif

  #ifdef DHT_BUS
  #ifdef ANY_MODULE_COMPILED
  ", "
  #else
  #define ANY_MODULE_COMPILED
  #endif
  "DHT_BUS"
  #endif

  #ifdef ENABLE_ESP32_OTA
  #ifdef ANY_MODULE_COMPILED
  ", "
  #else
  #define ANY_MODULE_COMPILED
  #endif
  "ENABLE_ESP32_OTA"
  #endif

  #ifdef ESP32_USE_SD
  #ifdef ANY_MODULE_COMPILED
  ", "
  #else
  #define ANY_MODULE_COMPILED
  #endif
  "ESP32_USE_SD"
  #endif

  #ifdef IPWE
  #ifdef ANY_MODULE_COMPILED
  ", "
  #else
  #define ANY_MODULE_COMPILED
  #endif
  "IPWE"
  #endif

  #ifdef JSONCONFIG
  #ifdef ANY_MODULE_COMPILED
  ", "
  #else
  #define ANY_MODULE_COMPILED
  #endif
  "JSONCONFIG"
  #endif

  #ifdef LOGGER
  #ifdef ANY_MODULE_COMPILED
  ", "
  #else
  #define ANY_MODULE_COMPILED
  #endif
  "LOGGER"
  #endif

  #ifdef MAX_CUL
  #ifdef ANY_MODULE_COMPILED
  ", "
  #else
  #define ANY_MODULE_COMPILED
  #endif
  "MAX_CUL"
  #endif

  #ifdef MDNS_SUPPORT
  #ifdef ANY_MODULE_COMPILED
  ", "
  #else
  #define ANY_MODULE_COMPILED
  #endif
  "MDNS_SUPPORT"
  #endif

  #ifdef MQTT
  #ifdef ANY_MODULE_COMPILED
  ", "
  #else
  #define ANY_MODULE_COMPILED
  #endif
  "MQTT"
  #endif

  #ifdef OFF_SITE_LOGGER
  #ifdef ANY_MODULE_COMPILED
  ", "
  #else
  #define ANY_MODULE_COMPILED
  #endif
  "OFF_SITE_LOGGER"
  #endif

  #ifdef ONE_WIRE_BUS
  #ifdef ANY_MODULE_COMPILED
  ", "
  #else
  #define ANY_MODULE_COMPILED
  #endif
  "ONE_WIRE_BUS"
  #endif

  #ifdef RGT_EMULATOR
  #ifdef ANY_MODULE_COMPILED
  ", "
  #else
  #define ANY_MODULE_COMPILED
  #endif
  "RGT_EMULATOR"
  #endif

  #ifdef ROOM_UNIT
  #ifdef ANY_MODULE_COMPILED
  ", "
  #else
  #define ANY_MODULE_COMPILED
  #endif
  "ROOM_UNIT"
  #endif

  #ifdef USE_ADVANCED_PLOT_LOG_FILE
  #ifdef ANY_MODULE_COMPILED
  ", "
  #else
  #define ANY_MODULE_COMPILED
  #endif
  "USE_ADVANCED_PLOT_LOG_FILE"
  #endif

  #ifdef USE_NTP
  #ifdef ANY_MODULE_COMPILED
  ", "
  #else
  #define ANY_MODULE_COMPILED
  #endif
  "USE_NTP"
  #endif

  #ifdef VERSION_CHECK
  #ifdef ANY_MODULE_COMPILED
  ", "
  #else
  #define ANY_MODULE_COMPILED
  #endif
  "VERSION_CHECK"
  #endif

  #ifdef WEBCONFIG
  #ifdef ANY_MODULE_COMPILED
  ", "
  #else
  #define ANY_MODULE_COMPILED
  #endif
  "WEBCONFIG"
  #endif

  #ifdef WEBSERVER
  #ifdef ANY_MODULE_COMPILED
  ", "
  #else
  #define ANY_MODULE_COMPILED
  #endif
  "WEBSERVER"
  #endif

  #ifdef WIFI
  #ifdef ANY_MODULE_COMPILED
  ", "
  #else
  #define ANY_MODULE_COMPILED
  #endif
  "WIFI"
  #endif

  #if defined(BLE_SENSORS) && defined(ESP32)
  #if defined(ANY_MODULE_COMPILED)
  ", "
  #else
  #define ANY_MODULE_COMPILED
  #endif
  "BLE_SENSORS"
  #endif

  #if !defined (ANY_MODULE_COMPILED)
  "NONE"
  #endif
  "<BR><BR>\r\n"));



#if defined LOGGER || defined WEBSERVER

  printToWebClient(STR_TEXT_FSP);
#if !defined(ESP32)
  uint32_t volFree = SD.vol()->freeClusterCount();
  uint32_t fs = (uint32_t)(volFree*SD.vol()->sectorsPerCluster()/2048);
  printFmtToWebClient(PSTR(": %lu MB<br>\r\n"), fs);
#else
  uint64_t fs = (SD.totalBytes() - SD.usedBytes());
  printFmtToWebClient(PSTR(": %llu Bytes<br>\r\n"), fs);
#endif

#endif
  printToWebClient(PSTR("<BR>\r\n"));

#ifndef WEBCONFIG
#ifdef AVERAGES
  if (LoggingMode & CF_LOGMODE_24AVG) {
    printToWebClient(CF_CALCULATION_TXT);
    printToWebClient(PSTR("<BR>\r\n"));
    printToWebClient(CF_PROGLIST_TXT);
    printToWebClient(PSTR(": <BR>\r\n"));

    for (int i = 0; i<numAverages; i++) {
      if (avg_parameters[i].number > 0) {
        printFmtToWebClient(PSTR("%g"), avg_parameters[i].number);
        if (avg_parameters[i].dest_addr != dest_address) {
          printFmtToWebClient(PSTR("!%d"), avg_parameters[i].dest_addr);
        }
        printFmtToWebClient(PSTR(" - %s: %d<BR>\r\n"), lookup_descr(avg_parameters[i].number), BSP_AVERAGES + i);
      }
    }
    printToWebClient(PSTR("<BR>"));
  }
#endif
  #ifdef LOGGER
  printFmtToWebClient(PSTR("<BR>" MENU_TEXT_LGP " \r\n%d"), log_interval);
  printToWebClient(PSTR(" " MENU_TEXT_SEC ": "));
  printyesno(LoggingMode & CF_LOGMODE_SD_CARD);
  printToWebClient(PSTR("<BR>\r\n"));
  for (int i = 0; i<numLogValues; i++) {
    if (log_parameters[i].number > 0) {
      printFmtToWebClient(PSTR("%g"), log_parameters[i].number);
      if (log_parameters[i].dest_addr != dest_address) {
        printFmtToWebClient(PSTR("!%d"), log_parameters[i].dest_addr);
      }
      printToWebClient(PSTR(" - "));
      printToWebClient(lookup_descr(log_parameters[i].number));
      printToWebClient(PSTR("<BR>\r\n"));
    }
  }

if (logTelegram) {
  printToWebClient(PSTR(MENU_TEXT_BDT "<BR>\r\n" MENU_TEXT_BUT ": "));
  printyesno(logTelegram & (LOGTELEGRAM_ON + LOGTELEGRAM_UNKNOWN_ONLY));
  printToWebClient(PSTR(MENU_TEXT_LBO ": "));
  printyesno(logTelegram & (LOGTELEGRAM_ON + LOGTELEGRAM_BROADCAST_ONLY));
  }
#endif
#endif
  printToWebClient(PSTR("<BR>\r\n"));
}

#if defined(BLE_SENSORS) && defined(ESP32)
void startBLEScan() {
  printFmtToDebug(PSTR("Initial BLE scan (%d sec)\r\n"), BLESensors_scanTime);
  BLESensors_init();
  BLESensors_scan_sensors(false, true);
  BLESensors_scan_sensors(true, false);
}
#endif

#if defined(WEBCONFIG) || defined(JSONCONFIG)
uint8_t takeNewConfigValueFromUI_andWriteToRAM(int option_id, char *buf) {
  bool found = false;
  configuration_struct cfg;
  char sscanf_buf[24];

  for (uint16_t f = 0; f < sizeof(config)/sizeof(config[0]); f++) {
    if (config[f].var_type == CDT_VOID) continue;
    memcpy(&cfg, &config[f], sizeof(cfg));
    if (cfg.id == option_id) {found = true; break;}
  }
  if (!found) {
    return 2;
  }

  switch (cfg.var_type) {
    case CDT_VOID: break;
    case CDT_BYTE:{
      byte variable = atoi(buf);
      writeToConfigVariable(option_id, (byte *)&variable);
      break;}
    case CDT_UINT16:{
      uint16_t variable = atoi(buf);
      writeToConfigVariable(option_id, (byte *)&variable);
      break;}
    case CDT_UINT32:{
      uint32_t variable = atoi(buf);
      writeToConfigVariable(option_id, (byte *)&variable);
      break;}
    case CDT_STRING:{
      char *variable = (char *)getConfigVariableAddress(option_id);
      memset(variable, 0, cfg.size);
      strncpy(variable, buf, cfg.size);
      break;}
    case CDT_MAC:{
      unsigned int i0, i1, i2, i3, i4, i5;
      uint16_t j = 0;
      char *ptr = buf;
      byte *variable = getConfigVariableAddress(option_id);
      memset(variable, 0, cfg.size);
      do{
        char *ptr_t = ptr;
        ptr = strchr(ptr, ',');
        if (ptr) ptr[0] = 0;
        strcpy_P(sscanf_buf, PSTR("%x:%x:%x:%x:%x:%x"));
        if(sscanf(ptr_t, sscanf_buf, &i0, &i1, &i2, &i3, &i4, &i5) == 6) {
          ((byte *)variable)[j * sizeof(mac) + 0] = (byte)(i0 & 0xFF);
          ((byte *)variable)[j * sizeof(mac) + 1] = (byte)(i1 & 0xFF);
          ((byte *)variable)[j * sizeof(mac) + 2] = (byte)(i2 & 0xFF);
          ((byte *)variable)[j * sizeof(mac) + 3] = (byte)(i3 & 0xFF);
          ((byte *)variable)[j * sizeof(mac) + 4] = (byte)(i4 & 0xFF);
          ((byte *)variable)[j * sizeof(mac) + 5] = (byte)(i5 & 0xFF);
        }
        if (ptr) {ptr[0] = ','; ptr++;}
        j++;
      }while (ptr && j < cfg.size/sizeof(mac));
      break;
    }
    case CDT_IPV4:{
      unsigned int i0, i1, i2, i3;
      strcpy_P(sscanf_buf, PSTR("%u.%u.%u.%u"));
      sscanf(buf, sscanf_buf, &i0, &i1, &i2, &i3);
      byte variable[4];
      variable[0] = (byte)(i0 & 0xFF);
      variable[1] = (byte)(i1 & 0xFF);
      variable[2] = (byte)(i2 & 0xFF);
      variable[3] = (byte)(i3 & 0xFF);
      writeToConfigVariable(option_id, variable);
      break;
    }
    case CDT_PROGNRLIST:{
      uint16_t j = 0;
      char *ptr = buf;
      byte *variable = getConfigVariableAddress(option_id);
      memset(variable, 0, cfg.size);
      do{
        char *ptr_t = ptr;
        ptr = strchr(ptr, ',');
        if (ptr) ptr[0] = 0;
        ((parameter *)variable)[j] = parsingStringToParameter(ptr_t);
        if (ptr) {ptr[0] = ','; ptr++;}
        j++;
      }while (ptr && j < cfg.size/sizeof(parameter));

      break;}
    case CDT_DHTBUS:{
      uint16_t j = 0;
      char *ptr = buf;
      byte *variable = getConfigVariableAddress(option_id);
      memset(variable, 0, cfg.size);
      do{
        char *ptr_t = ptr;
        ptr = strchr(ptr, ',');
        if (ptr) ptr[0] = 0;
        variable[j] = (byte)atoi(ptr_t);
        if (ptr) {ptr[0] = ','; ptr++;}

        j++;
      }while (ptr && j < cfg.size/sizeof(byte));

      break;}
#ifdef MAX_CUL
    case CDT_MAXDEVICELIST:{
      uint16_t j = 0;
      char *ptr = buf;
      byte *variable = getConfigVariableAddress(option_id);
      memset(variable, 0, cfg.size);
      do{
        char *ptr_t = ptr;
        ptr = strchr(ptr, ',');
        if (ptr) ptr[0] = 0;
        strncpy((char *)(variable + j * sizeof(max_device_list[0])), ptr_t, sizeof(max_device_list[0]));
        if (ptr) {ptr[0] = ','; ptr++;}
        j++;
      }while (ptr && j < cfg.size/sizeof(max_device_list[0]));

      UpdateMaxDeviceList();
      break;}
#endif
    default: break;
  }

return 1;
}

bool SaveConfigFromRAMtoEEPROM() {
  bool buschanged = false;
  bool needReboot = false;

  for (uint8_t i = 0; i < CF_LAST_OPTION; i++) {
    if (writeToEEPROM(i)) {
#ifdef DEVELOPER_DEBUG
      printFmtToDebug(PSTR("Option %d updated. EEPROM address: %04d\n"), i, getEEPROMaddress(i));
#endif
      switch (i) {
        case CF_BUSTYPE:
        case CF_OWN_BSBLPBADDR:
        case CF_DEST_BSBLPBADDR:
        case CF_PPS_MODE:
          buschanged = true;
          break;
        case CF_RX_PIN:
        case CF_TX_PIN:
        case CF_DEVICE_FAMILY:
        case CF_DEVICE_VARIANT:







        case CF_MAC:
        case CF_DHCP:
        case CF_IPADDRESS:
        case CF_MASK:
        case CF_GATEWAY:
        case CF_DNS:
        case CF_ONEWIREBUS:
        case CF_WWWPORT:
        case CF_WIFI_SSID:
        case CF_WIFI_PASSWORD:
        case CF_MDNS_HOSTNAME:
        case CF_ESP32_ENERGY_SAVE:
          needReboot = true;
          break;
        case CF_BMEBUS:
          if(BME_Sensors > 16) BME_Sensors = 16;
          needReboot = true;
          break;
#if defined(ESP32) && defined(ENABLE_ESP32_OTA)
        case CF_OTA_UPDATE:
          if (enable_ota_update){
            init_ota_update();
          } else {
            update_server.stop();
            printlnToDebug(PSTR("Update Server stopped."));
          }
          break;
#endif
        case CF_TWW_PUSH_PIN_ID:
        case CF_RGT1_PRES_PIN_ID:
        case CF_RGT2_PRES_PIN_ID:
        case CF_RGT3_PRES_PIN_ID:
          needReboot = true;
          break;
#ifdef AVERAGES
        case CF_AVERAGESLIST:
          resetAverageCalculation();
          break;
#endif
#ifdef MAX_CUL
        case CF_MAX:
        case CF_MAX_IPADDRESS:
          connectToMaxCul();
          break;
#endif
#ifdef MQTT
        case CF_MQTT:
        case CF_MQTT_IPADDRESS:
          mqtt_disconnect();
          break;
#endif
#if defined(BLE_SENSORS) && defined(ESP32)
        case CF_BLE_SENSORS_MACS:





        case CF_ENABLE_BLE:
          needReboot = true;





          break;
#endif
        default: break;
      }
    }
  }


  EEPROM_dump();

  if (buschanged) {
    setBusType();
    SetDevId();
  }
  return needReboot;
}
#endif

int returnENUMID4ConfigOption(uint8_t id) {
  int i = 0;
  switch (id) {
    case CF_BUSTYPE:
      i=findLine(65532,0,NULL);
      break;
    case CF_LOGTELEGRAM:
      i=findLine(65531,0,NULL);
      break;
    case CF_DEBUG:
      i=findLine(65530,0,NULL);
      break;
    case CF_MQTT:
      i=findLine(65529,0,NULL);
      break;
    case CF_WRITEMODE:
      i=findLine(65528,0,NULL);
      break;
    case CF_PPS_MODE:
      i=findLine(65527,0,NULL);
      break;
    case CF_LOGMODE:
      i=findLine(65526,0,NULL);
      break;
    default:
      i = -1;
      break;
  }
  return i;
}

#if defined(JSONCONFIG) || defined(WEBCONFIG)
void printMAClistToWebClient(byte *variable, uint16_t size) {
  bool isFirst = true;
  for (uint16_t j = 0; j < size/sizeof(mac); j++) {
    bool mac_valid = false;
    for(uint m = 0; m < sizeof(mac); m++){
      if(variable[j * sizeof(mac) + m]){
        mac_valid = true;
        break;
      }
    }
    if (mac_valid) {
      if (!isFirst) printToWebClient(PSTR(","));
      isFirst = false;
      bin2hex(outBuf, variable + j * sizeof(mac), sizeof(mac), ':');
      printToWebClient(outBuf);
      outBuf[0] = 0;
    }
  }
}

void printDHTlistToWebClient(byte *variable, uint16_t size) {
  bool isFirst = true;
  for (uint16_t j = 0; j < size/sizeof(byte); j++) {
    if (variable[j]) {
      if (!isFirst) printToWebClient(PSTR(","));
      isFirst = false;
      printFmtToWebClient(PSTR("%d"), variable[j]);
    }
  }
}

void printProglistToWebClient(parameter *variable, uint16_t size) {
  bool isFirst = true;
  for (uint16_t j = 0; j < size/sizeof(parameter); j++) {
    if (variable[j].number) {
      if (!isFirst) printToWebClient(PSTR(","));
      isFirst = false;
      printFmtToWebClient(PSTR("%g"), variable[j].number);
      if (variable[j].dest_addr > 0) {
        printFmtToWebClient(PSTR("!%d"), variable[j].dest_addr);
      }
    }
  }
}

void printMAXlistToWebClient(byte *variable, uint16_t size) {
  bool isFirst = true;
  for (uint16_t j = 0; j < size/sizeof(byte); j += sizeof(max_device_list[0])) {
    if (variable[j]) {
      if (!isFirst) printToWebClient(PSTR(","));
      isFirst = false;
      printFmtToWebClient(PSTR("%s"), variable + j);
    }
  }
}
#endif

#ifdef WEBCONFIG
void applyingConfig() {
  bool k_flag = false;
  int i = 0;
  int option_id = 0;

  while (client.available()) {
    char c = client.read();
    if (!k_flag && i == OUTBUF_LEN - 2) {
      for (int j = 0; j < 16; j++) {
        outBuf[j] = outBuf[i - 16 + j];
      }
      i = 16;
    }

    if (c == '=') {
      char *ptr = strstr_P(outBuf, PSTR("option_"));
      if (!ptr) continue;
      ptr += 7;
      option_id = atoi(ptr) - 1;
      printFmtToDebug(PSTR("Option ID: %d\r\n"), option_id);
      k_flag = true;
      i = 0;
      outBuf[i] = 0;
      continue;
    }
    if (k_flag && (c == '&' || i == OUTBUF_LEN - 2 || !client.available())) {
      if (!client.available()) {
        outBuf[i++] = c;
        outBuf[i] = 0;
      }

      if (takeNewConfigValueFromUI_andWriteToRAM(option_id, outBuf) == 1)
        printFmtToDebug(PSTR("Option value: %s\r\n"), outBuf);

      k_flag = false;
      i = 0;
      outBuf[i] = 0;
      continue;
    }
    if (c == '+') {
      c = ' ';
    }
    if (c == '%') {
      if (client.available()) {outBuf[i] = client.read();}
      if (client.available()) {outBuf[i + 1] = client.read();}
      outBuf[i + 2] = 0;
      unsigned int symbol;
      sscanf(outBuf + i, "%x", &symbol);
      c = symbol & 0xFF;
    }

    outBuf[i++] = c;
    outBuf[i] = 0;
  }

}

void printConfigWebPossibleValues(int i, uint16_t temp_value, bool printCurrentSelectionOnly) {
  uint16_t enumstr_len=get_cmdtbl_enumstr_len(i);
  uint_farptr_t enumstr = calc_enum_offset(get_cmdtbl_enumstr(i), enumstr_len, 0);
  if(printCurrentSelectionOnly){
    listEnumValues(enumstr, enumstr_len, NULL, NULL, NULL, NULL, NULL, temp_value, PRINT_DESCRIPTION|PRINT_VALUE_FIRST|PRINT_ONLY_VALUE_LINE, DO_NOT_PRINT_DISABLED_VALUE);
  } else {
    listEnumValues(enumstr, enumstr_len, STR_OPTION_VALUE, PSTR("'>"), STR_SELECTED, STR_CLOSE_OPTION, NULL, temp_value, PRINT_VALUE|PRINT_DESCRIPTION|PRINT_VALUE_FIRST, DO_NOT_PRINT_DISABLED_VALUE);
  }
}

void generateWebConfigPage(bool printOnly) {
  printlnToWebClient(PSTR(MENU_TEXT_CFG "<BR>"));
  if(!printOnly){

    printToWebClient(PSTR("<script>function bvc(e,v){o=e.closest('div').querySelector('input');n=Number(o.value);n&v?p=n-v:p=n+v;o.value=p}</script>"));
    printToWebClient(PSTR("<form id=\"config\" method=\"post\" action=\""));
    if (PASSKEY[0]) {printToWebClient(PSTR("/")); printToWebClient(PASSKEY);}
    printToWebClient(PSTR("/CI\">"));
  }
  printToWebClient(PSTR("<table align=\"center\"><tbody>\r\n"));
  for (uint16_t i = 0; i < sizeof(config)/sizeof(config[0]); i++) {
    configuration_struct cfg;

    memcpy(&cfg, &config[i], sizeof(cfg));

    if (cfg.var_type == CDT_VOID) continue;
    if(!printOnly){
      if(config_level == 0 && !(cfg.flags & OPT_FL_BASIC)) continue;
      if(config_level == 1 && !(cfg.flags & OPT_FL_ADVANCED)) continue;
    }
    byte *variable = getConfigVariableAddress(cfg.id);
    if (!variable) continue;

    printToWebClient(PSTR("<tr><td>"));

    printToWebClient(catalist[cfg.category].desc);
    const char fieldDelimiter[] PROGMEM = "</td><td>\r\n";
    printToWebClient(fieldDelimiter);

    printToWebClient(cfg.desc);
    printToWebClient(fieldDelimiter);



   if(!printOnly){
     switch (cfg.input_type) {
       case CPI_TEXT:
       printFmtToWebClient(PSTR("<input type=text id='option_%d' name='option_%d' "), cfg.id + 1, cfg.id + 1);
       switch (cfg.var_type) {
         case CDT_MAC:
           printToWebClient(PSTR("pattern='([0-9A-Fa-f]{2}[-:]){5}[0-9A-Fa-f]{2}'"));
           break;
         case CDT_IPV4:
           printToWebClient(PSTR("pattern='((^|\\.)(25[0-5]|2[0-4]\\d|[01]?\\d{1,2})){4}'"));
           break;
         case CDT_PROGNRLIST:
           printToWebClient(PSTR("pattern='((^|,)\\d{1,5}(\\.\\d)?((!|!-)\\d{1,3})?)+'"));
           break;
         }
       printToWebClient(PSTR(" value='"));
       break;
       case CPI_SWITCH:
       case CPI_DROPDOWN:
          printFmtToWebClient(PSTR("<select id='option_%d' name='option_%d'>\r\n"), cfg.id + 1, cfg.id + 1);
       break;
       case CPI_CHECKBOXES:
          printFmtToWebClient(PSTR("<div><input type=hidden id='option_%d' name='option_%d' value='%d'>\r\n"), cfg.id + 1, cfg.id + 1, (int)variable[0]);
       break;
       default: break;
     }
   } else {
     printFmtToWebClient(PSTR("<output id='option_%d' name='option_%d'>\r\n"), cfg.id + 1, cfg.id + 1);
   }


   switch (cfg.var_type) {
     case CDT_VOID: break;
     case CDT_BYTE:
       switch (cfg.input_type) {
         case CPI_TEXT: printFmtToWebClient(PSTR("%d"), (int)variable[0]); break;
         case CPI_SWITCH:{
           int i;
           switch (cfg.id) {
             case CF_USEEEPROM:
               i=findLine(65534,0,NULL);
               break;
             default:
               i=findLine(65533,0,NULL);
               break;
           }
           printConfigWebPossibleValues(i, (uint16_t)variable[0], printOnly);
           break;}
         case CPI_CHECKBOXES:{
           int i = returnENUMID4ConfigOption(cfg.id);
           if (i > 0) {
             uint16_t enumstr_len=get_cmdtbl_enumstr_len(i);
             uint_farptr_t enumstr = calc_enum_offset(get_cmdtbl_enumstr(i), enumstr_len, 0);
             listEnumValues(enumstr, enumstr_len, PSTR("<label style='display:flex;flex-direction:row;justify-content:flex-start;align-items:center'><input type='checkbox' style='width:40px;' onclick=\"bvc(this,"), PSTR(")\">"), PSTR(")\" checked>"), PSTR("</label>"), NULL, variable[0], PRINT_DESCRIPTION|PRINT_VALUE|PRINT_VALUE_FIRST|PRINT_ENUM_AS_DT_BITS, DO_NOT_PRINT_DISABLED_VALUE);
           }
         break;}
         case CPI_DROPDOWN:{
           int i = returnENUMID4ConfigOption(cfg.id);
           if (i > 0) {
             printConfigWebPossibleValues(i, variable[0], printOnly);
           }
           break;}
         }
       break;
     case CDT_UINT16:
       switch (cfg.input_type) {
         case CPI_TEXT: printFmtToWebClient(PSTR("%u"), ((uint16_t *)variable)[0]); break;
         case CPI_DROPDOWN:{
           int i;
           switch (cfg.id) {
             case CF_ROOM_DEVICE:
               i=findLine(15000 + PPS_QTP,0,NULL);
               break;
             default:
               i = -1;
               break;
           }
           if (i > 0) {
             printConfigWebPossibleValues(i, ((uint16_t *)variable)[0], printOnly);
           }
           break;}
         }
       break;
     case CDT_UINT32:
       printFmtToWebClient(PSTR("%lu"), ((uint32_t *)variable)[0]);
       break;
     case CDT_STRING:
       printFmtToWebClient(PSTR("%s"), (char *)variable);
       break;
     case CDT_MAC:
       printMAClistToWebClient((byte *)variable, cfg.size);
       break;
     case CDT_IPV4:
       printFmtToWebClient(PSTR("%u.%u.%u.%u"), (int)variable[0], (int)variable[1], (int)variable[2], (int)variable[3]);
       break;
     case CDT_PROGNRLIST:
       printProglistToWebClient((parameter *)variable, cfg.size);
       break;
     case CDT_DHTBUS:
       printDHTlistToWebClient((byte *)variable, cfg.size);
       break;
     case CDT_MAXDEVICELIST:
       printMAXlistToWebClient((byte *)variable, cfg.size);
       break;
     default: break;
   }


   if(!printOnly){
     switch (cfg.input_type) {
       case CPI_TEXT: printToWebClient(PSTR("'>")); break;
       case CPI_SWITCH:
       case CPI_DROPDOWN: printToWebClient(PSTR("</select>")); break;
       case CPI_CHECKBOXES: printToWebClient(PSTR("</div>"));break;
       default: break;
     }
   } else {
     printToWebClient(PSTR("</output>"));
   }
    printToWebClient(PSTR("</td></td>\r\n"));
  }
  printToWebClient(PSTR("</tbody></table><p>"));
  if(!printOnly){
    printToWebClient(PSTR("<input type=\"submit\" value=\""));
    printToWebClient(STR_SAVE);
    printToWebClient(PSTR("\"></p>\r\n</form>\r\n"));
  }
}
#endif


#if defined(JSONCONFIG)
void printConfigJSONPossibleValues(int i, bool its_a_bits_enum) {
  printToWebClient(PSTR("    \"possibleValues\": [\r\n"));
  uint16_t enumstr_len=get_cmdtbl_enumstr_len(i);
  uint_farptr_t enumstr = calc_enum_offset(get_cmdtbl_enumstr(i), enumstr_len, 0);
  listEnumValues(enumstr, enumstr_len, PSTR("      { \"enumValue\": \""), PSTR("\", \"desc\": \""), NULL, PSTR("\" }"), PSTR(",\r\n"), 0,
    its_a_bits_enum?PRINT_VALUE|PRINT_DESCRIPTION|PRINT_VALUE_FIRST|PRINT_ENUM_AS_DT_BITS:
    PRINT_VALUE|PRINT_DESCRIPTION|PRINT_VALUE_FIRST, DO_NOT_PRINT_DISABLED_VALUE);
  printToWebClient(PSTR("\r\n      ]"));
}

void generateJSONwithConfig() {
  bool notFirst = false;
  for (uint16_t i = 0; i < sizeof(config)/sizeof(config[0]); i++) {
    configuration_struct cfg;

    memcpy(&cfg, &config[i], sizeof(cfg));

    if (cfg.var_type == CDT_VOID) continue;
    if(config_level == 0 && !(cfg.flags & OPT_FL_BASIC)) continue;
    if(config_level == 1 && !(cfg.flags & OPT_FL_ADVANCED)) continue;
    byte *variable = getConfigVariableAddress(cfg.id);
    if (!variable) continue;
    if (notFirst) {printToWebClient(PSTR("\r\n    },\r\n"));} else notFirst = true;

    printFmtToWebClient(PSTR("  \"%d\": {\r\n    \"parameter\": %d,\r\n    \"type\": %d,\r\n    \"format\": %d,\r\n    \"category\": \""), i, cfg.id, cfg.var_type, cfg.input_type);

    printToWebClient(catalist[cfg.category].desc);

    printToWebClient(PSTR("\",\r\n    \"name\": \""));

    printToWebClient(cfg.desc);
    printToWebClient(PSTR("\",\r\n    \"value\": \""));


   switch (cfg.var_type) {
     case CDT_VOID: break;
     case CDT_BYTE:
       switch (cfg.input_type) {
         case CPI_TEXT: printFmtToWebClient(PSTR("%d\""), (int)variable[0]); break;
         case CPI_SWITCH:{
           int i;
           switch (cfg.id) {
             case CF_USEEEPROM:
               i=findLine(65534,0,NULL);
               break;
             default:
               i=findLine(65533,0,NULL);
               break;
           }
           printFmtToWebClient(PSTR("%u\",\r\n"), (uint16_t)variable[0]);
           printConfigJSONPossibleValues(i, false);
           break;}
         case CPI_CHECKBOXES:
         case CPI_DROPDOWN:{
           int i = returnENUMID4ConfigOption(cfg.id);
           if (i > 0) {
             printFmtToWebClient(PSTR("%u\",\r\n"), (uint16_t)variable[0]);
             printConfigJSONPossibleValues(i, cfg.input_type == CPI_CHECKBOXES);
           }
           break;}
         }
       break;
     case CDT_UINT16:
       switch (cfg.input_type) {
         case CPI_TEXT: printFmtToWebClient(PSTR("%u\""), ((uint16_t *)variable)[0]); break;
         case CPI_DROPDOWN:{
           int i;
           switch (cfg.id) {
             case CF_ROOM_DEVICE:
               i=findLine(15000 + PPS_QTP,0,NULL);
               break;
             default:
               i = -1;
               break;
           }
           if (i > 0) {
             printFmtToWebClient(PSTR("%u\",\r\n"), ((uint16_t *)variable)[0]);
             printConfigJSONPossibleValues(i, false);
           }
           break;}
         }
       break;
     case CDT_UINT32:
       printFmtToWebClient(PSTR("%lu\""), ((uint32_t *)variable)[0]);
       break;
     case CDT_STRING:
       printFmtToWebClient(PSTR("%s\""), (char *)variable);
       break;
     case CDT_MAC:
       printMAClistToWebClient((byte *)variable, cfg.size);
       printToWebClient(PSTR("\""));

       break;
     case CDT_IPV4:
       printFmtToWebClient(PSTR("%u.%u.%u.%u\""), (int)variable[0], (int)variable[1], (int)variable[2], (int)variable[3]);
       break;
     case CDT_PROGNRLIST:
       printProglistToWebClient((parameter *)variable, cfg.size);
       printToWebClient(PSTR("\""));
       break;
     case CDT_DHTBUS:
       printDHTlistToWebClient((byte *)variable, cfg.size);
       printToWebClient(PSTR("\""));
       break;
     case CDT_MAXDEVICELIST:
       printMAXlistToWebClient((byte *)variable, cfg.size);
       printToWebClient(PSTR("\""));
       break;
     default: break;
   }
  }
  printToWebClient(PSTR("\r\n    }\r\n"));
}
#endif
# 3313 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
#ifdef LOGGER
void LogTelegram(byte* msg) {
  if (!(logTelegram & LOGTELEGRAM_ON)) return;
  File dataFile;
  uint32_t cmd;
  int i=0;
  int save_i=0;
  bool known=0;
  uint32_t c;
  uint8_t cmd_type=0;
  float operand=1;
  uint8_t precision=0;
  int data_len;
  float dval;
  float line = 0;
#if !defined(ESP32)
  if (SD.vol()->freeClusterCount() < MINIMUM_FREE_SPACE_ON_SD) return;
#else
  if (SD.totalBytes() - SD.usedBytes() < MINIMUM_FREE_SPACE_ON_SD) return;
#endif

  if (bus->getBusType() != BUS_PPS) {
    if (msg[4+(bus->getBusType()*4)]==TYPE_QUR || msg[4+(bus->getBusType()*4)]==TYPE_SET || (((msg[2]!=ADDR_ALL && bus->getBusType()==BUS_BSB) || (msg[2]<0xF0 && bus->getBusType()==BUS_LPB)) && msg[4+(bus->getBusType()*4)]==TYPE_INF)) {
      cmd=(uint32_t)msg[6+(bus->getBusType()*4)]<<24 | (uint32_t)msg[5+(bus->getBusType()*4)]<<16 | (uint32_t)msg[7+(bus->getBusType()*4)] << 8 | (uint32_t)msg[8+(bus->getBusType()*4)];
    } else {
      cmd=(uint32_t)msg[5+(bus->getBusType()*4)]<<24 | (uint32_t)msg[6+(bus->getBusType()*4)]<<16 | (uint32_t)msg[7+(bus->getBusType()*4)] << 8 | (uint32_t)msg[8+(bus->getBusType()*4)];
    }
  } else {

    cmd=msg[1];
  }

  c=get_cmdtbl_cmd(i);

  while (c!=CMD_END) {
    line=get_cmdtbl_line(i);
    if ((bus->getBusType() != BUS_PPS && c == cmd) || (bus->getBusType() == BUS_PPS && line >= 15000 && (cmd == ((c & 0x00FF0000) >> 16)))) {
      uint8_t dev_fam = get_cmdtbl_dev_fam(i);
      uint8_t dev_var = get_cmdtbl_dev_var(i);
      if ((dev_fam == my_dev_fam || dev_fam == DEV_FAM(DEV_ALL)) && (dev_var == my_dev_var || dev_var == DEV_VAR(DEV_ALL))) {
        if (dev_fam == my_dev_fam && dev_var == my_dev_var) {
          break;
        } else if ((!known && dev_fam!=my_dev_fam) || (dev_fam==my_dev_fam)) {
          known=1;
          save_i=i;
        }
      }
    }
    if (known && c!=cmd) {
      i=save_i;
      break;
    }
    i++;
    c=get_cmdtbl_cmd(i);
  }
  if (cmd <= 0) return;
  bool logThis = false;
  switch (logTelegram) {
    case LOGTELEGRAM_ON: logThis = true; break;
    case LOGTELEGRAM_ON + LOGTELEGRAM_UNKNOWN_ONLY: if (known == 0) logThis = true; break;
    case LOGTELEGRAM_ON + LOGTELEGRAM_BROADCAST_ONLY: if ((msg[2]==ADDR_ALL && bus->getBusType()==BUS_BSB) || (msg[2]>=0xF0 && bus->getBusType()==BUS_LPB)) logThis = true; break;
    case LOGTELEGRAM_ON + LOGTELEGRAM_UNKNOWNBROADCAST_ONLY: if (known == 0 && ((msg[2]==ADDR_ALL && bus->getBusType()==BUS_BSB) || (msg[2]>=0xF0 && bus->getBusType()==BUS_LPB))) logThis = true; break;
    default: logThis = false; break;
  }
  if (logThis) {
    dataFile = SD.open(journalFileName, FILE_APPEND);
    if (dataFile) {
      int outBufLen = 0;
      outBufLen += sprintf_P(outBuf, PSTR("%lu;%s;"), millis(), GetDateTime(outBuf + outBufLen + 80));
      if (!known) {

        outBufLen += strlen(strcpy_P(outBuf + outBufLen, PSTR("UNKNOWN")));
      } else {

        line=get_cmdtbl_line(i);
        cmd_type=get_cmdtbl_type(i);
        outBufLen += sprintf_P(outBuf + outBufLen, PSTR("%g"), line);
        }

      uint8_t msg_len = 0;
      if (bus->getBusType() != BUS_PPS) {
        outBufLen += sprintf_P(outBuf + outBufLen, PSTR(";%s->%s %s;"), TranslateAddr(msg[1+(bus->getBusType()*2)], outBuf + outBufLen + 40), TranslateAddr(msg[2], outBuf + outBufLen + 50), TranslateType(msg[4+(bus->getBusType()*4)], outBuf + outBufLen + 60));
        msg_len = msg[bus->getLen_idx()]+bus->getBusType();
      } else {
        strcat_P(outBuf + outBufLen, PSTR(";"));
        const char *getfarstrings;
        switch (msg[0] & 0x0F) {
          case 0x0D: getfarstrings = PSTR("PPS INF"); break;
          case 0x0E: getfarstrings = PSTR("PPS REQ"); break;
          case 0x07: getfarstrings = PSTR("PPS RTS"); break;
          default: getfarstrings = PSTR(""); break;
        }
        switch (msg[0]) {
          case 0xF8:
          case 0xFB:
          case 0xFD:
          case 0xFE:
            getfarstrings = PSTR("PPS ANS"); break;
          default: getfarstrings = PSTR(""); break;
        }
        strcat_P(outBuf + outBufLen, getfarstrings);
        strcat_P(outBuf + outBufLen, PSTR(";"));
        outBufLen += strlen(outBuf + outBufLen);

        msg_len = 9;
      }

      outBufLen += bin2hex(outBuf + outBufLen, msg, msg_len, ' ');


      if (bus->getBusType() != BUS_PPS && (msg[4+(bus->getBusType()*4)] == TYPE_INF || msg[4+(bus->getBusType()*4)] == TYPE_SET || msg[4+(bus->getBusType()*4)] == TYPE_ANS) && msg[bus->getLen_idx()] < 17+bus->getBusType()) {
        outBufLen += strlen(strcat_P(outBuf + outBufLen, PSTR(";")));
        if (bus->getBusType() == BUS_LPB) {
          data_len=msg[1]-14;
        } else {
          data_len=msg[3]-11;
        }
        dval = 0;
        operand=optbl[cmd_type].operand;
        precision=optbl[cmd_type].precision;

        for (i=0;i<data_len-1+bus->getBusType();i++) {
          if (bus->getBusType() == BUS_LPB) {
            dval = dval + long(msg[14+i-(msg[8]==TYPE_INF)]<<((data_len-2-i)*8));
          } else {
            dval = dval + long(msg[10+i-(msg[4]==TYPE_INF)]<<((data_len-2-i)*8));
          }
        }
        dval = dval / operand;
        _printFIXPOINT(outBuf + outBufLen, dval, precision);

        char *p = strchr(outBuf + outBufLen,'.');
        if (p != NULL) *p=',';
        outBufLen += strlen(outBuf + outBufLen);
      }
      strcat_P(outBuf + outBufLen, PSTR("\r\n"));
      dataFile.print(outBuf);
      dataFile.close();
    }
  }
}
#else
void LogTelegram(byte* msg) {
  msg = msg;
}
#endif

#define MAX_PARAM_LEN 22
# 3483 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
int set(float line
      , const char *val
      , bool setcmd)
{
  byte msg[33];
  byte tx_msg[33];
  int i;
  uint32_t c;
  uint8_t param[MAX_PARAM_LEN];
  uint8_t param_len = 0;
  char sscanf_buf[36];
#if defined(ESP32)
  esp_task_wdt_reset();
#endif

  if (line < 0) {
    return 0;
  }

  i=findLine(line,0,&c);
  if (i<0) return 0;


  if (programIsreadOnly(get_cmdtbl_flags(i))) {
    printlnToDebug(PSTR("Parameter is readonly!"));
    return 2;
  }

#ifdef MQTT



  if (setcmd) {
    lastMQTTTime = millis() - log_interval * 1000 + 1000;
  }
#endif

  loadPrognrElementsFromTable(line, i);

  if ((line >= BSP_INTERNAL && line < BSP_END))
    {
      if(line == BSP_INTERNAL + 6){
        if (atoi(val)) resetDurations(); return 1;
      }
      if ((line >= BSP_FLOAT && line < BSP_FLOAT + numCustomFloats)) {
        custom_floats[(int)line - BSP_FLOAT] = atof(val);
        return 1;
      }
      if ((line >= BSP_LONG && line < BSP_LONG + numCustomLongs)) {
        char sscanf_buf[8];
        strcpy_P(sscanf_buf, PSTR("%ld"));
        sscanf(val, sscanf_buf, &custom_longs[(int)line - BSP_LONG]);
        return 1;
      }

      return 2;
    }

  if (bus->getBusType() == BUS_PPS && line >= 15000 && line < 15000 + PPS_ANZ) {
    int cmd_no = line - 15000;
    switch (decodedTelegram.type) {
      case VT_TEMP: pps_values[cmd_no] = atof(val) * 64; break;
      case VT_WEEKDAY:
      {
        int dow = atoi(val);
        pps_values[PPS_DOW] = dow;
      #if defined(ESP32)





      #else
        setTime(hour(), minute(), second(), dow, 1, 2018);
      #endif
#ifdef DEVELOPER_DEBUG
        printFmtToDebug(PSTR("Setting weekday to %d\r\n"), weekday());
#endif
        pps_wday_set = true;
        break;
      }
      case VT_PPS_TIME:
      {
        int hour=0, minute=0, second=0;
        strcpy_P(sscanf_buf, PSTR("%d.%d.%d"));
        sscanf(val, sscanf_buf, &hour, &minute, &second);
        setTime(hour, minute, second, weekday(), 1, 2018);
#ifdef DEVELOPER_DEBUG
        printFmtToDebug(PSTR("Setting time to %d:%d:%d\r\n"), hour, minute, second);
#endif
        pps_time_set = true;
        break;
      }
      case VT_HOUR_MINUTES:
      {
        uint8_t h=atoi(val);
        uint8_t m=0;
        while (*val!='\0' && *val!=':' && *val!='.') val++;
        if (*val==':' || *val=='.') {
          val++;
          m=atoi(val);
        }
        pps_values[cmd_no] = h * 6 + m / 10;
        break;
      }
      default: pps_values[cmd_no] = atoi(val); break;
    }

    uint8_t flags=get_cmdtbl_flags(i);
    if ((flags & FL_EEPROM) == FL_EEPROM && EEPROM_ready) {

      printFmtToDebug(PSTR("Writing EEPROM slot %d with value %u"), cmd_no, pps_values[cmd_no]);
      writelnToDebug();
      writeToEEPROM(CF_PPS_VALUES);
    }

    return 1;
  }


  switch (decodedTelegram.type) {




    case VT_MONTHS:
    case VT_MONTHS_N:
    case VT_MINUTES_SHORT:
    case VT_MINUTES_SHORT_N:
    case VT_PERCENT:
    case VT_PERCENT_NN:
    case VT_PERCENT1:
    case VT_ENUM:
    case VT_BINARY_ENUM:
    case VT_WEEKDAY:
    case VT_ONOFF:
    case VT_CLOSEDOPEN:
    case VT_YESNO:
    case VT_DAYS:
    case VT_HOURS_SHORT:
    case VT_HOURS_SHORT_N:
    case VT_BIT:
    case VT_BYTE:
    case VT_BYTE_N:
    case VT_BYTE10:
    case VT_BYTE10_N:
    case VT_TEMP_SHORT:
    case VT_TEMP_SHORT_US:
    case VT_TEMP_SHORT_US_N:
    case VT_TEMP_PER_MIN:
    case VT_TEMP_SHORT5_US:
    case VT_TEMP_SHORT5:
    case VT_PERCENT5:
    case VT_TEMP_SHORT64:
    case VT_SECONDS_SHORT2:
    case VT_SECONDS_SHORT2_N:
    case VT_SECONDS_SHORT4:
    case VT_SECONDS_SHORT5:
    case VT_PRESSURE:
    case VT_PRESSURE_NN:
    case VT_GRADIENT_SHORT:
    case VT_LPBADDR:
    case VT_SECONDS_SHORT:
    case VT_SECONDS_SHORT_N:
    case VT_VOLTAGE:
    case VT_VOLTAGE_N:
    case VT_VOLTAGE_WORD1:
    case VT_LITER:
    case VT_POWER_SHORT:
    case VT_POWER_SHORT_N:


    case VT_UINT:
    case VT_UINT_N:
    case VT_UINT100_WORD_N:
    case VT_SINT:
    case VT_SINT_NN:
    case VT_PERCENT_WORD1:
    case VT_METER:
    case VT_HOURS_WORD:
    case VT_HOURS_WORD_N:
    case VT_MINUTES_WORD:
    case VT_MINUTES_WORD_N:
    case VT_MINUTES_WORD10:
    case VT_UINT5:
    case VT_UINT10:
    case VT_MSECONDS_WORD:
    case VT_MSECONDS_WORD_N:
    case VT_SECONDS_WORD:
    case VT_SECONDS_WORD_N:
    case VT_SECONDS_WORD16:
    case VT_TEMP_WORD:
    case VT_CELMIN:
    case VT_CELMIN_N:
    case VT_PERCENT_100:
    case VT_PERCENT_WORD:
    case VT_LITERPERHOUR:
    case VT_LITERPERHOUR_N:
    case VT_LITERPERHOUR100:
    case VT_LITERPERHOUR100_N:
    case VT_LITERPERMIN:
    case VT_LITERPERMIN_N:
    case VT_CONSUMPTION:
    case VT_PRESSURE_WORD1:
    case VT_PRESSURE_1000:
    case VT_PPM:
    case VT_FP02:
    case VT_SECONDS_WORD5:
    case VT_SECONDS_WORD5_N:
    case VT_SECONDS_WORD4:
    case VT_SECONDS_WORD4_N:
    case VT_TEMP_WORD5_US:
    case VT_GRADIENT:
    case VT_POWER_W:
    case VT_POWER_WORD:
    case VT_POWER_WORD_N:
    case VT_MONTHS_WORD:
    case VT_DAYS_WORD:
    case VT_FREQ:
    case VT_FREQ10:


    case VT_UINT100:
    case VT_ENERGY:
    case VT_ENERGY_N:
    case VT_ENERGY_MWH:
    case VT_ENERGY_MWH_N:
    case VT_AMP:
    case VT_CUBICMETER:
    case VT_MINUTES:
    case VT_TEMP_DWORD:
      {
      char* val1 = (char *)val;
      if (val[0] == '-') {
        val1++;
      }
      uint32_t t = atoi(val1) * decodedTelegram.operand;
      val1 = strchr(val, '.');
      if(val1) {
        val1++;
        int len = strlen(val1);
        uint32_t tpart = atoi(val1) * decodedTelegram.operand;
        for(int d = 0; d < len; d++) {
          if(tpart % 10 > 4 && d + 1 == len) tpart += 10;
          tpart /= 10;
        }
        t += tpart;
      }
      if (val[0] == '-') {
        t = -1 * (int) t;
      }
      for (int x=decodedTelegram.payload_length;x>0;x--) {
        param[decodedTelegram.payload_length-x+1] = (t >> ((x-1)*8)) & 0xff;
      }
      if (val[0] == '\0' || (decodedTelegram.type == VT_ENUM && t == 0xFFFF)) {
        param[0]=decodedTelegram.enable_byte-1;
      } else {
        param[0]=decodedTelegram.enable_byte;
      }
      param_len=decodedTelegram.payload_length + 1;
      }
      break;



    case VT_DWORD:
    case VT_DWORD_N:
    case VT_DWORD10:
      {
      if (val[0]!='\0') {
        uint32_t t = (uint32_t)strtoul(val, NULL, 10);
        param[0]=decodedTelegram.enable_byte;
        param[1]=(t >> 24) & 0xff;
        param[2]=(t >> 16) & 0xff;
        param[3]=(t >> 8) & 0xff;
        param[4]= t & 0xff;
      } else {
        param[0]=decodedTelegram.enable_byte-1;
        param[1]=0x00;
        param[2]=0x00;
        param[3]=0x00;
        param[4]=0x00;
      }
      param_len=5;
      }
      break;



    case VT_HOUR_MINUTES:
    case VT_HOUR_MINUTES_N:
      {
      if (val[0]!='\0') {
        uint8_t h=atoi(val);
        uint8_t m=0;
        while (*val!='\0' && *val!=':' && *val!='.') val++;
        if (*val==':' || *val=='.') {
          val++;
          m=atoi(val);
        }
        param[0]=decodedTelegram.enable_byte;
        param[1]= h;
        param[2]= m;
      } else {
        param[0]=decodedTelegram.enable_byte-1;
        param[1]=0x00;
        param[2]=0x00;
      }
      param_len=3;
      }
      break;



    case VT_STRING:
      {
      strncpy((char *)param,val,MAX_PARAM_LEN);
      param[MAX_PARAM_LEN-1]='\0';
      param_len=strlen((char *)param)+1;
      }
      break;

    case VT_SINT1000:
      {
      uint16_t t=atof(val)*1000.0;
      if (setcmd) {
        if (val[0]!='\0') {
          param[0]=decodedTelegram.enable_byte;
        } else {
          param[0]=decodedTelegram.enable_byte-1;
        }
        param[1]=(t >> 8);
        param[2]= t & 0xff;
      } else {
        param[0]=(t >> 8);
        param[1]= t & 0xff;
        param[2]=0x00;
      }
      param_len=3;
      }
      break;




    case VT_TEMP:
    case VT_TEMP_N:
      {
      uint32_t t=((int)(atof(val)*decodedTelegram.operand));
      if (setcmd) {
        if (val[0]!='\0') {
          param[0]=decodedTelegram.enable_byte;
        } else {
          param[0]=decodedTelegram.enable_byte-1;
        }
        param[1]=(t >> 8);
        param[2]= t & 0xff;
      } else {
        if ((get_cmdtbl_flags(i) & FL_SPECIAL_INF) == FL_SPECIAL_INF) {
          param[0]=0;
          param[1]=(t >> 8);
          param[2]= t & 0xff;
        } else {
          param[0]=(t >> 8);
          param[1]= t & 0xff;
          param[2]=0x00;
        }
      }
      param_len=3;
      }
      break;



    case VT_YEAR:
    case VT_DAYMONTH:
    case VT_TIME:
    case VT_VACATIONPROG:
    case VT_DATETIME:
      {

      int d = 1; int m = 1; int y = 0xFF; int hour = y; int min = y; int sec = y;
      uint8_t date_flag = 0;
      const char *error_msg = NULL;
      if (val[0]!='\0') {
        switch(decodedTelegram.type){
          case VT_YEAR:
            strcpy_P(sscanf_buf, PSTR("%d"));
            if (1 != sscanf(val, sscanf_buf, &y)) {
              decodedTelegram.error = 262;
              error_msg = PSTR("year!");
            } else {

              printFmtToDebug(PSTR("year: %d\r\n"), y);
              date_flag = 0x0F;
            }
          break;
          case VT_DAYMONTH:
          case VT_VACATIONPROG:
            strcpy_P(sscanf_buf, PSTR("%d.%d."));
            if (2 != sscanf(val, sscanf_buf, &d, &m)) {
              decodedTelegram.error = 262;
              error_msg = PSTR("day/month!");
            } else {

              printFmtToDebug(PSTR("day/month: %d.%d.\r\n"), d, m);
              if (decodedTelegram.type == VT_DAYMONTH) {
                date_flag = 0x16;
              } else {
                date_flag = 0x17;
              }
            }
          break;
          case VT_TIME:
            strcpy_P(sscanf_buf, PSTR("%d:%d:%d"));
            if (3 != sscanf(val, sscanf_buf, &hour, &min, &sec)) {
              decodedTelegram.error = 262;
              error_msg = PSTR("time!");
            } else {

              printFmtToDebug(PSTR("time: %d:%d:%d\r\n"), hour, min, sec);
              date_flag = 0x1D;
            }
          break;
          case VT_DATETIME:
            strcpy_P(sscanf_buf, PSTR("%d.%d.%d_%d:%d:%d"));
            if (6 != sscanf(val, sscanf_buf, &d, &m, &y, &hour, &min, &sec)) {
              decodedTelegram.error = 262;
              error_msg = PSTR("date/time!");
            } else {

              printFmtToDebug(PSTR("date time: %d.%d.%d %d:%d:%d\r\n"), d, m, y, hour, min, sec);
              date_flag = 0x00;
            }
          break;
        }
        if(decodedTelegram.error == 262){
          printToDebug(PSTR("Too few/many arguments for "));
          printlnToDebug(error_msg);
          return 0;
        }
        param[0]=decodedTelegram.enable_byte;
      } else {
        param[0]=decodedTelegram.enable_byte-1;
      }

      uint8_t dow = dayofweek(d,m,y);
      if (decodedTelegram.type == VT_VACATIONPROG) {
        y = 1900; dow = 0; hour = dow; min = dow; sec = dow; date_flag = 0x17;
      }
      param[1]=y-1900;
      param[2]=m;
      param[3]=d;
      param[4]=dow;
      param[5]=hour;
      param[6]=min;
      param[7]=sec;
      param[8]=date_flag;
      param_len=9;
      }
      break;



    case VT_TIMEPROG:
      {




      int h1s=0x80,m1s=0x00,h2s=0x80,m2s=0x00,h3s=0x80,m3s=0x00;
      int h1e=0x80,m1e=0x00,h2e=0x80,m2e=0x00,h3e=0x80,m3e=0x00;
      int ret;
      strcpy_P(sscanf_buf, PSTR("%d:%d-%d:%d_%d:%d-%d:%d_%d:%d-%d:%d"));
      ret=sscanf(val,sscanf_buf,&h1s,&m1s,&h1e,&m1e,&h2s,&m2s,&h2e,&m2e,&h3s,&m3s,&h3e,&m3e);

      if (ret<4)
        return 0;
      param[0]=h1s;
      param[1]=m1s;
      param[2]=h1e;
      param[3]=m1e;

      param[4]=h2s;
      param[5]=m2s;
      param[6]=h2e;
      param[7]=m2e;

      param[8]=h3s;
      param[9]=m3s;
      param[10]=h3e;
      param[11]=m3e;
      param_len=12;
      }
      break;

    case VT_CUSTOM_ENUM:
    {
      uint8_t t=atoi(val);
      bus->Send(TYPE_QUR, c, msg, tx_msg);
      int data_len;
      if (bus->getBusType() == BUS_LPB) {
        data_len=msg[bus->getLen_idx()]-14;
      } else {
        data_len=msg[bus->getLen_idx()]-11;
      }

      if (data_len > 18) {
        printFmtToDebug(PSTR("Set failed, invalid data length: %d\r\n"), data_len);
        return 0;
      }

      uint8_t idx = pgm_read_byte_far(decodedTelegram.enumstr+0);

      for (int x=bus->getPl_start();x<bus->getPl_start()+data_len;x++) {
        param[x-bus->getPl_start()] = msg[x];
      }
      param[idx] = t;
      param_len = data_len;
      break;
    }
# 4015 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
    default:
      printlnToDebug(PSTR("Unknown type or read-only parameter"));
      return 2;
    break;
  }


  printFmtToDebug(PSTR("setting line: %g val: "), line);
  SerialPrintRAW(param,param_len);
  writelnToDebug();

  uint8_t t=setcmd?TYPE_SET:TYPE_INF;

  if ((get_cmdtbl_flags(i) & FL_SPECIAL_INF) == FL_SPECIAL_INF) {
    c=((c & 0xFF000000) >> 8) | ((c & 0x00FF0000) << 8) | (c & 0x0000FF00) | (c & 0x000000FF);
  }


  if (!bus->Send(t
             , c
             , msg
             , tx_msg
             , param
             , param_len
             , setcmd))
  {
    printFmtToDebug(PSTR("set failed\r\n"));
    return 0;
  }


  if (verbose) {
    printTelegram(tx_msg, line);
#ifdef LOGGER
    LogTelegram(tx_msg);
#endif
  }


  if (t!=TYPE_SET) return 1;


  printTelegram(msg, line);
#ifdef LOGGER
  LogTelegram(msg);
#endif

  if (msg[4+(bus->getBusType()*4)]!=TYPE_ACK) {
    printlnToDebug(PSTR("set failed NACK"));
    return 0;
  }

  return 1;
}
# 4086 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
int queryDefaultValue(float line, byte *msg, byte *tx_msg) {
  uint32_t c;
  resetDecodedTelegram();
  if (line < 0) {
    decodedTelegram.error = 258;
    return 0;
  }
  int i=findLine(line,0,&c);
  if ( i < 0) {
    decodedTelegram.error = 258;
    return 0;
  } else {
    if (!bus->Send(TYPE_QRV, c, msg, tx_msg)) {
      decodedTelegram.error = 261;
      return 0;
    } else {

      if (verbose) {
        printTelegram(tx_msg, line);
#ifdef LOGGER
        LogTelegram(tx_msg);
#endif
      }


      printTelegram(msg, line);
#ifdef LOGGER
      LogTelegram(msg);
#endif
    }
  }
  return 1;
}

const char* printError(uint16_t error) {
  const char *errormsgptr;
  switch (error) {
    case 0: errormsgptr = PSTR(""); break;
    case 7: errormsgptr = PSTR(" (parameter not supported)"); break;
    case 256: errormsgptr = PSTR(" - decoding error"); break;
    case 257: errormsgptr = PSTR(" unknown command"); break;
    case 258: errormsgptr = PSTR(" - not found"); break;
    case 259: errormsgptr = PSTR(" no enum str"); break;
    case 260: errormsgptr = PSTR(" - unknown type"); break;
    case 261: errormsgptr = PSTR(" query failed"); break;
    default: if (error < 256) errormsgptr = PSTR(" (bus error)"); else errormsgptr = PSTR(" (??? error)"); break;
  }
  return errormsgptr;
}
# 4152 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
char *build_pvalstr(bool extended) {
  int len = 0;
  outBuf[len] = 0;
  if (extended && decodedTelegram.error != 257) {
    len+=sprintf_P(outBuf, PSTR("%g "), decodedTelegram.prognr);

    len+=strlen(strcpy_PF(outBuf + len, decodedTelegram.catdescaddr));
    len+=strlen(strcpy_P(outBuf + len, PSTR(" - ")));
#ifdef AVERAGES
    if (decodedTelegram.prognr >= BSP_AVERAGES && decodedTelegram.prognr < BSP_AVERAGES + numAverages) {
      len+=strlen(strcpy_P(outBuf + len, PSTR(STR_24A_TEXT)));
      len+=strlen(strcpy_P(outBuf + len, PSTR(". ")));
    }
#endif
    len+=strlen(strcpy_PF(outBuf + len, decodedTelegram.prognrdescaddr));
    if (decodedTelegram.sensorid) {
      len+=sprintf_P(outBuf + len, PSTR(" #%d"), decodedTelegram.sensorid);
    }
    len+=strlen(strcpy_P(outBuf + len, PSTR(": ")));
  }
  if (decodedTelegram.value[0] != 0 && decodedTelegram.error != 260) {
    len+=strlen(strcpy(outBuf + len, decodedTelegram.value));
  }
  if (decodedTelegram.data_type == DT_ENUM || decodedTelegram.data_type == DT_BITS) {
    if (decodedTelegram.enumdescaddr) {
      strcpy_P(outBuf + len, PSTR(" - "));
      strcat_PF(outBuf + len, decodedTelegram.enumdescaddr);
      len+=strlen(outBuf + len);
     }
  } else {
    if (decodedTelegram.unit[0] != 0 && decodedTelegram.error != 7) {
      strcpy_P(outBuf + len, PSTR(" "));
      strcat(outBuf + len, decodedTelegram.unit);
      len+=strlen(outBuf + len);
    }
  }
  if (decodedTelegram.telegramDump) {
    strcpy_P(outBuf + len, PSTR(" "));
    strcat(outBuf + len, decodedTelegram.telegramDump);
    len+=strlen(outBuf + len);
  }

  strcpy_P(outBuf + len, printError(decodedTelegram.error));
  return outBuf;
}

void query_program_and_print_result(int line, const char* prefix, const char* suffix) {
  if (prefix) printToWebClient(prefix);
  query(line);
  if (decodedTelegram.prognr < 0) return;
  printToWebClient_prognrdescaddr();
  if (suffix) {
    printToWebClient(suffix);
  } else {
    printToWebClient(PSTR(": "));
  }
  printToWebClient(build_pvalstr(0));
}
# 4226 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
void query_printHTML() {
  if (decodedTelegram.msg_type == TYPE_ERR) {
    if (decodedTelegram.error == 7 && !show_unknown) return;
    printToWebClient(PSTR("<tr style='color: #7f7f7f'><td>"));
  } else {
    printToWebClient(PSTR("<tr><td>"));
  }
  printToWebClient(build_pvalstr(1));
# 4251 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
    const char fieldDelimiter[] PROGMEM = "</td><td>";
      printToWebClient(fieldDelimiter);
      if (decodedTelegram.msg_type != TYPE_ERR && decodedTelegram.type != VT_UNKNOWN) {
        if (decodedTelegram.data_type == DT_ENUM || decodedTelegram.data_type == DT_BITS) {
          printToWebClient(PSTR("<select "));
          if (decodedTelegram.data_type == DT_BITS) {
            printToWebClient(PSTR("multiple "));
          }
          printFmtToWebClient(PSTR("id='value%g'>\r\n"), decodedTelegram.prognr);
          uint16_t value = 0;
          if (decodedTelegram.data_type == DT_BITS) {
            for (int i = 0; i < 8; i++) {
              if (decodedTelegram.value[i] == '1') value+=1<<(7-i);
            }
          } else {
            value = strtod(decodedTelegram.value, NULL);
            if ((decodedTelegram.type == VT_BINARY_ENUM || decodedTelegram.type == VT_ONOFF || decodedTelegram.type == VT_YESNO|| decodedTelegram.type == VT_CLOSEDOPEN || decodedTelegram.type == VT_VOLTAGEONOFF) && value != 0) {
              value = 1;
            }
            if (decodedTelegram.readwrite == FL_WONLY) value = 65535;
          }
          listEnumValues(decodedTelegram.enumstr, decodedTelegram.enumstr_len, STR_OPTION_VALUE, PSTR("'>"), STR_SELECTED, STR_CLOSE_OPTION, NULL, value,
            decodedTelegram.data_type == DT_BITS?(PRINT_VALUE|PRINT_DESCRIPTION|PRINT_VALUE_FIRST|PRINT_ENUM_AS_DT_BITS):
            (PRINT_VALUE|PRINT_DESCRIPTION|PRINT_VALUE_FIRST),
            decodedTelegram.type==VT_ENUM?PRINT_DISABLED_VALUE:DO_NOT_PRINT_DISABLED_VALUE);
          printToWebClient(PSTR("</select>"));
          printToWebClient(fieldDelimiter);
          if (decodedTelegram.readwrite != FL_RONLY) {
            printToWebClient(PSTR("<input type=button value='Set' onclick=\"set"));
            if (decodedTelegram.type == VT_BIT) {
              printToWebClient(PSTR("bit"));
            }
            printFmtToWebClient(PSTR("(%g)\">"), decodedTelegram.prognr);
          }
        } else {
          printFmtToWebClient(PSTR("<input type=text id='value%g' VALUE='%s'>"), decodedTelegram.prognr, decodedTelegram.value);
          printToWebClient(fieldDelimiter);
          if (decodedTelegram.readwrite != FL_RONLY) {
            printFmtToWebClient(PSTR("<input type=button value='Set' onclick=\"set(%g)\">"), decodedTelegram.prognr);
          }
        }
      }
      printToWebClient(PSTR("</td></tr>\r\n"));
# 4302 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
  flushToWebClient();
}

#ifdef BME280
void tcaselect(uint8_t i) {
  if (i > 7) return;

  Wire.beginTransmission(TCA9548A_ADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}
#endif
# 4327 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
void queryVirtualPrognr(float line, int table_line) {
  loadCategoryDescAddr();
  printFmtToDebug(PSTR("\r\nVirtual parameter %g queried. Table line %d\r\n"), line, table_line);
  decodedTelegram.msg_type = TYPE_ANS;
  decodedTelegram.prognr = line;
  switch (recognizeVirtualFunctionGroup(line)) {
    case 1: {
      uint32_t val = 0;
      switch ((uint16_t)line) {
        case BSP_INTERNAL + 0: val = brenner_duration; break;
        case BSP_INTERNAL + 1: val = brenner_count; break;
        case BSP_INTERNAL + 2: val = brenner_duration_2; break;
        case BSP_INTERNAL + 3: val = brenner_count_2; break;
        case BSP_INTERNAL + 4: val = TWW_duration; break;
        case BSP_INTERNAL + 5: val = TWW_count; break;
        case BSP_INTERNAL + 6: val = 0; break;
      }
#if !defined(ESP32)
      sprintf_P(decodedTelegram.value, PSTR("%ld"), val);
#else
      sprintf_P(decodedTelegram.value, PSTR("%d"), val);
#endif
      return;
    }
    case 2: {
  #ifdef AVERAGES
      size_t tempLine = roundf(line - BSP_AVERAGES);
      _printFIXPOINT(decodedTelegram.value, avgValues[tempLine], 1);
      return;
   #endif
      break;
    }
    case 3: {
#ifdef DHT_BUS
      size_t log_sensor = roundf(line - BSP_DHT22);
      int tempLine = (int)roundf((line - BSP_DHT22) * 10) % 10;
      if (tempLine == 0) {
        sprintf_P(decodedTelegram.value, PSTR("%d"), DHT_Pins[log_sensor]);
        return;
      }
      unsigned long temp_timer = millis();
      if (DHT_Timer + 2000 < temp_timer || DHT_Timer > temp_timer) last_DHT_pin = 0;
      if (last_DHT_pin != DHT_Pins[log_sensor]) {
        last_DHT_pin = DHT_Pins[log_sensor];
        DHT_Timer = millis();
#if defined(ESP32)
        dht.setup(last_DHT_pin, DHTesp::DHT22);
#else
        dht.setup(last_DHT_pin, DHTesp::AUTO_DETECT);
#endif
      }

      printFmtToDebug(PSTR("DHT22 sensor: %d - "), last_DHT_pin);
      switch (dht.getStatus()) {
        case DHTesp::ERROR_CHECKSUM:
          decodedTelegram.error = 256;
          printlnToDebug(PSTR("Checksum error"));
          break;
          case DHTesp::ERROR_TIMEOUT:
          decodedTelegram.error = 261;
          printlnToDebug(PSTR("Time out error"));
         break;
        default:
          printlnToDebug(PSTR("OK"));
          break;
      }

      float hum = dht.getHumidity();
      float temp = dht.getTemperature();
      if (hum > 0 && hum < 101) {
        printFmtToDebug(PSTR("#dht_temp[%d]: %.2f, hum[%d]:  %.2f\r\n"), log_sensor, temp, log_sensor, hum);
        switch (tempLine) {
          case 1:
            _printFIXPOINT(decodedTelegram.value, temp, 2);
            break;
          case 2:
            _printFIXPOINT(decodedTelegram.value, hum, 2);
            break;
          case 3:
            _printFIXPOINT(decodedTelegram.value, (216.7*(hum/100.0*6.112*exp(17.62*temp/(243.12+temp))/(273.15+temp))), 2);
            break;
        }
      } else {
        undefinedValueToBuffer(decodedTelegram.value);
      }
      return;
#endif
      break;
    }
    case 4: {
#ifdef ONE_WIRE_BUS
      size_t log_sensor = roundf(line - BSP_ONEWIRE);
      if (One_Wire_Pin && numSensors) {
        switch (((int)roundf((line - BSP_ONEWIRE) * 10)) % 10) {
          case 0:
            DeviceAddress device_address;
            sensors->getAddress(device_address, log_sensor);
            bin2hex(decodedTelegram.value, device_address, 8, 0);
            break;
          case 1: {
            float t=sensors->getTempCByIndex(log_sensor);
            if (t == DEVICE_DISCONNECTED_C) {
              decodedTelegram.error = 261;
              undefinedValueToBuffer(decodedTelegram.value);
              return;
            }
            _printFIXPOINT(decodedTelegram.value, t, 2);
            break;
          }
          default: break;
        }
        return;
      }
  #endif
      break;
    }
    case 5: {
#ifdef MAX_CUL
      size_t log_sensor = roundf(line - BSP_MAX);
      if (enable_max_cul) {
        if (max_devices[log_sensor]) {
          switch (((int)roundf((line - BSP_MAX) * 10)) % 10){
            case 0:
              strcpy(decodedTelegram.value, max_device_list[log_sensor]);
              break;
            case 1:
              if (max_dst_temp[log_sensor] > 0) {
                sprintf_P(decodedTelegram.value, PSTR("%.2f"), ((float)max_cur_temp[log_sensor] / 10));
              } else {
                decodedTelegram.error = 261;
                undefinedValueToBuffer(decodedTelegram.value);
              }
              break;
            case 2:
              if (max_dst_temp[log_sensor] > 0) {
                sprintf_P(decodedTelegram.value, PSTR("%.2f"), ((float)max_dst_temp[log_sensor] / 2));
              } else {
                decodedTelegram.error = 261;
                undefinedValueToBuffer(decodedTelegram.value);
              }
              break;
            case 3:
              if (max_valve[log_sensor] > -1) {
                sprintf_P(decodedTelegram.value, PSTR("%d"), max_valve[log_sensor]);
              } else {
                decodedTelegram.error = 261;
                undefinedValueToBuffer(decodedTelegram.value);
              }
              break;
          }
          return;
        }
      }
  #endif
    break;
    }
    case 6: {
      sprintf_P(decodedTelegram.value, PSTR("%.2f"), custom_floats[((uint16_t)line) - BSP_FLOAT]);
      return;
    }
    case 7: {
      sprintf_P(decodedTelegram.value, PSTR("%ld"), custom_longs[((uint16_t)line) - BSP_LONG]);
      return;
    }
    case 8: {
#ifdef BME280
      size_t log_sensor = roundf(line - BSP_BME280);
      uint8_t selector = ((int)roundf((line - BSP_BME280) * 10)) % 10;
      if (selector == 0) {
        if(BME_Sensors > 2){
          sprintf_P(decodedTelegram.value, PSTR("%02X-%02X"), log_sensor & 0x07, 0x76 + log_sensor / 8);
        } else {
          sprintf_P(decodedTelegram.value, PSTR("%02X"), 0x76 + log_sensor);
        }
        return;
      }
      if(BME_Sensors > 2){
        tcaselect(log_sensor & 0x07);
      }
      switch(bme[log_sensor].checkID()) {
        case 0x58: if(selector == 2 || selector == 5) selector = 6; break;
        case 0x60: break;
        default: selector = 6; break;
      }
      switch (selector) {
        case 1: _printFIXPOINT(decodedTelegram.value, bme[log_sensor].readTempC(), 2); break;
        case 2: _printFIXPOINT(decodedTelegram.value, bme[log_sensor].readHumidity(), 2); break;
        case 3: _printFIXPOINT(decodedTelegram.value, bme[log_sensor].readPressure(), 2); break;
        case 4: _printFIXPOINT(decodedTelegram.value, bme[log_sensor].readAltitudeMeter(), 2); break;
        case 5: {float temp = bme[log_sensor].readTempC(); _printFIXPOINT(decodedTelegram.value, (216.7*(bme[log_sensor].readHumidity()/100.0*6.112*exp(17.62*temp/(243.12+temp))/(273.15+temp))), 2);} break;
        case 6: decodedTelegram.error = 261; undefinedValueToBuffer(decodedTelegram.value); break;
      }
      return;
#endif
      break;
    }
    case 9:
      {
#if defined(BLE_SENSORS) && defined(ESP32)
        size_t log_sensor = (int)roundf(line - BSP_BLE);
        uint8_t selector = ((int)roundf((line - BSP_BLE) * 10)) % 10;
        if (!BLESensors_statusIsCorrect(log_sensor) && selector != 0) selector = 5;
        switch (selector) {
          case 0: bin2hex(decodedTelegram.value, ((byte *)BLE_sensors_macs) + log_sensor * sizeof(mac), sizeof(mac), ':'); break;
          case 1: _printFIXPOINT(decodedTelegram.value, BLESensors_readTemp(log_sensor), 2); break;
          case 2: _printFIXPOINT(decodedTelegram.value, BLESensors_readHumidity(log_sensor), 2); break;
          case 3:
            decodedTelegram.error = 261;
            undefinedValueToBuffer(decodedTelegram.value);
            break;
          case 4: _printFIXPOINT(decodedTelegram.value, BLESensors_readVbat(log_sensor), 3); break;
          case 5:
            decodedTelegram.error = 261;
            undefinedValueToBuffer(decodedTelegram.value);
            break;
        }
        return;
#endif
        break;
      }
    case 10: {
      size_t log_sensor = (int)roundf(line - BSP_MQTT_TEMP);
      uint8_t selector = ((int)roundf((line - BSP_MQTT_TEMP) * 10)) % 10;
      switch (selector) {
        case 0: {
          if (MQTTTemps[log_sensor] > MQTT_TEMP_VALID_THRESHOLD) {
            decodedTelegram.error = 261;
            undefinedValueToBuffer(decodedTelegram.value);
          } else {
            _printFIXPOINT(decodedTelegram.value, MQTTTemps[log_sensor], 2);
          }
          break;
        }
        case 1: strcpy(decodedTelegram.value, MQTTSensors[log_sensor]); break;
        case 2: {
          if (MQTTTemps[log_sensor] == MQTT_TEMP_UNRESPONSIVE_SENSOR)
          {
            strcpy(decodedTelegram.value, "MQTT_TEMP_UNRESPONSIVE_SENSOR");
          } else if (MQTTTemps[log_sensor] == MQTT_TEMP_WINDOW_OPEN) {
            strcpy(decodedTelegram.value, "MQTT_TEMP_WINDOW_OPEN");
          } else if (MQTTTemps[log_sensor] == MQTT_TEMP_UNKNOWN) {
            strcpy(decodedTelegram.value, "MQTT_TEMP_UNKNOWN");
          } else {
            strcpy(decodedTelegram.value, "OK");
          }
          break;
        }
      }
      return;
    }
  }
  decodedTelegram.error = 7;
  decodedTelegram.msg_type = TYPE_ERR;
  return;
}
# 4602 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
void query(float line) {
  byte msg[33] = { 0 };
  byte tx_msg[33] = { 0 };
  uint32_t c;
  int i=0;
  int retry;
  resetDecodedTelegram();
#if defined(ESP32)
  esp_task_wdt_reset();
#endif

  i=findLine(line,0,&c);
  if (i>=0) {
    loadPrognrElementsFromTable(line, i);
    uint8_t flags = get_cmdtbl_flags(i);
    if (decodedTelegram.readwrite == FL_WONLY) {
      printFmtToDebug(PSTR("%g "), line);
      loadCategoryDescAddr();
      printToDebug(PSTR(" - "));
      printToDebug(decodedTelegram.prognrdescaddr);
      printToDebug(PSTR(" - write-only\r\n"));
      return;
    }


    if ((line >= BSP_INTERNAL && line < BSP_END)) {
      queryVirtualPrognr(line, i);
      return;
    }

    if (c!=CMD_UNKNOWN && (flags & FL_NO_CMD) != FL_NO_CMD) {
      if (bus->getBusType() != BUS_PPS) {
        retry=QUERY_RETRIES;
        while (retry) {
          uint8_t query_type = TYPE_QUR;
          if (bus->Send(query_type, c, msg, tx_msg)) {

            if (verbose) {
              printTelegram(tx_msg, line);
#ifdef LOGGER
              LogTelegram(tx_msg);
#endif
            }


            printTelegram(msg, line);
            printFmtToDebug(PSTR("#%g: "), line);
            printlnToDebug(build_pvalstr(0));
            SerialOutput->flush();
#ifdef LOGGER
            LogTelegram(msg);
#endif
            break;
          } else {
            printlnToDebug(printError(261));
            retry--;
          }
        }
        if (retry==0) {
          if (bus->getBusType() == BUS_LPB && msg[8] == TYPE_ERR) {
            printFmtToDebug(PSTR("error %d\r\n"), msg[9]);
          } else {
            printFmtToDebug(PSTR("%g\r\n"), line);
          }
          decodedTelegram.error = 261;
        }
      } else {
        uint32_t cmd = get_cmdtbl_cmd(i);
        uint16_t temp_val = 0;
        switch (decodedTelegram.type) {



          case VT_ONOFF:
          case VT_YESNO: temp_val = pps_values[((uint16_t)line)-15000]; decodedTelegram.isswitch = 1; break;


          default: temp_val = pps_values[((uint16_t)line)-15000]; break;
        }

        msg[1] = ((cmd & 0x00FF0000) >> 16);
        msg[4+(bus->getBusType()*4)]=TYPE_ANS;
        msg[bus->getPl_start()]=temp_val >> 8;
        msg[bus->getPl_start()+1]=temp_val & 0xFF;






        printTelegram(msg, line);

        printFmtToDebug(PSTR("#%g: "), line);
        printlnToDebug(build_pvalstr(0));
        SerialOutput->flush();
      }
    } else {


    }
  } else {


  }
}
# 4731 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
void query(float line_start
          , float line_end
          , bool no_print)
{
  float line = line_start;
  do {
    query(line);
    if (decodedTelegram.prognr != -1) {
      if (!no_print) {
        if (!client.connected()) return;
        query_printHTML();
      }
    }
    line = get_next_prognr(line, findLine(line, 0, NULL));
  } while(line >= line_start && line <= line_end);
}

void GetDevId() {
  byte msg[33] = { 0 };
  byte tx_msg[33] = { 0 };
  bus->Send(TYPE_QUR, 0x053D0064, msg, tx_msg);
  my_dev_fam = msg[10+bus->getBusType()*4];
  my_dev_var = msg[12+bus->getBusType()*4];
  my_dev_id = (msg[15+bus->getBusType()*4] << 24) + (msg[16+bus->getBusType()*4] << 16) + (msg[17+bus->getBusType()*4] << 8) + (msg[18+bus->getBusType()*4]);







  return;
}
# 4779 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
void SetDevId() {
  if (fixed_device_family < 1) {
    GetDevId();






  } else {
    my_dev_fam = fixed_device_family;
    my_dev_var = fixed_device_variant;
  }
# 4813 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
  printFmtToDebug(PSTR("Device family: %d\r\nDevice variant: %d\r\n"), my_dev_fam, my_dev_var);
}
# 4829 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
void SetDateTime() {
  byte rx_msg[33];
  byte tx_msg[33];
  uint32_t c;

#if defined(ESP32) && defined(USE_NTP)
  printlnToDebug(PSTR("Trying to get NTP time..."));
  struct tm timeinfo;
  configTime(0, 0, ntp_server);
  setenv("TZ",local_timezone,1);
  tzset();

  if(getLocalTime(&timeinfo)){
    printFmtToDebug(PSTR("Date and time acquired: %02d.%02d.%02d %02d:%02d:%02d\r\n"), timeinfo.tm_mday, timeinfo.tm_mon+1, timeinfo.tm_year-100, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

    return;
  } else {
    printlnToDebug(PSTR("Acquisition failed, trying again in one minute..."));
  }
#endif

  if (bus->getBusType() != BUS_PPS) {
    findLine(0,0,&c);
    if (c!=CMD_UNKNOWN) {
      if (bus->Send(TYPE_QUR, c, rx_msg, tx_msg)) {
        if (bus->getBusType() == BUS_LPB) {
          setTime(rx_msg[18], rx_msg[19], rx_msg[20], rx_msg[16], rx_msg[15], rx_msg[14]+1900);
        } else {
          setTime(rx_msg[14], rx_msg[15], rx_msg[16], rx_msg[12], rx_msg[11], rx_msg[10]+1900);
        }
      }
    }
  }
}

#ifdef IPWE
#include "include/print_ipwe.h"
#endif
# 4881 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
uint16_t setPPS(uint8_t pps_index, int16_t value) {
  uint16_t log_parameter = 0;
  if (pps_values[pps_index] != value) {
    if (LoggingMode & CF_LOGMODE_SD_CARD) {
      for (int i=0; i < numLogValues; i++) {
        if (log_parameters[i].number == 15000 + pps_index) {
          log_parameter = log_parameters[i].number;
        }
      }
    }
    pps_values[pps_index] = value;
  }
  return log_parameter;
}

#if defined LOGGER || defined WEBSERVER
# 4909 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
void transmitFile(File dataFile) {
  int logbuflen = (OUTBUF_USEFUL_LEN + OUTBUF_LEN > 1024)?1024:(OUTBUF_USEFUL_LEN + OUTBUF_LEN);
  byte *buf = 0;
#ifdef ESP32
  buf = (byte*)malloc(4096);
#endif
  if (buf) logbuflen=4096; else buf=(byte*)bigBuff;
  flushToWebClient();
  int chars_read = dataFile.read(buf, logbuflen);
  if (chars_read < 0) {
   printToWebClient(PSTR("Error: Failed to read from SD card - if problem remains after reformatting, card may be incompatible."));
   forcedflushToWebClient();
  }
  while (chars_read == logbuflen && client.connected()) {
    client.write(buf, logbuflen);
    chars_read = dataFile.read(buf, logbuflen);
#if defined(ESP32)
    esp_task_wdt_reset();
#endif
    }
  if (chars_read > 0 && client.connected()) client.write(buf, chars_read);
  if (buf != (byte*)bigBuff) free(buf);
}

#endif
# 4948 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
void resetBoard() {
#ifdef MQTT
  mqtt_disconnect();
#endif
  forcedflushToWebClient();
  delay(10);
  client.stop();
  delay(300);
#if defined(__SAM3X8E__)

  rstc_start_software_reset(RSTC);
  while (1==1) {}
#elif defined(ESP32)
  ESP.restart();
#else
  printlnToDebug(PSTR("Reset function not implementing"));
#endif

}

#ifdef AVERAGES
void resetAverageCalculation() {
  for (int i=0;i<numAverages;i++) {
    avgValues[i] = 0;
    avgValues_Old[i] = -9999;
    avgValues_Current[i] = 0;
  }
  avgCounter = 1;
  #ifdef LOGGER
  SD.remove(averagesFileName);
  #endif
}
#endif


#ifdef LOGGER

const char* datalogFileHeader = PSTR("Milliseconds;Date;Parameter;Description;Value;Unit\r\n");

const char *cleanupDatalog(unsigned nDays) {


  unsigned long spaceRequired=MINIMUM_FREE_SPACE_ON_SD, spaceAvailable =
#ifdef ESP32
    SD.totalBytes() - SD.usedBytes();
#else
    SD.vol()->freeClusterCount() * SD.vol()->bytesPerCluster();
#endif
  {
    File indexFile = SD.open(datalogIndexFileName);
    if (!indexFile) return PSTR("Cannot open datalog index");
    unsigned long nBytes = nDays * datalogIndexEntrySize;
    spaceRequired += nBytes;
    long indexOffset = indexFile.size() - nBytes;
    if (indexOffset <= 0) return PSTR("Nothing to do (not that many days in the datalog)");

    File dataFile = SD.open(datalogFileName);
    if (!dataFile) return PSTR("Cannot open datalog");
    File indexTmpFile = SD.open(datalogIndexTemporaryFileName, FILE_WRITE);
    if (!indexTmpFile) return PSTR("Cannot open temporary index");
    compactDate_t date;
    unsigned long pos, dataStart=0, dataOffset=0, nDataBytes=0;
    indexFile.seek(indexOffset);
    while (nDays--) {
      if (indexFile.read((byte*)&date,sizeof(date)) != sizeof(date) ||
          indexFile.read((byte*)&pos ,sizeof(pos )) != sizeof(pos ))
        return PSTR("Error reading index file");
      if (!dataStart) {
        dataStart = pos;
        dataOffset = pos - strlen(datalogFileHeader);
        nDataBytes = dataFile.size() - dataStart;
        spaceRequired += nDataBytes;
        if (spaceRequired > spaceAvailable) return PSTR("Not enough space on device");
      }
      pos -= dataOffset;
      if (indexTmpFile.write((byte*)&date,sizeof(date)) != sizeof(date) ||
          indexTmpFile.write((byte*)&pos ,sizeof(pos )) != sizeof(pos ))
        return PSTR("Error writing index file");
    }

    File dataTmpFile = SD.open(datalogTemporaryFileName, FILE_WRITE);
    if (!dataTmpFile) return PSTR("Cannot open temporary datalog");

    unsigned bufSize=1, maxSize=sizeof(bigBuff);
    while (bufSize <= maxSize) bufSize <<= 1;
    bufSize >>= 1;

    dataTmpFile.print(datalogFileHeader);
    dataFile.seek(dataStart);
    unsigned long i=0;
    while (nDataBytes) {
#ifdef ESP32
      esp_task_wdt_reset();
#endif
      if (++i % 10 == 0) {
        client.write('.');
        if (i % 100 == 0) client.write(' ');
        if (i % 500 == 0) client.write('\r'), client.write('\n');
        client.flush();
      }
      unsigned nBytesToDo = nDataBytes<bufSize ? nDataBytes : bufSize;
      if (dataFile.read((byte*)bigBuff, nBytesToDo) != (int)nBytesToDo)
        return PSTR("Error reading datalog");
      if (dataTmpFile.write((byte*)bigBuff, nBytesToDo) != nBytesToDo)
        return PSTR("Error writing datalog");
      nDataBytes -= nBytesToDo;
    }
  }

  SD.remove(datalogFileName);
  SD.rename(datalogTemporaryFileName, datalogFileName);
  SD.remove(datalogIndexFileName);
  SD.rename(datalogIndexTemporaryFileName, datalogIndexFileName);
  return PSTR("Success");
}

void readFirstAndPreviousDatalogDateFromFile() {
  firstDatalogDate.combined = previousDatalogDate.combined = 0;
  File indexFile = SD.open(datalogIndexFileName, FILE_READ);
  if (indexFile) {
    unsigned long indexFileSize = indexFile.size();
    if (indexFileSize >= datalogIndexEntrySize) {
      indexFile.read((byte*)&firstDatalogDate, sizeof(firstDatalogDate));
      indexFile.seek(indexFileSize - datalogIndexEntrySize);
      indexFile.read((byte*)&previousDatalogDate, sizeof(previousDatalogDate));
    }
    indexFile.close();
  }
}

void createDatalogIndexFile() {
  SD.remove(datalogIndexFileName);
  File indexFile = SD.open(datalogIndexFileName, FILE_WRITE);
  if (indexFile) indexFile.close();
  firstDatalogDate.combined = previousDatalogDate.combined = 0;
}

bool createdatalogFileAndWriteHeader() {
  File dataFile = SD.open(datalogFileName, FILE_WRITE);
  if (dataFile) {
    dataFile.print(datalogFileHeader);
    dataFile.close();
    outBuf[0] = 0;
    createDatalogIndexFile();
    return true;
  }
  return false;
}

#endif

#ifdef MAX_CUL
void connectToMaxCul() {
  if (max_cul) {
    max_cul->stop();
    delete max_cul;
    max_cul = NULL;
    if (!enable_max_cul) return;
  }

  max_cul = new ComClient();
  printToDebug(PSTR("Connection to max_cul: "));
  if (max_cul->connect(IPAddress(max_cul_ip_addr[0], max_cul_ip_addr[1], max_cul_ip_addr[2], max_cul_ip_addr[3]), 2323)) {
    printlnToDebug(PSTR("established"));
  } else {
    printlnToDebug(PSTR("failed"));
  }
}
#endif

void clearEEPROM(void) {
  printlnToDebug(PSTR("Clearing EEPROM..."));
#if defined(ESP32)
  for (uint16_t x=0; x<EEPROM_SIZE; x++) {
    EEPROM.write(x, 0xFF);
  }
  EEPROM.commit();
#else
  uint8_t empty_block[4096] = { 0xFF };
  EEPROM.fastBlockWrite(0, empty_block, 4096);
#endif
  printlnToDebug(PSTR("Cleared EEPROM"));
}

void internalLEDBlinking(uint16_t period, uint16_t count) {
  for (uint16_t i=0; i<count; i++) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(period);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(period);
  }
  digitalWrite(LED_BUILTIN, LOW);
}

#include "include/pps_handling.h"
#include "include/broadcast_msg_handling.h"
# 5161 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
void loop() {
  byte msg[33] = { 0 };
  byte tx_msg[33] = { 0 };
  char c = '\0';
  const byte MaxArrayElement=252;
  char cLineBuffer[MaxArrayElement];
  byte bPlaceInBuffer;
  uint16_t log_now = 0;

#ifndef WIFI
 if (ip_addr[0] == 0 || useDHCP) {
    switch (Ethernet.maintain()) {
      case 1:

        printlnToDebug(PSTR("Error: renewed fail"));
        break;
      case 2:

        printlnToDebug(PSTR("Renewed success"));

        printToDebug(PSTR("My IP address: "));
        {IPAddress t = Ethernet.localIP();
        printFmtToDebug(PSTR("%d.%d.%d.%d\r\n"), t[0], t[1], t[2], t[3]);}
        break;
      case 3:

        printlnToDebug(PSTR("Error: rebind fail"));
        break;
      case 4:

        printlnToDebug(PSTR("Rebind success"));

        printToDebug(PSTR("My IP address: "));
        {IPAddress t = Ethernet.localIP();
        printFmtToDebug(PSTR("%d.%d.%d.%d\r\n"), t[0], t[1], t[2], t[3]);}
      break;

      default:

        break;
    }
  }
#endif




  bool busmsg = false;
  if (monitor) {
    busmsg=bus->Monitor(msg);
    if (busmsg==true) {
      LogTelegram(msg);
    }
  }
  if (!monitor || busmsg == true) {


    if (bus->GetMessage(msg) || busmsg == true) {

      if (verbose && bus->getBusType() != BUS_PPS && !monitor) {
        printTelegram(msg, -1);
        LogTelegram(msg);
      }


      broadcast_msg_handling(msg);


      if (bus->getBusType() == BUS_PPS) {
        log_now = pps_bus_handling(msg);
      }

    }




    if (pps_values[PPS_QTP] == 0xEA && pps_write == 1) {
      if (millis() - pps_mcba_timer > 500) {
        pps_query_mcba();
        pps_mcba_timer = millis();
      }
    }

  }


  client = server->available();
  if ((client || SerialOutput->available()) && client_flag == false) {
    client_flag = true;

    IPAddress remoteIP = client.remoteIP();

    if ((trusted_ip_addr[0] != 0 && ! (remoteIP == trusted_ip_addr))
       && (trusted_ip_addr2[0] != 0 && ! (remoteIP == trusted_ip_addr2))) {

      printFmtToDebug(PSTR("Rejected access from %d.%d.%d.%d (Trusted 1: %d.%d.%d.%d, Trusted 2: %d.%d.%d.%d.\r\n"),
        remoteIP[0], remoteIP[1], remoteIP[2], remoteIP[3],
        trusted_ip_addr[0], trusted_ip_addr[1], trusted_ip_addr[2], trusted_ip_addr[3],
        trusted_ip_addr2[0], trusted_ip_addr2[1], trusted_ip_addr2[2], trusted_ip_addr2[3]);
      client.stop();
    }

    loopCount = 0;

    bPlaceInBuffer=0;
    while (client.connected() || SerialOutput->available()) {
      if (client.available() || SerialOutput->available()) {

        loopCount = 0;
        if (client.available()) {
          c = client.read();
          printFmtToDebug(PSTR("%c"), c);
        }
        if (SerialOutput->available()) {
          c = SerialOutput->read();
          printFmtToDebug(PSTR("%c"), c);
          int timeout = 0;
          while (SerialOutput->available() == 0 && c!='\r' && c!='\n') {
            delay(1);
            timeout++;
            if (timeout > 2000) {
              printlnToDebug(PSTR("Serial input timeout"));
              break;
            }
          }
        }

        if ((c!='\n') && (c!='\r') && (bPlaceInBuffer<MaxArrayElement)) {
          cLineBuffer[bPlaceInBuffer++]=c;
          continue;
        }

        writelnToDebug();



        static int buffershift = 48;
        size_t charcount=buffershift;
        uint8_t httpflags = 0;





        memset(outBuf,0,sizeof(outBuf));
        bool currentLineIsBlank = false;
        while (client.connected()) {
          if (client.available()) {
            char c = client.read();
            outBuf[charcount]=c;
            if (charcount < sizeof(outBuf)-1-buffershift) charcount++;
            if (c == '\n' && currentLineIsBlank) {
              break;
            }
            if (c == '\n') {

              currentLineIsBlank = true;
#ifdef WEBSERVER

              if (!(httpflags & HTTP_GZIP) && strstr_P(outBuf + buffershift,PSTR("Accept-Encoding")) != 0 && strstr_P(outBuf+16 + buffershift, PSTR("gzip")) != 0) {
                httpflags |= HTTP_GZIP;
              }
#endif
              if (!(httpflags & HTTP_ETAG)) {
                char *ptr = strstr_P(outBuf + buffershift, PSTR("If-None-Match:"));
                if (ptr != 0) {
                  httpflags |= HTTP_ETAG;
                  ptr = strchr(ptr, ':');
                  do{
                    ptr++;
                  } while (ptr[0] == ' ');
                  strcpy(outBuf, ptr);

                }
              }

              char base64_user_pass[88] = { 0 };
              int user_pass_len = strlen(USER_PASS);
              Base64.encode(base64_user_pass, USER_PASS, user_pass_len);
              if (!(httpflags & HTTP_AUTH) && USER_PASS[0] && strstr_P(outBuf + buffershift,PSTR("uthorization: Basic"))!=0 && strstr(outBuf + buffershift,base64_user_pass)!=0) {
                httpflags |= HTTP_AUTH;
              }
              memset(outBuf + buffershift,0, charcount);
              charcount=buffershift;
            } else if (c != '\r') {

              currentLineIsBlank = false;
            }
          }
        }
        cLineBuffer[bPlaceInBuffer++]=0;

        if (USER_PASS[0] && !(httpflags & HTTP_AUTH)) {
          printHTTPheader(HTTP_AUTH_REQUIRED, MIME_TYPE_TEXT_HTML, HTTP_ADD_CHARSET_TO_HEADER, HTTP_FILE_NOT_GZIPPED, HTTP_NO_DOWNLOAD, HTTP_DO_NOT_CACHE);
          printPStr(auth_req_html, sizeof(auth_req_html));
          forcedflushToWebClient();
          client.stop();
          break;
        }





#ifdef WEBSERVER

        if (!strncmp_P(cLineBuffer, PSTR("HEAD"), 4))
          httpflags |= HTTP_HEAD_REQ;
#endif
        char *u_s = strchr(cLineBuffer,' ');
        if (!u_s) u_s = cLineBuffer;
        char *u_e = strchr(u_s + 1,' ');
        if (u_e) u_e[0] = 0;
        if (u_s != cLineBuffer) strcpy(cLineBuffer, u_s + 1);

#ifdef IPWE
        if (enable_ipwe && !strcmp_P(cLineBuffer, PSTR("/ipwe.cgi"))) {
          Ipwe();
          break;
        }
#endif


        if (!strcmp_P(cLineBuffer, PSTR("/favicon.ico"))) {
          printHTTPheader(HTTP_OK, MIME_TYPE_IMAGE_ICON, HTTP_DO_NOT_ADD_CHARSET_TO_HEADER, HTTP_FILE_NOT_GZIPPED, HTTP_NO_DOWNLOAD, HTTP_AUTO_CACHE_AGE);
          printToWebClient(PSTR("\r\n"));
#ifdef WEBSERVER
          File dataFile = SD.open(cLineBuffer + 1);
          if (dataFile) {
            flushToWebClient();
            transmitFile(dataFile);
            dataFile.close();
          } else {
#endif
#if !defined(I_DO_NOT_NEED_NATIVE_WEB_INTERFACE)
            printPStr(favicon, sizeof(favicon));
#endif
            flushToWebClient();
#ifdef WEBSERVER
            }
#endif
          break;
        }

        if (!strcmp_P(cLineBuffer, PSTR("/favicon.svg"))) {
          printHTTPheader(HTTP_OK, MIME_TYPE_IMAGE_SVG, HTTP_DO_NOT_ADD_CHARSET_TO_HEADER, HTTP_FILE_NOT_GZIPPED, HTTP_NO_DOWNLOAD, HTTP_AUTO_CACHE_AGE);
          printToWebClient(PSTR("\r\n"));
#ifdef WEBSERVER
          File dataFile = SD.open(cLineBuffer + 1);
          if (dataFile) {
            flushToWebClient();
            transmitFile(dataFile);
            dataFile.close();
          } else {
#endif
#if !defined(I_DO_NOT_NEED_NATIVE_WEB_INTERFACE)
            printPStr(svg_favicon_header, sizeof(svg_favicon_header));
            printPStr(svg_favicon, sizeof(svg_favicon));
#endif
            flushToWebClient();
#ifdef WEBSERVER
            }
#endif
          break;
        }


        char *p=cLineBuffer;
        if (PASSKEY[0]) {

          p=strchr(cLineBuffer+1,'/');
          if (p==NULL) {
            break;
          }
          *p='\0';
          if (strncmp(cLineBuffer+1, PASSKEY, strlen(PASSKEY))) {
            printlnToDebug(PSTR("no matching passkey"));
            while(client.available()) client.read();
            client.stop();

            break;
          }
          *p='/';
        }

#ifdef WEBSERVER
        printToDebug(PSTR("URL: "));
        if (!strcmp_P(p, PSTR("/"))) {
          httpflags |= HTTP_GET_ROOT;
          strcpy_P(p + 1, PSTR("index.html"));
        }
          printlnToDebug(p);
        char *dot = strchr(p, '.');
        char *dot_t = NULL;
        while (dot) {
          dot_t = ++dot;
          dot = strchr(dot, '.');
        }
        dot = dot_t;

        int mimetype = recognize_mime(dot);

        if (mimetype) {
          File dataFile;


          int suffix = 0;
          if ((httpflags & HTTP_GZIP)) {
            suffix = strlen(p);
            strcpy_P(p + suffix, PSTR(".gz"));
            dataFile = SD.open(p);
          }
          if (!dataFile) {

            if (suffix) p[suffix] = 0;
            httpflags &= ~HTTP_GZIP;
            dataFile = SD.open(p);
          }

          if (dataFile) {
            unsigned long filesize = dataFile.size();
            uint16_t lastWrtYr = 0;
            byte monthval = 0;
            byte dayval = 0;
            byte hourval = 0;
            byte minval = 0;
            byte secval = 0;
#if !defined(ESP32)
            {uint16_t pdate;
            uint16_t ptime;
            dataFile.getModifyDateTime(&pdate, &ptime);
            lastWrtYr = FS_YEAR(pdate);
            monthval = FS_MONTH(pdate);
            dayval = FS_DAY(pdate);
            hourval = FS_HOUR(ptime);
            minval = FS_MINUTE(ptime);
            secval = FS_SECOND(ptime);
            }
#else
            {struct stat st;
            if(stat(p, &st) == 0){
              struct tm tm;
              time_t mtime = st.st_mtime;
              localtime_r(&mtime, &tm);
              lastWrtYr = tm.tm_year + 1900;
              monthval = tm.tm_mon + 1;
              dayval = tm.tm_mday;
              hourval = tm.tm_hour;
              minval = tm.tm_min;
              secval = tm.tm_sec ;
            }}
#endif
            if ((httpflags & HTTP_ETAG)) {
              if (memcmp(outBuf, outBuf + buffershift, sprintf_P(outBuf + buffershift, PSTR("\"%02d%02d%d%02d%02d%02d%lu\""), dayval, monthval, lastWrtYr, hourval, minval, secval, filesize))) {

                httpflags &= ~HTTP_ETAG;
              }
            }

            printToDebug(PSTR("File opened from SD: "));
            printlnToDebug(p);

            uint16_t code = 0;
            if ((httpflags & HTTP_ETAG)) {
              code = HTTP_NOT_MODIFIED;
            } else {
              code = HTTP_OK;
            }
            printHTTPheader(code, mimetype, HTTP_DO_NOT_ADD_CHARSET_TO_HEADER, (httpflags & HTTP_GZIP), HTTP_NO_DOWNLOAD, HTTP_AUTO_CACHE_AGE);
            if (lastWrtYr) {
              char monthname[4];
              char downame[4];
              uint8_t dowval = dayofweek((uint8_t)dayval, (uint8_t)monthval, lastWrtYr);
              if (dowval < 1 && dowval > 7) dowval = 8;
              memcpy_P(downame, PSTR("MonTueWedThuFriSatSunERR") + dowval * 3 - 3, 3);
              downame[3] = 0;

              if (monthval < 1 && monthval > 12) monthval = 13;
              memcpy_P(monthname, PSTR("JanFebMarAprMayJunJulAugSepOctNovDecERR") + monthval * 3 - 3, 3);
              monthname[3] = 0;
              printFmtToWebClient(PSTR("Last-Modified: %s, %02d %s %d %02d:%02d:%02d GMT\r\n"), downame, dayval, monthname, lastWrtYr, hourval, minval, secval);
            }

            printFmtToWebClient(PSTR("ETag: \"%02d%02d%d%02d%02d%02d%lu\"\r\nContent-Length: %lu\r\n\r\n"), dayval, monthval, lastWrtYr, hourval, minval, secval, filesize, filesize);
            flushToWebClient();

            if (!(httpflags & HTTP_ETAG) && !(httpflags & HTTP_HEAD_REQ)) {
              transmitFile(dataFile);
            }
            printToDebug((httpflags & HTTP_HEAD_REQ)?PSTR("HEAD"):PSTR("GET")); printlnToDebug(PSTR(" request received"));

            dataFile.close();
          } else {
#if !defined(I_DO_NOT_NEED_NATIVE_WEB_INTERFACE)

            if ((httpflags & HTTP_GET_ROOT)) {
              webPrintSite();
              break;
            }
#endif
            printHTTPheader(HTTP_NOT_FOUND, MIME_TYPE_TEXT_HTML, HTTP_ADD_CHARSET_TO_HEADER, HTTP_FILE_NOT_GZIPPED, HTTP_NO_DOWNLOAD, HTTP_DO_NOT_CACHE);
            printToWebClient(PSTR("\r\n<h2>File not found!</h2><br>File name: "));
            printToWebClient(p);
            flushToWebClient();
          }
          client.flush();
          break;
        }
#endif
        if (p[1] != 'J' && p[1] != 'C') {
          while (client.available()) client.read();
        } else {
          if ((httpflags & HTTP_ETAG)) {
            strcpy_P(outBuf + buffershift, PSTR("\""));
            strcat_P(outBuf + buffershift, BSB_VERSION);
            strcat_P(outBuf + buffershift, PSTR("\""));
            if (memcmp(outBuf, outBuf + buffershift, strlen(outBuf + buffershift))) {

              httpflags &= ~HTTP_ETAG;
            }
          }
        }
#ifndef WEBSERVER
#if !defined(I_DO_NOT_NEED_NATIVE_WEB_INTERFACE)

        if (!strcmp_P(p, PSTR("/"))) {
          webPrintSite();
          break;
        }
#else
#endif
#endif


#if !defined(I_DO_NOT_NEED_NATIVE_WEB_INTERFACE)
        if (!isdigit(p[1]) && strchr_P(PSTR("ABCDEGIJKLMNPQRSUVWXY"), p[1])==NULL) {
#else
        if (!isdigit(p[1]) && strchr_P(PSTR("CDGJNQUWX"), p[1])==NULL) {
#endif
          webPrintHeader();
          webPrintFooter();
          break;
        }

#ifdef WEBSERVER

        if (p[1]=='W') {
          p++;
          httpflags |= HTTP_FRAG;
        }
#endif

#if !defined(I_DO_NOT_NEED_NATIVE_WEB_INTERFACE)

        if (p[1]=='V') {
          p+=2;
          verbose=atoi(p);
          webPrintHeader();
          if (verbose>0) {
            printToWebClient(PSTR(MENU_TEXT_VB1 "<BR>"));
          } else {
            printToWebClient(PSTR(MENU_TEXT_VB2 "<BR>"));
          }
          printToWebClient(PSTR("\r\n" MENU_TEXT_VB3 "\r\n"));
          webPrintFooter();
          break;
        }

        if (p[1]=='M') {
          p+=2;
          monitor=atoi(p);
          webPrintHeader();
          if (monitor>0) {
            printToWebClient(PSTR(MENU_TEXT_SR1));
            monitor = 1;
          } else {
            printToWebClient(PSTR(MENU_TEXT_SR2));
          }
          printToWebClient(PSTR("\r\n" MENU_TEXT_SR3 "\r\n"));
          webPrintFooter();
          break;
        }
# 5653 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
        if ( p[1]=='S'
          || p[1]=='I')
        {
          float line;
          bool setcmd= (p[1]=='S');
          uint8_t destAddr = bus->getBusDest();
          p+=2;
          if (!(httpflags & HTTP_FRAG)) webPrintHeader();

          if (!isdigit(*p)) {
            printToWebClient(PSTR(MENU_TEXT_ER1 "\r\n"));
          } else {
            uint8_t save_my_dev_fam = my_dev_fam;
            uint8_t save_my_dev_var = my_dev_var;
            parameter param = parsingStringToParameter(p);
            line = param.number;

            if (param.dest_addr > -1) {
              set_temp_destination(param.dest_addr);






            }

            p=strchr(p,'=');
            if (p==NULL) {
                printToWebClient(PSTR(MENU_TEXT_ER2 "\r\n"));
            } else {
              p++;

              printFmtToDebug(PSTR("set ProgNr %g = %s"), line, p);
              writelnToDebug();

              int setresult = 0;
              setresult = set(line,p,setcmd);

              if (setresult!=1) {
                printToWebClient(PSTR(MENU_TEXT_ER3 "\r\n"));
                if (setresult == 2) {
                  printToWebClient(PSTR(" - " MENU_TEXT_ER4 "\r\n"));
                  if ((default_flag & FL_SW_CTL_RONLY) == FL_SW_CTL_RONLY && programWriteMode == 0) {
                    printToWebClient(PSTR(" " MENU_TEXT_ER8 "\r\n"));
                  }
                }
              } else {
                if (setcmd) {

                  query(line);
                  query_printHTML();
                } else {

                }
              }
              if (bus->getBusDest() != destAddr) {
                return_to_default_destination(destAddr);
                my_dev_fam = save_my_dev_fam;
                my_dev_var = save_my_dev_var;
              }
            }
          }
          if (!(httpflags & HTTP_FRAG)) webPrintFooter();
          flushToWebClient();
          break;
        }

        if (p[1]=='K' && !isdigit(p[2])) {

          webPrintHeader();
          printToWebClient(PSTR("<table><tr><td>&nbsp;</td><td>&nbsp;</td></tr>\r\n"));
          float cat_min = -1, cat_max = -1;
          for (int cat=0;cat<CAT_UNKNOWN;cat++) {
            if ((bus->getBusType() != BUS_PPS) || (bus->getBusType() == BUS_PPS && (cat == CAT_PPS || cat == CAT_USERSENSORS))) {
              printFmtToWebClient(PSTR("<tr><td><a href='K%d'>"), cat);
              printENUM(ENUM_CAT,sizeof(ENUM_CAT),cat,1);
              cat_min = ENUM_CAT_NR[cat*2];
              cat_max = ENUM_CAT_NR[cat*2+1];
              printToWebClient(decodedTelegram.enumdescaddr);
              writelnToDebug();
              printFmtToWebClient(PSTR("</a></td><td>%g - %g</td></tr>\r\n"), cat_min, cat_max);
            }
          }
          printToWebClient(PSTR("</table>"));
          webPrintFooter();
          flushToWebClient();
          break;
        }

        if (p[1]=='E') {
          webPrintHeader();
          uint16_t line = atof(&p[2]);
          int i=findLine(line,0,NULL);
          if (i>=0) {
            loadPrognrElementsFromTable(line, i);
            uint8_t flag = 0;

            switch (decodedTelegram.type) {
              case VT_ENUM: flag = PRINT_DISABLED_VALUE + 1; break;
              case VT_WEEKDAY:
              case VT_CUSTOM_ENUM:
              case VT_CUSTOM_BIT:
              case VT_BINARY_ENUM:
              case VT_BIT: flag = DO_NOT_PRINT_DISABLED_VALUE + 1; break;
            }
            if (flag) {
              listEnumValues(decodedTelegram.enumstr, decodedTelegram.enumstr_len, NULL, PSTR(" - "), NULL, PSTR("<br>\r\n"), NULL, 0, PRINT_VALUE|PRINT_DESCRIPTION|PRINT_VALUE_FIRST, flag - 1);
            } else {
              printToWebClient(PSTR(MENU_TEXT_ER5));
            }
          } else {
            printToWebClient(PSTR(MENU_TEXT_ER6));
          }
          webPrintFooter();
          break;
        }

        if (p[1]=='R') {
          webPrintHeader();
          if (!queryDefaultValue(atof(&p[2]), msg, tx_msg)) {
            if (decodedTelegram.error == 258) {
              printToWebClient(PSTR(MENU_TEXT_ER6 "\r\n"));
            } else if (decodedTelegram.error == 261) {
              printlnToDebug(printError(decodedTelegram.error));
              printToWebClient(PSTR(MENU_TEXT_ER3 "\r\n"));
            }
          } else {

            build_pvalstr(0);
            if (outBuf[0]>0) {
              printToWebClient(outBuf);
              printToWebClient(PSTR("<br>"));
            }
          }
          webPrintFooter();
          break;
        }
#endif
        if (p[1]=='Q' && p[2] !='D') {
          webPrintHeader();
          if (bus_type > 1) {
            printToWebClient(PSTR(MENU_TEXT_NOQ "\r\n\r\n"));
          } else {
            if (my_dev_fam == 0) {
              printToWebClient(PSTR(MENU_TEXT_QNC "<BR>\r\n"));
            } else {
              printToWebClient(PSTR(MENU_TEXT_QIN "<BR><BR>\r\n"));
              printToWebClient(PSTR("<A HREF='/"));
              printPassKey();
              printToWebClient(PSTR("QD'>" MENU_TEXT_QDL "</A><BR>\r\n"));
            }
          }
          webPrintFooter();
          break;
        }
        if (p[1]=='Q' && p[2]=='D') {

          if (bus_type > 1) {
            printHTTPheader(HTTP_OK, MIME_TYPE_TEXT_PLAIN, HTTP_ADD_CHARSET_TO_HEADER, HTTP_FILE_NOT_GZIPPED, HTTP_NO_DOWNLOAD, HTTP_AUTO_CACHE_AGE);
            printToWebClient(PSTR(MENU_TEXT_NOQ "\r\n\r\n"));
            break;
          }

          printHTTPheader(HTTP_OK, MIME_TYPE_FORCE_DOWNLOAD, HTTP_ADD_CHARSET_TO_HEADER, HTTP_FILE_NOT_GZIPPED, HTTP_IS_DOWNLOAD, HTTP_AUTO_CACHE_AGE);
          printToWebClient(PSTR("\r\n"));
          flushToWebClient();

          if (bus_type > 1) {
            printToWebClient(PSTR(MENU_TEXT_NOQ "\r\n\r\n"));
            break;
          }

          uint8_t myAddr = bus->getBusAddr();
          uint8_t destAddr = bus->getBusDest();
          printToWebClient(PSTR(MENU_TEXT_QIN "\r\n\r\n"));
          printToWebClient(PSTR(MENU_TEXT_VER ": "));
          printToWebClient(BSB_VERSION);
          printToWebClient(PSTR("\r\n"));
          printToWebClient(PSTR(MENU_TEXT_QSC "...\r\n"));
          switch (bus->getBusType()) {
            case BUS_BSB: bus->setBusType(BUS_BSB, myAddr, 0x7F); break;
            case BUS_LPB: bus->setBusType(BUS_LPB, myAddr, 0xFF); break;
          }

          flushToWebClient();

          uint8_t found_ids[10] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
          if (bus->Send(TYPE_QINF, 0x053D0002, msg, tx_msg, NULL, 0, false)) {
            printTelegram(tx_msg, -1);
            unsigned long startquery = millis();
            while (millis() - startquery < 10000) {
              if (bus->GetMessage(msg)) {
                printTelegram(msg, -1);
                uint8_t found_id = 0;
                bool found = false;
                if (bus->getBusType() == BUS_BSB && msg[4] == 0x02) {
                  found_id = msg[1] & 0x7F;
                }
                if (bus->getBusType() == BUS_LPB && msg[8] == 0x02) {
                  found_id = msg[3];
                }
                for (int i=0;i<10;i++) {
                  if (found_ids[i] == found_id) {
                    found = true;
                    break;
                  }
                  if (found_ids[i] == 0xFF) {
                    found_ids[i] = found_id;
                    break;
                  }
                }
                if (!found) {
                  printFmtToWebClient(PSTR(MENU_TEXT_QFD ": %hu\r\n"),found_id);
                  flushToWebClient();
                }
              }
              delay(1);
            }
          } else {
            printToWebClient(PSTR(MENU_TEXT_QFA "!"));
          }
          for (int x=0; x<10 && client.connected(); x++) {
            if (found_ids[x]==0xFF) {
              continue;
            }
            bus->setBusType(bus->getBusType(), myAddr, found_ids[x]);
            printFmtToWebClient(PSTR(MENU_TEXT_QRT " %hu..."), found_ids[x]);
            flushToWebClient();

            uint32_t c=0;
            float l;
            int orig_dev_fam = my_dev_fam;
            int orig_dev_var = my_dev_var;
            query_program_and_print_result(6224, PSTR("\r\n"), NULL);
            query_program_and_print_result(6225, PSTR("\r\n"), NULL);
            int temp_dev_fam = strtod(decodedTelegram.value,NULL);
            query_program_and_print_result(6226, PSTR("\r\n"), NULL);
            int temp_dev_var = strtod(decodedTelegram.value,NULL);
            my_dev_fam = temp_dev_fam;
            my_dev_var = temp_dev_var;
            if (temp_dev_fam == 97) temp_dev_fam = 64;
# 5916 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
            my_dev_fam = orig_dev_fam;
            my_dev_var = orig_dev_var;

            if (p[3] == 'F') {
              printToWebClient(PSTR(MENU_TEXT_QST "...\r\n"));
              flushToWebClient();
              for (int j=0; j<10000 && client.connected(); j++) {
                uint32_t cc = get_cmdtbl_cmd(j);
                if (cc == c) {
                  continue;
                } else {
                  c = cc;
                }
                if (c==CMD_END) break;
                l=get_cmdtbl_line(j);
                uint8_t dev_fam = get_cmdtbl_dev_fam(j);
                uint8_t dev_var = get_cmdtbl_dev_var(j);
#if defined(ESP32)
                esp_task_wdt_reset();
#endif
                if (((dev_fam != temp_dev_fam && dev_fam != DEV_FAM(DEV_ALL)) || (dev_var != temp_dev_var && dev_var != DEV_VAR(DEV_ALL))) && c!=CMD_UNKNOWN) {
                  printFmtToDebug(PSTR("%02X\r\n"), c);
                  if (!bus->Send(TYPE_QUR, c, msg, tx_msg)) {
                    print_bus_send_failed();
                  } else {
                    if (msg[4+(bus->getBusType()*4)]!=TYPE_ERR) {

                      printTelegram(tx_msg, -1);
#ifdef LOGGER
                      LogTelegram(tx_msg);
#endif

                      printTelegram(msg, -1);
#ifdef LOGGER
                      LogTelegram(msg);
#endif
                      if (decodedTelegram.msg_type != TYPE_ERR) {
                        my_dev_fam = temp_dev_fam;
                        my_dev_var = temp_dev_var;
                        query(l);
                        my_dev_fam = orig_dev_fam;
                        my_dev_var = orig_dev_var;
                        if (decodedTelegram.msg_type == TYPE_ERR) {
                          printFmtToWebClient(PSTR("\r\n%g - "), l);
                          printToWebClient(decodedTelegram.catdescaddr);
                          printToWebClient(PSTR(" - "));
                          printToWebClient_prognrdescaddr();
                          printFmtToWebClient(PSTR("\r\n0x%08X\r\n"), c);
                          int outBufLen = strlen(outBuf);
                          bin2hex(outBuf + outBufLen, tx_msg, tx_msg[bus->getLen_idx()]+bus->getBusType(), ' ');
                          printToWebClient(outBuf + outBufLen);
                          printToWebClient(PSTR("\r\n"));
                          bin2hex(outBuf + outBufLen, msg, msg[bus->getLen_idx()]+bus->getBusType(), ' ');
                          printToWebClient(outBuf + outBufLen);
                          outBuf[outBufLen] = 0;
                          printToWebClient(PSTR("\r\n"));
                        }
                        forcedflushToWebClient();
                      }
                    }
                  }
                }
              }
              printToWebClient(PSTR("\r\n" MENU_TEXT_QTE ".\r\n"));
              flushToWebClient();
            }
            printToWebClient(PSTR("\r\nComplete dump:\r\n"));
            c = 0;
            int outBufLen = strlen(outBuf);
            unsigned long timeout = millis() + 3000;
            while (!bus->Send(TYPE_QUR, 0x053D0001, msg, tx_msg) && (millis() < timeout)) {
              printTelegram(tx_msg, -1);
              printTelegram(msg, -1);
              delay(500);
            }
            printTelegram(tx_msg, -1);
            printTelegram(msg, -1);
            bin2hex(outBuf + outBufLen, msg, msg[bus->getLen_idx()]+bus->getBusType(), ' ');
            printToWebClient(outBuf + outBufLen);
            printToWebClient(PSTR("\r\n"));
            timeout = millis() + 3000;
            while (!bus->Send(TYPE_QUR, 0x053D0064, msg, tx_msg) && (millis() < timeout)) {
              printTelegram(tx_msg, -1);
              printTelegram(msg, -1);
              delay(500);
            }
            printTelegram(tx_msg, -1);
            printTelegram(msg, -1);
            bin2hex(outBuf + outBufLen, msg, msg[bus->getLen_idx()]+bus->getBusType(), ' ');
            printToWebClient(outBuf + outBufLen);
            printToWebClient(PSTR("\r\n"));
            flushToWebClient();
            timeout = millis() + 3000;
            while (!bus->Send(TYPE_IQ1, c, msg, tx_msg) && (millis() < timeout)) {
              printTelegram(tx_msg, -1);
              printTelegram(msg, -1);
              printToWebClient(PSTR("Didn't receive matching telegram, resending...\r\n"));
              delay(500);
            }
            printTelegram(tx_msg, -1);
            printTelegram(msg, -1);
            int IA1_max = (msg[7+bus->getBusType()*4] << 8) + msg[8+bus->getBusType()*4];
            if (msg[4+bus->getBusType()*4] == 0x13 && IA1_max > 0) {
              timeout = millis() + 3000;
              while (!bus->Send(TYPE_IQ2, c, msg, tx_msg) && (millis() < timeout)) {
                printToWebClient(PSTR("Didn't receive matching telegram, resending...\r\n"));
                delay(500);
              }
              int IA2_max = (msg[5+bus->getBusType()*4] << 8) + msg[6+bus->getBusType()*4];
              int outBufLen = strlen(outBuf);

              for (int IA1_counter = 1; IA1_counter <= IA1_max && client.connected(); IA1_counter++) {
#if defined(ESP32)
                esp_task_wdt_reset();
#endif
                timeout = millis() + 3000;
                while (!bus->Send(TYPE_IQ1, IA1_counter, msg, tx_msg) && (millis() < timeout)) {
                  printToWebClient(PSTR("Didn't receive matching telegram, resending...\r\n"));
                  delay(500);
                }
                bin2hex(outBuf + outBufLen, msg, msg[bus->getLen_idx()]+bus->getBusType(), ' ');
                printToWebClient(outBuf + outBufLen);
                printToWebClient(PSTR("\r\n"));
                flushToWebClient();
              }
              for (int IA2_counter = 1; IA2_counter <= IA2_max && client.connected(); IA2_counter++) {
#if defined(ESP32)
                esp_task_wdt_reset();
#endif
                timeout = millis() + 3000;
                while (!bus->Send(TYPE_IQ2, IA2_counter, msg, tx_msg) && (millis() < timeout)) {
                  printToWebClient(PSTR("Didn't receive matching telegram, resending...\r\n"));
                  delay(500);
                }
                bin2hex(outBuf + outBufLen, msg, msg[bus->getLen_idx()]+bus->getBusType(), ' ');
                printToWebClient(outBuf + outBufLen);
                printToWebClient(PSTR("\r\n"));
                flushToWebClient();
              }
              outBuf[outBufLen] = 0;
            } else {
              printToWebClient(PSTR("\r\nNot supported by this device. No problem.\r\n"));
            }
          }
          bus->setBusType(bus->getBusType(), myAddr, destAddr);
          printToWebClient(PSTR("\r\n" MENU_TEXT_QFE ".\r\n"));

          forcedflushToWebClient();
          break;
        }
#if !defined(I_DO_NOT_NEED_NATIVE_WEB_INTERFACE)
        if (p[1]=='Y') {
#ifdef DEBUG
          if (debug_mode) {
            webPrintHeader();
            uint8_t type = strtol(&p[2],NULL,16);
            uint32_t c = (uint32_t)strtoul(&p[5],NULL,16);
            uint8_t param[MAX_PARAM_LEN] = { 0 };
            uint8_t param_len = 0;
            uint8_t counter = 13;
            if (p[counter] == ',') {
              counter++;
              while (p[counter] && p[counter+1]) {
                param[param_len] = char2int(p[counter])*16 + char2int(p[counter+1]);
                param_len++;
                counter = counter + 2;
              }
            }
            if (!bus->Send(type, c, msg, tx_msg, param, param_len, true)) {
              print_bus_send_failed();
            } else {

              printTelegram(tx_msg, -1);
#ifdef LOGGER
              LogTelegram(tx_msg);
#endif
            }

            printTelegram(msg, -1);
#ifdef LOGGER
            LogTelegram(msg);
#endif

            build_pvalstr(1);
            if (outBuf[0]>0) {
              printToWebClient(outBuf);
              printToWebClient(PSTR("<br>"));
            }
            bin2hex(outBuf, tx_msg, tx_msg[bus->getLen_idx()]+bus->getBusType(), ' ');
            printToWebClient(outBuf);
            printToWebClient(PSTR("\r\n<br>\r\n"));
            bin2hex(outBuf, msg, msg[bus->getLen_idx()]+bus->getBusType(), ' ');
            printToWebClient(outBuf);
            outBuf[0] = 0;
            writelnToWebClient();
            webPrintFooter();
          }
#endif
          break;
        }
#endif
        if (p[1]=='J') {
          uint32_t cmd=0;

          char json_value_string[52];
          float json_parameter = -1, cat_min = -1, cat_max = -1, cat_param = -1;
          bool json_type = 0;
          bool p_flag = false;
          bool v_flag = false;
          bool t_flag = false;
          bool d_flag = false;
          bool output = false;
          bool been_here = false;
          uint8_t destAddr = bus->getBusDest();
          uint8_t tempDestAddr = 0;
          uint8_t tempDestAddrOnPrevIteration = 0;
          uint8_t save_my_dev_fam = my_dev_fam;
          uint8_t save_my_dev_var = my_dev_var;
          uint8_t opening_brackets = 0;
          char* json_token = strtok(p, "=,");
          json_token = strtok(NULL, ",");
          {
            int16_t http_code = HTTP_OK;
            long cache_time = HTTP_DO_NOT_CACHE;

            if (json_token!=NULL && (p[2] == 'C' || (p[2] == 'K' && atoi(json_token) != CAT_USERSENSORS))) {
              cache_time = 300;
              if ((httpflags & HTTP_ETAG))
                http_code = HTTP_NOT_MODIFIED;
            }

            printHTTPheader(http_code, MIME_TYPE_APP_JSON, HTTP_ADD_CHARSET_TO_HEADER, HTTP_FILE_NOT_GZIPPED, HTTP_NO_DOWNLOAD, cache_time);
            if (cache_time != HTTP_DO_NOT_CACHE) {
              printToWebClient(PSTR("ETag: \""));
              printToWebClient(BSB_VERSION);
              printToWebClient(PSTR("\"\r\n"));
            }
            printToWebClient(PSTR("\r\n"));
            if (http_code == HTTP_NOT_MODIFIED) {
              forcedflushToWebClient();
              break;
            }
          }
          printToWebClient(PSTR("{\r\n"));
          if (strchr_P(PSTR("BCIKLQRSVW"), p[2]) == NULL) {
            printToWebClient(PSTR("}"));
            forcedflushToWebClient();
            break;
          }

          if (p[2] == 'V'){
            printFmtToWebClient(PSTR("\"api_version\": \"" JSON_MAJOR "." JSON_MINOR "\"\r\n}"));
            forcedflushToWebClient();
            break;
          }

          if (p[2] == 'I'){
            bool not_first = false;
            int i;
            printToWebClient(PSTR("  \"name\": \"BSB-LAN\",\r\n  \"version\": \""));
            printToWebClient(BSB_VERSION);
            printToWebClient(PSTR("\",\r\n  \"hardware\": \""));
            printDeviceArchToWebClient();
            printFmtToWebClient(PSTR("\",\r\n  \"freeram\": %d,\r\n  \"uptime\": %lu,\r\n  \"MAC\": \"%02hX:%02hX:%02hX:%02hX:%02hX:%02hX\",\r\n  \"freespace\": "), freeRam(), millis(), mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
#if defined LOGGER || defined WEBSERVER
#if !defined(ESP32)
            uint32_t freespace = SD.vol()->freeClusterCount();
            freespace = (uint32_t)(freespace*SD.vol()->sectorsPerCluster()/2048);
            printFmtToWebClient(PSTR("%d"), freespace);
#else
            uint64_t freespace = SD.totalBytes() - SD.usedBytes();
            printFmtToWebClient(PSTR("%llu"), freespace);
#endif
#else
            printFmtToWebClient(PSTR("0"));
#endif


            json_parameter = 0;
            i = bus->getBusType();
            if (i != BUS_PPS) {
              if ((default_flag & FL_RONLY) != FL_RONLY || ((default_flag & FL_SW_CTL_RONLY) == FL_SW_CTL_RONLY && programWriteMode)) json_parameter = 1;
            } else {
              if (pps_write == 1) json_parameter = 1;
            }

            switch (i) {
              case 0: strcpy_P(json_value_string, PSTR("BSB")); break;
              case 1: strcpy_P(json_value_string, PSTR("LPB")); break;
              case 2: strcpy_P(json_value_string, PSTR("PPS")); break;
            }
            printFmtToWebClient(PSTR(",\r\n  \"bus\": \"%s\",\r\n  \"buswritable\": %d,\r\n"), json_value_string, json_parameter);
            printFmtToWebClient(PSTR("  \"busaddr\": %d,\r\n  \"busdest\": %d,\r\n"), bus->getBusAddr(), bus->getBusDest());

            printFmtToWebClient(PSTR("  \"monitor\": %d,\r\n  \"verbose\": %d"), monitor, verbose);

            #ifdef ONE_WIRE_BUS
            printFmtToWebClient(PSTR(",\r\n  \"onewirebus\": %d"), One_Wire_Pin);
            printFmtToWebClient(PSTR(",\r\n  \"onewiresensors\": %d"), numSensors);
            #endif
            #ifdef DHT_BUS
            printToWebClient(PSTR(",\r\n  \"dhtbus\": [\r\n"));
            not_first = false;
            int numDHTSensors = sizeof(DHT_Pins) / sizeof(DHT_Pins[0]);
            for (i=0;i<numDHTSensors;i++) {
              if (DHT_Pins[i]) {
                if (not_first) {
                  printToWebClient(PSTR(",\r\n"));
                } else {
                  not_first = true;
                }
                printFmtToWebClient(PSTR("    { \"pin\": %d }"), DHT_Pins[i]);
              }
            }
            printToWebClient(PSTR("\r\n  ]"));
            #endif


#ifdef AVERAGES
            if (LoggingMode & CF_LOGMODE_24AVG) {
              printToWebClient(PSTR(",\r\n  \"averages\": [\r\n"));
              not_first = false;
              for (i = 0; i < numAverages; i++) {
                if (avg_parameters[i].number > 0) {
                  if (not_first) {
                    printToWebClient(PSTR(",\r\n"));
                  } else {
                    not_first = true;
                  }
                  printParameterInJSON_ToWebClient(avg_parameters[i]);
                }
              }
              printToWebClient(PSTR("\r\n  ]"));
            }
#endif

          #ifdef LOGGER
            printFmtToWebClient(PSTR(",\r\n  \"loggingmode\": %d,\r\n  \"loginterval\": %d,\r\n  \"logged\": [\r\n"), LoggingMode, log_interval);
            not_first = false;
            for (i=0; i<numLogValues; i++) {
              if (log_parameters[i].number > 0) {
                if (not_first) {
                  printToWebClient(PSTR(",\r\n"));
                } else {
                  not_first = true;
                }
                printParameterInJSON_ToWebClient(log_parameters[i]);
              }
            }
            printToWebClient(PSTR("\r\n  ]"));
          #endif
            printToWebClient(PSTR("\r\n}\r\n"));
            forcedflushToWebClient();
            break;
          }
#if defined(JSONCONFIG)
          if (p[2] == 'L') {
            generateJSONwithConfig();
            printToWebClient(PSTR("\r\n}\r\n"));
            forcedflushToWebClient();
            break;
          }
#endif
          if (p[2] == 'B'){
            bool notfirst = false;
            for (uint cat = 1; cat < CAT_UNKNOWN; cat++) {
              if ((bus->getBusType() != BUS_PPS) || (bus->getBusType() == BUS_PPS && (cat == CAT_PPS || cat == CAT_USERSENSORS))) {
                cat_min = ENUM_CAT_NR[cat * 2];
                cat_max = ENUM_CAT_NR[cat * 2 + 1];
                if (cat*2+2 < sizeof(ENUM_CAT_NR)/sizeof(*ENUM_CAT_NR)) {
                  if (cat_max > ENUM_CAT_NR[cat*2+2]) {
                    cat_max = ENUM_CAT_NR[cat*2+2]-1;
                  }
                }
                float j = cat_min;

                do{
                  int i_line = findLine(j, 0, &cmd);
                  if (i_line < 0 || (cmd == CMD_UNKNOWN && json_parameter < BSP_INTERNAL)) {
                    continue;
                  }
                  loadPrognrElementsFromTable(j, i_line);
                  if (decodedTelegram.readwrite == FL_WRITEABLE) {
                    query(j);
                    if (decodedTelegram.error == 0) {
                      if (notfirst) {printToWebClient(PSTR(",\r\n"));} else {notfirst = true;}
                      printFmtToWebClient(PSTR("  \"%g\":{\"parameter\":\"%g\", \"value\":\"%s\", \"type\":\"%d\"}"), j, j, decodedTelegram.value, 1);
                    }
                  }
                  j = get_next_prognr(j, i_line);
                }while(j >= cat_min && j <= cat_max);
              }
            }
            printToWebClient(PSTR("\r\n}\r\n"));
            forcedflushToWebClient();
            break;
          }





          while (client.available()) {
            if (client.read()=='{') {
              opening_brackets++;
              break;
            }
          }
          tempDestAddr = destAddr;
          tempDestAddrOnPrevIteration = destAddr;
          while ((client.available() && opening_brackets > 0) || json_token!=NULL) {
            json_value_string[0] = 0;
            if (client.available()) {
              bool opening_quotation = false;
              tempDestAddr = destAddr;
              while (client.available()) {
                char c = client.read();
                if (c == '{') {
                  opening_brackets++;
                  if (opening_brackets > 2) {
                    opening_brackets = 0;
                    break;
                  }
                }
                if (c == '}') { output = true; opening_brackets--;}
                if (c == '\"') {opening_quotation = opening_quotation?false:true;}
                if (opening_quotation) {
                  if ((c == 'P' || c == 'p') && t_flag != true) { p_flag = true; }
                  if (c == 'V' || c == 'v') { v_flag = true; }
                  if (c == 'T' || c == 't') { t_flag = true; }
                  if ((c == 'D' || c == 'd') && t_flag != true) { d_flag = true; }
                  if ( p_flag || v_flag || t_flag || d_flag) {
                    uint8_t stage_f = 0;
                    bool stage_v = 0;
                    uint8_t j_char_idx = 0;
                    char json_temp[sizeof(json_value_string)];
                    while (client.available()){
                      c = client.read();
                      if (c == '\"') stage_f++;
                      if (stage_f == 1 && c == ':') stage_f+=16;
                      if (stage_f > 1) break;
                    }
                    if (stage_f != 17) {
                      while (client.available()){client.read();}
                      break;
                    } else {
                      opening_quotation = false;
                    }
                    char *jptr = NULL;
                    size_t jsize = 0;
                    if (v_flag) {
                      if (p[2] == 'W') {
                        jptr = outBuf;
                        jsize = sizeof(outBuf);
                      } else {
                        jptr = json_value_string;
                        jsize = sizeof(json_value_string);
                      }
                    } else {
                      jptr = json_temp;
                      jsize = sizeof(json_temp);
                    }
                    while (client.available() && j_char_idx < jsize - 1) {
                      c = client.read();
                      if (!stage_v && (c == ' ' || c == ',' || c == '}' || c == '\n' || c == '\r')) {
                        if (c == ' ') continue;
                        if (c == '}') { output = true; opening_brackets--;}
                      break;
                      }
                      if (c == '\"') {
                        if (!stage_v) {
                          stage_v = true;
                          j_char_idx = 0;
                          continue;
                        } else {
                          break;
                        }
                      }
                      jptr[j_char_idx] = c;
                      j_char_idx++;
                    }
                    jptr[j_char_idx] = '\0';
                    if (p_flag == true) {
                      json_parameter = ((float)roundf(atof(jptr) * 10)) / 10;
                      p_flag = false;
                    }
                    if (v_flag == true) {

                      v_flag = false;
                    }
                    if (t_flag == true) {
                      json_type = atoi(jptr);
                      t_flag = false;
                    }
                    if (d_flag == true) {
                      tempDestAddr = atoi(jptr);
                      d_flag = false;
                    }
                  }
                }
                if (output) break;
              }
            } else {
              if (p[2] == 'S' || p[2] == 'W') {
                json_token = NULL;
              } else {
                parameter param = parsingStringToParameter(json_token);
                json_parameter = param.number;
                if(param.dest_addr > -1) tempDestAddr = param.dest_addr;
              }
            }
            if (tempDestAddr != tempDestAddrOnPrevIteration) {
              set_temp_destination(tempDestAddr);
            }
            if (output || json_token != NULL) {
              if (p[2] != 'K' && p[2] != 'W') {
                int i_line=findLine(json_parameter,0,&cmd);
                if ((p[2] == 'Q' || p[2] == 'C') && (i_line<0 || (cmd == CMD_UNKNOWN && json_parameter < BSP_INTERNAL))) {
                  json_token = strtok(NULL,",");
                  continue;
                }
              }

              output = false;
              if (json_parameter == -1) continue;

              if (p[2]=='K' && !isdigit(p[4])) {
                bool notfirst = false;
                for (uint cat=0;cat<CAT_UNKNOWN;cat++) {
                  if ((bus->getBusType() != BUS_PPS) || (bus->getBusType() == BUS_PPS && (cat == CAT_PPS || cat == CAT_USERSENSORS))) {
                    if (notfirst) {printToWebClient(PSTR(",\r\n"));} else {notfirst = true;}
                    printFmtToWebClient(PSTR("\"%d\": { \"name\": \""), cat);
                    printENUM(ENUM_CAT,sizeof(ENUM_CAT),cat,1);
                    cat_min = ENUM_CAT_NR[cat*2];
                    cat_max = ENUM_CAT_NR[cat*2+1];
                    if (cat*2+2 < sizeof(ENUM_CAT_NR)/sizeof(*ENUM_CAT_NR)) {
                      if (cat_max > ENUM_CAT_NR[cat*2+2]) {
                        cat_max = ENUM_CAT_NR[cat*2+2]-1;
                      }
                    }
                    printToWebClient(decodedTelegram.enumdescaddr);
                    printFmtToWebClient(PSTR("\", \"min\": %g, \"max\": %g }"), cat_min, cat_max);
                  }
                }
                json_token = NULL;
              }

              if (p[2]=='K' && isdigit(p[4])) {
                if (cat_min < 0) {
                  uint search_cat = atoi(&p[4]) * 2;

                  if(search_cat > sizeof(ENUM_CAT_NR)/sizeof(ENUM_CAT_NR[0])) {
                    search_cat = 0;
                  }
                  cat_min = ENUM_CAT_NR[search_cat];
                  cat_max = ENUM_CAT_NR[search_cat+1];
                  if (search_cat+2 < sizeof(ENUM_CAT_NR)/sizeof(*ENUM_CAT_NR)) {
                    if (cat_max > ENUM_CAT_NR[search_cat+2]) {
                      cat_max = ENUM_CAT_NR[search_cat+2]-1;
                    }
                  }
                  cat_param = cat_min;
                } else {

                  cat_param = get_next_prognr(cat_param, findLine(cat_param,0,NULL));
                }
                if (cat_param <= cat_max) {
                  json_parameter = cat_param;
                }
                if (cat_param >= cat_max) {
                  json_token = NULL;
                }
              }

              if (p[2]=='Q' || p[2]=='C' || (p[2]=='K' && isdigit(p[4]))) {
                int i_line=findLine(json_parameter,0,&cmd);
                if (i_line<0 || (cmd == CMD_UNKNOWN && json_parameter < BSP_INTERNAL)) {
                  continue;
                }

                if (!been_here) been_here = true; else printToWebClient(PSTR(",\r\n"));
                if (p[2]=='Q') {
                  query(json_parameter);
                } else {
                  loadPrognrElementsFromTable(json_parameter, i_line);
                  decodedTelegram.prognr = json_parameter;
                }
                printFmtToWebClient(PSTR("  \"%g\": {\r\n    \"name\": \""), json_parameter);
                printToWebClient_prognrdescaddr();
                printToWebClient(PSTR("\",\r\n    \"dataType_name\": \""));
                printToWebClient(decodedTelegram.progtypedescaddr);
                printToWebClient(PSTR("\",\r\n    \"dataType_family\": \""));
                printToWebClient(decodedTelegram.data_type_descaddr);
                printFmtToWebClient(PSTR("\",\r\n    \"destination\": \"%d\",\r\n"), tempDestAddr);

                if (p[2]=='Q') {
                  printFmtToWebClient(PSTR("    \"error\": %d,\r\n    \"value\": \"%s\",\r\n    \"desc\": \""), decodedTelegram.error, decodedTelegram.value);
                  if (decodedTelegram.data_type == DT_ENUM && decodedTelegram.enumdescaddr)
                    printToWebClient(decodedTelegram.enumdescaddr);
                  printToWebClient(PSTR("\",\r\n"));
                }

                if (p[2] != 'Q') {
                  printToWebClient(PSTR("    \"possibleValues\": [\r\n"));
                    if (decodedTelegram.enumstr_len > 0) {
                      listEnumValues(decodedTelegram.enumstr, decodedTelegram.enumstr_len, PSTR("      { \"enumValue\": "), PSTR(", \"desc\": \""), NULL, PSTR("\" }"), PSTR(",\r\n"), 0, PRINT_VALUE|PRINT_DESCRIPTION|PRINT_VALUE_FIRST, decodedTelegram.type==VT_ENUM?PRINT_DISABLED_VALUE:DO_NOT_PRINT_DISABLED_VALUE);
                    }

                  printFmtToWebClient(PSTR("\r\n    ],\r\n    \"isswitch\": %d,\r\n"), decodedTelegram.isswitch);
                }
                if (decodedTelegram.data_type == DT_VALS) {
                  char pre_buf[10];
                  pre_buf[0] = 0;
                  if (decodedTelegram.precision != 0) {
                    strcpy_P(pre_buf, PSTR("0."));
                    for (uint8_t j = 1; j < decodedTelegram.precision; j ++) {
                      strcat_P(pre_buf, PSTR("0"));
                    }
                  }
                  strcat_P(pre_buf, PSTR("1"));
                  printFmtToWebClient(PSTR("    \"precision\": %s,\r\n"), pre_buf);
                }
                printFmtToWebClient(PSTR("    \"dataType\": %d,\r\n    \"readonly\": %d,\r\n    \"readwrite\": %d,\r\n    \"unit\": \"%s\"\r\n  }"), decodedTelegram.data_type, decodedTelegram.readwrite == FL_RONLY?1:0, decodedTelegram.readwrite, decodedTelegram.unit);
              }

              if (p[2]=='S') {
                if (!been_here) been_here = true; else printToWebClient(PSTR(",\r\n"));
                int status = set(json_parameter, json_value_string, json_type);
                printFmtToWebClient(PSTR("  \"%g\": {\r\n    \"status\": %d\r\n  }"), json_parameter, status);

                printFmtToDebug(PSTR("Setting parameter %g to \"%s\" with type %d to destination %d\r\n"), json_parameter, json_value_string, json_type, tempDestAddr);
              }

              if (p[2]=='R') {
                if (!been_here) been_here = true; else printToWebClient(PSTR(",\r\n"));
                queryDefaultValue(json_parameter, msg, tx_msg);
                printFmtToWebClient(PSTR("  \"%g\": {\r\n    \"error\": %d,\r\n    \"value\": \"%s\"\r\n  }"), json_parameter, decodedTelegram.error, decodedTelegram.value);

                printFmtToDebug(PSTR("Default value of parameter %g for destination %d is \"%s\"\r\n"), json_parameter, tempDestAddr, decodedTelegram.value);
              }

#if defined(JSONCONFIG)
              if (p[2]=='W') {
                if (!been_here) been_here = true; else printToWebClient(PSTR(",\r\n"));
                int status = takeNewConfigValueFromUI_andWriteToRAM(json_parameter, outBuf);
                printFmtToWebClient(PSTR("  \"%g\": {\r\n    \"status\": %d\r\n  }"), json_parameter, status);

                printFmtToDebug(PSTR("Setting parameter %g to \"%s\"\r\n"), json_parameter, outBuf);
              }
#endif

              if (json_token != NULL && ((p[2] != 'K' && !isdigit(p[4])) || p[2] == 'Q' || p[2] == 'C' || p[2] == 'R')) {
                json_token = strtok(NULL,",");
              }
            }
            tempDestAddrOnPrevIteration = tempDestAddr;
            json_parameter = -1;
          }
          if (tempDestAddr != destAddr) {
            return_to_default_destination(destAddr);
            my_dev_fam = save_my_dev_fam;
            my_dev_var = save_my_dev_var;
          }
#if defined(JSONCONFIG)
          bool needReboot = false;
          if (p[2]=='W') {
            needReboot = SaveConfigFromRAMtoEEPROM();
#ifdef MAX_CUL
            UpdateMaxDeviceList();
#endif
          }
#endif
          printFmtToWebClient(PSTR("\r\n}\r\n"));
          forcedflushToWebClient();
#if defined(JSONCONFIG)
          if (needReboot) {
            resetBoard();
          }
#endif
          break;
        }

#ifdef LOGGER
        if (p[1]=='D') {
  #if defined(ESP32) && !defined(WIFI)
          if (esp32_save_energy == true) {
            bus->disableInterface();
            setCpuFrequencyMhz(240);
            bus->enableInterface();
          }
  #endif
          if (p[2]=='0' || ((p[2]=='D' || p[2]=='J') && p[3]=='0')) {
            webPrintHeader();
            File dataFile;
            bool filewasrecreated = false;

            if (p[2]=='J' || p[2]=='0') {
              SD.remove(journalFileName);
              dataFile = SD.open(journalFileName, FILE_WRITE);
              if (dataFile) {
                dataFile.close();
                filewasrecreated = true;
                printToDebug(journalFileName);
                printToWebClient(journalFileName);
              }
            }

              if (p[2]=='D' || p[2]=='0') {
                if (p[2]=='0') {
                  printToDebug(PSTR(", "));
                  printToWebClient(PSTR(", "));
                }
              SD.remove(datalogFileName);
              if (createdatalogFileAndWriteHeader()) {
                filewasrecreated = true;
                printToDebug(datalogFileName);
                printToWebClient(datalogFileName);
              }
            }
            if (filewasrecreated) {
              printToWebClient(PSTR(MENU_TEXT_DTR "\r\n"));
              printToDebug(PSTR(": file(s) was removed and recreated."));
            } else {
              printToWebClient(PSTR(MENU_TEXT_DTF "\r\n"));
            }
            webPrintFooter();
#if !defined(I_DO_NOT_NEED_NATIVE_WEB_INTERFACE)
          } else if (p[2]=='G') {
            webPrintHeader();
#if !defined(I_WILL_USE_EXTERNAL_INTERFACE)
            printToWebClient(PSTR("<A HREF='D'>" MENU_TEXT_DTD "</A><div align=center></div>\r\n"));
            printPStr(graph_html, sizeof(graph_html));
#else
            printToWebClient(PSTR("/DG command disabled because I_WILL_USE_EXTERNAL_INTERFACE defined<br>\r\n"));
#endif
            webPrintFooter();
#endif
          } else {
            printHTTPheader(HTTP_OK, MIME_TYPE_TEXT_PLAIN, HTTP_ADD_CHARSET_TO_HEADER, HTTP_FILE_NOT_GZIPPED, HTTP_NO_DOWNLOAD, HTTP_AUTO_CACHE_AGE);
            printToWebClient(PSTR("\r\n"));
            flushToWebClient();
            File dataFile;
            if (p[2]=='J') {
              dataFile = SD.open(journalFileName);

              if (dataFile) {
                unsigned long startdump = millis();
                transmitFile(dataFile);
                dataFile.close();
                printFmtToDebug(PSTR("Duration: %lu\r\n"), millis()-startdump);
              } else printToWebClient(PSTR(MENU_TEXT_DTO "\r\n"));
            } else if (p[2]=='A') {
              printFmtToWebClient("%04d-%02d-%02d", firstDatalogDate.elements.year, firstDatalogDate.elements.month, firstDatalogDate.elements.day);
            } else if (p[2]=='B') {
              printFmtToWebClient("%04d-%02d-%02d", previousDatalogDate.elements.year, previousDatalogDate.elements.month, previousDatalogDate.elements.day);
            } else if (p[2]=='I') {
              File indexFile = SD.open(datalogIndexFileName);
              if (indexFile) {
                compactDate_t date;
                unsigned long pos;
                printToWebClient("Date;DatalogPosition\r\n");
                while (indexFile.read((byte*)&date,sizeof(date)) > 0 &&
                       indexFile.read((byte*)&pos ,sizeof(pos )) > 0)
                  printFmtToWebClient("%04d-%02d-%02d;%lu\r\n", date.elements.year, date.elements.month, date.elements.day, pos);
                indexFile.close();
              }
            } else if (p[2]=='K') {
              int nDays;
              if (sscanf(p+3,"%d",&nDays)==1 && nDays>0) {
                printFmtToWebClient("\r\n%s\r\n",cleanupDatalog(nDays));

                SD.remove(datalogTemporaryFileName);
                SD.remove(datalogIndexTemporaryFileName);

                readFirstAndPreviousDatalogDateFromFile();
              }
            } else {
              dataFile = SD.open(datalogFileName);

              if (dataFile) {
                unsigned long startdump = millis();

                int ay, am, ad, by, bm, bd, n;
                n = sscanf(p+2, "%d-%d-%d,%d-%d-%d", &ay,&am,&ad, &by,&bm,&bd);
                if (n<1 || ay<1) transmitFile(dataFile);
                else {

                  client.write((byte*)datalogFileHeader,strlen(datalogFileHeader));
                  File indexFile = SD.open(datalogIndexFileName);
                  if (!indexFile) transmitFile(dataFile);
                  else {
                    unsigned long datalogTargetPosition;
                    if (n==6) {
                      compactDate_t a, b, date, prevDate;
                      a.elements.year = ay;
                      a.elements.month = am;
                      a.elements.day = ad;
                      b.elements.year = by;
                      b.elements.month = bm;
                      b.elements.day = bd;
                      prevDate.combined = 0;
                      unsigned long datalogFromPosition=0, datalogToPosition=0;

                      for (unsigned long pos=indexFile.size(); pos>=datalogIndexEntrySize; pos-=datalogIndexEntrySize) {
                        indexFile.seek(pos-datalogIndexEntrySize);
                        if (indexFile.read((byte*)&date,sizeof(date)) <= 0 ||
                            indexFile.read((byte*)&datalogTargetPosition,sizeof(datalogTargetPosition)) <= 0)
                          break;
                        if (prevDate.combined && prevDate.combined < date.combined) break;
                        if (date.combined > b.combined) datalogToPosition = datalogTargetPosition;
                        if (date.combined >= a.combined) datalogFromPosition = datalogTargetPosition;
                        else break;
                        prevDate = date;
                      }
                      if (datalogFromPosition) {
                        dataFile.seek(datalogFromPosition);
                        if (!datalogToPosition) datalogToPosition = dataFile.size();
                        unsigned long nBytesToDo = datalogToPosition - datalogFromPosition;

                        unsigned logbuflen = (OUTBUF_USEFUL_LEN + OUTBUF_LEN > 1024)?1024:(OUTBUF_USEFUL_LEN + OUTBUF_LEN);
                        byte *buf = 0;
#ifdef ESP32
                        buf = (byte*)malloc(4096);
#endif
                        if (buf) logbuflen=4096; else buf=(byte*)bigBuff;
                        while (nBytesToDo && client.connected()) {
                          int n = dataFile.read(buf, nBytesToDo<logbuflen ?nBytesToDo :logbuflen);
                          if (n < 0) {
                            printToWebClient(PSTR("Error: Failed to read from SD card - if problem remains after reformatting, card may be incompatible."));
                            forcedflushToWebClient();
                          }
                          if (n <= 0) break;
                          client.write(buf, n);
#ifdef ESP32
                          esp_task_wdt_reset();
#endif
                          nBytesToDo -= n;
                        }
                        if (buf != (byte*)bigBuff) free(buf);
                      }
                    }
                    else {
                      unsigned long bytesToBackup = ay * datalogIndexEntrySize;
                      unsigned long indexFileSize = indexFile.size();
                      if (indexFileSize >= bytesToBackup) {
                        indexFile.seek(indexFileSize - bytesToBackup + sizeof(compactDate_t));
                        indexFile.read((byte*)&datalogTargetPosition, sizeof(datalogTargetPosition));
                        dataFile.seek(datalogTargetPosition);
                      }
                      transmitFile(dataFile);
                    }
                    indexFile.close();
                  }
                }
                dataFile.close();
                printFmtToDebug(PSTR("Duration: %lu\r\n"), millis()-startdump);
              } else printToWebClient(PSTR(MENU_TEXT_DTO "\r\n"));
            }
          }
          flushToWebClient();
  #if defined(ESP32) && !defined(WIFI)
          if (esp32_save_energy == true) {
            bus->disableInterface();
            setCpuFrequencyMhz(80);
            bus->enableInterface();
          }
  #endif
          break;
        }
#endif
#if !defined(I_DO_NOT_NEED_NATIVE_WEB_INTERFACE)
        if (p[1]=='C'){
          if (!(httpflags & HTTP_FRAG)) webPrintHeader();

          switch (p[2]) {
#ifdef CONFIG_IN_EEPROM
            case 'E':
              if (p[3]=='=') {
                if (p[4]=='1') {
                  UseEEPROM = 0x96;
                } else {
                  UseEEPROM = 0x69;
                }
              }

              if (UseEEPROM == 0x96) {
                printyesno(1) ;
              } else {
                printyesno(0) ;
              }
              break;
            case 'S':
                for (uint8_t i = 0; i < CF_LAST_OPTION; i++) {
                  writeToEEPROM(i);
                }


              break;
#endif
#ifdef WEBCONFIG
            case 'I': {
              applyingConfig();
              generateWebConfigPage(false);
              generateConfigPage();
#ifdef MAX_CUL
              UpdateMaxDeviceList();
#endif
              if (!(httpflags & HTTP_FRAG)) webPrintFooter();
              flushToWebClient();
              if (SaveConfigFromRAMtoEEPROM() == true) {
                resetBoard();
              }
            }
            break;

            case 'O': {
              generateConfigPage();
              #if defined(BLE_SENSORS) && defined(ESP32)
                if (EnableBLE) {
                  printListBLEDevicesAround();
                }
     #endif
              generateWebConfigPage(true);
              if (!(httpflags & HTTP_FRAG)) webPrintFooter();
              flushToWebClient();
            }
            break;
#endif
            default:
#ifdef WEBCONFIG
              generateWebConfigPage(false);
#endif
              generateConfigPage();
              if (!(httpflags & HTTP_FRAG)) webPrintFooter();
              flushToWebClient();


              EEPROM_dump();
              break;
            }
          break;
        }
#if !defined(I_DO_NOT_WANT_URL_CONFIG)
        if (p[1]=='L') {
          webPrintHeader();
          switch (p[2]) {
            case 'C':
              if (p[3]=='=') {
                if (p[4]=='1') {
                  LoggingMode |= CF_LOGMODE_SD_CARD;
                } else {
                  LoggingMode &= ~CF_LOGMODE_SD_CARD;
                }

                printyesno(LoggingMode & CF_LOGMODE_SD_CARD) ;
              }
              break;
            case 'B':
              if (p[3]=='=') {
                if (p[4]=='1') {
                  logTelegram |= LOGTELEGRAM_BROADCAST_ONLY;
                } else {
                  if (logTelegram & LOGTELEGRAM_BROADCAST_ONLY)
                    logTelegram -= LOGTELEGRAM_BROADCAST_ONLY;
                }
                printToWebClient(PSTR(MENU_TEXT_LBO ": "));
                printyesno(logTelegram & LOGTELEGRAM_BROADCAST_ONLY) ;
              }
              break;
            case 'N':
              log_now = 1;
              printToWebClient(PSTR(MENU_TEXT_LIR));
              break;
            case 'U':
              if (p[3]=='=') {
                if (p[4]=='1') {
                  logTelegram |= LOGTELEGRAM_UNKNOWN_ONLY;
                } else {
                  if (logTelegram & LOGTELEGRAM_UNKNOWN_ONLY)
                    logTelegram -= LOGTELEGRAM_UNKNOWN_ONLY;
                }
                printToWebClient(PSTR(MENU_TEXT_BUT ": "));
                printyesno(logTelegram & LOGTELEGRAM_UNKNOWN_ONLY);
              }
              break;

            case '=':
            {
              char* log_token = strtok(p,"=,");
              log_token = strtok(NULL, "=,");

              if (log_token != 0) {
                log_interval = atoi(log_token);

                lastLogTime = millis();
#ifdef MQTT
                lastMQTTTime = millis();
#endif
                printFmtToWebClient(PSTR(MENU_TEXT_LGI ": %d"), log_interval);
                printToWebClient(PSTR(" " MENU_TEXT_SEC "<BR>\r\n"));
              }
              log_token = strtok(NULL,"=,");
              int token_counter = 0;
              if (log_token != 0) {
                for (int i = 0; i < numLogValues; i++) {
                  log_parameters[i].number = 0;
                  log_parameters[i].dest_addr = -1;
                }
              printToWebClient(PSTR(MENU_TEXT_LGN ": "));
              }
              while (log_token!=0) {
                log_parameters[token_counter] = parsingStringToParameter(log_token);

                if (token_counter < numLogValues) {
                  printFmtToWebClient(PSTR("%g"), log_parameters[token_counter].number);
                  if (log_parameters[token_counter].dest_addr > -1) {
                    printFmtToWebClient(PSTR("!%d"), log_parameters[token_counter].dest_addr);
                  }
                  printToWebClient(PSTR(" \r\n"));
                  token_counter++;
                }
                log_token = strtok(NULL,"=,");
              }
            }
            break;
          case 'E':
          case 'D':
            if (p[2]=='E') {
              logTelegram |= LOGTELEGRAM_ON;
            } else {
              if (logTelegram & LOGTELEGRAM_ON) logTelegram -= LOGTELEGRAM_ON;
            }
            printToWebClient(PSTR(MENU_TEXT_LOT ": "));
            printyesno(logTelegram);
            break;
          default:
            printToWebClient(PSTR(MENU_TEXT_ER1));
            break;
          }
        webPrintFooter();
        break;
        }
        if (p[1]=='P') {
          webPrintHeader();
          uint8_t myAddr = 0;
          uint8_t destAddr = 0;
          char* token = strtok(p,",");
          token = strtok(NULL, ",");
          if (token != 0) {
            int val = atoi(token);
            myAddr = (uint8_t)val;
          }
          token = strtok(NULL, ",");
          if (token != 0) {
            int val = atoi(token);
            destAddr = (uint8_t)val;
          }

          printToWebClient(PSTR(MENU_TEXT_BUS ": "));
          uint8_t savedbus = bus_type;
          bus_type = p[2] - '0';
          switch (bus_type) {
            case 0:
              own_address = myAddr;
              dest_address = destAddr;
              printToWebClient(PSTR("BSB"));
              break;
            case 1:
              own_address = myAddr;
              dest_address = destAddr;
              printToWebClient(PSTR("LPB"));
              break;
            case 2:
              pps_write = myAddr;
              printToWebClient(PSTR("PPS"));
              break;
            default:
              bus_type = savedbus;
              break;
          }
          setBusType();
          printToWebClient(PSTR("\r\n"));
          if (bus->getBusType() != BUS_PPS) {
            printFmtToWebClient(PSTR(" (%d, %d)"), myAddr, destAddr);
          } else {
            if (pps_write == 1) {
              printToWebClient(PSTR(" " MENU_TEXT_BRW));
            } else {
              printToWebClient(PSTR(" " MENU_TEXT_BRO));
            }
            printToWebClient(PSTR("\r\n"));
          }

          SetDevId();
          webPrintFooter();
          break;
        }
#endif
#endif
        if (p[1]=='N'){
          webPrintHeader();
          if (p[2]=='E') {
            printToWebClient(PSTR("Clearing EEPROM (affects MAX! devices and PPS-Bus settings)...<BR>\r\n"));
            forcedflushToWebClient();
          }

#ifdef LOGGER


          File dataFile = SD.open(datalogFileName, FILE_APPEND);
          if (dataFile) {
            dataFile.close();
          }
#endif
          if (p[2]=='E' && EEPROM_ready) {
            clearEEPROM();
          }
          printToWebClient(PSTR("Restarting micro-controller...\r\n"));
          webPrintFooter();
          resetBoard();
          break;
        }

        webPrintHeader();
        char* range;
        range = strtok(p,"/");
        while (range!=0) {
          if (range[0]=='U') {
            for (int i=0;i<numCustomFloats;i++) {
              printFmtToWebClient(PSTR("<tr><td>\r\ncustom_float[%d]: %.2f\r\n</td></tr>\r\n"), i, custom_floats[i]);
            }
            for (int i=0;i<numCustomLongs;i++) {
              printFmtToWebClient(PSTR("<tr><td>\r\ncustom_long[%d]: %ld\r\n</td></tr>\r\n"),i, custom_longs[i]);
            }
          } else if (range[0]=='X') {
#ifdef MAX_CUL
            if (enable_max_cul) {
              int max_avg_count = 0;
              float max_avg = 0;
              for (uint16_t x=0;x<MAX_CUL_DEVICES;x++) {
                if (max_cur_temp[x] > 0) {
                  max_avg += (float)(max_cur_temp[x] & 0x1FF) / 10;
                  max_avg_count++;
                  printFmtToWebClient(PSTR("<tr><td>%s (%lx): %.2f / %.2f"), max_device_list[x], max_devices[x], ((float)max_cur_temp[x] / 10),((float)max_dst_temp[x] / 2));
                  if (max_valve[x] > -1) {
                    printFmtToWebClient(PSTR(" (%hd%%)"), max_valve[x]);
                  }
                  printToWebClient(PSTR("</td></tr>"));
                }
              }
              if (max_avg_count > 0) {
                printFmtToWebClient(PSTR("<tr><td>AvgMax: %.2f</td></tr>\r\n"), max_avg / max_avg_count);
              } else {
                printToWebClient(PSTR("<tr><td>" MENU_TEXT_MXN "</td></tr>"));
              }
            }
#endif
#if !defined(I_DO_NOT_WANT_URL_CONFIG)
          } else if (range[0]=='A') {
#ifdef AVERAGES
            if (range[1]=='C' && range[2]=='=') {
              if (range[3]=='1') {
                LoggingMode |= CF_LOGMODE_24AVG;
              } else {
                LoggingMode &= ~CF_LOGMODE_24AVG;
              }
            }
            if (LoggingMode & CF_LOGMODE_24AVG) {
              if (range[1]=='=') {
                char* avg_token = strtok(range,"=,");
                avg_token = strtok(NULL,"=,");
                int token_counter = 0;
                if (avg_token != 0) {
                  resetAverageCalculation();
                  for (int i = 0; i < numAverages; i++) {
                    avg_parameters[i].number = 0;
                    avg_parameters[i].dest_addr = -1;
                  }
                  printToWebClient(PSTR(MENU_TEXT_24N ": "));
                }
                while (avg_token!=0) {
                  avg_parameters[token_counter] = parsingStringToParameter(avg_token);

                  if (token_counter < numAverages*2) {
                    printFmtToWebClient(PSTR("%g"), avg_parameters[token_counter].number);
                    if (avg_parameters[token_counter].dest_addr > -1) {
                      printFmtToWebClient(PSTR("!%d"), avg_parameters[token_counter].dest_addr);
                    }
                    printToWebClient(PSTR(" \r\n"));
                    token_counter++;
                  }
                  avg_token = strtok(NULL,"=,");
                }
                avgCounter = 1;
              }
            }
#else
            printToWebClient("AVERAGES module not compiled\r\n");
#endif
#endif
          } else if (range[0]=='G') {
            uint8_t val;
            uint8_t pin;

            p=range+1;
            if (!isdigit(*p)) {
              printToWebClient(PSTR(MENU_TEXT_ER1 "\r\n"));
              break;
            }
            pin=(uint8_t)atoi(p);

            char* dir_token = strchr(range,',');
            if (dir_token!=NULL) {
              dir_token++;
            }
            p=strchr(p,'=');
            if (p==NULL) {
              if (dir_token!=NULL) {
                if (*dir_token=='I') {
                  pinMode(pin, INPUT);
                  printFmtToDebug(PSTR("Pin %d set to input.\r\n"), pin);
                }
              }
              val=digitalRead(pin);
            } else {
              p++;
              uint16_t i = 0;
              while (p[i]) {
                p[i] |= 0x20;
                i++;
              }
              if (!strncmp_P(p, PSTR("on"), 2) || !strncmp_P(p, PSTR("high"), 2) || *p=='1') {
                val=HIGH;
              } else {
                val=LOW;
              }
              pinMode(pin, OUTPUT);
              digitalWrite(pin, val);
            }
            printFmtToWebClient(PSTR("GPIO%hu: %d"), pin, val!=LOW?1:0);
#if !defined(I_DO_NOT_NEED_NATIVE_WEB_INTERFACE)
          } else if (range[0]=='B') {
            if (range[1]=='0') {
              printToWebClient(STR20006);
              printToWebClient(PSTR(".<br>\r\n"));
              resetDurations();
            }
          } else {
            char* line_start;
            char* line_end;
            float start=-1;
            float end=-1;
            uint8_t save_my_dev_fam = my_dev_fam;
            uint8_t save_my_dev_var = my_dev_var;
            uint8_t destAddr = bus->getBusDest();
            if (range[0]=='K') {

              parameter param = parsingStringToParameter(range+1);
              if (param.dest_addr > -1) {
                set_temp_destination(param.dest_addr);
              }
              uint cat = param.number * 2;
              if (cat >= sizeof(ENUM_CAT_NR)/sizeof(*ENUM_CAT_NR)) {
                cat = (sizeof(ENUM_CAT_NR)/sizeof(*ENUM_CAT_NR))-2;
              }
              start = ENUM_CAT_NR[cat];
              end = ENUM_CAT_NR[cat+1];
              if (cat+2 < sizeof(ENUM_CAT_NR)/sizeof(*ENUM_CAT_NR)) {
                if (end > ENUM_CAT_NR[cat+2]) {
                  end = ENUM_CAT_NR[cat+2]-1;
                }
              }
            } else {

              line_start=range;
              line_end=strchr(range,'-');
              if (line_end==NULL) {
                line_end=line_start;
              } else {
                *line_end='\0';
                line_end++;
              }
              parameter param = parsingStringToParameter(line_end);
              end = param.number;
              if (param.dest_addr > -1) {
                set_temp_destination(param.dest_addr);






              }

              start=atof(line_start);
            }
            query(start,end,0);
            if (bus->getBusDest() != destAddr) {
              return_to_default_destination(destAddr);
              my_dev_fam = save_my_dev_fam;
              my_dev_var = save_my_dev_var;
            }
#endif
          }
          range = strtok(NULL,"/");
        }
        webPrintFooter();
        break;
      }

      delay(1);
      loopCount++;
      if (loopCount > 1000) {
        client.stop();
        printlnToDebug(PSTR("\r\nTimeout"));
      }

    }






    client_flag = false;
    if (client.available()) {
      printToDebug(PSTR("\r\nClient buffer gets discarded:\r\n"));
      while (client.available()) Serial.print((char)client.read());
    }


    delay(1);

    client.flush();
    client.stop();
  }

#ifdef MQTT
#if defined(ESP32) && defined(WIFI)
  if(!localAP){
#else
  {
#endif
    if (mqtt_broker_ip_addr[0] && (LoggingMode & CF_LOGMODE_MQTT)) {

      mqtt_connect();
      MQTTPubSubClient->loop();

      if ((((millis() - lastMQTTTime >= (log_interval * 1000)) && log_interval > 0) || log_now > 0) && numLogValues > 0) {
        lastMQTTTime = millis();
        uint8_t destAddr = bus->getBusDest();
        uint8_t d_addr = destAddr;
        uint8_t save_my_dev_fam = my_dev_fam;
        uint8_t save_my_dev_var = my_dev_var;
        for (int i=0; i < numLogValues; i++) {
          if (log_parameters[i].number > 0) {
            if (log_parameters[i].dest_addr > -1){
              if(log_parameters[i].dest_addr != d_addr) {
                d_addr = log_parameters[i].dest_addr;
                set_temp_destination(log_parameters[i].dest_addr);
              }
            } else {
              if (destAddr != d_addr) {
                d_addr = destAddr;
                return_to_default_destination(destAddr);
                my_dev_fam = save_my_dev_fam;
                my_dev_var = save_my_dev_var;
              }
            }
            mqtt_sendtoBroker(log_parameters[i]);
          }
        }
        if (destAddr != d_addr) {
          return_to_default_destination(destAddr);
          my_dev_fam = save_my_dev_fam;
          my_dev_var = save_my_dev_var;
        }
        if (MQTTPubSubClient != NULL && !(LoggingMode & CF_LOGMODE_MQTT)) {

          printlnToDebug(PSTR("MQTT will be disconnected on order through web interface"));
        }
      }
    }
    if (!(LoggingMode & CF_LOGMODE_MQTT)) {
      mqtt_disconnect();
    }
  }
#endif


#ifdef LOGGER
  if ((LoggingMode & (CF_LOGMODE_SD_CARD|CF_LOGMODE_UDP))) {
    if (((millis() - lastLogTime >= (log_interval * 1000)) && log_interval > 0) || log_now > 0) {

      log_now = 0;
      File dataFile;
      if (LoggingMode & CF_LOGMODE_SD_CARD) {
#if defined(ESP32)
        uint64_t freespace = SD.totalBytes() - SD.usedBytes();
#else
        uint32_t freespace = SD.vol()->freeClusterCount();
#endif
        if (freespace > MINIMUM_FREE_SPACE_ON_SD) {
          dataFile = SD.open(datalogFileName, FILE_APPEND);
          if (!dataFile) {

            printToWebClient(PSTR(MENU_TEXT_DTO "\r\n"));
            printFmtToDebug(PSTR("Error opening %s!\r\n"), datalogFileName);
          }
        }
      }
      IPAddress broadcast_ip;
      if (LoggingMode & CF_LOGMODE_UDP) {
#ifdef WIFI
        IPAddress local_ip = WiFi.localIP();
#else
        IPAddress local_ip = Ethernet.localIP();
#endif
        broadcast_ip = IPAddress(local_ip[0], local_ip[1], local_ip[2], 0xFF);
      }
      uint8_t destAddr = bus->getBusDest();
      uint8_t d_addr = destAddr;
      uint8_t save_my_dev_fam = my_dev_fam;
      uint8_t save_my_dev_var = my_dev_var;
      for (int i = 0; i < numLogValues; i++) {
        int outBufLen = 0;
        if (log_parameters[i].number > 0) {
          if (log_parameters[i].dest_addr > -1){
            if (log_parameters[i].dest_addr != d_addr) {
              d_addr = log_parameters[i].dest_addr;
              set_temp_destination(log_parameters[i].dest_addr);
            }
          } else {
            if (destAddr != d_addr) {
              d_addr = destAddr;
              return_to_default_destination(destAddr);
              my_dev_fam = save_my_dev_fam;
              my_dev_var = save_my_dev_var;
            }
          }
          query(log_parameters[i].number);
          if (decodedTelegram.prognr < 0) continue;
          if (LoggingMode & CF_LOGMODE_UDP) udp_log.beginPacket(broadcast_ip, UDP_LOG_PORT);
          outBufLen += sprintf_P(outBuf + outBufLen, PSTR("%lu;%s;%g;"), millis(), GetDateTime(outBuf + outBufLen + 80), log_parameters[i].number);

#ifdef AVERAGES
          if ((log_parameters[i].number >= BSP_AVERAGES && log_parameters[i].number < BSP_AVERAGES + numAverages)) {

            outBufLen += strlen(strcpy_P(outBuf + outBufLen, PSTR(STR_24A_TEXT ". ")));
          }
#endif
          if (dataFile) {
            if (previousDatalogDate.combined != currentDate.combined) {
              File indexFile = SD.open(datalogIndexFileName, FILE_APPEND);
              if (indexFile) {
                previousDatalogDate.combined = currentDate.combined;
                long currentDatalogPosition = dataFile.size();
                indexFile.write((byte*)&currentDate, sizeof(currentDate));
                indexFile.write((byte*)&currentDatalogPosition, sizeof(currentDatalogPosition));
                indexFile.close();
                if (!firstDatalogDate.combined) firstDatalogDate.combined = currentDate.combined;
              }
              else printFmtToDebug(PSTR("Error opening %s for writing!\r\n"), datalogIndexFileName);
            }
            dataFile.print(outBuf);
          }
          if (LoggingMode & CF_LOGMODE_UDP) udp_log.print(outBuf);
          outBufLen = 0;
          strcpy_PF(outBuf + outBufLen, decodedTelegram.prognrdescaddr);
          if (dataFile) dataFile.print(outBuf);
          if (LoggingMode & CF_LOGMODE_UDP) udp_log.print(outBuf);
          outBufLen = 0;
          if (decodedTelegram.sensorid) {
            outBufLen += sprintf_P(outBuf + outBufLen, PSTR("#%d"), decodedTelegram.sensorid);
          }
          outBufLen += sprintf_P(outBuf + outBufLen, PSTR(";%s;%s\r\n"), decodedTelegram.value, decodedTelegram.unit);
          if (dataFile) dataFile.print(outBuf);
          if (LoggingMode & CF_LOGMODE_UDP) udp_log.print(outBuf);
          if (LoggingMode & CF_LOGMODE_UDP) udp_log.endPacket();
        }
      }
      if (dataFile) dataFile.close();
      lastLogTime = millis();
      if (destAddr != d_addr) {
        return_to_default_destination(destAddr);
        my_dev_fam = save_my_dev_fam;
        my_dev_var = save_my_dev_var;
      }
    }
  }
#endif


#ifdef AVERAGES
  if (LoggingMode & CF_LOGMODE_24AVG) {
    if (millis() / 60000 != lastAvgTime) {
      if (avgCounter == 1441) {
        for (int i=0; i<numAverages; i++) {
          avgValues_Old[i] = avgValues[i];
          avgValues_Current[i] = 0;
        }
        avgCounter = 1;
      }

      uint8_t destAddr = bus->getBusDest();
      uint8_t d_addr = destAddr;
      uint8_t save_my_dev_fam = my_dev_fam;
      uint8_t save_my_dev_var = my_dev_var;
      for (int i = 0; i < numAverages; i++) {
        if (avg_parameters[i].number > 0) {
          if (avg_parameters[i].dest_addr > -1){
            if (avg_parameters[i].dest_addr != d_addr) {
              d_addr = avg_parameters[i].dest_addr;
              set_temp_destination(d_addr);
              GetDevId();
            }
          } else {
            if (destAddr != d_addr) {
              d_addr = destAddr;
              return_to_default_destination(destAddr);
              my_dev_fam = save_my_dev_fam;
              my_dev_var = save_my_dev_var;
            }
          }
          query(avg_parameters[i].number);
          float reading = strtod(decodedTelegram.value,NULL);
          printFmtToDebug(PSTR("%f\r\n"), reading);
          if (isnan(reading)) {} else {
            avgValues_Current[i] = (avgValues_Current[i] * (avgCounter-1) + reading) / avgCounter;
            if (avgValues_Old[i] == -9999) {
              avgValues[i] = avgValues_Current[i];
            } else {
              avgValues[i] = ((avgValues_Old[i]*(1440-avgCounter))+(avgValues_Current[i]*avgCounter)) / 1440;
            }
          }
          printFmtToDebug(PSTR("%f\r\n"), avgValues[i]);
        }
      }
      if (destAddr != d_addr) {
        return_to_default_destination(destAddr);
        my_dev_fam = save_my_dev_fam;
        my_dev_var = save_my_dev_var;
      }

      avgCounter++;
      lastAvgTime = millis() / 60000;

  #ifdef LOGGER


      if (avg_parameters[0].number > 0) {
        File avgfile = SD.open(averagesFileName, FILE_WRITE);
        if (avgfile) {
          avgfile.seek(0);
          for (int i = 0; i < numAverages; i++) {
            sprintf_P(outBuf, PSTR("%f\r\n%f\r\n%f\r\n"), avgValues[i], avgValues_Old[i], avgValues_Current[i]);
            avgfile.print(outBuf);
          }
          avgfile.println(avgCounter);
          avgfile.close();
        }
      }
  #endif
  }
  } else {
    avgCounter = 1;
  }
#endif

#ifdef ONE_WIRE_BUS
  {
    if (One_Wire_Pin) {
      unsigned long tempTime = millis() / ONE_WIRE_REQUESTS_PERIOD;
      if (tempTime != lastOneWireRequestTime) {
        sensors->requestTemperatures();
        lastOneWireRequestTime = tempTime;
      }
    }
  }
#endif

#ifdef RGT_EMULATOR
  {
    byte tempTime = (millis() / 60000) % 60;
    if (newMinuteValue != tempTime) {
      newMinuteValue = tempTime;
      uint8_t k = 3;
      if (bus->getBusType() == BUS_PPS) {
        k = 1;
      }
      for (uint8_t i = 0; i < k; i++) {
        if (rgte_sensorid[i][0].number != 0) {
          uint8_t z = 0;
          float value = 0;
          float lowest = MQTT_TEMP_VALID_THRESHOLD;
          const int numSensors = sizeof(rgte_sensorid[i]) / sizeof(rgte_sensorid[i][0]);
          for (uint8_t j = 0; j < numSensors; j++) {
            if (rgte_sensorid[i][j].number != 0) {
              if(rgte_sensorid[i][j].dest_addr != -1) set_temp_destination(rgte_sensorid[i][j].dest_addr);
              query(rgte_sensorid[i][j].number);
              if(rgte_sensorid[i][j].dest_addr != -1) return_to_default_destination(dest_address);
              if (decodedTelegram.type == VT_TEMP && decodedTelegram.error == 0) {
                z++;
                value += atof(decodedTelegram.value);
                if (value < lowest) {
                  lowest = value;
                }
              }
            }
          }
          if (z != 0) {
            switch (rgte_calculation[i]) {
              case RGT_CALC_LOW:
                _printFIXPOINT(decodedTelegram.value, lowest, 2);
                break;
              default:
                _printFIXPOINT(decodedTelegram.value, value / z, 2);
                break;
            }
            printFmtToDebug(PSTR("Calculated temperature: %s\r\n"), decodedTelegram.value);
            if (bus->getBusType() != BUS_PPS) {



              set(10000 + i, decodedTelegram.value, false);

            } else {
              set(15000 + PPS_RTI, decodedTelegram.value, false);
            }
          }
        }
      }
    }
  }
#endif

#ifdef BUTTONS
  if (PressedButtons) {
    for (uint8_t i = 0; i < 8; i++) {
      switch (PressedButtons & (0x01 << i)) {
        case TWW_PUSH_BUTTON_PRESSED:
          strcpy_P(decodedTelegram.value, PSTR("1"));
          set(1603, decodedTelegram.value, true);
          PressedButtons &= ~TWW_PUSH_BUTTON_PRESSED;
          break;
        case ROOM1_PRESENCE_BUTTON_PRESSED:
          switchPresenceState(701, 10102);
          PressedButtons &= ~ROOM1_PRESENCE_BUTTON_PRESSED;
          break;
        case ROOM2_PRESENCE_BUTTON_PRESSED:
          switchPresenceState(1001, 10103);
          PressedButtons &= ~ROOM2_PRESENCE_BUTTON_PRESSED;
          break;
        case ROOM3_PRESENCE_BUTTON_PRESSED:
          switchPresenceState(1301, 10104);
          PressedButtons &= ~ROOM3_PRESENCE_BUTTON_PRESSED;
          break;
        default: PressedButtons &= ~(0x01 << i); break;
      }
    }
  }
#endif

#ifdef WATCH_SOCKETS
  ShowSockStatus();
  checkSockStatus();
#endif





#ifdef CUSTOM_COMMANDS
  {
    custom_timer = millis();
    #include "BSB_LAN_custom.h"
  }
#endif

#ifdef MAX_CUL
  if (enable_max_cul) {
  byte max_str_index = 0;
#if OUTBUF_LEN < 60
#error "OUTBUF_LEN must be at least 60. In other case MAX! will not work."
#endif

  while (max_cul->available() && EEPROM_ready) {
    c = max_cul->read();
#ifdef DEVELOPER_DEBUG
    printFmtToDebug(PSTR("%c"), c);
#endif
    if ((c!='\n') && (c!='\r') && (max_str_index<60)) {
      outBuf[max_str_index++]=c;
    } else {
#ifdef DEVELOPER_DEBUG
      writelnToDebug();
#endif
      break;
    }
  }
  if (max_str_index > 0) {
    if (outBuf[0] == 'Z') {
      char max_hex_str[9];
      char max_id[sizeof(max_device_list[0])] = { 0 };
      bool known_addr = false;
      bool known_eeprom = false;

      strncpy(max_hex_str, outBuf+7, 2);
      max_hex_str[2]='\0';
      uint8_t max_msg_type = (uint8_t)strtoul(max_hex_str, NULL, 16);
      strncpy(max_hex_str, outBuf+1, 2);
      max_hex_str[2]='\0';
      uint8_t max_msg_len = (uint8_t)strtoul(max_hex_str, NULL, 16);
      if (max_msg_type == 0x02) {
        strncpy(max_hex_str, outBuf+15, 6);
      } else {
        strncpy(max_hex_str, outBuf+9, 6);
      }
      max_hex_str[6]='\0';
      int32_t max_addr = (int32_t)strtoul(max_hex_str,NULL,16);
      uint16_t max_idx=0;
      for (max_idx=0;max_idx < MAX_CUL_DEVICES;max_idx++) {
        if (max_addr == max_devices[max_idx]) {
          known_addr = true;
          break;
        }
      }

      if (max_msg_type == 0x00) {
        for (int x=0;x<10;x++) {
          strncpy(max_hex_str, outBuf+29+(x*2), 2);
          max_hex_str[2]='\0';
          max_id[x] = (char)strtoul(max_hex_str,NULL,16);
        }
        max_id[sizeof(max_device_list[0]) - 1] = '\0';
        printFmtToDebug(PSTR("MAX device info received:\r\n%08lX\r\n%s\r\n"), max_addr, max_id);

        for (uint16_t x=0;x<MAX_CUL_DEVICES;x++) {
          if (max_devices[x] == max_addr) {
            printlnToDebug(PSTR("Device already in EEPROM"));
            known_eeprom = true;
            break;
          }
        }

        if (!known_eeprom) {
          for (uint16_t x=0;x<MAX_CUL_DEVICES;x++) {
            if (max_devices[x] < 1) {
              strcpy(max_device_list[x], max_id);
              max_devices[x] = max_addr;

              writeToEEPROM(CF_MAX_DEVICES);
              writeToEEPROM(CF_MAX_DEVADDR);
              printlnToDebug(PSTR("Device stored in EEPROM"));
              break;
            }
          }
        }
      }

      if (max_msg_type == 0x02) {
        strncpy(max_hex_str, outBuf+27, 2);
        max_hex_str[2]='\0';
        max_valve[max_idx] = (uint32_t)strtoul(max_hex_str,NULL,16);
        printFmtToDebug(PSTR("Valve position from associated thermostat received:\r\n%08lX\r\n%lu\r\n"), max_addr, max_valve[max_idx]);
      }

      if ((max_msg_type == 0x42 || max_msg_type == 0x60) && known_addr == true) {
        uint8_t temp_str_offset;
        uint32_t max_temp_status;
        uint8_t str_len;

        switch (max_msg_len) {
          case 0x0C: temp_str_offset = 23; str_len = 4; break;
          case 0x0E: temp_str_offset = 25; str_len = 8; break;
          default: temp_str_offset = 0; str_len = 8; break;
        }
        strncpy(max_hex_str, outBuf+temp_str_offset, str_len);
        max_hex_str[str_len]='\0';
        max_temp_status = (uint32_t)strtoul(max_hex_str,NULL,16);
#ifdef DEVELOPER_DEBUG
        printFmtToDebug(PSTR("%d\r\n%08lX\r\n"), max_msg_len, max_temp_status);
#endif
        if (max_msg_type == 0x42) {
          max_cur_temp[max_idx] = (((max_temp_status & 0x8000) >> 7) + ((max_temp_status & 0xFF)));
          max_dst_temp[max_idx] = (max_temp_status & 0x7F00) >> 8;
        }
        if (max_msg_type == 0x60) {
          max_cur_temp[max_idx] = (max_temp_status & 0x0100) + (max_temp_status & 0xFF);
          max_dst_temp[max_idx] = (max_temp_status & 0xFF0000) >> 16;
          max_valve[max_idx] = (max_temp_status & 0xFF000000) >> 24;
        }

        printlnToDebug(PSTR("MAX temperature message received:"));
        printFmtToDebug(PSTR("%08lX\r\n%f\r\n%f\r\n%lu\r\n"), max_addr, ((float)max_cur_temp[max_idx] / 10), (float)(max_dst_temp[max_idx] / 2), max_valve[max_idx]);
      }
    }
  }
}
#endif

  if (debug_mode == 2) {
    if (haveTelnetClient == false) {
      telnetClient = telnetServer->available();
    }
    if (telnetClient && haveTelnetClient == false) {
      telnetClient.flush();
      haveTelnetClient = true;
      printToDebug(PSTR("Version: "));
      printlnToDebug(BSB_VERSION);
    }
    if (!telnetClient.connected()) {
      haveTelnetClient = false;
      telnetClient.stop();
    }
  }
#if defined(MDNS_SUPPORT) && !defined(ESP32)
  if(mDNS_hostname[0]) {
    mdns.run();
  }
#endif
#if defined(ESP32) && defined(ENABLE_ESP32_OTA)
 if(enable_ota_update)
  update_server.handleClient();
#endif

#if defined(WIFI) && defined(ESP32)
  if (localAP == true && millis() - localAPtimeout > 30 * 60 * 1000) {
    resetBoard();
  }
#endif

  if (millis() - maintenance_timer > 60000) {
    printFmtToDebug(PSTR("%lu Ping!\r\n"), millis());
    maintenance_timer = millis();

    if (bus->getBusType() != BUS_PPS && !my_dev_fam) {
      SetDevId();
    }
#if defined(ESP32)
    struct tm now;
    getLocalTime(&now,100);
    if (now.tm_year < 100) {
      SetDateTime();
    }
#endif

#if defined(WIFI) && defined(ESP32)

    bool not_preferred_bssid = false;
    if (WiFi.BSSID() != NULL) {
      for (int x=0;x<6;x++) {
        if (WiFi.BSSID()[x] != bssid[x] && bssid[x] > 0) {
          not_preferred_bssid = true;
        }
      }
    }

    if ((WiFi.status() != WL_CONNECTED || not_preferred_bssid == true) && localAP == false) {
      printFmtToDebug(PSTR("Reconnecting to WiFi...\r\n"));
      scanAndConnectToStrongestNetwork();
    }
#endif
  }
#if defined(ESP32)
  esp_task_wdt_reset();
#endif
}

#if defined(WIFI) && defined(ESP32)

String scanAndConnectToStrongestNetwork() {
  int sum_bssid = 0;
  for (int x=0;x<6;x++) {
    sum_bssid += bssid[x];
  }
  if (sum_bssid > 0) {
    printToDebug(PSTR("Using default BSSID to connect to WiFi..."));
    esp_wifi_disconnect();
    WiFi.begin(wifi_ssid, wifi_pass, 0, bssid);
    unsigned long timeout = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - timeout < 5000) {
      delay(100);
      printFmtToDebug(PSTR("."));
    }
    printlnToDebug(PSTR(""));
    if (WiFi.status() == WL_CONNECTED) {
      return ("Connection successful using default BSSID.");
    } else {
      printlnToDebug(PSTR("Connection with default BSSID failed, trying to scan..."));
    }
  }
  esp_wifi_disconnect();
  int i_strongest = -1;
  int32_t rssi_strongest = -100;
  printFmtToDebug(PSTR("Start scanning for SSID %s\r\n"), wifi_ssid);

  int n = WiFi.scanNetworks(false, false);

  printToDebug(PSTR("Scan done."));

  if (n == 0) {
    printlnToDebug(PSTR("No networks found!"));
    return ("");
  } else {
    printFmtToDebug(PSTR("%d networks found:\r\n"), n);
    for (int i = 0; i < n; ++i) {

      printFmtToDebug(PSTR("%d: BSSID: %s  %2ddBm, %3d%%  %9s  %s\r\n"), i, WiFi.BSSIDstr(i).c_str(), WiFi.RSSI(i), constrain(2 * (WiFi.RSSI(i) + 100), 0, 100), (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "open" : "encrypted", WiFi.SSID(i).c_str());
      if ((String(wifi_ssid) == String(WiFi.SSID(i)) && (WiFi.RSSI(i)) > rssi_strongest)) {
        rssi_strongest = WiFi.RSSI(i);
        i_strongest = i;
      }
    }
  }

  if (i_strongest < 0) {
    printFmtToDebug(PSTR("No network with SSID %s found!\r\n"), wifi_ssid);
    return ("");
  }
  printFmtToDebug(PSTR("SSID match found at %d. Connecting...\r\n"), i_strongest);
  WiFi.begin(wifi_ssid, wifi_pass, 0, WiFi.BSSID(i_strongest));
  return (WiFi.BSSIDstr(i_strongest));
}

void printWifiStatus()
{
  if (WiFi.SSID() != NULL) {

    printFmtToDebug(PSTR("SSID: %s\r\n"), WiFi.SSID());
    printFmtToDebug(PSTR("BSSID: %02X:%02X:%02X:%02X:%02X:%02X\r\n"), WiFi.BSSID()[0], WiFi.BSSID()[1], WiFi.BSSID()[2], WiFi.BSSID()[3], WiFi.BSSID()[4], WiFi.BSSID()[5]);

    IPAddress t = WiFi.localIP();
    printFmtToDebug(PSTR("IP Address: %d.%d.%d.%d\r\n"), t[0], t[1], t[2], t[3]);


    long rssi = WiFi.RSSI();
    printFmtToDebug(PSTR("Signal strength (RSSI): %l dBm\r\n"), rssi);
  }
}
#endif


#if defined LOGGER || defined WEBSERVER

  #if !defined(ESP32)
void dateTime(uint16_t* date, uint16_t* time) {

  *date = FS_DATE(year(), month(), day());


  *time = FS_TIME(hour(), minute(), second());

}
  #endif
#endif
# 7889 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
void setup() {
#ifdef BtSerial
  SerialOutput = &Serial2;
  Serial2.begin(115200, SERIAL_8N1);
#else
  SerialOutput = &Serial;
  Serial.begin(115200, SERIAL_8N1);
#endif

  decodedTelegram.telegramDump = NULL;
  pinMode(EEPROM_ERASING_PIN, INPUT_PULLUP);
#if defined(EEPROM_ERASING_GND_PIN)
  pinMode(EEPROM_ERASING_GND_PIN, OUTPUT);
#endif

  SerialOutput->println(F("READY"));

#if defined(__arm__)
  Wire.begin();
  if (!EEPROM.ready()) {
    EEPROM_ready = false;
    SerialOutput->println(F("EEPROM not ready"));
  }
#endif

  pinMode(LED_BUILTIN, OUTPUT);

#ifdef ESP32
  EEPROM.begin(EEPROM_SIZE);
#endif

  if (!digitalRead(EEPROM_ERASING_PIN)) {
    digitalWrite(LED_BUILTIN, HIGH);
    clearEEPROM();
    internalLEDBlinking(125, 16);
  }




  initConfigTable(0);
#ifdef MAX_CUL
  registerConfigVariable(CF_MAX_DEVICES, (byte *)max_device_list);
  registerConfigVariable(CF_MAX_DEVADDR, (byte *)max_devices);
#endif
  registerConfigVariable(CF_PPS_VALUES, (byte *)pps_values);
#ifdef CONFIG_IN_EEPROM
  uint8_t EEPROMversion = 0;
  registerConfigVariable(CF_USEEEPROM, (byte *)&UseEEPROM);
  registerConfigVariable(CF_VERSION, (byte *)&EEPROMversion);
  uint32_t crc;
  registerConfigVariable(CF_CRC32, (byte *)&crc);

  registerConfigVariable(CF_BUSTYPE, (byte *)&bus_type);
  registerConfigVariable(CF_OWN_BSBLPBADDR, (byte *)&own_address);
  registerConfigVariable(CF_DEST_BSBLPBADDR, (byte *)&dest_address);
  registerConfigVariable(CF_PPS_MODE, (byte *)&pps_write);
  registerConfigVariable(CF_LOGTELEGRAM, (byte *)&logTelegram);
  registerConfigVariable(CF_AVERAGESLIST, (byte *)avg_parameters);
  registerConfigVariable(CF_LOGCURRINTERVAL, (byte *)&log_interval);
  registerConfigVariable(CF_CURRVALUESLIST, (byte *)log_parameters);
  registerConfigVariable(CF_ESP32_ENERGY_SAVE, (byte *)&esp32_save_energy);
#ifdef WEBCONFIG
  registerConfigVariable(CF_ROOM_DEVICE, (byte *)&pps_values[PPS_QTP]);
  registerConfigVariable(CF_MAC, (byte *)mac);
  registerConfigVariable(CF_DHCP, (byte *)&useDHCP);
  registerConfigVariable(CF_IPADDRESS, (byte *)ip_addr);
  registerConfigVariable(CF_MASK, (byte *)subnet_addr);
  registerConfigVariable(CF_GATEWAY, (byte *)gateway_addr);
  registerConfigVariable(CF_DNS, (byte *)dns_addr);
  registerConfigVariable(CF_WWWPORT, (byte *)&HTTPPort);
  registerConfigVariable(CF_TRUSTEDIPADDRESS, (byte *)trusted_ip_addr);
  registerConfigVariable(CF_TRUSTEDIPADDRESS2, (byte *)trusted_ip_addr2);
  registerConfigVariable(CF_WIFI_SSID, (byte *)wifi_ssid);
  registerConfigVariable(CF_WIFI_PASSWORD, (byte *)wifi_pass);
  registerConfigVariable(CF_PASSKEY, (byte *)PASSKEY);
  registerConfigVariable(CF_BASICAUTH, (byte *)USER_PASS);
  registerConfigVariable(CF_ONEWIREBUS, (byte *)&One_Wire_Pin);
  registerConfigVariable(CF_DHTBUS, (byte *)DHT_Pins);
  registerConfigVariable(CF_BMEBUS, (byte *)&BME_Sensors);
  registerConfigVariable(CF_OTA_UPDATE, (byte *)&enable_ota_update);
  registerConfigVariable(CF_MDNS_HOSTNAME, (byte *)mDNS_hostname);
  registerConfigVariable(CF_IPWE, (byte *)&enable_ipwe);
  registerConfigVariable(CF_IPWEVALUESLIST, (byte *)ipwe_parameters);
  registerConfigVariable(CF_MAX, (byte *)&enable_max_cul);
  registerConfigVariable(CF_MAX_IPADDRESS, (byte *)max_cul_ip_addr);
  registerConfigVariable(CF_MQTT, (byte *)&mqtt_mode);
  registerConfigVariable(CF_MQTT_IPADDRESS, (byte *)mqtt_broker_ip_addr);
  registerConfigVariable(CF_MQTT_USERNAME, (byte *)MQTTUsername);
  registerConfigVariable(CF_MQTT_PASSWORD, (byte *)MQTTPassword);
  registerConfigVariable(CF_MQTT_TOPIC, (byte *)MQTTTopicPrefix);
  registerConfigVariable(CF_MQTT_DEVICE, (byte *)MQTTDeviceID);
  registerConfigVariable(CF_LOGMODE, (byte *)&LoggingMode);
  if (default_flag & FL_SW_CTL_RONLY) {
    registerConfigVariable(CF_WRITEMODE, (byte *)&programWriteMode);
  }
  registerConfigVariable(CF_DEBUG, (byte *)&debug_mode);
  registerConfigVariable(CF_VERBOSE, (byte *)&verbose);
  registerConfigVariable(CF_MONITOR, (byte *)&monitor);
  registerConfigVariable(CF_SHOW_UNKNOWN, (byte *)&show_unknown);
  registerConfigVariable(CF_CHECKUPDATE, (byte *)&enable_version_check);
  registerConfigVariable(CF_RGT1_SENSOR_ID, (byte *)&rgte_sensorid[0][0]);
  registerConfigVariable(CF_RGT2_SENSOR_ID, (byte *)&rgte_sensorid[1][0]);
  registerConfigVariable(CF_RGT3_SENSOR_ID, (byte *)&rgte_sensorid[2][0]);
  registerConfigVariable(CF_TWW_PUSH_PIN_ID, (byte *)&button_on_pin[0]);
  registerConfigVariable(CF_RGT1_PRES_PIN_ID, (byte *)&button_on_pin[1]);
  registerConfigVariable(CF_RGT2_PRES_PIN_ID, (byte *)&button_on_pin[2]);
  registerConfigVariable(CF_RGT3_PRES_PIN_ID, (byte *)&button_on_pin[3]);
  registerConfigVariable(CF_RX_PIN, (byte *)&bus_pins[0]);
  registerConfigVariable(CF_TX_PIN, (byte *)&bus_pins[1]);
  registerConfigVariable(CF_DEVICE_FAMILY, (byte *)&fixed_device_family);
  registerConfigVariable(CF_DEVICE_VARIANT, (byte *)&fixed_device_variant);
#endif
#if defined(JSONCONFIG) || defined(WEBCONFIG)
  registerConfigVariable(CF_CONFIG_LEVEL, (byte *)&config_level);
#endif
#ifdef WEBCONFIG
#if defined(BLE_SENSORS) && defined(ESP32)
  registerConfigVariable(CF_ENABLE_BLE, (byte *)&EnableBLE);
  registerConfigVariable(CF_BLE_SENSORS_MACS, (byte *)BLE_sensors_macs);
#endif
#endif

  readFromEEPROM(CF_PPS_VALUES);
  byte UseEEPROM_in_config_h = UseEEPROM;
  if (UseEEPROM) {
    SerialOutput->println(F("Reading EEPROM"));
    readFromEEPROM(CF_USEEEPROM);
    readFromEEPROM(CF_VERSION);
  }
    bool crc_correct = true;
  if (UseEEPROM == 0x96) {
    readFromEEPROM(CF_CRC32);
    if (crc == initConfigTable(EEPROMversion)) {

      for (uint8_t i = 0; i < sizeof(config)/sizeof(config[0]); i++) {
#ifdef DEVELOPER_DEBUG
        SerialOutput->print(F(" Read parameter # ")); SerialOutput->println(i);
#endif

        if (config[i].version > 0 && config[i].version <= EEPROMversion) readFromEEPROM(config[i].id);
      }
    } else {
      SerialOutput->println(F("EEPROM schema CRC mismatch"));
      crc_correct = false;
    }
  } else {
    SerialOutput->println(F("Using settings from config file"));
    initConfigTable(EEPROMversion);
  }
  SerialOutput->println(F("Reading done."));


  uint8_t maxconfversion = 0;
  for (uint8_t i = 0; i < sizeof(config)/sizeof(config[0]); i++) {
    if (config[i].version > maxconfversion) maxconfversion = config[i].version;
  }
  SerialOutput->print(F("EEPROM schema v."));
  SerialOutput->print(EEPROMversion);
  SerialOutput->print(F(" Program schema v."));
  SerialOutput->println(maxconfversion);

  if (maxconfversion != EEPROMversion || !crc_correct) {
    crc = initConfigTable(maxconfversion);
    EEPROMversion = maxconfversion;
    if (UseEEPROM_in_config_h == 0x01) {
      if (UseEEPROM != 0x69 && UseEEPROM != 0x96) UseEEPROM = 0x96;
      SerialOutput->println(F("Update EEPROM schema"));
      for (uint8_t i = 0; i < CF_LAST_OPTION; i++) {

        if (i != CF_MAX_DEVICES && i != CF_MAX_DEVADDR) {
          writeToEEPROM(i);
  #ifdef DEVELOPER_DEBUG
          SerialOutput->print(F("Write option # ")); SerialOutput->println(i);
  #endif
        }
      }
    }
  }

  unregisterConfigVariable(CF_VERSION);
  unregisterConfigVariable(CF_CRC32);
#endif

  byte save_debug_mode = debug_mode;
  debug_mode = 1;

  for (uint8_t i = 0; i < CF_LAST_OPTION; i++) {
    printFmtToDebug(PSTR("Address EEPROM option %d: %d\r\n"), i, getEEPROMaddress(i));
  }

  if (save_debug_mode == 2) printToDebug(PSTR("Logging output to Telnet\r\n"));
  printFmtToDebug(PSTR("Size of cmdtbl: %d\r\n"),sizeof(cmdtbl));
  printFmtToDebug(PSTR("free RAM: %d\r\n"), freeRam());

  while (SerialOutput->available()) {
    printFmtToDebug(PSTR("%c"), SerialOutput->read());
  }
  printFmtToDebug(PSTR("BSB-LAN version: %s\r\n"), BSB_VERSION);

  byte temp_bus_pins[2];
  if (bus_pins[0] && bus_pins[1]) {
    temp_bus_pins[0] = bus_pins[0];
    temp_bus_pins[1] = bus_pins[1];
  } else {
#if defined(ESP32)
  #if defined(RX1) && defined(TX1)
    #if ETH_PHY_POWER == 12
      printToDebug(PSTR("Microcontroller: ESP32/Olimex PoE\r\n"));
    #else
      printToDebug(PSTR("Microcontroller: ESP32/Olimex EVB\r\n"));
    #endif
    pinMode(4, INPUT);
    if (digitalRead(4) == 0) {
      temp_bus_pins[0] = RX1;
    #if ETH_PHY_POWER == 12
      temp_bus_pins[1] = 5;
    #else
      temp_bus_pins[1] = 17;
    #endif
    } else {
      temp_bus_pins[0] = RX1;
      temp_bus_pins[1] = TX1;
    }
  #else
    printToDebug(PSTR("Microcontroller: ESP32/NodeMCU\r\n"));
    temp_bus_pins[0] = 16;
    temp_bus_pins[1] = 17;
  #endif
#else
    printToDebug(PSTR("Microcontroller: ARM/Arduino Due\r\n"));

    temp_bus_pins[0] = 19;
    temp_bus_pins[1] = 18;
#endif
  }

#if defined(__arm__)
  pinMode(temp_bus_pins[0], INPUT);
#endif

  printFmtToDebug(PSTR("Using RX/TX pins %d, %d\r\n"), temp_bus_pins[0], temp_bus_pins[1]);
  bus = new BSB(temp_bus_pins[0], temp_bus_pins[1]);
  setBusType();

  printToDebug(PSTR("Bus type defined in config: "));
  switch (bus_type) {
    case BUS_BSB: printlnToDebug(PSTR("BSB")); break;
    case BUS_LPB: printlnToDebug(PSTR("LPB")); break;
    case BUS_PPS: printlnToDebug(PSTR("PPS")); break;
    default: printlnToDebug(PSTR("unknown!"));
  }

  bus->enableInterface();
#if defined(ESP32)
  if (esp32_save_energy == true) {
    bus->disableInterface();
    setCpuFrequencyMhz(80);
    bus->enableInterface();
    printToDebug("Power-saving activated.\r\n");
  }
  #ifndef WDT_TIMEOUT

    #define WDT_TIMEOUT 120
  #endif
  esp_task_wdt_init(WDT_TIMEOUT, true);
  esp_task_wdt_add(NULL);
#endif

#ifdef ONE_WIRE_BUS
  if (One_Wire_Pin) {
    printToDebug(PSTR("Init One Wire bus...\r\n"));

    oneWire = new OneWire(One_Wire_Pin);

    sensors = new DallasTemperature(oneWire);

    sensors->begin();
    numSensors=sensors->getDeviceCount();
    printFmtToDebug(PSTR("numSensors: %d\r\n"), numSensors);
  }
#endif

#ifdef BME280
  if(BME_Sensors) {
    printToDebug(PSTR("Init BMx280 sensor(s)...\r\n"));
    if(BME_Sensors > 16) BME_Sensors = 16;
    bme = new BlueDot_BME280[BME_Sensors];
    for (uint8_t f = 0; f < BME_Sensors; f++) {
      bme[f].parameter.communication = 0;

      if(BME_Sensors > 2){


        bme[f].parameter.I2CAddress = 0x76 + f / 8;
        tcaselect(f & 0x07);
      } else {
        bme[f].parameter.I2CAddress = 0x76 + f;
      }
      bme[f].parameter.sensorMode = 0b11;
      bme[f].parameter.IIRfilter = 0b100;
      bme[f].parameter.humidOversampling = 0b101;
      bme[f].parameter.tempOversampling = 0b101;
      bme[f].parameter.pressOversampling = 0b101;
      bme[f].parameter.pressureSeaLevel = 1013.25;
      bme[f].parameter.tempOutsideCelsius = 15;
      bme[f].parameter.tempOutsideFahrenheit = 59;
      bool sensor_found = true;
      switch(bme[f].init()){
        case 0x58: printToDebug(PSTR("BMP280")); break;
        case 0x60: printToDebug(PSTR("BME280")); break;
        default: printToDebug(PSTR("Sensor")); sensor_found = false; break;
      }
      printFmtToDebug(PSTR(" with address 0x%x "), bme[f].parameter.I2CAddress);
      if (!sensor_found) {
        printToDebug(PSTR("NOT "));
      }
      printToDebug(PSTR("found\r\n"));
    }
  }
#endif

  printToDebug(PSTR("PPS settings:\r\n"));
  uint32_t temp_c = 0;
  int temp_idx = findLine(15000,0,&temp_c);
  for (int i=0; i<PPS_ANZ; i++) {
    int l = findLine(15000+i,temp_idx,&temp_c);
    if (l==-1) continue;

    uint8_t flags=get_cmdtbl_flags(l);
    if ((flags & FL_EEPROM) == FL_EEPROM) {
      allow_write_pps_values[i / 8] |= (1 << (i % 8));
    }

      if (pps_values[i] == (int16_t)0xFFFF) {
        pps_values[i] = 0;
      }
      if (pps_values[i] > 0 && pps_values[i]< (int16_t)0xFFFF) {
        printFmtToDebug(PSTR("Slot %d, value: %u\r\n"), i, pps_values[i]);
      }

  }
  if (pps_values[PPS_QTP] == 0 || UseEEPROM != 0x96) {
    pps_values[PPS_QTP] = QAA_TYPE;
    writeToEEPROM(CF_ROOM_DEVICE);
  }
  if (pps_values[PPS_RTI] != 0) {
    pps_values[PPS_RTI] = 0;
  }

#if defined LOGGER || defined WEBSERVER
  printToDebug(PSTR("Starting SD.."));
  #ifndef ESP32

  pinMode(10,OUTPUT);
  digitalWrite(10,HIGH);
  if (!SD.begin(4, SPI_DIV3_SPEED)) {
    printToDebug(PSTR("failed\r\n"));
  } else {
    printToDebug(PSTR("ok\r\n"));
  }
  #else
    #if defined(ESP32_USE_SD)




    if (!SD.begin(4)) {

      printToDebug(PSTR("failed\r\n"));
    } else {
      printToDebug(PSTR("ok\r\n"));
    }

    #else
    SD.begin(true);
    #endif
  #endif
#else
  #ifndef ESP32

  pinMode(10,OUTPUT);
  digitalWrite(10,LOW);


  pinMode(4,OUTPUT);
  digitalWrite(4,HIGH);
  #endif
#endif

#ifdef WIFI
  #ifndef ESP32
  WiFi.init(WIFI_SPI_SS_PIN);


  if (WiFi.status() == WL_NO_SHIELD) {
    printToDebug(PSTR("WiFi shield not present. Cannot continue.\r\n"));

    while (true);
  }

  if (!WiFi.checkProtocolVersion()) {
    printToDebug(PSTR("Protocol version mismatch. Please upgrade the WiFiSpiESP firmware of the ESP.\r\n"));

    while (true);
  }

  #endif
#endif


  if (!useDHCP && ip_addr[0]) {
    IPAddress ip(ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3]);
    IPAddress subnet;
    IPAddress gateway;
    IPAddress dnsserver;
    if (subnet_addr[0]) {
      subnet = IPAddress(subnet_addr[0], subnet_addr[1], subnet_addr[2], subnet_addr[3]);
    } else {
      subnet = IPAddress(255, 255, 255, 0);
    }
    if (gateway_addr[0]) {
      gateway = IPAddress(gateway_addr[0], gateway_addr[1], gateway_addr[2], gateway_addr[3]);
    } else {
      gateway = IPAddress(ip_addr[0], ip_addr[1], ip_addr[2], 1);
    }
    if (dns_addr[0]) {
      dnsserver = IPAddress(dns_addr[0], dns_addr[1], dns_addr[2], dns_addr[3]);
    } else {
      dnsserver = IPAddress(ip_addr[0], ip_addr[1], ip_addr[2], 1);
    }
#ifndef WIFI
    Ethernet.begin(mac, ip, dnsserver, gateway, subnet);
  } else {
    Ethernet.begin(mac);
    printToDebug(PSTR("Waiting for DHCP address"));
    unsigned long timeout = millis();
    while (!Ethernet.localIP() && millis() - timeout < 20000) {
      printToDebug(PSTR("."));
      delay(100);
    }
    writelnToDebug();
  }
  SerialOutput->println(Ethernet.localIP());
  SerialOutput->println(Ethernet.subnetMask());
  SerialOutput->println(Ethernet.gatewayIP());
#else
  #if !defined(ESP32)
    WiFi.config(ip, dnsserver, gateway, subnet);
  #else
    WiFi.config(ip, gateway, subnet, dnsserver);
  #endif
  }

  unsigned long timeout;
  #ifdef ESP32

  esp_wifi_disconnect();
  WiFi.mode(WIFI_STA);
  esp_wifi_set_bandwidth(WIFI_IF_STA, WIFI_BW_HT20);

  printToDebug(PSTR("Setting up WiFi interface"));
  WiFi.begin();
  timeout = millis();
  while (WiFi.status() == WL_DISCONNECTED && millis() - timeout < 5000) {
    delay(100);
    printToDebug(PSTR("."));
  }
  writelnToDebug();
  #endif

#if defined(ESP32)
  WiFi.setHostname(mDNS_hostname);
  scanAndConnectToStrongestNetwork();
  #endif

  printFmtToDebug(PSTR("Attempting to connect to WPA SSID: %s"), wifi_ssid);
  timeout = millis();
  delay(1000);
  while (WiFi.status() != WL_CONNECTED && millis() - timeout < 10000) {
    printToDebug(PSTR("."));
    delay(1000);
  }
  if (WiFi.status() != WL_CONNECTED) {
    printlnToDebug(PSTR("Connecting to WiFi network failed."));
  #if defined(ESP32)
    esp_wifi_disconnect();
    printlnToDebug(PSTR(" Setting up AP 'BSB-LAN'"));
    WiFi.softAP("BSB-LAN", "BSB-LPB-PPS-LAN");
    IPAddress t = WiFi.softAPIP();
    localAP = true;
    esp_wifi_set_bandwidth(WIFI_IF_AP, WIFI_BW_HT20);

    printFmtToDebug(PSTR("IP address of BSB-LAN: %d.%d.%d.%d\r\n"), t[0], t[1], t[2], t[3]);
    printlnToDebug(PSTR("Connect to access point 'BSB-LAN' with password 'BSB-LPB-PPS-LAN' and open the IP address."));
  #endif
  } else {

    printToDebug(PSTR("\r\nYou're connected to the network:\r\n"));
  #if defined(__arm__) || defined(ESP32)
    WiFi.macAddress(mac);
  #endif
  #if defined(ESP32)
    printWifiStatus();
  #endif
  }
#endif

  server = new ComServer(HTTPPort);
  if (save_debug_mode == 2) telnetServer = new ComServer(23);

#if defined LOGGER || defined WEBSERVER
  #ifndef ESP32
  digitalWrite(10,HIGH);
  #endif
#endif

  printToDebug(PSTR("Waiting 3 seconds to give Ethernet shield time to get ready...\r\n"));

  digitalWrite(LED_BUILTIN, HIGH);

  long diff = 3000;


  SetDateTime();

  if (bus->getBusType() != BUS_PPS) {


    SetDevId();
    if (my_dev_fam != 0) {

      internalLEDBlinking(250, 2);

    }
  } else {
    if (pps_values[PPS_QTP] == 0xEA) {
      my_dev_fam = DEV_FAM(DEV_PPS_MCBA);
      my_dev_var = DEV_VAR(DEV_PPS_MCBA);
    } else {
      my_dev_fam = DEV_FAM(DEV_PPS);
      my_dev_var = DEV_VAR(DEV_PPS);
    }
  }

#if defined LOGGER || defined WEBSERVER
  printToDebug(PSTR("Calculating free space on SD..."));
  uint32_t m = millis();
  #if !defined(ESP32)
  uint32_t freespace = SD.vol()->freeClusterCount();
  freespace = (uint32_t)(freespace*SD.vol()->sectorsPerCluster()/2048);
  printFmtToDebug(PSTR("%d MB free\r\n"), freespace);
  #else
  uint64_t freespace = SD.totalBytes() - SD.usedBytes();
  printFmtToDebug(PSTR("%llu Bytes free\r\n"), freespace);
  #endif
  diff -= (millis() - m);
#endif
  if (diff > 0) delay(diff);
  digitalWrite(LED_BUILTIN, LOW);

  printlnToDebug(PSTR("Start network services"));
  server->begin();

  if (save_debug_mode == 2)
  telnetServer->begin();
# 8514 "/home/algirdas/SynologyDrive/Development/Projektai/BSB-LAN/BSB_LAN/BSB_LAN.ino"
#ifdef AVERAGES
  resetAverageCalculation();
#endif

#ifdef WATCH_SOCKETS
  unsigned long thisTime = millis();

  for (int i=0;i<MAX_SOCK_NUM;i++) {
    connectTime[i] = thisTime;
  }
#endif

#ifdef LOGGER


#ifdef AVERAGES
  if (SD.exists(averagesFileName)) {
    File avgfile = SD.open(averagesFileName, FILE_READ);
    if (avgfile) {
      char c;
      char num[15];
      uint8_t x;
      for (int i=0; i<numAverages; i++) {
        c = avgfile.read();
        x = 0;
        while (avgfile.available() && c != '\n' && x < sizeof(num)-1) {
          if (x < sizeof(num)-1) {
            num[x] = c;
          }
          x++;
          c = avgfile.read();
        }
        num[x]='\0';
        avgValues[i] = atof(num);
        c = avgfile.read();
        x = 0;
        while (avgfile.available() && c != '\n' && x < sizeof(num)-1) {
          if (x < sizeof(num)-1) {
            num[x] = c;
          }
          x++;
          c = avgfile.read();
        }
        num[x]='\0';
        avgValues_Old[i] = atof(num);
        if (isnan(avgValues_Old[i])) {
          avgValues_Old[i] = -9999;
        }

        c = avgfile.read();
        x = 0;
        while (avgfile.available() && c != '\n' && x < sizeof(num)-1) {
          if (x < sizeof(num)-1) {
            num[x] = c;
          }
          x++;
          c = avgfile.read();
        }
        num[x]='\0';
        avgValues_Current[i] = atof(num);
      }

      c = avgfile.read();
      x = 0;
      while (avgfile.available() && c != '\n' && x < sizeof(num)-1) {
        if (x < sizeof(num)-1) {
          num[x] = c;
        }
        x++;
        c = avgfile.read();
      }
      num[x]='\0';
      avgCounter = atoi(num);
    }
    avgfile.close();
  }
#endif

  if (!SD.exists(datalogFileName)) {
    createdatalogFileAndWriteHeader();
  }
  else readFirstAndPreviousDatalogDateFromFile();
#endif

#ifdef MAX_CUL
  if (enable_max_cul) {
    UpdateMaxDeviceList();
    connectToMaxCul();
  }
#endif

  printlnToDebug((char *)destinationServer);

#ifdef MDNS_SUPPORT
  if(mDNS_hostname[0]) {
#if defined(ESP32)
    MDNS.begin(mDNS_hostname);
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("BSB-LAN web service._http", "tcp", 80);
#else
#ifdef WIFI
    mdns.begin(WiFi.localIP(), mDNS_hostname);
#else
    mdns.begin(Ethernet.localIP(), mDNS_hostname);
#endif
    mdns.addServiceRecord(PSTR("BSB-LAN web service._http"), HTTPPort, MDNSServiceTCP);
#endif
    printFmtToDebug(PSTR("Starting MDNS service with hostname %s\r\n"), mDNS_hostname);
  }
#endif


#if defined(ESP32) && defined(ENABLE_ESP32_OTA)
  init_ota_update();
#endif

#ifdef CUSTOM_COMMANDS
#include "BSB_LAN_custom_setup.h"
#endif

#ifdef BUTTONS
  if (button_on_pin[0]) {
    pinMode(button_on_pin[0], INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(button_on_pin[0]), interruptHandlerTWWPush, FALLING);
  }
  if (button_on_pin[1]) {
    pinMode(button_on_pin[1], INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(button_on_pin[1]), interruptHandlerPresenceROOM1, FALLING);
  }
  if (button_on_pin[2]) {
    pinMode(button_on_pin[2], INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(button_on_pin[2]), interruptHandlerPresenceROOM2, FALLING);
  }
  if (button_on_pin[3]) {
    pinMode(button_on_pin[3], INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(button_on_pin[3]), interruptHandlerPresenceROOM3, FALLING);
  }
#endif

#if defined(BLE_SENSORS) && defined(ESP32)
  if (EnableBLE) {
    startBLEScan();
  }
#endif

#if defined LOGGER || defined WEBSERVER
  #if !defined(ESP32)
  FsDateTime::setCallback(dateTime);
  #endif
#endif
  printlnToDebug(PSTR("Setup complete"));
  debug_mode = save_debug_mode;
}