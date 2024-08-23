#include "core/globals.h"
#include "core/sd_functions.h" // using sd functions called to rename and manage sd files
#include "core/wifi_common.h"  // using common wifisetup
#include "core/mykeyboard.h"   // using keyboard when calling rename
#include "core/display.h"      // using displayRedStripe as error msg
#include "core/serialcmds.h"
#include "webInterface.h"

#include <WebServer.h>
//#include <ESPWebServerSecure.hpp>

WebServer* server=nullptr;               // initialise webserver
//ESPWebServerSecure* server=nullptr;

unsigned char example_der[] = {
  0x30, 0x82, 0x02, 0x18, 0x30, 0x82, 0x01, 0x81, 0x02, 0x01, 0x02, 0x30,
  0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b,
  0x05, 0x00, 0x30, 0x54, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04,
  0x06, 0x13, 0x02, 0x44, 0x45, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55,
  0x04, 0x08, 0x0c, 0x02, 0x42, 0x45, 0x31, 0x0f, 0x30, 0x0d, 0x06, 0x03,
  0x55, 0x04, 0x07, 0x0c, 0x06, 0x42, 0x65, 0x72, 0x6c, 0x69, 0x6e, 0x31,
  0x12, 0x30, 0x10, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x09, 0x4d, 0x79,
  0x43, 0x6f, 0x6d, 0x70, 0x61, 0x6e, 0x79, 0x31, 0x13, 0x30, 0x11, 0x06,
  0x03, 0x55, 0x04, 0x03, 0x0c, 0x0a, 0x6d, 0x79, 0x63, 0x61, 0x2e, 0x6c,
  0x6f, 0x63, 0x61, 0x6c, 0x30, 0x1e, 0x17, 0x0d, 0x32, 0x30, 0x30, 0x36,
  0x30, 0x36, 0x32, 0x31, 0x34, 0x33, 0x35, 0x33, 0x5a, 0x17, 0x0d, 0x33,
  0x30, 0x30, 0x36, 0x30, 0x34, 0x32, 0x31, 0x34, 0x33, 0x35, 0x33, 0x5a,
  0x30, 0x55, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13,
  0x02, 0x44, 0x45, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x08,
  0x0c, 0x02, 0x42, 0x45, 0x31, 0x0f, 0x30, 0x0d, 0x06, 0x03, 0x55, 0x04,
  0x07, 0x0c, 0x06, 0x42, 0x65, 0x72, 0x6c, 0x69, 0x6e, 0x31, 0x12, 0x30,
  0x10, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x09, 0x4d, 0x79, 0x43, 0x6f,
  0x6d, 0x70, 0x61, 0x6e, 0x79, 0x31, 0x14, 0x30, 0x12, 0x06, 0x03, 0x55,
  0x04, 0x03, 0x0c, 0x0b, 0x65, 0x73, 0x70, 0x33, 0x32, 0x2e, 0x6c, 0x6f,
  0x63, 0x61, 0x6c, 0x30, 0x81, 0x9f, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86,
  0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x81, 0x8d,
  0x00, 0x30, 0x81, 0x89, 0x02, 0x81, 0x81, 0x00, 0xce, 0xe6, 0x25, 0x6f,
  0x3c, 0xfb, 0xda, 0x38, 0x2b, 0xd7, 0x62, 0xcc, 0x04, 0x9c, 0x58, 0xc8,
  0xc8, 0x91, 0x14, 0x93, 0xb4, 0x0e, 0xa4, 0x09, 0x78, 0xd8, 0xcb, 0xfe,
  0x13, 0x30, 0x6d, 0xd8, 0x22, 0x21, 0xf6, 0xdf, 0x7e, 0xc3, 0xc6, 0x92,
  0x2f, 0xc6, 0x84, 0x43, 0xff, 0xb4, 0xa1, 0x6b, 0x13, 0x5f, 0x36, 0xa7,
  0xc3, 0x99, 0x32, 0x03, 0x89, 0x50, 0x30, 0x1e, 0xad, 0xc6, 0x36, 0xe7,
  0x73, 0x48, 0x09, 0x28, 0x4f, 0xb9, 0x46, 0xbf, 0xdd, 0x76, 0x10, 0xc3,
  0x07, 0x33, 0x9b, 0xd0, 0x8d, 0xb2, 0x24, 0xd4, 0xcb, 0x2e, 0x90, 0x06,
  0xbf, 0xf1, 0xfa, 0xae, 0x06, 0x5c, 0xec, 0x5d, 0xe8, 0x61, 0x06, 0x4a,
  0x3a, 0x2e, 0x2b, 0x1e, 0x60, 0xf2, 0xc4, 0x09, 0xca, 0xe6, 0x27, 0x64,
  0x31, 0x9c, 0xbd, 0x2d, 0x3a, 0x56, 0x27, 0x6d, 0x23, 0x67, 0x21, 0x11,
  0x6f, 0x50, 0xca, 0x11, 0x02, 0x03, 0x01, 0x00, 0x01, 0x30, 0x0d, 0x06,
  0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b, 0x05, 0x00,
  0x03, 0x81, 0x81, 0x00, 0x82, 0x74, 0x62, 0xfb, 0xce, 0xca, 0xee, 0xfb,
  0x01, 0x16, 0x91, 0xf8, 0x5d, 0x56, 0xb5, 0x70, 0x6e, 0xc9, 0x12, 0x78,
  0x5f, 0x9f, 0xbd, 0x16, 0x13, 0xe3, 0x23, 0xa0, 0x2d, 0x52, 0x02, 0xad,
  0x08, 0xd5, 0x7b, 0xcb, 0xc3, 0x5a, 0x13, 0x4f, 0x3d, 0x1c, 0x93, 0x95,
  0x2b, 0x61, 0xf9, 0xe6, 0xa2, 0x62, 0xb7, 0x54, 0x1f, 0x06, 0x65, 0xe2,
  0x30, 0x54, 0xf6, 0x72, 0x4b, 0x87, 0xe8, 0xb7, 0x34, 0xee, 0xad, 0x12,
  0x90, 0x30, 0xf7, 0x13, 0x36, 0x4e, 0x32, 0xb1, 0x06, 0xf3, 0xfa, 0x37,
  0x8b, 0x8c, 0xb7, 0x30, 0x2a, 0x04, 0x3a, 0x47, 0xd5, 0x99, 0x67, 0x06,
  0x42, 0x40, 0x41, 0xbe, 0xbb, 0x59, 0x48, 0xcb, 0xe7, 0xef, 0x1c, 0xed,
  0x22, 0x1a, 0xe8, 0x25, 0x83, 0x7f, 0x3d, 0x40, 0x05, 0x8d, 0x5b, 0x0b,
  0x6a, 0x69, 0x2b, 0xea, 0x4b, 0xf4, 0xd4, 0x88, 0xdb, 0xd2, 0xcf, 0x7e
};
unsigned int example_der_len = 540;


