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

// Create a new DHT class.
dht11 DHT11;

// Random ID Generation and String assignment.
long roomIDRandom = 1107;
String randomID = String(roomIDRandom);

// Room Identity. Will be saved to EEPROM at initialization() and read from there
// in case of a reset to see if this device already is a part of the system or an
// newly installed device waiting to be assigned to a room.
int roomIdentity = 1000;
int pipeNo;

// EEPROM address that holds the Room Number.
int addr = 0;

// Shortened by one character 20/3/2015
const String hardwareId = "EOS001M" + randomID;

// Sensor Related variables.
const int sensingPower = 7;
const int ldrAnalogPin = A0;
const int sendingCheckPin = 4;
const int motionSensorOut = 5;
int delayTime = 4;

boolean requestAnswered = false;
boolean feedbackReceived = false;
boolean different = true;

// Light related variables.
int ldr_value;
int lastTemp = 0;
int lastLum = 0;
int lum;

// RF CE and CSN pin assignment.
EOSCommunication comm(9, 10, sendingCheckPin);

void setup(void)
{
  EEPROM.write(addr, 50);
  
  Serial.begin(9600);
  Serial.print("Data at adress 0x00 is: ");
  Serial.println(EEPROM.read(addr));
  Serial.println("This is my room ID and the pipe # I will use to send data to the gateway.");
  
  pinMode(motionSensorOut,INPUT);
  pinMode(ldrAnalogPin,INPUT);
  
  comm.begin();
  comm.setWritingPipe(1);
  comm.setReadingPipe(0);
  
  if (EEPROM.read(addr)==50){
    idDeclaration();
    assignRoom();
    
  }
  
}

void idDeclaration(){
  
  Serial.println("Initializing device..");
  Serial.println("Waiting for Gateway initialization complete message");
  
  String messageReceived = "";
   
  while(messageReceived != randomID + "RECOGNIZED") {
    comm.transmit("HELLO|" + hardwareId);
    delay(20);
    messageReceived = comm.receive();
    delay(30);
      if (messageReceived == randomID + "RECOGNIZED"){
        Serial.println("Device recognized by the Gateway.");
      }
  }
}

void assignRoom(){
  
    Serial.println("Waiting for Room Assignment");
    String messageReceived = comm.receive();
    
    while (messageReceived.substring(0,12) != randomID + randomID + "Room")
    {
      messageReceived = comm.receive();
      delay(10);
    }
    
    Serial.println("Room Assignment Received");
    Serial.println("Transmitting ACK message to the Gateway");
    
    pipeNo = (messageReceived.substring(12,14).toInt())%6;
    
    while (messageReceived != "ACKREC" + randomID){
      comm.transmit("ACK" + randomID);
      delay(5);
      messageReceived = comm.receive();
      delay(5);
    }
    
    Serial.println("ACK message successfully sent");
    Serial.println("Beginning normal mode of operation");
        
    EEPROM.write(addr,pipeNo);
    Serial.println("Initialization complete.");
    
}

String sens(void){
  
  int chk = DHT11.read(DHT11PIN);
  ldr_value = analogRead(ldrAnalogPin);          //reads the LDR values
  lum = ldr_value;
  if(lum < 0){
    lum = 0;
  }
  digitalWrite(sensingPower, LOW);

  int temp = DHT11.temperature;

  if((temp != lastTemp) || ((lastLum + 10)<lum) || ((lastLum - 10)>lum)){
    different = true;
  } else {
    different = false;
  }

  String messageToSend = "";
  messageToSend = "SV|";
  messageToSend += hardwareId;
   
  messageToSend = messageToSend + "|T=";
  if(DHT11.temperature < 10) {
    messageToSend += "0";
  }
  messageToSend += DHT11.temperature;
  lastTemp = DHT11.temperature;
  lastLum = lum;
  messageToSend = messageToSend + "|";
  messageToSend = messageToSend + "H=";
  if(DHT11.humidity < 10) {
    messageToSend += "0";
  }
  messageToSend += DHT11.humidity;
  messageToSend = messageToSend + "|";

  messageToSend = messageToSend + "L=";
  messageToSend += lum;
  messageToSend = messageToSend + "|";
  Serial.println(messageToSend);
  
  return messageToSend;
}

void loop(void){
  
  int workingPipe = EEPROM.read(addr);
  comm.setWritingPipe(workingPipe);
  comm.transmit(sens());
  delay(500); 
  
}
