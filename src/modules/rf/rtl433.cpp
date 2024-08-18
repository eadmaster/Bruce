// https://github.com/NorthernMan54/rtl_433_ESP/blob/main/example/OOK_Receiver/OOK_Receiver.ino

/*2fux:
 * /home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/bt/common/osi/list.c:175: multiple definition of `list_remove'; .pio/build/esp32-s3-devkitc-1/libb46/librtl_433_ESP.a(list.c.o):/home/andy/github/Bruce/.pio/libdeps/esp32-s3-devkitc-1/rtl_433_ESP/src/rtl_433/list.c:47: first defined here
/home/andy/.platformio/packages/toolchain-xtensa-esp32s3/bin/../lib/gcc/xtensa-esp32s3-elf/8.4.0/../../../../xtensa-esp32s3-elf/bin/ld: /home/andy/.platformio/packages/framework-arduinoespressif32/tools/sdk/esp32s3/lib/libbt.a(list.c.obj): in function `list_clear':
/home/runner/work/esp32-arduino-lib-builder/esp32-arduino-lib-builder/esp-idf/components/bt/common/osi/list.c:235: multiple definition of `list_clear'; .pio/build/esp32-s3-devkitc-1/libb46/librtl_433_ESP.a(list.c.o):/home/andy/github/Bruce/.pio/libdeps/esp32-s3-devkitc-1/rtl_433_ESP/src/rtl_433/list.c:61: first defined here
* */

/*
 Basic rtl_433_ESP example for OOK/ASK Devices

*/

#include <ArduinoJson.h>
//#include <ArduinoLog.h>
#include <rtl_433_ESP.h>

#ifndef RF_MODULE_FREQUENCY
#  define RF_MODULE_FREQUENCY 433.92
#endif

#define JSON_MSG_BUFFER 512

char messageBuffer[JSON_MSG_BUFFER];

rtl_433_ESP rf; // use -1 to disable transmitter

int count = 0;

void rtl_433_Callback(char* message) {
  Serial.begin(115200);
  Serial.println(message);
  /*
  JsonDocument jsonBuffer2;
  //JsonObject& RFrtl_433_ESPdata = jsonBuffer2.parseObject(message);
  deserializeJson(jsonBuffer2, message);
    serializeJsonPretty(jsonBuffer2, Serial);
  //JsonObject& obj = jb.parseObject(input);
  //logJson(RFrtl_433_ESPdata);
  * */
  count++;
}


void rtl433_setup() {
  //Serial.begin(921600);

  //pinMode(CC1101_GDO0_PIN, INPUT);
  
  Serial.println(" ");
  Serial.println("****** setup ******" );
  rf.initReceiver(RF_MODULE_RECEIVER_GPIO, RF_MODULE_FREQUENCY);
  rf.setCallback(rtl_433_Callback, messageBuffer, JSON_MSG_BUFFER);
  rf.enableReceiver();
  Serial.println("****** setup complete ******" );
  //rf.getModuleStatus();
}

unsigned long uptime() {
  static unsigned long lastUptime = 0;
  static unsigned long uptimeAdd = 0;
  unsigned long uptime = millis() / 1000 + uptimeAdd;
  if (uptime < lastUptime) {
    uptime += 4294967;
    uptimeAdd += 4294967;
  }
  lastUptime = uptime;
  return uptime;
}

int next = uptime() + 30;

#if defined(setBitrate) || defined(setFreqDev) || defined(setRxBW)

#  ifdef setBitrate
#    define TEST    "setBitrate" // 17.24 was suggested
#    define STEP    2
#    define stepMin 1
#    define stepMax 300
// #    define STEP    1
// #    define stepMin 133
// #    define stepMax 138
#  elif defined(setFreqDev) // 40 kHz was suggested
#    define TEST    "setFrequencyDeviation"
#    define STEP    1
#    define stepMin 5
#    define stepMax 200
#  elif defined(setRxBW)
#    define TEST "setRxBandwidth"

#    ifdef defined(RF_SX1276) || defined(RF_SX1278)
#      define STEP    5
#      define stepMin 5
#      define stepMax 250
#    else
#      define STEP    5
#      define stepMin 58
#      define stepMax 812
// #      define STEP    0.01
// #      define stepMin 202.00
// #      define stepMax 205.00
#    endif
#  endif
float step = stepMin;
#endif

bool rtl433_loop(int max_loops) {
    while(max_loops>0) {
	max_loops -= 1;
      rf.loop();
      
    #if defined(setBitrate) || defined(setFreqDev) || defined(setRxBW)
      char stepPrint[8];
      if (uptime() > next) {
	next = uptime() + 120; // 60 seconds
	dtostrf(step, 7, 2, stepPrint);
	Serial.println( "Finished %s: %s, count: %d" ), TEST, stepPrint, count);
	step += STEP;
	if (step > stepMax) {
	  step = stepMin;
	}
	dtostrf(step, 7, 2, stepPrint);
	Serial.println("Starting %s with %s" ), TEST, stepPrint);
	count = 0;

	int16_t state = 0;
    #  ifdef setBitrate
	state = rf.setBitRate(step);
	RADIOLIB_STATE(state, TEST);
    #  elif defined(setFreqDev)
	state = rf.setFrequencyDeviation(step);
	RADIOLIB_STATE(state, TEST);
    #  elif defined(setRxBW)
	state = rf.setRxBandwidth(step);
	if ((state) != RADIOLIB_ERR_NONE) {
	  Serial.println( "Setting  %s: to %s, failed" ), TEST, stepPrint);
	  next = uptime() - 1;
	}
    #  endif

	rf.receiveDirect();
	// rf.getModuleStatus();
      }
    #endif
    }
    return true;
} 
