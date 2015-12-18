#include <Timer.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <EEPROM.h>
#include <Button.h>
#include <EOSComm.h>

using namespace std;

const uint32_t DEVICE_ADDR = 0x10000000;

uint32_t intervall;

Timer t;
int receiveEvent = -1;

RF24 radio(9, 10);
EOSComm comm(radio, DEVICE_ADDR, "HW-ACT-SW");

void setSwitchState(uint8_t isOn) {
  if(isOn) {
    digitalWrite(4, HIGH);
    PORTD |= _BV(2); 
  } else {
    digitalWrite(4, LOW);
    PORTD &= ~_BV(2); 
  }
}

void setup(void)
{
  comm.begin();
  reset();
}

void reset() {
  t.stop(receiveEvent);
  receiveEvent = -1;

  uint8_t isOn = 0;
  EEPROM.get(0, isOn);
  setSwitchState(isOn);
  
  comm.hi_ho();
  receiveEvent = t.every(10, receive, (void*)0);
}

void receive(void* params_unused) {
  if (comm.readIfAvailable()) {
    if (comm.message_type() == EOSComm::MSG_C1) {
      intervall = comm.c1_listen_int();
      Serial.println("Initialized");
      comm.write_go();

      t.stop(receiveEvent);
      receiveEvent = t.every(intervall, receive, (void*)0);
    } else if (comm.message_type() == EOSComm::MSG_RS)  {
      Serial.println("received MSG_RS");
      reset();
    } else if (comm.message_type() == EOSComm::MSG_IV && comm.cm_type() == EOSComm::CM_TYPE_SW) {
      EEPROM.put(0, (uint8_t) comm.cm_sw_on());
      comm.write_ack();
    } else if (comm.message_type() == EOSComm::MSG_CM && comm.cm_type() == EOSComm::CM_TYPE_SW) {
      setSwitchState(comm.cm_sw_on());
      comm.write_ack();
    }
  }
}

void loop() {
  t.update();
}
