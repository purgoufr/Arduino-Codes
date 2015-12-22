#ifndef EOSComm_h
#define EOSComm_h

#include "Arduino.h"
#include <string.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

class EOSComm
{
  public:
    static const uint16_t MSG_HI = 0x4849;
    static const uint16_t MSG_HO = 0x484F;
    static const uint16_t MSG_RS = 0x5253;
    static const uint16_t MSG_C1 = 0x4331;
    static const uint16_t MSG_GO = 0x474F;
    static const uint16_t MSG_SD = 0x5344;
    static const uint16_t MSG_ED = 0x4544;
    static const uint16_t MSG_EA = 0x4541;
    static const uint16_t MSG_CM = 0x434D;
    static const uint16_t MSG_CA = 0x4341;
    static const uint16_t MSG_IV = 0x4956;
    static const uint16_t MSG_IA = 0x4941;

    static const uint64_t ADDR_BROADCAST = 0x90F0F0F0F0LL;
    static const uint64_t ADDR_NULL = 0x0LL;
    static const uint64_t ADDR_SLAVE_PREFIX = 0x6C00000000LL;

    static const uint64_t ADDR_GATEWAY_1 = 0xB3B4B3B4B1LL;
    static const uint64_t ADDR_GATEWAY_2 = 0xB3B4B3B4B2LL;
    static const uint64_t ADDR_GATEWAY_3 = 0xB3B4B3B4B3LL;
    static const uint64_t ADDR_GATEWAY_4 = 0xB3B4B3B4B4LL;
    static const uint64_t ADDR_GATEWAY_5 = 0xB3B4B3B4B5LL;

    static const uint16_t SD_TYPE_MS = 0x4D53;
    static const uint16_t SD_TYPE_CM = 0x434D;
    static const uint16_t SD_TYPE_PA = 0x5041;

    static const uint16_t CM_TYPE_ID = 0x4944;
    static const uint16_t CM_TYPE_AC = 0x4143;
    static const uint16_t CM_TYPE_TH = 0x5448;
    static const uint16_t CM_TYPE_SW = 0x5357;

    static const uint16_t ED_TYPE_PC = 0x5043;

  private:
    RF24 &radio;
    uint32_t myDeviceAddress;
    const char* myDeviceType;
    
    char buffer[33];

  private:
    uint32_t *DEVICE_ADDRESS = (uint32_t*) &buffer[0];
    uint16_t *MESSAGE_TYPE = (uint16_t*) &buffer[4];

    char *HI_DEVICE_TYPE = (char*) &buffer[6];

    uint32_t *C1_LISTEN_INT = (uint32_t*) &buffer[6];
    uint32_t *C1_LISTEN_LEN = (uint32_t*) &buffer[10];
    uint32_t *C1_SYNC_INT = (uint32_t*) &buffer[14];

    uint16_t *SD_TYPE = (uint16_t*) &buffer[6];
    float *SD_MS_TEMPERATURE = (float*) &buffer[8];	
    float *SD_MS_HUMIDITY = (float*) &buffer[12];
    float *SD_MS_LUMINOCITY = (float*) &buffer[16];
	float *SD_MS_GAS = (float*) &buffer[20];

    float *SD_CM_POWER = (float*) &buffer[8];

    float *SD_PA_VOLTAGE = (float*) &buffer[8];
    float *SD_PA_AMPERAGE = (float*) &buffer[12];
	float *SD_PA_WATT = (float*) &buffer[16];

    uint16_t *CM_TYPE = (uint16_t*) &buffer[6];    
    uint8_t *CM_ID_DIMMING_LEVEL = (uint8_t*) &buffer[8];
    uint8_t *CM_AC_ON = (uint8_t*) &buffer[8];
    uint8_t *CM_AC_TEMPERATURE_LEVEL = (uint8_t*) &buffer[9];
    uint8_t *CM_TH_VALVE_LEVEL = (uint8_t*) &buffer[8];
    uint8_t *CM_SW_ON = (uint8_t*) &buffer[8];

    uint16_t *ED_TYPE = (uint16_t*) &buffer[6];
    uint16_t *ED_PC_PERSON_COUNT = (uint16_t*) &buffer[8];

  public:
    EOSComm(RF24 &radio, uint32_t device_address, const char* device_type);

    void begin();
    void hi_ho();

    // reading:
    bool readIfAvailable();

    uint32_t device_address() { return *DEVICE_ADDRESS; }
    uint16_t message_type() { return *MESSAGE_TYPE; }

    uint32_t c1_listen_int() { return *C1_LISTEN_INT; }
    uint32_t c1_listen_len() { return *C1_LISTEN_LEN; }
    uint32_t c1_sync_int() { return *C1_SYNC_INT; }

    uint16_t ed_type() { return *ED_TYPE; }

    uint16_t ed_pc_person_count() { return *ED_PC_PERSON_COUNT; }

    uint16_t cm_type() { return *CM_TYPE; }

    uint8_t cm_id_dimming_level() { return *CM_ID_DIMMING_LEVEL; }
    uint8_t cm_ac_on() { return *CM_AC_ON; }
    uint8_t cm_ac_temperature_level() { return *CM_AC_TEMPERATURE_LEVEL; }
    uint8_t cm_th_valve_level() { return *CM_TH_VALVE_LEVEL; }
    uint8_t cm_sw_on() { return *CM_SW_ON; }

    // writing
    void write_hi(const char* device_type);

    void write_go();

    void write_sd_ms(float temperature, float humidity, float luminocity, float gas);
    void write_sd_cm(float power);
	void write_sd_pa(float voltage, float amperage ,float watt);
    void write_ed_pc(uint16_t person_count);

    void write_ack();

  private:
    void write(uint64_t target_address);
};

#endif
