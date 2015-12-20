/*
Created by Mehmet Ugur Kebir
Date: 20/12/2015
*/
const int buttonPin = 6;    // the number of the pushbutton pin
const int ledPin = 13;      // the number of the LED pin

int lastButtonState = LOW;

double pressTime = 0;
double duration=0;;

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
}

void led_on_off_1second(){        //1 saniyelik yanıp-sönme
  digitalWrite(ledPin, HIGH);   
  delay(1000);           
  digitalWrite(ledPin, LOW);    
  delay(1000);      
}

void led_on_off_2second(){        //2 saniyelik yanıp-sönme
  digitalWrite(ledPin, HIGH);   
  delay(2000);              
  digitalWrite(ledPin, LOW); 
  delay(2000);    
}

void loop() {   
  int reading = digitalRead(buttonPin);
  if(reading == HIGH && lastButtonState == LOW) { //button basıldığındaki anı yakala
    pressTime = millis();
    lastButtonState = HIGH;
  } 
    else if(reading == LOW && lastButtonState == HIGH) {  //button çekildiğindeki an ile button basıldığı andaki süre farkını al  
    duration = abs(pressTime-millis());
         
    Serial.println(duration);
    if(duration > 100.0 && duration < 8000.0) {    //real time daki 1 sn butona basma  
    Serial.print("ok3\n");
      for(int i=0;i<2;i++){       //2 kere yanıp-sönme
      led_on_off_1second();
      delay(i);
    }
      for(int i=0;i<2;i++){
      led_on_off_2second();
      delay(i);
    } 
   }
      else if(duration >= 8000.0 ) {       //real time daki 2 sn veya daha uzun süre butona basma  
      Serial.print("ok4\n");
      for(int i=0; i<4;i++){      //4 kere yanıp-sönme
       led_on_off_1second();
      delay(i);
      }
      for(int i=0; i<4;i++){
      led_on_off_2second();
      delay(i);
      }
    }    
       lastButtonState = LOW;
       pressTime = 0;
  }  
}
