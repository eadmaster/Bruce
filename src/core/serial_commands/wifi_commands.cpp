#include "wifi_commands.h"
#include "core/wifi/webInterface.h"
#include "core/wifi/wifi_common.h" //to return MAC addr
#include <globals.h>
#include <WiFi.h>


uint32_t wifiCallback(cmd *c) {
    Command cmd(c);
    Argument statusArg = cmd.getArgument("status");
    String status = statusArg.getValue();
    status.trim();

    if (status == "off") {
        wifiDisconnect();
        return true;
    }
    else if (status == "status") {
         if (wifiConnected) {
            Serial.println("Wifi already connected");
            int curr_pow = WiFi.getTxPower();
            Serial.println("current Tx power (RAW value): " + String(curr_pow)); 
            Serial.println("current RSSI: " + String(WiFi.RSSI()) + " dBm"); 
        } else {
            Serial.println("Wifi not connected");
        }
        return true;
    }
    // else if (status == "on") {
    //     if (wifiConnected) {
    //         Serial.println("Wifi already connected");
    //         return true;
    //     }
    //     connectToWifi();
    //     return true;
    // }
    else if (status == "power") {
        String argStr = cmd.getArg("arg").getValue();
        int argValue = argStr.toInt();
        
        if (argStr == "max") {
            Serial.println("Setting wifi tx power to MAX (21dBm)");
            // WIFI_POWER_21dBm = 84,      // 21dBm
            WiFi.setTxPower((wifi_power_t)84);
            return true;
        }
        /*
        if (argStr == "max2") {
            Serial.println("Setting wifi tx power to MAX (21dBm)");
            esp_wifi_set_max_tx_power();
            return true;
        }*/
        if (argStr == "long") {
            WiFi.enableLongRange(true);
            return true;
        }
        else if (argValue != 0 && argValue >= -4 && argValue <= 84) {   // allowed values https://github.com/espressif/arduino-esp32/blob/13cd0d3c3fb6d6f719c55cdb671d6807874427b5/libraries/WiFi/src/WiFiGeneric.h#L67
            Serial.println("Setting wifi tx power to " + argStr);
            WiFi.setTxPower((wifi_power_t)argValue);
            return true;
        }
        // else
        return false;
    }
    else {
        Serial.println("Invalid status: " + status);
        return false;
    }
}

uint32_t webuiCallback(cmd *c) {
    Command cmd(c);

    Argument arg = cmd.getArgument("noAp");
    bool noAp = arg.isSet();

    Serial.println("Starting Web UI " + !noAp ? "AP" : "STA");
    Serial.println("Press ESC to quit");
    startWebUi(!noAp); // MEMO: will quit when check(EscPress)

    return true;
}

void createWifiCommands(SimpleCLI *cli) {
    Command webuiCmd = cli->addCommand("webui", webuiCallback);
    webuiCmd.addFlagArg("noAp");

    Command wifiCmd = cli->addCommand("wifi", wifiCallback);
    wifiCmd.addPosArg("status");
    wifiCmd.addPosArg("arg");
}
