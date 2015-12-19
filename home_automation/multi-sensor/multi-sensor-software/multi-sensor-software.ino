#include <dht11.h>
#include <Timer.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <EEPROM.h>
#define DHT11PIN 2
#include <EOSComm.h>

using namespace std;

const uint32_t DEVICE_ADDR = 0x00000003;

Timer t;
int receiveEvent = -1;
int sendEvent = -1;

RF24 radio(9, 10);
EOSComm comm(radio, DEVICE_ADDR, "HW-MS");

dht11 DHT11;

const int sensingPower = 7;
const int ldrAnalogPin = A0;
const int sendingCheckPin = 4;
const int motionSensorOut = 5;

void setup(void)
{
  pinMode(sendingCheckPin, OUTPUT);
  pinMode(motionSensorOut, INPUT);
  pinMode(ldrAnalogPin, INPUT);
  Serial.begin(9600);
  comm.begin();
  reset();
}

void reset() {
  t.stop(sendEvent);
  t.stop(receiveEvent);
  sendEvent = -1;
  receiveEvent = -1;
  comm.hi_ho();
  receiveEvent = t.every(10, receive, (void*)0);
}

void receive(void* params_unused) {
  if (comm.readIfAvailable()) {
    if (comm.message_type() == EOSComm::MSG_C1) {
      Serial.println("Initialized");
      comm.write_go();

      t.stop(receiveEvent);
      receiveEvent = t.every(comm.c1_listen_int(), receive, (void*)0);
      t.stop(sendEvent);
      sendEvent = t.every(comm.c1_sync_int(),sendData,(void*)0);
      
    } else if (comm.message_type() == EOSComm::MSG_RS)  {
      Serial.println("received MSG_RS");
      reset();
    }
  }
}

void sendData(void* params_unused) {
  int chk = DHT11.read(DHT11PIN);
  int lum = analogRead(ldrAnalogPin);

  comm.write_sd_ms(DHT11.temperature, DHT11.humidity, lum);
}

void loop(void) {
  t.update();
}



