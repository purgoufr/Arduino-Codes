#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Wire.h>
#include <I2C_eeprom.h>
#include <avr/pgmspace.h>
#include <IRremote.h>
#include <Timer.h>
#include <TimerOne.h>
#include <EEPROM.h>
#include <EOSComm.h>

using namespace std;

const uint32_t DEVICE_ADDR = 0x00000022;

uint32_t intervall;

Timer t;
int receiveEvent = -1;
int sendEvent = -1;

RF24 radio(9, 10);
EOSComm comm(radio, DEVICE_ADDR, "HW-ACT_AC");

boolean feedbackReceived = true;

int temperature = 0;

const int adr = 456;

long resStart;

IRsend irsend;
I2C_eeprom ee(0x50);

void setup(void)
{
  pinMode(8, OUTPUT);
  Serial.begin(9600);
  comm.begin();
  //reset();
  ee.begin();
}

void reset() {
  t.stop(sendEvent);
  t.stop(receiveEvent);
  sendEvent = -1;
  receiveEvent = -1;
  comm.hi_ho();
  receiveEvent = t.every(10, receive, (void*)0);
}

int readIntEeprom(int address) {

  // read value
  int a = ee.readByte(address);
  int b = ee.readByte(address + 1);

  int value = 0;

  value = a * 256 + b;

  return value;
}

void exeCommand(int address) {
  unsigned int tmp[228];

  for (int i = 0; i < 228; i++) {
    tmp[i] = readIntEeprom(i * 2 + address);
  }

  irsend.sendRaw(tmp, 228, 38);
}

void receive(void* params_unused) {
  if (comm.readIfAvailable()) {
    if (comm.message_type() == EOSComm::MSG_C1) {
      intervall = comm.c1_listen_int();
      Serial.println("Initialized");
      comm.write_go();

      t.stop(receiveEvent);
      receiveEvent = t.every(intervall, receive, (void*)0);

      if (sendEvent != -1) {
        t.stop(sendEvent);
      }
    } else if (comm.message_type() == EOSComm::MSG_RS)  {
      Serial.println("received MSG_RS");
      reset();
    } else if (comm.message_type() == EOSComm::MSG_IV && comm.cm_type() == EOSComm::CM_TYPE_AC) {
      //EEPROM.put(0, (int) comm.cm_ac_temperature_level());
      comm.write_ack();
    } else if (comm.message_type() == EOSComm::MSG_CM && comm.cm_type() == EOSComm::CM_TYPE_AC) {
      temperature = comm.cm_ac_temperature_level();

      comm.write_ack();
    }
  }
}

void setACTmp(int temp) {
  switch (temp) {
    case 18:
      exeCommand(adr * 2);
      break;
    case 19:
      exeCommand(adr * 3);
      break;
    case 20:
      exeCommand(adr * 4);
      break;
    case 21:
      exeCommand(adr * 5);
      break;
    case 22:
      exeCommand(adr * 6);
      break;
    case 23:
      exeCommand(adr * 7);
      break;
    case 24:
      exeCommand(adr * 8);
      break;
    case 25:
      exeCommand(adr * 9);
      break;
    case 26:
      exeCommand(adr * 10);
      break;
    case 27:
      exeCommand(adr * 11);
      break;
    case 28:
      exeCommand(adr * 12);
      break;
    case 29:
      exeCommand(adr * 13);
      break;
    case 30:
      exeCommand(adr * 14);
      break;
  }
}

void setACOff() {
  exeCommand(456);
}

void setACOn() {
  exeCommand(0);
}

//void manageIREValue(void) {
//  if (messageReceived.length() > 10) {
//    Serial.println(messageReceived);
//    compareMsg = messageReceived.substring(0, 16);
//    if (compareMsg == expectedMsgValve) {
//      tempVal = messageReceived.substring(17, 19);
//      Serial.println("VALUE: " + tempVal);
//      if (tempVal == "ON") {
//        setACOn();
//      } else if (tempVal == "OF") {
//        setACOff();
//      } else {
//        temperature = atoi(tempVal.c_str());
//        setACTmp(temperature);
//      }
//    }
//  }
//}

//void rec(void) {
//  int len = 30;
//  if (radio.available()) {
//    bool done = false;
//    while (!done) {
//      len = radio.getDynamicPayloadSize();
//      done = radio.read(&RecvPayload, len);
//      messageReceived = RecvPayload;
//      RecvPayload[len] = 0;
//      RecvPayload[0] = 0;
//      manageIREValue();
//      //Serial.println(messageReceived);
//      digitalWrite(8, HIGH);
//    }
//  } else {
//    RecvPayload[len] = 0;
//    RecvPayload[0] = 0;
//  }
//}

void loop() {

  //rec();
//
 
  
  setACOn();
  delay(1000);
  
  setACOff();
  delay(1000);



//  exeCommand(0);
//  delay(4000);
//
//  exeCommand(adr);
//  delay(4000);

}
