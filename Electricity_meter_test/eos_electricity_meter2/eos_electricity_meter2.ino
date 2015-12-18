/*created 01 December 2015 by M. Ugur Kebir
 
 modified 07 December 2015
 by M. Ugur Kebir

 */
/* NOTE :: There is no voltage sensor in the circuit, therefore you have to measure the grid voltage while the initial setup!!! You can measure the grid voltage with any multimeter.*/
/* NOTE :: You can get information about AWG cable size and voltage drop with this adress;  http://www.calculator.net/voltage-drop-calculator.html */

#include <Timer.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <EOSComm.h>
#include <math.h>

using namespace std;

const uint32_t DEVICE_ADDR = 0xABCDABCD;

Timer t;
int receiveEvent = -1;
int sendEvent = -1;
const int pin_clamp=A2;
double current=0;
double watt=0;
double sensor_volt=0;
double current_values[20] ;
double current_max=0; 
double current_final;
double acvolt;
double vdrop=0;   
int awg=16;    //cable size; 14AWG(2.08mm^2)>>15AWG(1.65mm^2)>>16AWG(1.31mm^2)>>17AWG(1.04mm^2)>>18AWG(0.823mm^2)
int distance=10;  //distance=10 meter
int counter = 0;
int time=0;

RF24 radio(9, 10);
EOSComm comm(radio, DEVICE_ADDR, "HW-EM");

void setup() {
  pinMode(pin_clamp,INPUT);
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
  
 while(1){
   if(time >= 100) {
   for(int i=0;i<=20;i++)
  {
     if (current_max < current_values[i]) {                    
     current_max = current_values[i];  
    }
  }    
    current_final=current_max/1.414214;    
    vdrop= 0.02*current_final*(pow(1.26,awg-15))*distance; 
    acvolt=230.0-vdrop;       // I measure the grid voltage (~230 Volts) help with ABAX Inverter in EOS Istanbul Office where stay in ITU Ayazaga Campus .
    watt= (current_final)*acvolt;          
    comm.write_sd_pa(acvolt, current_final,watt);
    delay(50);
    time = 0;    
    counter = 0;
    current_max=0;
  }
  sensor_volt= analogRead(pin_clamp)*5.0/1023.0;
  current = (sensor_volt-2.5)/0.066;
  current_values[counter] = current;
  counter = counter+1;
  time = time+5;    
  delay(5); 
  
 }
}
void loop() {
  t.update(); 
}