unsigned char example_key_der[] = {
  0x30, 0x82, 0x02, 0x5d, 0x02, 0x01, 0x00, 0x02, 0x81, 0x81, 0x00, 0xce,
  0xe6, 0x25, 0x6f, 0x3c, 0xfb, 0xda, 0x38, 0x2b, 0xd7, 0x62, 0xcc, 0x04,
  0x9c, 0x58, 0xc8, 0xc8, 0x91, 0x14, 0x93, 0xb4, 0x0e, 0xa4, 0x09, 0x78,
  0xd8, 0xcb, 0xfe, 0x13, 0x30, 0x6d, 0xd8, 0x22, 0x21, 0xf6, 0xdf, 0x7e,
  0xc3, 0xc6, 0x92, 0x2f, 0xc6, 0x84, 0x43, 0xff, 0xb4, 0xa1, 0x6b, 0x13,
  0x5f, 0x36, 0xa7, 0xc3, 0x99, 0x32, 0x03, 0x89, 0x50, 0x30, 0x1e, 0xad,
  0xc6, 0x36, 0xe7, 0x73, 0x48, 0x09, 0x28, 0x4f, 0xb9, 0x46, 0xbf, 0xdd,
  0x76, 0x10, 0xc3, 0x07, 0x33, 0x9b, 0xd0, 0x8d, 0xb2, 0x24, 0xd4, 0xcb,
  0x2e, 0x90, 0x06, 0xbf, 0xf1, 0xfa, 0xae, 0x06, 0x5c, 0xec, 0x5d, 0xe8,
  0x61, 0x06, 0x4a, 0x3a, 0x2e, 0x2b, 0x1e, 0x60, 0xf2, 0xc4, 0x09, 0xca,
  0xe6, 0x27, 0x64, 0x31, 0x9c, 0xbd, 0x2d, 0x3a, 0x56, 0x27, 0x6d, 0x23,
  0x67, 0x21, 0x11, 0x6f, 0x50, 0xca, 0x11, 0x02, 0x03, 0x01, 0x00, 0x01,
  0x02, 0x81, 0x80, 0x7b, 0x28, 0x2e, 0x12, 0x58, 0x27, 0xc6, 0xce, 0xf6,
  0xf1, 0xe0, 0x02, 0x77, 0xa0, 0x25, 0x8d, 0x67, 0x2e, 0x4d, 0x24, 0x5a,
  0xe2, 0xf8, 0x2c, 0x17, 0x3e, 0x5d, 0xb7, 0x60, 0xee, 0xcc, 0x04, 0x02,
  0xd5, 0x5a, 0xe1, 0xd0, 0xd0, 0x72, 0xcc, 0x24, 0x1a, 0x34, 0x33, 0x51,
  0xeb, 0xd0, 0xc6, 0x2f, 0x22, 0xd7, 0x22, 0xe7, 0xe0, 0xb2, 0x0f, 0xbe,
  0xd5, 0xf7, 0xbe, 0xdb, 0x4c, 0x08, 0xf3, 0x8b, 0xb2, 0x04, 0x7e, 0x45,
  0x2d, 0x7e, 0xff, 0x98, 0xc2, 0x4f, 0xce, 0xa4, 0x98, 0x06, 0x08, 0x36,
  0x2e, 0x6c, 0xd3, 0xc6, 0x1c, 0x29, 0x26, 0x96, 0xcd, 0xeb, 0x40, 0xa5,
  0xf5, 0xf1, 0x1c, 0xd6, 0x21, 0xbd, 0x1b, 0x2b, 0xba, 0x0f, 0xba, 0x69,
  0xf4, 0xb9, 0x39, 0x78, 0xbc, 0xfe, 0x95, 0x3a, 0xb9, 0xbf, 0x85, 0x9e,
  0x86, 0xfb, 0x39, 0x5c, 0xd1, 0xf0, 0x37, 0xbc, 0x40, 0xfc, 0x51, 0x02,
  0x41, 0x00, 0xfd, 0xd7, 0x02, 0xf9, 0xc6, 0xf6, 0x24, 0x0e, 0x57, 0x9e,
  0xb1, 0xf0, 0x55, 0x9a, 0x10, 0xa8, 0x65, 0xf9, 0x55, 0x54, 0xe7, 0x99,
  0x0f, 0xd5, 0x5d, 0xe4, 0xff, 0x70, 0x68, 0xc1, 0xbe, 0x58, 0x78, 0x2c,
  0x84, 0xb7, 0xd2, 0x8a, 0xde, 0xa5, 0x6e, 0x01, 0x12, 0xc8, 0x58, 0x02,
  0x46, 0x7d, 0x43, 0xd7, 0x5b, 0x43, 0xfb, 0x97, 0x20, 0x22, 0x87, 0x71,
  0x87, 0xd3, 0x44, 0x8c, 0xfd, 0xbd, 0x02, 0x41, 0x00, 0xd0, 0xa8, 0xdf,
  0xcd, 0xc8, 0x55, 0x12, 0x80, 0xf6, 0xb8, 0x1c, 0x55, 0xa7, 0x6a, 0xd7,
  0xad, 0x7f, 0xab, 0xed, 0xc5, 0x19, 0xfa, 0x9a, 0x89, 0x11, 0x6f, 0xc9,
  0xf2, 0xa9, 0x03, 0x99, 0x0b, 0xe4, 0xda, 0x17, 0x02, 0x11, 0xb7, 0x80,
  0x3b, 0x7d, 0x30, 0xae, 0xa9, 0x8b, 0xc8, 0xc6, 0x39, 0x9c, 0x73, 0xa5,
  0xe3, 0x16, 0xe2, 0x15, 0xed, 0xf8, 0x38, 0xff, 0xce, 0x71, 0x0e, 0x10,
  0xe5, 0x02, 0x41, 0x00, 0xb1, 0xc8, 0xfe, 0xf7, 0x8c, 0x47, 0x66, 0xf7,
  0x78, 0x9c, 0xd8, 0x89, 0xb8, 0x9a, 0xc0, 0x62, 0x01, 0x92, 0x01, 0x17,
  0x07, 0x62, 0xa7, 0xb9, 0x4c, 0x1b, 0x10, 0x61, 0x5d, 0xad, 0x9c, 0xb0,
  0x7f, 0xf2, 0xc6, 0x3d, 0xad, 0x43, 0xc0, 0x2e, 0xe3, 0x7d, 0xf2, 0xf6,
  0xc8, 0xd5, 0x47, 0x23, 0x82, 0xf9, 0x79, 0x9d, 0x82, 0xbf, 0xd5, 0x2c,
  0xf9, 0xea, 0x25, 0x34, 0x6e, 0x45, 0xc5, 0x8d, 0x02, 0x40, 0x5d, 0x25,
  0x86, 0x03, 0x0f, 0x13, 0x2b, 0x17, 0x77, 0x0b, 0xe9, 0x5a, 0x33, 0x4a,
  0x76, 0xcd, 0x74, 0xd9, 0x03, 0x63, 0xa1, 0x9d, 0x45, 0xaf, 0x3a, 0xa1,
  0x74, 0xbd, 0x66, 0xc5, 0xbc, 0x64, 0x9a, 0xdc, 0xe0, 0xb8, 0x83, 0xc0,
  0x2e, 0xf6, 0x5f, 0x84, 0x83, 0xf4, 0x1b, 0xfa, 0x9c, 0xc2, 0xcb, 0x1c,
  0xb5, 0x49, 0x12, 0xc6, 0x0a, 0x94, 0x18, 0xe3, 0x19, 0x0e, 0xc7, 0x59,
  0x48, 0x21, 0x02, 0x41, 0x00, 0xaa, 0x5d, 0x55, 0xc3, 0xee, 0xf7, 0x45,
  0xbd, 0xa5, 0x00, 0x32, 0xb9, 0xa1, 0x71, 0x49, 0xd5, 0x8c, 0x32, 0xe0,
  0xc7, 0xd5, 0xf0, 0x64, 0xa9, 0xb5, 0xaf, 0x1b, 0x25, 0xdf, 0x34, 0xed,
  0xd4, 0xa6, 0xe1, 0x77, 0xfe, 0x9b, 0xc3, 0xed, 0x9b, 0x74, 0xca, 0xbf,
  0x6d, 0xa4, 0x85, 0x5a, 0x37, 0xd8, 0xf3, 0xad, 0xae, 0x91, 0x4f, 0xa1,
  0x30, 0x24, 0xef, 0x3c, 0x4f, 0x49, 0xec, 0x34, 0xa5
};
unsigned int example_key_der_len = 609;


