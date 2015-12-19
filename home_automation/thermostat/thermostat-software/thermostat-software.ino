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
#include <Stepper.h>

using namespace std;

const uint32_t DEVICE_ADDR = 0x00100000;

const int LED_INIT_1 = -1;
const int LED_INIT_2 = -2;
const int LED_INIT_3 = -3;
const int LED_INIT_4 = -4;

uint32_t intervall;

Timer t;
int receiveEvent = -1;
int sendEvent = -1;
int moveMotorEvent = -1;

RF24 radio(9, 10);
EOSComm comm(radio, DEVICE_ADDR, "HW-ACT-THERM");

// MOTOR Pins and Arduino correspondents.
int in1Pin = 5;//12;
int in2Pin = 4;//11;
int in3Pin = 3;//10;
int in4Pin = 2;//9;

// Button one is for movement towards the motor.
Button button1 = Button(6, PULLDOWN);
// Button two is for movement away from the motor.
Button button2 = Button(7, PULLDOWN);

// Stepper motor values and pin setup.
Stepper motor(48, in1Pin, in2Pin, in3Pin, in4Pin);
int motorValue = 0;  // 3 steps
int currentValue = 0;  // 3 steps

int curLevel;
int targetLevel;

void setup(void)
{
  //Pin initialization for LEDs
  pinMode(A5, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A1, OUTPUT);

  // Pin initialization for Stepper Motor
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(in3Pin, OUTPUT);
  pinMode(in4Pin, OUTPUT);

  // Light LEDs on power up
  digitalWrite(A5, HIGH);
  digitalWrite(A4, HIGH);
  digitalWrite(A3, HIGH);
  digitalWrite(A2, HIGH);
  digitalWrite(A1, HIGH);

  // Motor speed and steps
  motor.setSpeed(900);
  comm.begin();
  reset();
}

void reset() {
  curLevel = 0;
  setLEDLevel(LED_INIT_1);
  motor.step(1 * 480);
  setLEDLevel(LED_INIT_2);
  motor.step(1 * 480);
  setLEDLevel(LED_INIT_3);
  motor.step(1 * 480);
  setLEDLevel(LED_INIT_2);
  motor.step(1 * 480);
  setLEDLevel(LED_INIT_1);
  motor.step(1 * 480);
  setLEDLevel(LED_INIT_4);
  setPinsLow();
  t.stop(sendEvent);
  t.stop(receiveEvent);
  sendEvent = -1;
  receiveEvent = -1;
  comm.hi_ho();
  setLEDLevel(0);
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

    } else if (comm.message_type() == EOSComm::MSG_CM && comm.cm_type() == EOSComm::CM_TYPE_TH) {
      // Send ACK before moving the motor, since that is a blocking operation!
      targetLevel = comm.cm_th_valve_level();
      comm.write_ack();
      if(moveMotorEvent == -1) {
        moveMotorEvent = t.every(1, movemotor, (void*) 0);
      }
    }
  }
}

void movemotor(void* unused) {
  int change = (curLevel - targetLevel);
  if (change != 0) {
    int change_direction = change / abs(change);
    int single_change = 1 * change_direction;
    motor.step(single_change * 48);
    curLevel -= single_change;
    
    setLEDLevel(curLevel / 5);
    
    if (curLevel == targetLevel && targetLevel == 0) {
      motor.step(3 * 48);
    }
  }

  if(curLevel == targetLevel) {
    setPinsLow();
    t.stop(moveMotorEvent);
    moveMotorEvent = -1;
  }
}

// Function which sets the motor pins low
void setPinsLow() {
  digitalWrite(in1Pin, LOW);
  digitalWrite(in2Pin, LOW);
  digitalWrite(in3Pin, LOW);
  digitalWrite(in4Pin, LOW);
}

void setLEDLevel(int lvl) {

  switch (lvl) {
    case LED_INIT_1:
      digitalWrite(A5, LOW);
      digitalWrite(A4, HIGH);
      digitalWrite(A3, HIGH);
      digitalWrite(A2, HIGH);
      digitalWrite(A1, LOW);
      break;
    case LED_INIT_2:
      digitalWrite(A5, HIGH);
      digitalWrite(A4, LOW);
      digitalWrite(A3, HIGH);
      digitalWrite(A2, LOW);
      digitalWrite(A1, HIGH);
      break;
    case LED_INIT_3:
      digitalWrite(A5, HIGH);
      digitalWrite(A4, HIGH);
      digitalWrite(A3, LOW);
      digitalWrite(A2, HIGH);
      digitalWrite(A1, HIGH);
      break;
    case LED_INIT_4:
      digitalWrite(A5, LOW);
      digitalWrite(A4, HIGH);
      digitalWrite(A3, LOW);
      digitalWrite(A2, HIGH);
      digitalWrite(A1, LOW);
      break;
    case 0:
      digitalWrite(A5, HIGH);
      digitalWrite(A4, HIGH);
      digitalWrite(A3, HIGH);
      digitalWrite(A2, HIGH);
      digitalWrite(A1, HIGH);
      break;
    case 1:
      digitalWrite(A5, LOW);
      digitalWrite(A4, HIGH);
      digitalWrite(A3, HIGH);
      digitalWrite(A2, HIGH);
      digitalWrite(A1, HIGH);
      break;
    case 2:
      digitalWrite(A5, LOW);
      digitalWrite(A4, LOW);
      digitalWrite(A3, HIGH);
      digitalWrite(A2, HIGH);
      digitalWrite(A1, HIGH);
      break;
    case 3:
      digitalWrite(A5, LOW);
      digitalWrite(A4, LOW);
      digitalWrite(A3, LOW);
      digitalWrite(A2, HIGH);
      digitalWrite(A1, HIGH);
      break;
    case 4:
      digitalWrite(A5, LOW);
      digitalWrite(A4, LOW);
      digitalWrite(A3, LOW);
      digitalWrite(A2, LOW);
      digitalWrite(A1, HIGH);
      break;
    case 5:
      digitalWrite(A5, LOW);
      digitalWrite(A4, LOW);
      digitalWrite(A3, LOW);
      digitalWrite(A2, LOW);
      digitalWrite(A1, LOW);
      break;
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
