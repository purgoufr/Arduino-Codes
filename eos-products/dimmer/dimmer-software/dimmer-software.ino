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

const uint32_t DEVICE_ADDR = 0x00001000;

uint32_t intervall;

Timer t;
int receiveEvent = -1;
int sendEvent = -1;

RF24 radio(9, 10);
EOSComm comm(radio, DEVICE_ADDR, "HW-ACT-DIMM");

int AC_LOAD = 3;    // Output to Opto Triac pin
int dimming = 150;  // Dimming level (0-150)  0 = ON, 150 = OFF

Button button1 = Button(6, PULLDOWN);
Button button2 = Button(7, PULLDOWN);

boolean dimOn = false;
boolean dimOff = false;

volatile int i = 0;             // Variable to use as a counter
volatile boolean zero_cross = 0; // Boolean to store a "switch" to tell us if we have crossed zero

void setup(void)
{
  comm.begin();
  dimming = 150;
  EEPROM.get(0, dimming);
  if (dimming < 0 || dimming > 150) {
    dimming = 150;
  }
  String text = "EEPROM ";
  Serial.println(text + dimming);
  pinMode(AC_LOAD, OUTPUT);
  attachInterrupt(0, zero_crosss_int, RISING);
  Timer1.initialize();                      // Initialize TimerOne library for the freq we need
  Timer1.attachInterrupt(dim_check, 50);

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
      dimOn = false;
      dimOff = false;
      dimming = comm.cm_id_dimming_level();
      comm.write_ack();
    }
  }
}

  void zero_crosss_int() {
    zero_cross = true;
    i = 0;
    if (dimming > 0) {
      digitalWrite(AC_LOAD, LOW);
    }
  }

  void dim_check() {
    if (zero_cross == true) {
      if (i >= dimming) {
        if (dimming < 150) {
          digitalWrite(AC_LOAD, HIGH); // turn on light
        }
        i = 0; // reset time step counter
        zero_cross = false; //reset zero cross detection
      }
      else {
        i++; // increment time step counter
      }
    }
  }

  // manual dimming up
  void buttonDimmUp() {
    if (dimming == 90) {
      dimOn = true;
      dimOff = false;
    }
    else {
      dimming += 10;
      dimOn = false;
      dimOff = false;
    }
  }

  // manual dimming down
  void buttonDimmDown() {
    if (dimming == 10) {
      dimOn = false;
      dimOff = true;
    }
    else {
      dimming -= 10;
      dimOn = false;
      dimOff = false;
    }
  }

  void loop() {
    t.update();
    //  if(button1.uniquePress()){
    //    buttonDimmUp();
    //  }
    //  if(button2.uniquePress()){
    //    buttonDimmDown();
    //  }
    //
  }
