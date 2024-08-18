#include "mbedtls/aes.h"
#include "mbedtls/md.h"
#include "mbedtls/pkcs5.h"
#include <Arduino.h>


// alt: https://github.com/Joengenduvel/espwv32/blob/master/Storage.cpp

// https://chatgpt.com/c/0a63f4ed-a655-4c9d-b919-eb953a94f43a
// https://arduino.stackexchange.com/questions/96656/decrypting-openssl-cypertext-on-the-esp32


String aes_decrypt(uint8_t* inputData, size_t fileSize, const String& password_str) {
    // generate key, iv, salt
    const char *password = password_str.c_str();
    unsigned char key[32];
    unsigned char iv[16];
    unsigned char salt[8] = { /* The salt used by OpenSSL during encryption */ };

    mbedtls_md_context_t md_ctx;
    mbedtls_md_init(&md_ctx);
    mbedtls_md_setup(&md_ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 1);

    // Derive key and IV using PBKDF2
    mbedtls_pkcs5_pbkdf2_hmac(&md_ctx, (unsigned char*)password, strlen(password), salt, 8, 10000, 32, key);
    mbedtls_pkcs5_pbkdf2_hmac(&md_ctx, (unsigned char*)password, strlen(password), salt, 8, 10000, 16, iv);

    mbedtls_md_free(&md_ctx);
    
    unsigned char *outputData = new unsigned char[fileSize];
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_dec(&aes, key, 256);

    mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, fileSize, iv, inputData, outputData);

    mbedtls_aes_free(&aes);
    
    // Convert the decrypted data to a string
    String decryptedText = String(outputData, fileSize);
    free(outputData);

    return decryptedText;
}
    

String aes_decryptold(uint8_t* cipher_bin, size_t cipher_len, const String& password) {
    // Convert the password to a C string
    const char* pass = password.c_str();
    
    // Check for minimum length of ciphertext (16 bytes: 8 for header + 8 for salt)
    if (cipher_len < 16) {
        // Not enough data for salt and encrypted content
        Serial.println("cypertext too small");
        return "";
    }

    // Check if the header matches "Salted__"
    /*
    String ciphertext = String(cipher_bin);  // risky
    if (ciphertext.substring(0, 8) != "Salted__") {
        Serial.println("no Salted header");
        return "";
    }*/

    // Extract the salt (the next 8 bytes)
    uint8_t salt[8];
    //ciphertext.substring(8, 16).getBytes(salt, 8);
    memcpy(salt, cipher_bin+8, 8);

    // Extract the encrypted data
    uint8_t* encrypted_data = cipher_bin + 16;

    // Prepare variables for key and IV generation
    uint8_t key[32]; // AES-256 key size
    uint8_t iv[16];  // AES block size for IV
    const size_t key_iv_len = sizeof(key) + sizeof(iv);
    uint8_t key_iv[key_iv_len];
    memset(key_iv, 0, key_iv_len);

    // Generate key and IV using the password and salt
    mbedtls_md_context_t md_ctx;
    mbedtls_md_init(&md_ctx);
    const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(MBEDTLS_MD_MD5);
    mbedtls_md_setup(&md_ctx, md_info, 0);

    // Derive key and IV using OpenSSL compatible method
    mbedtls_md_starts(&md_ctx);
    mbedtls_md_update(&md_ctx, (const uint8_t*)pass, strlen(pass));
    mbedtls_md_update(&md_ctx, salt, sizeof(salt));
    mbedtls_md_finish(&md_ctx, key_iv);

    memcpy(key, key_iv, 32);
    memcpy(iv, key_iv + 32, 16);

    mbedtls_md_free(&md_ctx);

    // Initialize AES context
    mbedtls_aes_context aes_ctx;
    mbedtls_aes_init(&aes_ctx);
    mbedtls_aes_setkey_dec(&aes_ctx, key, 256);

    // Decrypt the ciphertext
    uint8_t* output = (uint8_t*)malloc(cipher_len+1);
    if (output == nullptr) {
        mbedtls_aes_free(&aes_ctx);
        return ""; // Memory allocation failed
    }

    mbedtls_aes_crypt_cbc(&aes_ctx, MBEDTLS_AES_DECRYPT, cipher_len, iv, cipher_bin, output);

    // Clean up
    mbedtls_aes_free(&aes_ctx);

    // Convert the decrypted data to a string
    output[cipher_len] = 0;  // null-termnate
    String decryptedText = String((char*)output, cipher_len);
    free(output);

    return decryptedText;
}
