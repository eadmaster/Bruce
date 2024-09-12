
#include <Arduino.h>
#include <MD5Builder.h>

#include "passwords.h"
#include "globals.h"
#include "sd_functions.h"
#include "mykeyboard.h"



bool isValidAscii(const String &text) {
  for (int i = 0; i < text.length(); i++) {
      char c = text[i];
      // Check if the character is within the printable ASCII range or is a newline/carriage return
      if (!(c >= 32 && c <= 126) && c != 10 && c != 13) {
          return false; // Invalid character found
      }
  }
  return true; // All characters are valid
}


String readDecryptedFile(FS &fs, String filepath) {
  String cyphertext = readSmallFile(fs, filepath);
  if(cyphertext.length() == 0) return "";
  
  if(cachedPassword.length()==0) {
    cachedPassword = keyboard("", 32, "password");
    if(cachedPassword.length()==0) return "";  // cancelled
  }
  
  //Serial.println(cyphertext);
  //Serial.println(cachedPassword);
  
  // else try to decrypt
  String plaintext = decryptString(cyphertext, cachedPassword);
  
  // check if really plaintext
  if(!isValidAscii(plaintext)) {
    // invalidate cached password -> will ask again on the next try
    cachedPassword = "";
    Serial.println("invalid password");
    //Serial.println(plaintext);
    return "";
  }
  
  // else
  return plaintext;
}


String xorEncryptDecrypt(const String &input, const String &password) {
  uint8_t md5Hash[16];
  
  MD5Builder md5;
  md5.begin();
  md5.add(password);
  md5.calculate();
  md5.getBytes(md5Hash);  // Store MD5 hash in the output array
  
  String output = input;  // Copy input to output for modification
  for (size_t i = 0; i < input.length(); i++) {
    output[i] = input[i] ^ md5Hash[i % 16];  // XOR each byte with the MD5 hash
  }

  return output;
}



String encryptString(String& plaintext, const String& password_str) {
  // TODO: add "XOR" header
  return xorEncryptDecrypt(plaintext, password_str);
}

String decryptString(String& cypertext, const String& password_str) {
  return xorEncryptDecrypt(cypertext, password_str);
}
