int fan=8;

void setup() {
  Serial.begin(9600);
  pinMode(fan,OUTPUT);
}

void loop() {
  int pos;
  if(Serial.available()){
    delay(100);
    while(Serial.available()>0){
      pos=Serial.read();
      if(pos=='0')
      digitalWrite(fan,LOW);
      else if(pos=='1')
      digitalWrite(fan,HIGH);
    }
  }
}
