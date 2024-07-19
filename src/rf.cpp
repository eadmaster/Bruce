//@IncursioHack - github.com/IncursioHack

#include "rf.h"
#include "globals.h"
#include "mykeyboard.h"
#include "display.h"
#include "PCA9554.h"
#include "sd_functions.h"
#include <driver/rmt.h>

// Cria um objeto PCA9554 com o endereço I2C do PCA9554PW
// PCA9554 extIo1(pca9554pw_address);

#define RMT_RX_CHANNEL  RMT_CHANNEL_6
#define RMT_BLOCK_NUM


#define RMT_CLK_DIV   80 /*!< RMT counter clock divider */
#define RMT_1US_TICKS (80000000 / RMT_CLK_DIV / 1000000)
#define RMT_1MS_TICKS (RMT_1US_TICKS * 1000)

#define SIGNAL_STRENGTH_THRESHOLD 1500 // Adjust this threshold as needed

#define DISPLAY_HEIGHT 130 // Height of the display area for the waveform
#define DISPLAY_WIDTH  240 // Width of the display area
#define LINE_WIDTH 2 // Adjust line width as needed

void initRMT() {
    rmt_config_t rxconfig;
    rxconfig.rmt_mode            = RMT_MODE_RX;
    rxconfig.channel             = RMT_RX_CHANNEL;
    rxconfig.gpio_num            = gpio_num_t(RfRx);
    //rxconfig.mem_block_num       = RMT_BLOCK_NUM;
    rxconfig.clk_div             = RMT_CLK_DIV;
    rxconfig.rx_config.filter_en = true;
    rxconfig.rx_config.filter_ticks_thresh = 200 * RMT_1US_TICKS;
    rxconfig.rx_config.idle_threshold = 3 * RMT_1MS_TICKS;

    ESP_ERROR_CHECK(rmt_config(&rxconfig));
    ESP_ERROR_CHECK(rmt_driver_install(rxconfig.channel, 2048, 0));
}

bool sendRF = false;

void rf_spectrum() { //@IncursioHack - https://github.com/IncursioHack ----thanks @aat440hz - RF433ANY-M5Cardputer

    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(1);
    tft.println("");
    tft.println("  RF433 - Spectrum");
    pinMode(RfRx, INPUT);
    initRMT();

        RingbufHandle_t rb = nullptr;
        rmt_get_ringbuf_handle(RMT_RX_CHANNEL, &rb);
        rmt_rx_start(RMT_RX_CHANNEL, true);
        while (rb) {
            size_t rx_size = 0;
            rmt_item32_t* item = (rmt_item32_t*)xRingbufferReceive(rb, &rx_size, 500);
            if (item != nullptr) {
                if (rx_size != 0) {
                    // Clear the display area
                    tft.fillRect(0, 20, DISPLAY_WIDTH, DISPLAY_HEIGHT, TFT_BLACK);
                    // Draw waveform based on signal strength
                    for (size_t i = 0; i < rx_size; i++) {
                        int lineHeight = map(item[i].duration0 + item[i].duration1, 0, SIGNAL_STRENGTH_THRESHOLD, 0, DISPLAY_HEIGHT/2);
                        int lineX = map(i, 0, rx_size - 1, 0, DISPLAY_WIDTH - 1); // Map i to within the display width
                        // Ensure drawing coordinates stay within the box bounds
                        int startY = constrain(20 + DISPLAY_HEIGHT / 2 - lineHeight / 2, 20, 20 + DISPLAY_HEIGHT);
                        int endY = constrain(20 + DISPLAY_HEIGHT / 2 + lineHeight / 2, 20, 20 + DISPLAY_HEIGHT);
                        tft.drawLine(lineX, startY, lineX, endY, TFT_PURPLE);
                    }
                }
                vRingbufferReturnItem(rb, (void*)item);
            }
                    if (checkEscPress()) {
                    rmt_rx_stop(RMT_RX_CHANNEL);
                    returnToMenu=true;
                    break;
                }
        }
            // Checks para sair do while

    rmt_rx_stop(RMT_RX_CHANNEL);
    delay(10);


}


void rf_jammerFull() { //@IncursioHack - https://github.com/IncursioHack -  thanks @EversonPereira - rfcardputer
    pinMode(RfTx, OUTPUT);
    tft.fillScreen(TFT_BLACK);
    tft.println("");
    tft.println("  RF433 - Jammer Full");
    tft.println("");
    tft.println("");
    tft.setTextSize(2);
    sendRF = true;
    digitalWrite(RfTx, HIGH); // Turn on Jammer
    int tmr0=millis();             // control total jammer time;
    tft.println("Sending... Press ESC to stop.");
    while (sendRF) {
        if (checkEscPress() || (millis() - tmr0 >20000)) {
            sendRF = false;
            returnToMenu=true;
            break;
        }
    }
    digitalWrite(RfTx, LOW); // Turn Jammer OFF
}


