#include <Timer.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <EEPROM.h>
#include <avr/wdt.h>
#include <EOSComm.h>

using namespace std;


const uint32_t DEVICE_ADDR = 0x00000055;

Timer t;
int receiveEvent = -1;
int sendEvent = -1;

RF24 radio(9, 10);
EOSComm comm(radio, DEVICE_ADDR, "HW-CM");

// Electrical value variables.
int voltage[50];
double Rburden = 1000.000;
int maximumVoltage = 0;
int minimumVoltage = 2000;

// Total value and number of sample count
// for above and below the treshold.
long totaltop = 0;
int counttop = 0;
long totalbot = 0;
int countbot = 0;

// Average values for below/above treshold
int averagetop = 0;
int averagebot = 0;

float power;
float averagePower;

void setup(void)
{
  pinMode(A0, INPUT);
  //wdt_enable(WDTO_8S);
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
      sendEvent = t.every(comm.c1_sync_int(), sendData, (void*)0);

    } else if (comm.message_type() == EOSComm::MSG_RS)  {
      Serial.println("received MSG_RS");
      reset();
    }
  }
}

void sendData(void* params_unused) {
  callculationpower();
  power = averagePower;
  comm.write_sd_cm(power);
}

int getVoltage(int voltageValues[]) {
  //get 50 voltage samples
  for (int x = 0; x < 50; x++) {
    voltageValues[x] = analogRead(A0);
    delay(10);
  }
}

int getmaxminVoltages(int voltageValues[]) {
  for (int k = 0; k < 50; k++) {
    if (voltageValues[k] >= maximumVoltage) {
      maximumVoltage = voltageValues[k];
    }
    if (voltageValues[k] < minimumVoltage) {
      minimumVoltage = voltageValues[k];
    }
  }
}

int gettopaverageVoltage(int voltageValues[], int halfpoint) {
  for (int k = 0; k < 50; k++) {
    if (voltageValues[k] >= halfpoint) {
      totaltop = totaltop + voltageValues[k];
      counttop++;
    }
  }
  averagetop = totaltop / counttop;
}

int getbotaverageVoltage(int voltageValues[], int halfpoint) {
  for (int k = 0; k < 50; k++) {
    if (voltageValues[k] < halfpoint) {
      totalbot = totalbot + voltageValues[k];
      countbot++;
    }
  }
  averagebot = totalbot / countbot;
}

void callculationpower() {
  int voltageArray[50];
  getVoltage(voltageArray);
  getmaxminVoltages(voltageArray);

  int peaktopeak = (maximumVoltage - minimumVoltage);
  int halfpeaktopeak = (peaktopeak / 2);
  int halfpoint = (minimumVoltage + halfpeaktopeak);
  int averagetopRelative = (averagetop - halfpoint);

  double actualvoltsAverage = (averagetopRelative * 5.000 / 1024.000);
  double averagetoploadCurrent = (averagetopRelative * 3600.000 * 5.000 / 1024.000 / 220.000);

  getbotaverageVoltage(voltageArray, halfpoint);
  int averagebotRelative = (halfpoint - averagebot);

  double actualvoltsbotAverage = (averagebotRelative * 5.000 / 1024.000);
  double averagebotloadCurrent = (averagebotRelative * 3600.000 * 5.000 / 1024.000 / 220.000);

  double averageCurrent = (averagebotloadCurrent + averagetoploadCurrent) / 2;
  float averagePower = averageCurrent * 220;
}

void loop(void) {
  t.update();
}


