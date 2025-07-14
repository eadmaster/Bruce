#include "core/display.h"
#include "core/mykeyboard.h"
#include <globals.h>

#ifdef RF_CC1101

/*
#define OOK_MODULATION true
#define RF_CC1101 "CC1101"
#define RTL_DEBUG 0
#define MINRSSI -82

#define RF_MODULE_SCK CC1101_SCK_PIN
#define RF_MODULE_MISO CC1101_MISO_PIN
#define RF_MODULE_MOSI CC1101_MOSI_PIN
#define RF_MODULE_CS CC1101_SS_PIN
#define RF_MODULE_GDO0 CC1101_GDO0_PIN
#define RF_MODULE_GDO2 CC1101_GDO2_PIN
*/

#ifndef RF_MODULE_FREQUENCY
#  define RF_MODULE_FREQUENCY 433.92
#endif

#define RF_MODULE_RECEIVER_GPIO CC1101_GDO0_PIN
#  define STR_MODULE              "CC1101"

// used as "_spi = &RADIOLIB_DEFAULT_SPI;" in .pio/libdeps/lilygo-t-embed-cc1101/RadioLib/src/Module.cpp 
#define RADIOLIB_DEFAULT_SPI  tft.getSPIinstance()
/*#if (TFT_MOSI > 0)
        #define RADIOLIB_DEFAULT_SPI  tft.getSPIinstance()
#elif (CC1101_MOSI_PIN == SDCARD_MOSI)
        #define RADIOLIB_DEFAULT_SPI  sdcardSPI
#elif (CC1101_MOSI_PIN != SDCARD_MOSI && CC1101_MOSI_PIN == NRF24_MOSI_PIN)
        #define RADIOLIB_DEFAULT_SPI  CC_NRF_SPI
#endif
*/

//#define RADIOLIB_DEFAULT_SPI_SETTINGS               SPISettings(2000000, MSBFIRST, SPI_MODE0)

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)


#include <rtl_433_ESP.h>
//#include <ArduinoJson.h>

// derived from https://github.com/thesavant42/T-Embed-OOK-Receive/blob/main/src/OOK_Receiver.ino



#define JSON_MSG_BUFFER 512
char messageBuffer[JSON_MSG_BUFFER];

rtl_433_ESP rf; // use -1 to disable transmitter


void rtl_433_Callback(char* message) {
  //DynamicJsonBuffer jsonBuffer2(JSON_MSG_BUFFER);
  //JsonObject& RFrtl_433_ESPdata = jsonBuffer2.parseObject(message);
  Serial.println(message);
  padprintln(message);
  //logJson(RFrtl_433_ESPdata);
  //count++;
}


void rtl433_scan() {
    drawMainBorderWithTitle("Sensors");
    padprintln("scanning...");
    padprintln("");
    padprintln("Press Any key to STOP.");
    
    // setup
    rf.initReceiver(RF_MODULE_RECEIVER_GPIO, RF_MODULE_FREQUENCY);
    rf.setCallback(rtl_433_Callback, messageBuffer, JSON_MSG_BUFFER);
    rf.enableReceiver();
    Serial.println("*** rtl_433 setup complete ***");
    Serial.println("RADIOLIB DEFAULT SPI: " TOSTRING(RADIOLIB_DEFAULT_SPI));
    rf.getModuleStatus();

/*        
#  ifdef setBitrate
    state = rf.setBitRate(step);
    RADIOLIB_STATE(state, TEST);
#  elif defined(setFreqDev)
    state = rf.setFrequencyDeviation(step);
    RADIOLIB_STATE(state, TEST);
#  elif defined(setRxBW)
    state = rf.setRxBandwidth(step);
    if ((state) != RADIOLIB_ERR_NONE) {
      Log.notice(F(CR "Setting  %s: to %s, failed" CR), TEST, stepPrint);
      next = uptime() - 1;
    }
#  endif
    */
      
    while (!check(AnyKeyPress)) {
        //max_loops -= 1;
        //if (max_loops <= 0) break; 
        
        rf.loop();
    }
}

#endif