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
#include <TimerOne.h>
#include <EOSComm.h>

using namespace std;

const uint32_t DEVICE_ADDR = 0x00000025;

uint32_t intervall;

Timer t;
int receiveEvent = -1;
int sendEvent = -1;

RF24 radio(9, 10);
EOSComm comm(radio, DEVICE_ADDR, "HW-ACT-DIMM-FL");

int AC_LOAD = 8;    // Output to Opto Triac pin
int dimming = 7;  // Dimming level (0-150)  0 = ON, 150 = OFF
int dimmingpin = 5;

void setup(void)
{
  comm.begin();
  
  pinMode(AC_LOAD, OUTPUT);
  pinMode(dimmingpin, OUTPUT);
  pinMode(4, OUTPUT);
  analogWrite(dimmingpin, dimming);

  reset();
}

void reset() {
  dimming = 150;
  EEPROM.get(0, dimming);
  if (dimming < 0 || dimming > 150) {
    dimming = 150;
  }
  t8_dimm();
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
    } else if (comm.message_type() == EOSComm::MSG_IV && comm.cm_type() == EOSComm::CM_TYPE_ID) {
      EEPROM.put(0, (int) comm.cm_id_dimming_level());
      comm.write_ack();
    } else if (comm.message_type() == EOSComm::MSG_CM && comm.cm_type() == EOSComm::CM_TYPE_ID) {
      dimming = comm.cm_id_dimming_level();
      t8_dimm();
      comm.write_ack();
    }
  }
}

void t8_dimm()  // function dimms the t8 flourecent lights
{
  if (dimming == 150) {
    digitalWrite(AC_LOAD, LOW);
  } else {
    digitalWrite(AC_LOAD, HIGH);
    analogWrite(dimmingpin, 255 - 1.7 * dimming);
  }
}

void loop() {
  t.update();
}
