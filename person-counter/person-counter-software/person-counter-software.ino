#include <Timer.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <Button.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <TimerOne.h>
#include <math.h>
#include <EEPROM.h>
#include <EOSComm.h>

using namespace std;

const int SEVEN_SEG_H = 10;
const int SEVEN_SEG_MINUS = -1;

const uint32_t DEVICE_ADDR = 0x00000069;

uint32_t intervall;

Timer t;
int receiveEvent = -1;
int sendEvent = -1;

RF24 radio(9, 10);
EOSComm comm(radio, DEVICE_ADDR, "HW-PC");

//PIN ASSIGNMENT
const int insidePin  = 5;
const int outsidePin = 4;
const int outputPin = 12;
const int outPinLed = 13;

// Variables for Counting
int prevState = 0;
int count = 0;

//Buttons
Button increaseButton = Button (6, PULLDOWN);
Button decreaseButton = Button (7, PULLDOWN);

void setup(void) {
  sevenSegWrite(SEVEN_SEG_MINUS);

  comm.begin();

  // initialize the inside&outside pin as an input:
  pinMode(insidePin, INPUT);
  pinMode(outsidePin, INPUT);

  pinMode(outputPin, OUTPUT);
  pinMode(outPinLed, OUTPUT);

  pinMode(A1, OUTPUT);
  digitalWrite(A1, HIGH);

  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);

  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);

  reset();
}

void reset() {
  t.stop(sendEvent);
  t.stop(receiveEvent);
  sendEvent = -1;
  receiveEvent = -1;

  count = 0;

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
    } else if (comm.message_type() == EOSComm::MSG_EA && comm.ed_type() == EOSComm::ED_TYPE_PC) {
      if (comm.ed_pc_person_count() == count && sendEvent != -1) {
        t.stop(sendEvent);
        sendEvent = -1;
      } else if (comm.ed_pc_person_count() != count && sendEvent == -1) {
        sendEvent = t.every(75, sendData, (void*)0);
      }
    }
  }
}

void sendData(void* params_unuesd) {
  comm.write_ed_pc(count);
}

void crossing() {
  int prevCount = count;

  // read the state of the pushbutton value:
  int insideState = digitalRead(insidePin);
  int outsideState = digitalRead(outsidePin);

  // Passaging
  if (prevState == 0) {
    if (insideState == LOW && outsideState == HIGH) {
      prevState = 1;
    }
    if (insideState == HIGH && outsideState == LOW) {
      prevState = -1;
    }
  }
  if (prevState == 1) {
    if (insideState == LOW && outsideState == LOW) {
      prevState = 2;
    }
  }
  if (prevState == -1) {
    if (insideState == LOW && outsideState == LOW) {
      prevState = -2;
    }
  }
  if (prevState == 2) {
    if (insideState == HIGH && outsideState == LOW) {
      prevState = 3;
    }
  }
  if (prevState == -2) {
    if (insideState == LOW && outsideState == HIGH) {
      prevState = -3;
    }
  }
  if (prevState == 3) {
    if (insideState == HIGH && outsideState == HIGH) {
      prevState = 0;

      count--;
    }
  }
  if (prevState == -3) {
    if (insideState == HIGH && outsideState == HIGH) {
      prevState = 0;
      count++;
    }
  }

  if (insideState == HIGH && outsideState == HIGH) {
    prevState = 0;
  }

  if (increaseButton.uniquePress()) {
    count++;
  }

  if (decreaseButton.uniquePress()) {
    count--;
  }

  if (count < 0) {
    count = 0;
  }

  if (count != prevCount) {
    if (sendEvent == -1) {
      sendEvent = t.every(75, sendData, (void*)0);
    }
    sendData(NULL);
  }
  sevenSegShowCount();
}

void sevenSegShowCount() {
  if (count < 10) {
    sevenSegWrite(count);
  } else {
    sevenSegWrite(SEVEN_SEG_H);
  }
}

