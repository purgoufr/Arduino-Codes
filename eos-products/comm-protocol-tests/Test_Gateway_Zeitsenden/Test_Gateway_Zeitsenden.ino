#include <SPI.h>
#include <dht11.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <EEPROM.h>
#include <EOSCommunication.h>
#define DHT11PIN 2
using namespace std;

const uint16_t MSG_C1 = 0x4331;
const uint16_t MSG_GO = 0x474F;
const uint16_t MSG_SD = 0x5344;

const uint64_t ADDR_SLAVE_PREFIX = 0x6C00000000LL;

const uint64_t ADDR_GATEWAY_1 = 0xB3B4B3B4B1LL;
const uint64_t ADDR_GATEWAY_2 = 0xB3B4B3B4B2LL;
const uint64_t ADDR_GATEWAY_3 = 0xB3B4B3B4B3LL;
const uint64_t ADDR_GATEWAY_4 = 0xB3B4B3B4B4LL;
const uint64_t ADDR_GATEWAY_5 = 0xB3B4B3B4B5LL;

const uint32_t DEVICE_ADDR = 0x00000001;

char buffer[33];

uint32_t *senderAddress = (uint32_t*) &buffer[0];
uint16_t *messageType = (uint16_t*) &buffer[4];

const uint8_t GO_LEN = 6;

uint16_t *SD_TYPE = (uint16_t*) &buffer[6];
uint16_t SD_TYPE_MS = 0x4D53;
uint16_t SD_TYPE_PA = 0x5041;

uint32_t *C1_LISTEN_INT = (uint32_t*) &buffer[6];
uint32_t *C1_LISTEN_LEN = (uint32_t*) &buffer[10];
uint32_t *C1_SYNC_INT = (uint32_t*) &buffer[14];
const uint8_t C1_LEN = 18;

float *SD_MS_TEMPERATURE = (float*) &buffer[8];
float *SD_MS_HUMIDITY = (float*) &buffer[12];
float *SD_MS_LUMINOCITY = (float*) &buffer[16];
const uint8_t SD_MS_LEN = 20;

float *SD_PA_VOLTAGE = (float*) &buffer[8];
float *SD_PA_AMPERAGE = (float*) &buffer[12];
const uint8_t SD_PA_LEN = 16;

uint32_t intervall = 0;
boolean initialized = false;

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

       radio.openReadingPipe(1, ADDR_GATEWAY_1);       
       radio.openReadingPipe(2, ADDR_GATEWAY_2);
       radio.openReadingPipe(3, ADDR_GATEWAY_3);
       radio.openReadingPipe(4, ADDR_GATEWAY_4);
       radio.openReadingPipe(5, ADDR_GATEWAY_5);
}

void loop(void){

 Serial.println("Starting Program"); 
  
   if (initialized == false) {
     radio.openWritingPipe(ADDR_SLAVE_PREFIX | DEVICE_ADDR); 
     *senderAddress = DEVICE_ADDR;
     *messageType = MSG_C1;
     *C1_LISTEN_INT = 0;
     *C1_LISTEN_LEN = 0;
     *C1_SYNC_INT = 2000;
     Serial.println("Write Buffer");
     radio.write(buffer, C1_LEN);  
     Serial.println("Sending C1_MSG");
   }
      
  Serial.println("Start Listening");
  
  radio.startListening();
   
  if (radio.available()) {
    radio.read(buffer, 32);
    if(*senderAddress == DEVICE_ADDR && *messageType == MSG_SD) {
      if(*SD_TYPE == SD_TYPE_MS) {
        Serial.println(String(*SD_MS_TEMPERATURE) + "C " + String(*SD_MS_HUMIDITY) + "% " + String(*SD_MS_LUMINOCITY) + "lux"); 
      }
    } else if(*senderAddress == DEVICE_ADDR && *messageType == MSG_GO) {
      initialized = true;
      Serial.println("Received MSG_GO");
    } else {
      Serial.println("tralala");
    }
  } else {
    Serial.println("no data");
    
  }
  radio.stopListening();
}


