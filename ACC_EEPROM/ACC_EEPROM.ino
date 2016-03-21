/*
 * Universal Air Conditioner Controller
 * Version 1.0 March 2016
 * Copyright 2015 M. Ugur Kebir
 * ugurkebir.com
 */

#include <EEPROM.h>
#include <IRremote.h>


#define maxLen 300
IRsend irsend;


volatile  unsigned int irBuffer[maxLen]; //stores timings - volatile because changed by ISR
volatile unsigned int x = 0; //Pointer thru irBuffer - volatile because changed by ISR
unsigned int result[228];
int count=0,count1=1,dongu=0,dongu1=0,y;

void setup() {
  Serial.begin(9600); //change BAUD rate as required
  pinMode(7,INPUT);
  pinMode(8,INPUT);
  attachInterrupt(0, rxIR_Interrupt_Handler, CHANGE);//set up ISR for receiving IR signal
}

void loop() {
  int butenter=digitalRead(7);
  int butsend=digitalRead(8);
  if(butenter==HIGH)
  Air();
  if(butsend==HIGH)
  AirSend();
  
}


void rxIR_Interrupt_Handler() {
  if (x > maxLen) return; //ignore if irBuffer is already full
  irBuffer[x++] = micros(); //just continually record the time-stamp of signal transitions
}


void AirSend(){
  Serial.println("Airsend");
  delay(3000);
  if(count1==1){
    dongu=0;
    Serial.println(dongu1);
  Serial.println(dongu);
     for (int i=1; i<y; i++){
      int a=EEPROM.read(i-1);
      int b=EEPROM.read(i);
      result[i-1]=a*256+b;
      }
      for (int i=1; i<y; i++){
      Serial.print(result[i-1]); 
      Serial.print(","); 
      }
      Serial.println();
      dongu+=y;
      dongu1+=y;
      count1++;
      
      irsend.sendRaw(result,228,38);
      delay(2000); 
    }
  else if(count1==2){
    Serial.println(dongu1);
  Serial.println(dongu);
    for (int i=1; i<y; i++){
      int a=EEPROM.read((dongu1*2)+(i*2)-2);
      int b=EEPROM.read((dongu1*2)+(i*2)-1);
      result[i-1]=a*256+b;
      }
      for (int i=1; i<y; i++){
      Serial.print(result[i-1]); 
      Serial.print(","); 
      }
      Serial.println();
      dongu+=y;
      dongu1+=y;
      count1++;
      irsend.sendRaw(result,228,38);
      delay(2000); }
  else if(count1==3){
    Serial.println(dongu1);
  Serial.println(dongu);
    for (int i=1; i<y; i++){
      int a=EEPROM.read((dongu1*2)+(i*2)-2);
      int b=EEPROM.read((dongu1*2)+(i*2)-1);
      result[i-1]=a*256+b;
      }
      for (int i=1; i<y; i++){
      Serial.print(result[i-1]); 
      Serial.print(","); 
      }
      Serial.println();
      dongu+=y;
      dongu1+=y;
      count1++;
      irsend.sendRaw(result,228,38);
      delay(2000); }
  else if(count1==4){
    for (int i=1; i<y; i++){
      int a=EEPROM.read((dongu*2)+(i*2)-2);
      int b=EEPROM.read((dongu*2)+(i*2)-1);
      result[i-1]=a*256+b;
      }
      for (int i=1; i<y; i++){
      Serial.print(result[i-1]); 
      Serial.print(","); 
      }
      Serial.println();
      dongu+=y;
      dongu1+=y;
      count1++;}
  else if(count1==5){
    for (int i=1; i<y; i++){
      int a=EEPROM.read((dongu*2)+(i*2)-2);
      int b=EEPROM.read((dongu*2)+(i*2)-1);
      result[i-1]=a*256+b;
      }
      for (int i=1; i<y; i++){
      Serial.print(result[i-1]); 
      Serial.print(","); 
      }
      Serial.println();
      dongu+=y;
      dongu1+=y;
      count1++;}
  else if(count1==6){
    for (int i=1; i<y; i++){
      int a=EEPROM.read((dongu*2)+(i*2)-2);
      int b=EEPROM.read((dongu*2)+(i*2)-1);
      result[i-1]=a*256+b;
      }
      for (int i=1; i<y; i++){
      Serial.print(result[i-1]); 
      Serial.print(","); 
      }
      Serial.println();
      dongu+=y;
      dongu1+=y;
      count1++;}
  else if(count1==7){
    for (int i=1; i<y; i++){
      int a=EEPROM.read((dongu*2)+(i*2)-2);
      int b=EEPROM.read((dongu*2)+(i*2)-1);
      result[i-1]=a*256+b;
      }
      for (int i=1; i<y; i++){
      Serial.print(result[i-1]); 
      Serial.print(","); 
      }
      Serial.println();
      dongu+=y;
      dongu1+=y;
      count1++;}
  else if(count1==8){
    for (int i=1; i<y; i++){
      int a=EEPROM.read((dongu*2)+(i*2)-2);
      int b=EEPROM.read((dongu*2)+(i*2)-1);
      result[i-1]=a*256+b;
      }
      for (int i=1; i<y; i++){
      Serial.print(result[i-1]); 
      Serial.print(","); 
      }
      Serial.println();
      dongu+=y;
      dongu1+=y;
      count1++;}
  else if(count1==9){
    for (int i=1; i<y; i++){
      int a=EEPROM.read((dongu*2)+(i*2)-2);
      int b=EEPROM.read((dongu*2)+(i*2)-1);
      result[i-1]=a*256+b;
      }
      for (int i=1; i<y; i++){
      Serial.print(result[i-1]); 
      Serial.print(","); 
      }
      Serial.println();
      dongu+=y;
      dongu1+=y;
      count1++;}
  else if(count1==10){
    for (int i=1; i<y; i++){
      int a=EEPROM.read((dongu*2)+(i*2)-2);
      int b=EEPROM.read((dongu*2)+(i*2)-1);
      result[i-1]=a*256+b;
      }
      for (int i=1; i<y; i++){
      Serial.print(result[i-1]); 
      Serial.print(","); 
      }
      Serial.println();
      dongu+=y;
      dongu1+=y;
      count1++;}
  else if(count1==11){
    for (int i=1; i<y; i++){
      int a=EEPROM.read((dongu*2)+(i*2)-2);
      int b=EEPROM.read((dongu*2)+(i*2)-1);
      result[i-1]=a*256+b;
      }
      for (int i=1; i<y; i++){
      Serial.print(result[i-1]); 
      Serial.print(","); 
      }
      Serial.println();
      dongu+=y;
      dongu1+=y;
      count1++;}
  else if(count1==12){
    for (int i=1; i<y; i++){
      int a=EEPROM.read((dongu*2)+(i*2)-2);
      int b=EEPROM.read((dongu*2)+(i*2)-1);
      result[i-1]=a*256+b;
      }
      for (int i=1; i<y; i++){
      Serial.print(result[i-1]); 
      Serial.print(","); 
      }
      Serial.println();
      dongu+=y;
      dongu1+=y;
      count1++;}
  else if(count1==13){
    for (int i=1; i<y; i++){
      int a=EEPROM.read((dongu*2)+(i*2)-2);
      int b=EEPROM.read((dongu*2)+(i*2)-1);
      result[i-1]=a*256+b;
      }
      for (int i=1; i<y; i++){
      Serial.print(result[i-1]); 
      Serial.print(","); 
      }
      Serial.println();
      dongu+=y;
      dongu1+=y;
      count1++;}
  else if(count1==14){
    for (int i=1; i<y; i++){
      int a=EEPROM.read((dongu*2)+(i*2)-2);
      int b=EEPROM.read((dongu*2)+(i*2)-1);
      result[i-1]=a*256+b;
      }
      for (int i=1; i<y; i++){
      Serial.print(result[i-1]); 
      Serial.print(","); 
      }
      Serial.println();
      dongu+=y;
      dongu1+=y;
      count1=0;}
  
 // irsend.sendRaw(result,228,38);
 // delay(2000); 
  }



