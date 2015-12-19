#include <Timer.h>

#include <SPI.h>
#include <dht11.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <EEPROM.h>
#define DHT11PIN 2
using namespace std;

const uint16_t MSG_HI = 0x4849;
const uint16_t MSG_HO = 0x484F;
const uint16_t MSG_RS = 0x5253;
const uint16_t MSG_C1 = 0x4331;
const uint16_t MSG_GO = 0x474F;
const uint16_t MSG_SD = 0x5344;

const uint64_t ADDR_BROADCAST = 0x90F0F0F0F0LL;
const uint64_t ADDR_NULL = 0x0LL;
const uint64_t ADDR_SLAVE_PREFIX = 0x6C00000000LL;

const uint64_t ADDR_GATEWAY_1 = 0xB3B4B3B4B1LL;
const uint64_t ADDR_GATEWAY_2 = 0xB3B4B3B4B2LL;
const uint64_t ADDR_GATEWAY_3 = 0xB3B4B3B4B3LL;
const uint64_t ADDR_GATEWAY_4 = 0xB3B4B3B4B4LL;
const uint64_t ADDR_GATEWAY_5 = 0xB3B4B3B4B5LL;

const uint32_t DEVICE_ADDR = 0x00000011;

char buffer[33];

uint32_t *senderAddress = (uint32_t*) &buffer[0];
uint16_t *messageType = (uint16_t*) &buffer[4];

const uint8_t HI_LEN = 6;
const uint8_t GO_LEN = 6;

uint16_t *SD_TYPE = (uint16_t*) &buffer[6];
uint16_t SD_TYPE_MS = 0x4D53;
uint16_t SD_TYPE_PA = 0x5041;

uint32_t *C1_LISTEN_INT = (uint32_t*) &buffer[6];
uint32_t *C1_LISTEN_LEN = (uint32_t*) &buffer[10];
uint32_t *C1_SYNC_INT = (uint32_t*) &buffer[14];

float *SD_MS_TEMPERATURE = (float*) &buffer[8];
float *SD_MS_HUMIDITY = (float*) &buffer[12];
float *SD_MS_LUMINOCITY = (float*) &buffer[16];
const uint8_t SD_MS_LEN = 20;

float *SD_PA_VOLTAGE = (float*) &buffer[8];
float *SD_PA_AMPERAGE = (float*) &buffer[12];
const uint8_t SD_PA_LEN = 16;

uint32_t intervall = 10;

boolean identity = false;
boolean initialized = false;

Timer t;
int receiveEvent = -1;
int sendEvent = -1;

RF24 radio(9, 10);

void setup(void)
{  
  Serial.begin(9600);

  radio.begin();
  radio.setRetries(15,15);
  radio.setChannel(76);
  radio.setDataRate(RF24_1MBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setCRCLength(RF24_CRC_16); 
  buffer[32] = 0;
  
  radio.openReadingPipe(1,ADDR_SLAVE_PREFIX | DEVICE_ADDR);

  hi_ho();  
}

void hi_ho() {
  while (identity == false) {
    radio.stopListening();
    radio.openWritingPipe(ADDR_GATEWAY_1);
    *senderAddress = DEVICE_ADDR;
    *messageType = MSG_HI;
    radio.write(buffer, HI_LEN);
    Serial.println("Send MSG_HI");
    radio.openReadingPipe(0,ADDR_NULL);
    
    radio.startListening();
    Serial.println("Waiting for MSG_HO");
    delay(500);
    if (radio.available()) {
      Serial.println("Radio_available");
      *messageType = 0;
      radio.read(buffer, 32);
      if(*senderAddress == DEVICE_ADDR && *messageType == MSG_HO) {
        identity =  true;
        Serial.println("clarified_identity ");
        radio.stopListening();
      }
    }   
  }
  
  radio.openReadingPipe(0,ADDR_BROADCAST);
  radio.startListening();
  receiveEvent = t.every(10,receiveConfig,(void*)0);
}

void receiveConfig(void* params_unused) {
  Serial.println("receiveConfig");
  if (radio.available()) {
    radio.read(buffer, 32);
    Serial.println("radio.available");
    if(*senderAddress == DEVICE_ADDR && *messageType == MSG_C1) {
      intervall = *((uint32_t*) &buffer[14]);
      Serial.println(String(intervall));
      initialized =  true;
      Serial.println("Initialized");
      radio.stopListening();
      radio.openWritingPipe(ADDR_GATEWAY_5);
      *messageType = MSG_GO;
      radio.write(buffer, GO_LEN);
      Serial.println("Send MSG_GO");
      radio.openReadingPipe(0,ADDR_BROADCAST);
      radio.startListening();
      
      t.stop(receiveEvent);
      receiveEvent = t.every(500, receiveConfig, (void*)0);
      
      if (sendEvent != -1) {
        t.stop(sendEvent);
      }
      sendEvent = t.every(intervall,sendData,(void*)0);
    } else if (*senderAddress == 0 && *messageType == MSG_RS)  {
        Serial.println("MSG_RS empfangen");
        delay(1000);  
        identity = false;
        t.stop(sendEvent);
        t.stop(receiveEvent);
        sendEvent = -1;
        receiveEvent = -1;
        hi_ho();
    }
      
  }
}

void sendData(void* params_unused) {
  *senderAddress = DEVICE_ADDR;
  *messageType = MSG_SD;
  *SD_TYPE = SD_TYPE_MS;
  *SD_MS_TEMPERATURE = 34.62f;
  *SD_MS_HUMIDITY = 92.4f;
  *SD_MS_LUMINOCITY = 100.2f;
  
  radio.stopListening();   
  radio.openWritingPipe(ADDR_GATEWAY_2);
  radio.write(buffer, SD_MS_LEN);
  Serial.println("Sende Sensordaten");
  radio.openReadingPipe(0,ADDR_BROADCAST);
  radio.startListening();
}

void loop(void){
  t.update();
}
  

      