void rf_jammerIntermittent() { //@IncursioHack - https://github.com/IncursioHack -  thanks @EversonPereira - rfcardputer
    pinMode(RfTx, OUTPUT);
    tft.fillScreen(TFT_BLACK);
    tft.println("");
    tft.println("  RF433 - Jammer Intermittent");
    tft.println("");
    tft.println("");
    tft.setTextSize(2);
    sendRF = true;
    tft.println("Sending... Press ESC to stop.");
    int tmr0 = millis();
    while (sendRF) {
        for (int sequence = 1; sequence < 50; sequence++) {
            for (int duration = 1; duration <= 3; duration++) {
                // Moved Escape check into this loop to check every cycle
                if (checkEscPress() || (millis()-tmr0)>20000) {
                    sendRF = false;
                    returnToMenu=true;
                    break;
                }
                digitalWrite(RfTx, HIGH); // Ativa o pino
                // Mantém o pino ativo por um período que aumenta com cada sequência
                for (int widthsize = 1; widthsize <= (1 + sequence); widthsize++) {
                    delayMicroseconds(50);
                }

                digitalWrite(RfTx, LOW); // Desativa o pino
                // Mantém o pino inativo pelo mesmo período
                for (int widthsize = 1; widthsize <= (1 + sequence); widthsize++) {
                    delayMicroseconds(50);
                }
            }
        }
    }

    digitalWrite(RfTx, LOW); // Desativa o pino
}


struct RfCodes {
  uint32_t frequency = 0;
  String protocol = "";
  String preset = "";
  String data = "";
  String filepath = "";
};


#include <RCSwitch.h>

// ported from https://github.com/sui77/rc-switch/blob/3a536a172ab752f3c7a58d831c5075ca24fd920b/RCSwitch.cpp
void RCSwitch_RAW_send(int nTransmitterPin, int * ptrtransmittimings, int nRepeatTransmit=1) {
  if (nTransmitterPin == -1)
    return;

  if (!ptrtransmittimings)
    return;

  bool currentlogiclevel = true;
  
  for (int nRepeat = 0; nRepeat < nRepeatTransmit; nRepeat++) {
    unsigned int currenttiming = 0;
    while( ptrtransmittimings[currenttiming] && currenttiming < RCSWITCH_MAX_CHANGES ) {
        if(ptrtransmittimings[currenttiming] >= 0) {
            currentlogiclevel = true;
        } else {
            // negative value
            currentlogiclevel = false;
            ptrtransmittimings[currenttiming] = (-1) * ptrtransmittimings[currenttiming];  // invert sign
        }
      digitalWrite(nTransmitterPin, currentlogiclevel ? HIGH : LOW);
      delayMicroseconds( ptrtransmittimings[currenttiming] );
      currenttiming++;
    }
  digitalWrite(nTransmitterPin, LOW);
  }  // end for
}


void sendRawRfCommand(struct RfCodes rfcode) { 
      uint32_t frequency = rfcode.frequency;
      String protocol = rfcode.protocol;
      String preset = rfcode.preset;
      String data = rfcode.data;
/*
    Serial.println("sendRawRfCommand");
    Serial.println(data);
    Serial.println(frequency);
    Serial.println(preset);
    Serial.println(protocol);
  */  
    if(frequency != 433920000) {
        Serial.print("unsupported frequency: ");
        Serial.println(frequency);
        return;
    }
    // MEMO: frequency is fixed with some transmitters https://github.com/sui77/rc-switch/issues/256
    // TODO: add frequency switching via CC1101  https://github.com/LSatan/SmartRC-CC1101-Driver-Lib
    
    if(! preset.startsWith("FuriHalSubGhzPresetOok")) {
        // only ASK/OOK is supported
        Serial.print("unsupported preset: ");
        Serial.println(preset);
        return;
    }
    /*  supported flipper presets:
        FuriHalSubGhzPresetIDLE, // < default configuration 
        FuriHalSubGhzPresetOok270Async, ///< OOK, bandwidth 270kHz, asynchronous 
        FuriHalSubGhzPresetOok650Async, ///< OOK, bandwidth 650kHz, asynchronous 
        FuriHalSubGhzPreset2FSKDev238Async, //< FM, deviation 2.380371 kHz, asynchronous 
        FuriHalSubGhzPreset2FSKDev476Async, //< FM, deviation 47.60742 kHz, asynchronous 
        FuriHalSubGhzPresetMSK99_97KbAsync, //< MSK, deviation 47.60742 kHz, 99.97Kb/s, asynchronous 
        FuriHalSubGhzPresetGFSK9_99KbAsync, //< GFSK, deviation 19.042969 kHz, 9.996Kb/s, asynchronous 
        FuriHalSubGhzPresetCustom, //Custom Preset
    */
    
    if(protocol != "RAW") {
        Serial.print("unsupported protocol: ");
        Serial.println(protocol);
        return;
    }
    
    digitalWrite(RfTx, LED_OFF);
    
    if(RfTx==0) RfTx=GROVE_SDA; // quick fix
    pinMode(RfTx, OUTPUT);
    //Serial.println(RfTx);
    
    //RCSwitch mySwitch = RCSwitch();
    //mySwitch.enableTransmit(RfTx);
    
    //mySwitch.setProtocol(1);
    //mySwitch.setPulseLength(pulse);
    //mySwitch.setRepeatTransmit(10);
    
    // alloc buffer for transmittimings
    int* transmittimings  = (int *) calloc(sizeof(int), data.length());  // should be smaller the data.length() 
    size_t transmittimings_idx = 0;
    
    // split data into words, convert to int, and store them in transmittimings
    String curr_word = "";
    int curr_val = 0;
    for(int i=0; i<data.length(); i++) {
        if(isspace(data[i])) {
            if(curr_word == "") continue;  // skip if empty
            // else append to transmittimings
            //transmittimings[transmittimings_idx] = curr_word.toInt();  // does not handle negative numbers
            transmittimings[transmittimings_idx] = atoi(curr_word.c_str());
            /*
            Serial.println(curr_word);
            Serial.println(transmittimings[transmittimings_idx]);
            */
            transmittimings_idx += 1;
            //if(transmittimings[transmittimings_idx]==0)  invalid int?
            curr_word = "";  // reset
            
        } else {
            curr_word += data[i];  // append to current word
        }
    }
    transmittimings[transmittimings_idx] = 0;  // termination
      
    // send rf command
    displayRedStripe("Sending..",TFT_WHITE,FGCOLOR);
    //mySwitch.send(transmittimings);
    RCSwitch_RAW_send(RfTx, transmittimings);
    free(transmittimings);
    
    Serial.println("finished sending");
    
    digitalWrite(RfTx, LED_OFF);
}


