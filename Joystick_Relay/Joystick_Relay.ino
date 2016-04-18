const int joystickPin = A0;     // the number of the pushbutton pin
const int relayPin =  7;      // the number of the LED pin

// variables will change:
int joystickValue = 0;         // variable for reading the pushbutton status

void setup() {
  // initialize the relay pin as an output:
  pinMode(relayPin, OUTPUT);
  // initialize the joystick pin as an input:
  pinMode(joystickPin, INPUT);
  Serial.begin(9600);
  Serial.print("Welcome");
}

void loop() {
  // read the state of the pushbutton value:
  joystickValue = analogRead(joystickPin);
  Serial.print("AnalogRead Value = ");
  Serial.println(joystickValue);
  delay(500);
  // check if the joystick is pressed.
  // if it is, the buttonState is HIGH:
  if (joystickValue > 600) {
    // turn light on:
    digitalWrite(relayPin, HIGH);
  }
  else if(joystickValue < 512) {
    // turn light off:
    digitalWrite(relayPin, LOW);
  }
}
