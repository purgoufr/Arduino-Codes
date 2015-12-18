/*created 01 December 2015 by M. Ugur Kebir
 
 modified 04 December 2015
 by M. Ugur Kebir

 */

#include <Timer.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <EOSComm.h>

using namespace std;

const uint32_t DEVICE_ADDR = 0xABCDABCD;

Timer t;
int receiveEvent = -1;
int sendEvent = -1;
const int pin=A2;
double current=0;
double watt=0;
double acvolt=225.0;
double sensor_volt=0;
double current_values[20] ;
double current_max=0; 
double current_final;
int counter = 0;
int time=0;

RF24 radio(9, 10);
EOSComm comm(radio, DEVICE_ADDR, "HW-EM");

void setup() {
  pinMode(pin,INPUT);
  comm.begin();  
  reset();  
}

void reset() {
  t.stop(sendEvent);
  t.stop(receiveEvent);
  sendEvent = -1;
  receiveEvent = -1;
  comm.hi_ho();
  receiveEvent = t.every(5, receive, (void*)0);
}

void receive(void* params_unused) {
  if (comm.readIfAvailable()) {
    if (comm.message_type() == EOSComm::MSG_C1) {
      comm.write_go();

      t.stop(receiveEvent);
      receiveEvent = t.every(5, receive, (void*)0);
      t.stop(sendEvent);
      sendEvent = t.every(5,sendData,(void*)0);      
    }
    else if (comm.message_type() == EOSComm::MSG_RS)  {
      reset();
    }
  }
}

void sendData(void* params_unused) {
 comm.write_sd_pa(acvolt, current_final);
}

void loop() {
  t.update();
  if(time >= 100) {
   for(int i=0;i<=20;i++)
  {
    if (current_max < current_values[i]) {                    
    current_max = current_values[i];  
    }
  }
    current_final= current_max/1.414214;
    watt= (current_final)*acvolt;  
    sendData;   
    time = 0;    
    counter = 0;
    current_max=0;   
 }
  sensor_volt= analogRead(pin)*5.0/1023.0;
  current = (sensor_volt-2.5)/0.066;
  current_values[counter] = current;
  counter = counter+1;
  time = time+5;    
  delay(5); 
}
