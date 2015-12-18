#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Button.h> 

const int currentPin = 5;
const unsigned long sampleTime = 100000UL;                           // sample over 100ms, it is an exact number of cycles for both 50Hz and 60Hz mains
const unsigned long numSamples = 250UL;                               // choose the number of samples to divide sampleTime exactly, but low enough for the ADC to keep up
const unsigned long sampleInterval = sampleTime/numSamples;  // the sampling interval, must be longer than then ADC conversion time
const int adc_zero = 510;                                                     // relative digital zero of the arudino input from ACS712 (could make this a variable and auto-adjust it)

const String hardwareId = "EOS001D000001";

const String valveOrderTag = "DO";

char RecvPayload[31] = "";
char SendPayload[31] = "";

String messageToSend = "";

long resStart;

Button button7 = Button(7,PULLDOWN);
Button button6 = Button(6,PULLDOWN);
Button button5 = Button(5,PULLDOWN);
Button button4 = Button(4,PULLDOWN);
Button button3 = Button(3,PULLDOWN);

RF24 radio(9,10);

const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

void setup(void){
  
  //Serial.begin(9600);
  
  pinMode(1, INPUT);
  
  radio.begin();
  radio.setRetries(15,15);
  radio.setDataRate(RF24_1MBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setCRCLength(RF24_CRC_16);
  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,pipes[0]);
}

void transmit(void){
    char *buf = (char*) malloc(sizeof(char)*messageToSend.length()+1);
  
    messageToSend.toCharArray(buf, messageToSend.length() + 1);
    bool ok = radio.write( buf, messageToSend.length() + 1 );
   
    free(buf);
    messageToSend = "";
}


void loop(){
  
   messageToSend = "EO|EOS001E000001|50|";

   transmit();

}