struct Config {
  String httpuser;
  String httppassword;       // password to access web admin
  int webserverporthttp;     // http port number for web admin
};

File uploadFile;
  // WiFi as a Client
String default_httpuser = "admin";
String default_httppassword = "bruce";
const int default_webserverporthttp = 80;
//const int default_webserverporthttp = 443;

//WiFi as an Access Point
IPAddress AP_GATEWAY(172, 0, 0, 1);  // Gateway

Config config;                        // configuration

const char* host = "bruce";
const String fileconf = "/bruce.txt";  // TODO: move inside bruce.conf
String uploadFolder=""; 



/**********************************************************************
**  Function: webUIMyNet
**  Display options to launch the WebUI
**********************************************************************/
void webUIMyNet() {
  if (WiFi.status() != WL_CONNECTED) {
    if(wifiConnectMenu()) startWebUi(false);
    else {
      displayError("Wifi Offline");
    }
  } else {
    //If it is already connected, just start the network
    startWebUi(false);
  }
  // On fail installing will run the following line
}


/**********************************************************************
**  Function: loopOptionsWebUi
**  Display options to launch the WebUI
**********************************************************************/
void loopOptionsWebUi() {
  // Definição da matriz "Options"
  options = {
      {"my Network", [=]() { webUIMyNet(); }},
      {"AP mode", [=]()    { startWebUi(true); }},
  };
  delay(200);

  loopOptions(options);
  // On fail installing will run the following line
}


