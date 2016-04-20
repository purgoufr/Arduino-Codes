/*created 10 February 2016 by M. Ugur Kebir
* 
* modified 10 February 2016
* by M. Ugur Kebir
* 
* Web : ugurkebir.com
*
*Firstly add libraries correctly then compile and upload the code.
*/

#include <SPI.h>
#include <nRF24L01p.h>
#include <LiquidCrystal.h>
#include "printf.h"
#include <stdlib.h>
#include <stdio.h>      /* printf, NULL */

LiquidCrystal lcd(10, 9, 5, 4, 3, 2);

nRF24L01p receiver(7,8);
/* CSN - > 7, CE -> 8 defined */

void setup(){
  Serial.begin(115200);
  lcd.begin(16, 2);
    
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  /* SPI initialized */
  receiver.channel(80);
  receiver.RXaddress("Ugur");
  receiver.init();
  /* Receiver settings done */
}

String temp_variable;
String hum_variable;

void loop(){ 
  while(receiver.available()){
    receiver.read();
    receiver.rxPL(temp_variable);
    delay(10);  
    receiver.rxPL(hum_variable); 
    
    /* Temperature and humidity datas received*/
    if(temp_variable.length()>0)
    {
      Serial.print("temperature = ");
      Serial.println(temp_variable);
      
      Serial.print("humidity = ");
      Serial.println(hum_variable);

       lcd.clear();
       lcd.setCursor(0,0);
       lcd.print("Humidity = ");
       lcd.print(hum_variable);
       lcd.setCursor(0,1);
       lcd.print("Temp = ");
       lcd.print(temp_variable);

      temp_variable="";
      hum_variable="";
      /* old datas deleted */
   }
  }
}
