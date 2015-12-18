/*created 01 December 2015 by M. Ugur Kebir
 
 modified 18 December 2015
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
#include <math.h>

#include "EmonLib.h"            
EnergyMonitor emon1;             

using namespace std;

const uint32_t DEVICE_ADDR = 0xABCDABCD;

  double watt;          
  double acvolt;   
  double current;            
  double vdrop=0;   
  int awg=16;    //cable size; 14AWG(2.08mm^2)>>15AWG(1.65mm^2)>>16AWG(1.31mm^2)>>17AWG(1.04mm^2)>>18AWG(0.823mm^2)
  int distance=10;  //distance=10 meter
  
Timer t;
int receiveEvent = -1;
int sendEvent = -1;

RF24 radio(9, 10);
EOSComm comm(radio, DEVICE_ADDR, "HW-EM");

void setup() {
  emon1.current(2, 15.45);  //ICAL for ACS712 Hall Effect-Based Linear Current Sensor IC   
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
  comm.write_sd_pa(acvolt,current,watt);
}

void loop() {
  double current_rms = emon1.calcIrms(500); 
  current = current_rms;
  
  vdrop = 0.02*current*(pow(1.26,awg-15))*distance; 
  acvolt = 230.0-vdrop;       // I measure the grid voltage (~230 Volts) help with ABAX Inverter in EOS Istanbul Office where stay in ITU Ayazaga Campus .     
  
  watt=acvolt*current; 
   
  sendData;
  t.update(); 
}