/**********************************************************************
**  Function: humanReadableSize
** Make size of files human readable
** source: https://github.com/CelliesProjects/minimalUploadAuthESP32
**********************************************************************/
String humanReadableSize(uint64_t bytes) {
  if (bytes < 1024) return String(bytes) + " B";
  else if (bytes < (1024 * 1024)) return String(bytes / 1024.0) + " kB";
  else if (bytes < (1024 * 1024 * 1024)) return String(bytes / 1024.0 / 1024.0) + " MB";
  else return String(bytes / 1024.0 / 1024.0 / 1024.0) + " GB";
}



/**********************************************************************
**  Function: listFiles
**  list all of the files, if ishtml=true, return html rather than simple text
**********************************************************************/
String listFiles(FS fs, bool ishtml, String folder, bool isLittleFS) {
  String returnText = "";
  Serial.println("Listing files stored on SD");

  if (ishtml) {
    returnText += "<table><tr><th align='left'>Name</th><th style=\"text-align=center;\">Size</th><th></th></tr>\n";
  }
  File root = fs.open(folder);
  File foundfile = root.openNextFile();
  String fileSys="SD";
  if (isLittleFS) fileSys="LittleFS";
  if (folder=="//") folder = "/";
  uploadFolder = folder;
  String PreFolder = folder;
  PreFolder = PreFolder.substring(0, PreFolder.lastIndexOf("/"));
  if(PreFolder=="") PreFolder= "/";
  returnText += "<tr><th align='left'><a onclick=\"listFilesButton('"+ PreFolder + "', '"+ fileSys +"')\" href='javascript:void(0);'>... </a></th><th align='left'></th><th></th></tr>\n";

  if (folder=="/") folder = "";
  while (foundfile) {
    if(foundfile.isDirectory()) {
      if (ishtml) {
        returnText += "<tr align='left'><td><a onclick=\"listFilesButton('"+ String(foundfile.path()) + "', '"+ fileSys +"')\" href='javascript:void(0);'>\n" + String(foundfile.name()) + "</a></td>";
        returnText += "<td></td>\n";
        returnText += "<td><i style=\"color: #ffabd7;\" class=\"gg-folder\" onclick=\"listFilesButton('" + String(foundfile.path()) + "')\"></i>&nbsp&nbsp";
        returnText += "<i style=\"color: #ffabd7;\" class=\"gg-rename\"  onclick=\"renameFile(\'" + String(foundfile.path()) + "\', \'" + String(foundfile.name()) + "\')\"></i>&nbsp&nbsp\n";
        returnText += "<i style=\"color: #ffabd7;\" class=\"gg-trash\"  onclick=\"downloadDeleteButton(\'" + String(foundfile.path()) + "\', \'delete\')\"></i></td></tr>\n\n";
      } else {
        returnText += "Folder: " + String(foundfile.name()) + " Size: " + humanReadableSize(foundfile.size()) + "\n";
      }
    }
    foundfile = root.openNextFile();
  }
  root.close();
  foundfile.close();

  if (folder=="") folder = "/";
  root = fs.open(folder);
  foundfile = root.openNextFile();
  while (foundfile) {
    if(!(foundfile.isDirectory())) {
      if (ishtml) {
        returnText += "<tr align='left'><td>" + String(foundfile.name());
        returnText += "</td>\n";
        returnText += "<td style=\"font-size: 10px; text-align=center;\">" + humanReadableSize(foundfile.size()) + "</td>\n";
        returnText += "<td><i class=\"gg-arrow-down-r\" onclick=\"downloadDeleteButton(\'"+ String(foundfile.path()) + "\', \'download\')\"></i>&nbsp&nbsp\n";
        //if (String(foundfile.name()).substring(String(foundfile.name()).lastIndexOf('.') + 1).equalsIgnoreCase("bin")) returnText+= "<i class=\"gg-arrow-up-r\" onclick=\"startUpdate(\'" + String(foundfile.path()) + "\')\"></i>&nbsp&nbsp\n";
        if (String(foundfile.name()).substring(String(foundfile.name()).lastIndexOf('.') + 1).equalsIgnoreCase("sub")) returnText+= "<i class=\"gg-data\" onclick=\"sendSubFile(\'" + String(foundfile.path()) + "\')\"></i>&nbsp&nbsp\n";
        if (String(foundfile.name()).substring(String(foundfile.name()).lastIndexOf('.') + 1).equalsIgnoreCase("ir")) returnText+= "<i class=\"gg-data\" onclick=\"sendIrFile(\'" + String(foundfile.path()) + "\')\"></i>&nbsp&nbsp\n";
        //if (String(foundfile.name()).substring(String(foundfile.name()).lastIndexOf('.') + 1).equalsIgnoreCase("enc")) returnText+= "<i class=\"gg-data\" onclick=\"viewDecryptedFile(\'" + String(foundfile.path()) + "\')\"></i>&nbsp&nbsp\n";
        #if defined(USB_as_HID)
          if (String(foundfile.name()).substring(String(foundfile.name()).lastIndexOf('.') + 1).equalsIgnoreCase("txt")) returnText+= "<i class=\"gg-data\" onclick=\"sendBadusbFile(\'" + String(foundfile.path()) + "\')\"></i>&nbsp&nbsp\n";
        #endif
        returnText += "<i class=\"gg-rename\"  onclick=\"renameFile(\'" + String(foundfile.path()) + "\', \'" + String(foundfile.name()) + "\')\"></i>&nbsp&nbsp\n";
        returnText += "<i class=\"gg-trash\"  onclick=\"downloadDeleteButton(\'" + String(foundfile.path()) + "\', \'delete\')\"></i></td></tr>\n\n";
      } else {
        returnText += "File: " + String(foundfile.name()) + " Size: " + humanReadableSize(foundfile.size()) + "\n";
      }
    }
    foundfile = root.openNextFile();
  }
  root.close();
  foundfile.close();

  if (ishtml) {
    returnText += "</table>";
  }

  return returnText;
}