void Air(){
  if(count==0){
  Serial.println(F("Press 18 Degrees-On Button"));
  count++;}
  else if(count==1){
  Serial.println(F("Press 19 Degrees Button"));
  count++;}
  else if(count==2){
  Serial.println(F("Press 20 Degrees Button"));
  count++;}
  else if(count==3){
  Serial.println(F("Press 21 Degrees Button"));
  count++;}
  else if(count==4){
  Serial.println(F("Press 22 Degrees Button"));
  count++;}
  else if(count==5){
  Serial.println(F("Press 23 Degrees Button"));
  count++;}
  else if(count==6){
  Serial.println(F("Press 24 Degrees Button"));
  count++;}
  else if(count==7){
  Serial.println(F("Press 25 Degrees Button"));
  count++;}
  else if(count==8){
  Serial.println(F("Press 26 Degrees Button"));
  count++;}
  else if(count==9){
  Serial.println(F("Press 27 Degrees Button"));
  count++;}
  else if(count==10){
  Serial.println(F("Press 28 Degrees Button"));
  count++;}
  else if(count==11){
  Serial.println(F("Press 29 Degrees Button"));
  count++;}
  else if(count==12){
  Serial.println(F("Press 30 Degrees Button"));
  count++;}
  else if(count==13){
  Serial.println(F("Press OFF Button"));
  count=0;}
  else{
    count=0;
    }
  
  delay(5000); // pause 5 secs
  if (x) { //if a signal is captured
    detachInterrupt(0);//stop interrupts & capture until finshed here
    for (int i = 1; i < x; i++) { //now dump the times
      Serial.print(irBuffer[i] - irBuffer[i - 1]);
      Serial.print(F(","));
    }
    
    Serial.println();
    //writing on EEProm
    for (int i=1; i<x; i++){
     int a=(irBuffer[i] - irBuffer[i - 1])/256;
     int b=(irBuffer[i] - irBuffer[i - 1])%256; 
     EEPROM.write((dongu*2)+(i*2)-2,a);
     EEPROM.write((dongu*2)+(i*2)-1,b);
     }
     Serial.println(dongu+(1*2)-2);
     //reading from EEPROm
     delay(1000);
//      for (int i=1; i<x; i++){
//      int a=EEPROM.read(dongu+(i*2)-2);
//      int b=EEPROM.read(dongu+(i*2)-1);
//      result[i-1]=a*256+b;
//      }
      //showing EEprom
//      for (int i=1; i<x; i++){
//      Serial.print(result[i-1]); 
//      Serial.print(","); 
//      }
    y=x;
    dongu+=x;
    x = 0;
    Serial.println();
    Serial.println();
    attachInterrupt(0, rxIR_Interrupt_Handler, CHANGE);//re-enable ISR for receiving IR signal
  }
  }
