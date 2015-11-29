#include <SPI.h>
#include <dht11.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <EEPROM.h>
#include <EOSCommunication.h>
#define DHT11PIN 2
using namespace std;

String messageSend = "";
String dataReceived = "";

EOSCommunication comm(9,10,-1);

void setup(void)
{
   Serial.begin(9600);
   comm.begin();
   comm.setWritingPipe(0);
   comm.setReadingPipe(1);
   
}

void loop(void) {
  
  delay (5000);
  
  comm.transmit("test");
  
  while (dataReceived == "") {
  dataReceived = comm.receive();
  Serial.println("Sende Anfrage");
  delay(2000);
  }
  
  delay (1000);
  
  Serial.println("Daten werden gesendet");
  if (dataReceived != "") {
    
    delay (1000);
    Serial.println("Daten erfolgreich ausgelesen");
  }
  delay (5000);
  
  dataReceived = "";
  
  comm.transmit("nodata");
 
 }


  
  
