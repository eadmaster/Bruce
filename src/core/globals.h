// Globals.h

//#define FGCOLOR TFT_PURPLE+0x3000
extern char16_t FGCOLOR;
#define ALCOLOR TFT_RED
#define BGCOLOR TFT_BLACK

#include <Arduino.h>
#include <functional>
#include <vector>
//#include <SPIFFS.h>
#include <LittleFS.h>
#include <NTPClient.h>
#include <Timezone.h>
#include <ESP32Time.h>
#include <EEPROM.h>
#include <ArduinoJson.h>

#if defined (STICK_C_PLUS) || defined (STICK_C)
  #include <AXP192.h>
  extern AXP192 axp192;
#endif

#if defined(STICK_C_PLUS) || defined(STICK_C_PLUS2)
  #include "../lib/RTC/cplus_RTC.h"
#endif

#if defined(CARDPUTER)
  #include <Keyboard.h>
  extern Keyboard_Class Keyboard;
#endif

// Declaração dos objetos TFT
#if defined(HAS_SCREEN)
  #include <TFT_eSPI.h>
  extern TFT_eSPI tft;
  extern TFT_eSprite sprite;
  extern TFT_eSprite draw;
#else
    #include "VectorDisplay.h"
    extern SerialDisplayClass tft;
    extern SerialDisplayClass& sprite;
    extern SerialDisplayClass& draw;
#endif

extern char timeStr[10];

extern bool clock_set;
extern time_t localTime;
extern struct tm* timeInfo;
extern ESP32Time rtc;
extern NTPClient timeClient;
extern Timezone myTZ;

extern int prog_handler;    // 0 - Flash, 1 - LittleFS, 2 - Download

extern bool sdcardMounted;  // inform if SD Cardis active or not

extern bool wifiConnected;  // inform if wifi is active or not

extern bool BLEConnected;  // inform if BLE is active or not

extern bool gpsConnected; // inform if GPS is active or not

extern std::vector<std::pair<std::string, std::function<void()>>> options;

extern  String ssid;

extern  String pwd;

extern String fileToCopy;

extern int rotation;

extern uint8_t buff[4096];

extern const int bufSize;

extern bool returnToMenu; // variable to check and break loops to return to main menu

extern int IrTx;

extern int IrRx;

extern int RfTx;

extern int RfRx;

// Screen sleep control variables
extern unsigned long previousMillis;
extern bool isSleeping;
extern bool isScreenOff;
extern bool dimmer;
extern int dimmerSet;

void readFGCOLORFromEEPROM();

void backToMenu();

void updateTimeStr(struct tm timeInfo);

extern JsonDocument settings;
extern unsigned long dimmerTemp;
extern int dimmerSet;
extern int bright;
extern bool dimmer;
extern  String wui_usr;
extern  String wui_pwd;
extern int tmz;

void _tone(unsigned int frequency, unsigned long duration);
