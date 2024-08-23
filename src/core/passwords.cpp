#include "mbedtls/aes.h"
#include "mbedtls/md.h"
#include "mbedtls/pkcs5.h"
#include <Arduino.h>

#include <mbedtls/base64.h>
#include <mbedtls/sha256.h>
#include <mbedtls/pkcs5.h>
#include <mbedtls/gcm.h>
#include <mbedtls/error.h>      // error code to string conversion


// https://github.com/tleonhardt/practical_cryptography_engineering/blob/master/kdf.c


const size_t iterations = 100000;
const size_t keyLength = 32;


String aes_decrypt(String encryptedBase64, const String& password_str) {
    const char* password = password_str.c_str();
    //char mbed_err[MBED_ERR_BUF];
    int ret;
    
    // decode base64
    size_t decodedLen;
    mbedtls_base64_decode(NULL, 0, &decodedLen, (const unsigned char *)encryptedBase64.c_str(), encryptedBase64.length());
    unsigned char* decodedData = (unsigned char*)malloc(decodedLen);
    memset(decodedData, 0, decodedLen);
    mbedtls_base64_decode(decodedData, decodedLen, &decodedLen, (const unsigned char *)encryptedBase64.c_str(), encryptedBase64.length());
    if(decodedLen<16 + 12 + 1) {
        Serial.println("base64 decode error or incomplete file");
        return "";
    }

    // Extract salt, iv, and encrypted content
    unsigned char* salt = decodedData;
    unsigned char* iv = decodedData + 16;
    unsigned char* encryptedContent = decodedData + 16 + 12;
    size_t encryptedContentLen = decodedLen - 16 - 12;
    
    // Derive key using PBKDF2
    unsigned char key[keyLength];
    mbedtls_md_context_t md_ctx;
    const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);

    mbedtls_md_init(&md_ctx);
    ret = mbedtls_md_setup(&md_ctx,
        md_info,
        1); // non-zero implies HMAC is going to be used (0 saves memory, but is less secure)
    if (md_info == NULL || ret != 0) {  
        Serial.println("Hash algorithm setup failed");
        Serial.println(ret);
        free(decodedData);
        //mbedtls_strerror(ret, mbed_err, MBED_ERR_BUF);
        //mbedtls_printf( "mbedTLS ERROR: %s\n", mbed_err);
        return "";
    }

    ret = mbedtls_pkcs5_pbkdf2_hmac(
        &md_ctx,
        (const unsigned char*)password,
        strlen(password),
        salt,
        16,  //  salt_bytes
        iterations,
        keyLength,
        key
    );

    mbedtls_md_free(&md_ctx);

    if (ret != 0) {
        Serial.println("Key derivation failed");
        Serial.println(ret);
        free(decodedData);
        //mbedtls_strerror(ret, mbed_err, MBED_ERR_BUF);
        //mbedtls_printf( "mbedTLS ERROR: %s\n", mbed_err);
        return "";
    }

    // Decrypt using AES-GCM
    unsigned char decryptedContent[encryptedContentLen] = {0};
    mbedtls_gcm_context gcm;
    mbedtls_gcm_init(&gcm);

    ret = mbedtls_gcm_setkey(&gcm, MBEDTLS_CIPHER_ID_AES, key, keyLength * 8);

    if (ret != 0) {
        Serial.println("Set Key failed");
        free(decodedData);
        Serial.println(ret);
        //mbedtls_strerror(ret, mbed_err, MBED_ERR_BUF);
        //mbedtls_printf( "mbedTLS ERROR: %s\n", mbed_err);
        return "";
    }
    
    ret = mbedtls_gcm_auth_decrypt(&gcm, encryptedContentLen, iv, 12, NULL, 0, NULL, 0, encryptedContent, decryptedContent);
    mbedtls_gcm_free(&gcm);

    String r = String((char*)decryptedContent);
    free(decodedData);
    //free(decryptedContent);
    
    if (ret == 0) {
        // Successfully decrypted
        return(r);
    } else {
        // Decryption failed
        Serial.println("Decryption failed");
        Serial.println(ret);
        //mbedtls_strerror(ret, mbed_err, MBED_ERR_BUF);
        //mbedtls_printf( "mbedTLS ERROR: %s\n", mbed_err);
        return("");
    }
}


String aes_decrypt_old(uint8_t* inputData, size_t fileSize, const String& password_str) {
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