RfCodes recent_rfcodes[16];  // TODO: save/load in EEPROM
int recent_rfcodes_last_used = 0;  // TODO: save/load in EEPROM

void addToRecentCodes(struct RfCodes rfcode)  {
    // copy rfcode -> recent_rfcodes[recent_rfcodes_last_used]
    recent_rfcodes[recent_rfcodes_last_used] = rfcode;
    recent_rfcodes_last_used += 1;
    if(recent_rfcodes_last_used == 16) recent_rfcodes_last_used  = 0; // cycle
}

struct RfCodes selectRecentMenu() {
    // show menu with filenames
    options = { };
    bool exit = false;
    struct RfCodes selected_code;
    for(int i=0; i<16; i++) {
        if(recent_rfcodes[i].filepath=="") continue; // not inited
        // else
        options.push_back({ recent_rfcodes[i].filepath.c_str(), [&](){ selected_code = recent_rfcodes[i]; }});
    }
    options.push_back({ "Main Menu" , [&](){ exit=true; }});    
    delay(200);
    loopOptions(options);
    return(selected_code);
}
    
void otherRFcodes() {
  File databaseFile;
  FS *fs = NULL;
  String filepath = "";
  struct RfCodes selected_code ;
  bool recent_menu_selected = false;
    options = {
      {"Recent", [&]()  { selected_code = selectRecentMenu(); recent_menu_selected = true;}},
      {"LittleFS", [&]()   { fs=&LittleFS; }},
    };
  if(setupSdCard()) options.push_back({"SD Card", [&]()  { fs=&SD; }});    

  delay(200);
  loopOptions(options);
  delay(200);
  
  if(recent_menu_selected == true) {
    if(selected_code.filepath!="") sendRawRfCommand(selected_code);  // a code was selected
    return;
    // no need to proceed, go back
  }
  
  filepath = loopSD(*fs, true);
  databaseFile = fs->open(filepath, FILE_READ);
  drawMainBorder();
  
  selected_code.filepath = filepath.substring( 1 + filepath.lastIndexOf("/") );

  if (!databaseFile) {
    Serial.println("Failed to open database file.");
    displayError("Fail to open file");
    delay(2000);
    return;
  }
  Serial.println("Opened sub file.");
  
  // format specs: https://github.com/flipperdevices/flipperzero-firmware/blob/dev/documentation/file_formats/SubGhzFileFormats.md
  String line;
  String txt;
  while (databaseFile.available() ) {
      line = databaseFile.readStringUntil('\n');
      txt=line.substring(line.indexOf(":") + 1);
      txt.trim();
      if(line.startsWith("Protocol:"))  selected_code.protocol = txt;
      if(line.startsWith("Preset:"))   selected_code.preset = txt;
      if(line.startsWith("Frequency:")) selected_code.frequency = txt.toInt();
      if(line.startsWith("RAW_Data:")) { selected_code.data += txt; }
  }
  databaseFile.close();
    
  addToRecentCodes(selected_code);
  sendRawRfCommand(selected_code);
    
  // TODO: menu to resend command/pick another file from the same dir?

  digitalWrite(RfTx, LED_OFF);
}