/**********************************************************************
**  Function: processor
** parses and processes webpages if the webpage has %SOMETHING%
** or %SOMETHINGELSE% it will replace those strings with the ones defined
**********************************************************************/

String processor(const String& var) {
  String processedHtml = var;
  processedHtml.replace("%FIRMWARE%", String(BRUCE_VERSION));
  processedHtml.replace("%FREESD%", humanReadableSize(SD.totalBytes() - SD.usedBytes()));
  processedHtml.replace("%USEDSD%", humanReadableSize(SD.usedBytes()));
  processedHtml.replace("%TOTALSD%", humanReadableSize(SD.totalBytes()));

  processedHtml.replace("%FREELittleFS%", humanReadableSize(LittleFS.totalBytes() - LittleFS.usedBytes()));
  processedHtml.replace("%USEDLittleFS%", humanReadableSize(LittleFS.usedBytes()));
  processedHtml.replace("%TOTALLittleFS%", humanReadableSize(LittleFS.totalBytes()));

  return processedHtml;
}


/**********************************************************************
**  Function: checkUserWebAuth
** used by server->on functions to discern whether a user has the correct
** httpapitoken OR is authenticated by username and password
**********************************************************************/
bool checkUserWebAuth() {
  bool isAuthenticated = false;
  if (server->authenticate(config.httpuser.c_str(), config.httppassword.c_str())) {
    isAuthenticated = true;
  }
  return isAuthenticated;
}