void sevenSegWrite(int digit) { //Digitial Display

  switch (digit) {

    case 0 :
      digitalWrite(A2, HIGH);
      digitalWrite(A3, LOW);
      digitalWrite(A4, LOW);
      digitalWrite(A5, LOW);
      digitalWrite(1, LOW);
      digitalWrite(2, LOW);
      digitalWrite(3, LOW);
      break;
    case 1 :
      digitalWrite(A2, HIGH);
      digitalWrite(A3, HIGH);
      digitalWrite(A4, HIGH);
      digitalWrite(A5, LOW);
      digitalWrite(1, HIGH);
      digitalWrite(2, HIGH);
      digitalWrite(3, LOW);
      break;
    case 2 :
      digitalWrite(A2, LOW);
      digitalWrite(A3, HIGH);
      digitalWrite(A4, LOW);
      digitalWrite(A5, LOW);
      digitalWrite(1, LOW);
      digitalWrite(2, LOW);
      digitalWrite(3, HIGH);
      break;
    case 3 :
      digitalWrite(A2, LOW);
      digitalWrite(A3, HIGH);
      digitalWrite(A4, LOW);
      digitalWrite(A5, LOW);
      digitalWrite(1, HIGH);
      digitalWrite(2, LOW);
      digitalWrite(3, LOW);
      break;
    case 4 :
      digitalWrite(A2, LOW);
      digitalWrite(A3, LOW);
      digitalWrite(A4, HIGH);
      digitalWrite(A5, LOW);
      digitalWrite(1, HIGH);
      digitalWrite(2, HIGH);
      digitalWrite(3, LOW);
      break;
    case 5 :
      digitalWrite(A2, LOW);
      digitalWrite(A3, LOW);
      digitalWrite(A4, LOW);
      digitalWrite(A5, HIGH);
      digitalWrite(1, HIGH);
      digitalWrite(2, LOW);
      digitalWrite(3, LOW);
      break;
    case 6 :
      digitalWrite(A2, LOW);
      digitalWrite(A3, LOW);
      digitalWrite(A4, LOW);
      digitalWrite(A5, HIGH);
      digitalWrite(1, LOW);
      digitalWrite(2, LOW);
      digitalWrite(3, LOW);
      break;
    case 7 :
      digitalWrite(A2, HIGH);
      digitalWrite(A3, HIGH);
      digitalWrite(A4, LOW);
      digitalWrite(A5, LOW);
      digitalWrite(1, HIGH);
      digitalWrite(2, HIGH);
      digitalWrite(3, LOW);
      break;
    case 8 :
      digitalWrite(A2, LOW);
      digitalWrite(A3, LOW);
      digitalWrite(A4, LOW);
      digitalWrite(A5, LOW);
      digitalWrite(1, LOW);
      digitalWrite(2, LOW);
      digitalWrite(3, LOW);
      break;
    case 9 :
      digitalWrite(A2, LOW);
      digitalWrite(A3, LOW);
      digitalWrite(A4, LOW);
      digitalWrite(A5, LOW);
      digitalWrite(1, HIGH);
      digitalWrite(2, LOW);
      digitalWrite(3, LOW);
      break;
    case SEVEN_SEG_H:
      digitalWrite(A2, LOW);
      digitalWrite(A3, LOW);
      digitalWrite(A4, HIGH);
      digitalWrite(A5, LOW);
      digitalWrite(1, LOW);
      digitalWrite(2, HIGH);
      digitalWrite(3, LOW);
      break;
    case SEVEN_SEG_MINUS:
      digitalWrite(A2, LOW);
      digitalWrite(A3, HIGH);
      digitalWrite(A4, HIGH);
      digitalWrite(A5, HIGH);
      digitalWrite(1, HIGH);
      digitalWrite(2, HIGH);
      digitalWrite(3, HIGH);
      break;
  }
}

void loop(void) {
  crossing();
  t.update();
}

