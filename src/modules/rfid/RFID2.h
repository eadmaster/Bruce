/**
 * @file RFID2.h
 * @author Rennan Cockles (https://github.com/rennancockles)
 * @brief Read and Write RFID tags using RFID2 module from M5Stack
 * @version 0.1
 * @date 2024-08-19
 */

#include "RFIDInterface.h"
#include "lib_mfrc522/mfrc522_i2c.h"


class RFID2 : public RFIDInterface {
public:
    MFRC522 mfrc522;

    /////////////////////////////////////////////////////////////////////////////////////
    // Constructor
    /////////////////////////////////////////////////////////////////////////////////////
    RFID2();

    /////////////////////////////////////////////////////////////////////////////////////
    // Life Cycle
    /////////////////////////////////////////////////////////////////////////////////////
    bool begin();

    /////////////////////////////////////////////////////////////////////////////////////
    // Operations
    /////////////////////////////////////////////////////////////////////////////////////
    int read();
    int clone();
    int erase();
    int write();
    int write_ndef();
    int load();
    int save(String filename);

private:
    /////////////////////////////////////////////////////////////////////////////////////
    // Converters
    /////////////////////////////////////////////////////////////////////////////////////
    void format_data();
    void parse_data();
    void set_uid();

    /////////////////////////////////////////////////////////////////////////////////////
    // PICC Helpers
    /////////////////////////////////////////////////////////////////////////////////////
    bool PICC_IsNewCardPresent();

    String get_tag_type();
    bool read_data_blocks();
    bool read_mifare_classic_data_blocks(byte piccType, MFRC522::MIFARE_Key *key);
    bool read_mifare_classic_data_sector(MFRC522::MIFARE_Key *key, byte sector);
    bool read_mifare_ultralight_data_blocks();

    int write_data_blocks();
    bool write_mifare_classic_data_block(int block, String data);
    bool write_mifare_ultralight_data_block(int block, String data);

    int erase_data_blocks();
    int write_ndef_blocks();
};