/**********************************************************************
**  Function: handleUpload
** handles uploads to the filserver
**********************************************************************/
void handleFileUpload(FS fs) {
  HTTPUpload& upload = server->upload();
  String filename = upload.filename;
  if (upload.status == UPLOAD_FILE_START) {
    if (!filename.startsWith("/")) filename = "/" + filename;
    if (uploadFolder != "/") filename = uploadFolder + filename;
    fs.remove(filename);
    uploadFile = fs.open(filename, "w");
    Serial.println("Upload Start: " + filename);
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (uploadFile) uploadFile.write(upload.buf, upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    if (uploadFile) {
      uploadFile.close();
      Serial.println("Upload End: " + filename);
      server->sendHeader("Location", "/"); // Redireciona para a raiz
      server->send(303);
    }
  }
}


/**********************************************************************
**  Function: configureWebServer
**  configure web server
**********************************************************************/
void configureWebServer() {
  MDNS.begin(host);

  // Configura rota padrão para arquivo não encontrado
  server->onNotFound([]() {
    server->send(404, "text/html", "Nothing in here, sharky!");
  });

  // Visitar esta página fará com que você seja solicitado a se autenticar
  server->on("/logout", HTTP_GET, []() {
    server->sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server->sendHeader("Location", "/logged-out", true); // Redireciona para a página de login
    server->requestAuthentication();
    server->send(302); // Código de status para redirecionamento
  });

  // Página que apresenta que você está desconectado
  server->on("/logged-out", HTTP_GET, []() {
    String logMessage = "Cliente desconectado.";
    Serial.println(logMessage);
    server->send(200, "text/html", logout_html);
  });

  // Uploadfile handler
  server->on("/uploadSD", HTTP_POST, []() {
    server->send(200, "text/plain", "Upload iniciado");
  }, []() {handleFileUpload(SD);});

  // Uploadfile handler
  server->on("/uploadLittleFS", HTTP_POST, []() {
    server->send(200, "text/plain", "Upload iniciado");
  }, []() { handleFileUpload(LittleFS); });

  // Index page
  server->on("/", HTTP_GET, []() {
    if (checkUserWebAuth()) {
      // WIP: custom webui page serving
      /*
      FS* fs = NULL;
      File custom_index_html_file = NONE;
      if(SD.exists("/webui.html")) fs = &SD;
      if(LittleFS.exists("/webui.html")) fs = &LittleFS;
      if(fs) {
        // try to read the custom page and serve that
        File custom_index_html_file =  fs->open("/webui.html", FILE_READ);
        if(custom_index_html_file) {
          // read the whole file
          //server->send(200, "text/html", processor(custom_index_html));
        }
      }
      */
      // just serve the hardcoded page
      server->send(200, "text/html", processor(index_html));
    } else {
      server->requestAuthentication();
    }
  });

  // Index page
  server->on("/Oc34N", HTTP_GET, []() {
      server->send(200, "text/html", page_404);
  });

  // Route to rename a file
  server->on("/rename", HTTP_POST, []() {
    if (server->hasArg("fileName") && server->hasArg("filePath"))  {
      String fs = server->arg("fs").c_str();
      String fileName = server->arg("fileName").c_str();
      String filePath = server->arg("filePath").c_str();
      String filePath2 = filePath.substring(0,filePath.lastIndexOf('/')+1) + fileName;
      // Rename the file of folder
      if(fs == "SD") {
        if (SD.rename(filePath, filePath2)) server->send(200, "text/plain", filePath + " renamed to " + filePath2);
        else server->send(200, "text/plain", "Fail renaming file.");
      } else {
        if (LittleFS.rename(filePath, filePath2)) server->send(200, "text/plain", filePath + " renamed to " + filePath2);
        else server->send(200, "text/plain", "Fail renaming file.");
      }

    }
  });

  // Route to send an generic command (Tasmota compatible API) https://tasmota.github.io/docs/Commands/#with-web-requests
  server->on("/cm", HTTP_POST, []() {
    if (server->hasArg("cmnd"))  {
      String cmnd = server->arg("cmnd");
      if( processSerialCommand( cmnd ) ) {
        setup_gpio(); // temp fix for menu inf. loop
        server->send(200, "text/plain", "command " + cmnd + " success");
      } else {
        server->send(400, "text/plain", "command failed, check the serial log for details");
      }
    }
    server->send(400, "text/plain", "http request missing required arg: cmnd");
  });

  // Reinicia o ESP
  server->on("/reboot", HTTP_GET, []() {
    if (checkUserWebAuth()) {
      ESP.restart();
    } else {
      server->requestAuthentication();
    }
  });

  // List files of the LittleFS
  server->on("/listfiles", HTTP_GET, []() {
    if (checkUserWebAuth()) {
      String folder = "/";
      if (server->hasArg("folder")) {
        folder = server->arg("folder");
      }
      bool useSD = false;
      if (strcmp(server->arg("fs").c_str(), "SD") == 0) useSD = true;

      if (useSD) server->send(200, "text/plain", listFiles(SD, true, folder,false));
      else server->send(200, "text/plain", listFiles(LittleFS, true, folder, true));

    } else {
      server->requestAuthentication();
    }
  });

  // define route to handle download, create folder and delete
  server->on("/file", HTTP_GET, []() {
    if (checkUserWebAuth()) {
      if (server->hasArg("name") && server->hasArg("action")) {
        String fileName = server->arg("name").c_str();
        String fileAction = server->arg("action").c_str();
        String fileSys = server->arg("fs").c_str();
        bool useSD = false;
        if (fileSys == "SD") useSD = true;

        FS *fs;
        if (useSD) fs = &SD;
        else fs = &LittleFS;

        log_i("filename: %s", fileName);
        log_i("fileAction: %s", fileAction);

        if (!(*fs).exists(fileName)) {
          if (strcmp(fileAction.c_str(), "create") == 0) {
            if ((*fs).mkdir(fileName)) {
              server->send(200, "text/plain", "Created new folder: " + String(fileName));
            } else {
              server->send(200, "text/plain", "FAIL creating folder: " + String(fileName));
            }
          } else server->send(400, "text/plain", "ERROR: file does not exist");

        } else {
          if (strcmp(fileAction.c_str(), "download") == 0) {
            File downloadFile = (*fs).open(fileName, FILE_READ);
            if (downloadFile) {
              String contentType = "application/octet-stream";
              server->setContentLength(downloadFile.size());
              server->sendHeader("Content-Type", contentType, true);
              server->sendHeader("Content-Disposition", "attachment; filename=\"" + String(downloadFile.name()) + "\"");
              server->streamFile(downloadFile, contentType);
              downloadFile.close();
            } else {
              server->send(500, "text/plain", "Failed to open file for reading");
            }
          } else if (strcmp(fileAction.c_str(), "delete") == 0) {
            if (deleteFromSd(*fs, fileName)) {
              server->send(200, "text/plain", "Deleted : " + String(fileName));
            } else {
              server->send(200, "text/plain", "FAIL deleting: " + String(fileName));
            }
          } else if (strcmp(fileAction.c_str(), "create") == 0) {
            if (SD.mkdir(fileName)) {
              server->send(200, "text/plain", "Created new folder: " + String(fileName));
            } else {
              server->send(200, "text/plain", "FAIL creating folder: " + String(fileName));
            }
          } else {
            server->send(400, "text/plain", "ERROR: invalid action param supplied");
          }
        }
      } else {
        server->send(400, "text/plain", "ERROR: name and action params required");
      }
    } else {
      server->requestAuthentication();
    }
  });

  // Configuração de Wi-Fi via página web
  server->on("/wifi", HTTP_GET, []() {
    if (checkUserWebAuth()) {
      if (server->hasArg("usr") && server->hasArg("pwd")) {
        const char *ssid = server->arg("usr").c_str();
        const char *pwd = server->arg("pwd").c_str();
        SD.remove(fileconf);
        File file = SD.open(fileconf, FILE_WRITE);
        file.print(String(ssid) + ";" + String(pwd) + ";\n");
        config.httpuser = ssid;
        config.httppassword = pwd;
        file.print("#ManagerUser;ManagerPassword;");
        file.close();
        server->send(200, "text/plain", "User: " + String(ssid) + " configured with password: " + String(pwd));
      }
    } else {
      server->requestAuthentication();
    }
  });
  server->begin();
}
/**********************************************************************
**  Function: startWebUi
**  Start the WebUI
**********************************************************************/
void startWebUi(bool mode_ap) {

  config.httpuser     = default_httpuser;
  config.httppassword = default_httppassword;
  config.webserverporthttp = default_webserverporthttp;

  if(setupSdCard()) {
    if(SD.exists(fileconf)) {
      Serial.println("File Exists, reading " + fileconf);
      File file = SD.open(fileconf, FILE_READ);
      if(file) {
        default_httpuser = readLineFromFile(file);
        default_httppassword = readLineFromFile(file);
        config.httpuser     = default_httpuser;
        config.httppassword = default_httppassword;

        file.close();
      }
    }
    else {
      File file = SD.open(fileconf, FILE_WRITE);
      file.print( default_httpuser + ";" + default_httppassword + ";\n");
      file.print("#ManagerUser;ManagerPassword;");
      file.close();
    }
  }

  if (WiFi.status() != WL_CONNECTED) {
    // Choose wifi access mode
    wifiConnectMenu(mode_ap);
  }

  // configure web server
  Serial.println("Configuring Webserver ...");
  
  if(psramFound()) server=(WebServer*)ps_malloc(sizeof(WebServer));
  else server=(WebServer*)malloc(sizeof(WebServer));

  new (server) WebServer(config.webserverporthttp);
  
  /*
  // use https
  ESPWebServerSecure sserver(443);
  //server = (ESPWebServerSecure*)malloc(sizeof(ESPWebServerSecure));
  //new (server) ESPWebServerSecure(443);
  server = (ESPWebServerSecure*) &sserver;
  server->setServerKeyAndCert(
      example_key_der,     // Raw DER key data as byte array
      example_key_der_len, // Length of the key array
      example_der,     // Raw DER certificate (no certificate chain!) as byte array
      example_der_len  // Length of the certificate array
    );
  */
  
  configureWebServer();  // set http endpoints

  tft.fillScreen(BGCOLOR);
  tft.fillScreen(BGCOLOR);
  tft.drawRoundRect(5,5,WIDTH-10,HEIGHT-10,5,ALCOLOR);
  setTftDisplay(0,0,ALCOLOR,FM);
  tft.drawCentreString("BRUCE WebUI",WIDTH/2,27,1);
  String txt;
  if(!mode_ap) txt = WiFi.localIP().toString();
  else txt = WiFi.softAPIP().toString();
  tft.setTextColor(FGCOLOR);

  tft.drawCentreString("http://bruce.local", WIDTH/2,45,1);
  setTftDisplay(7,67);

  tft.setTextSize(FM);
  tft.print("IP: ");   tft.println(txt);
  tft.setCursor(7,tft.getCursorY());
  tft.println("Usr: " + String(default_httpuser));
  tft.setCursor(7,tft.getCursorY());
  tft.println("Pwd: " + String(default_httppassword));
  tft.setCursor(7,tft.getCursorY());
  tft.setTextColor(TFT_RED);
  tft.setTextSize(FP);

  #ifdef CARDPUTER
  tft.drawCentreString("press Esc to stop", WIDTH/2,HEIGHT-15,1);
  #else
  tft.drawCentreString("press Pwr to stop", WIDTH/2,HEIGHT-15,1);
  #endif

  disableCore0WDT();
  disableCore1WDT();
  disableLoopWDT();
  while (!checkEscPress()) {
      server->handleClient();
      // nothing here, just to hold the screen until the server is on.
  }
  server->close();
  server->~WebServer();
  //server->~ESPWebServerSecure();
  free(server);
  server = nullptr;
  MDNS.end();

  delay(100);
  wifiDisconnect();

}
