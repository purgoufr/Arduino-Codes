#include "Arduino.h"
#include "EOSComm.h"

EOSComm::EOSComm(RF24 &radio, uint32_t device_address, const char* device_type) : radio(radio), myDeviceAddress(device_address), myDeviceType(device_type) {
  buffer[32] = 0;
}

// initialization:
void EOSComm::begin() {
  radio.begin();
  radio.setRetries(15,15);
  radio.setChannel(76);
  radio.setDataRate(RF24_1MBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setCRCLength(RF24_CRC_16);
  
  radio.openReadingPipe(1,ADDR_SLAVE_PREFIX | myDeviceAddress);
  radio.openReadingPipe(0, ADDR_BROADCAST);
  radio.startListening();
}

void EOSComm::hi_ho() {
  bool device_discovered = false;
  while (device_discovered == false) {
    write_hi(myDeviceType);
    delay(500);
    if(readIfAvailable()) {
      if(device_address() == myDeviceAddress && message_type() == MSG_HO) {
        device_discovered = true;
      }
    }
  }
}

// reading:
bool EOSComm::readIfAvailable() {
  if(radio.available()) {
    radio.read(buffer, 32);
    return true;
  } else {
    return false;
  }
}

// writing:
void EOSComm::write_hi(const char* device_type) {
  *MESSAGE_TYPE = MSG_HI;
  strcpy(HI_DEVICE_TYPE, device_type);  
  write(ADDR_GATEWAY_1);
}

void EOSComm::write_go() {
  *MESSAGE_TYPE = MSG_GO;
  write(ADDR_GATEWAY_5);
}

void EOSComm::write_sd_ms(float temperature, float humidity, float luminocity, float gas) {
  *MESSAGE_TYPE = MSG_SD;
  *SD_TYPE = SD_TYPE_MS;
  *SD_MS_TEMPERATURE = temperature;
  *SD_MS_HUMIDITY = humidity;
  *SD_MS_LUMINOCITY = luminocity;
  *SD_MS_GAS = gas;
  write(ADDR_GATEWAY_2);
}


void EOSComm::write_sd_cm(float power) {
  *MESSAGE_TYPE = MSG_SD;
  *SD_TYPE = SD_TYPE_CM;
  *SD_CM_POWER = power;
  write(ADDR_GATEWAY_2);
}

void EOSComm::write_sd_pa(float voltage, float amperage, float watt) {
  *MESSAGE_TYPE = MSG_SD;
  *SD_TYPE = SD_TYPE_PA;
  *SD_PA_VOLTAGE = voltage;
  *SD_PA_AMPERAGE = amperage;
  *SD_PA_WATT = watt;
  write(ADDR_GATEWAY_2);
}
    
void EOSComm::write_ed_pc(uint16_t person_count) {
  *MESSAGE_TYPE = MSG_ED;
  *ED_TYPE = ED_TYPE_PC;
  *ED_PC_PERSON_COUNT = person_count;
  write(ADDR_GATEWAY_4);
}

void EOSComm::write_ack() {
  if (*MESSAGE_TYPE == MSG_CM) {
    *MESSAGE_TYPE = MSG_CA;
  } else if (*MESSAGE_TYPE == MSG_IV) {
    *MESSAGE_TYPE = MSG_IA;
  } 
  write(ADDR_GATEWAY_5);
}
 
void EOSComm::write(uint64_t target_address) {
  *DEVICE_ADDRESS = myDeviceAddress;
  radio.stopListening();
  radio.openWritingPipe(target_address);
  radio.write(buffer, 32);
  radio.openReadingPipe(0, ADDR_BROADCAST);
  radio.startListening();
}



