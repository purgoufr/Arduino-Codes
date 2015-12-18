#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

using namespace std;

const String hardwareId = "FO|EOS001F00001|";

const int ALARM_LED = 7;
const int ALARM_BUZZER = 6;
const int DETECTOR_DIGITAL = 2;

String messageToSend = "";
String messageReceived = "";
String connectionKey = "";
char RecvPayload[31] = "";
char SendPayload[31] = "";

RF24 radio(9,10);

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[6] = {0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL, 0xF0F0F0F0E3LL, 0xF0F0F0F0E4LL, 0xF0F0F0F0E5LL, 0xF0F0F0F0E6LL};

void transmit(void){

  char *buf = (char*) malloc(sizeof(char)*messageToSend.length() + 1);
  Serial.println(messageToSend);
  
  messageToSend.toCharArray(buf, messageToSend.length() + 1);

  radio.stopListening();

  bool ok = radio.write(buf, messageToSend.length() + 1);

  if(ok){
  }

  free(buf);

  messageToSend = "";
  radio.startListening();
}

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(ALARM_LED, OUTPUT);
  pinMode(DETECTOR_DIGITAL, INPUT);
  
  radio.begin();
  radio.setRetries(15,15);
  radio.setDataRate(RF24_1MBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setCRCLength(RF24_CRC_16);

  radio.openWritingPipe(pipes[1]);
  
  radio.stopListening();
}

// the loop function runs over and over again forever
void loop() {
  // read smoke / gas detector: 1 if no alarm, 0 if alarm
  boolean alarm = digitalRead(DETECTOR_DIGITAL) == 0;
  
  // light alarm LED when fire is detected
  digitalWrite(ALARM_LED, alarm);
  
  // beep alarm buzzer when fire is detected
  digitalWrite(ALARM_BUZZER, alarm);
  
  // send current state to gateway
  String stateDescription = alarm ? "ON" : "OF";
    
  messageToSend = hardwareId + stateDescription + "|";
  for(int i = 0; i < 5; i++) {
    transmit();
    delay(50);
  }
}
