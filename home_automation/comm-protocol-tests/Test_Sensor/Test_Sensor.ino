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

String messageReceived = "";

String test = "100";

EOSCommunication comm(9, 10, -1);

void setup(void)
{
  Serial.begin(9600);
  comm.begin();
  comm.setWritingPipe(1);
  comm.setReadingPipe(0);

}

void loop(void){
  
  while (messageReceived != "test") {
    delay(20);
    messageReceived =  comm.receive();
    Serial.println("keine Anfrage empfangen");
    delay(2000);
    
    
    
  }
    Serial.println("Anfrage empfangen");  
    delay (1000);
    if (messageReceived == "test"){
      Serial.println("Daten werden ausgelesen");
    }
    comm.transmit(test);
      
    delay (5000);
    Serial.println("Daten gesendet");
    messageReceived = "ss";
}

      
