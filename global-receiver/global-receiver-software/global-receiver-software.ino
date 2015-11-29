#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <EOSCommunication.h>

using namespace std;

String MESSAGE_FILTER_PREFIX = "";

EOSCommunication comm(9, 10, -1);

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  
  Serial.println("Initializing");
  Serial.println("Send 'pre <prefix-string>' to only show messages starting with <prefix-string>.");
  Serial.println("Send 'nofilter' to show all messages that are received.");
  Serial.println("Send 'pipe <pipe-number from 0 to 5>' to set the receiving pipe");

  comm.begin();  
  comm.setReadingPipe(1);
  Serial.println("LISTENING ON PIPE 1");
}

// the loop function runs over and over again forever
void loop() {
  String messageReceived = comm.receive();
  if(messageReceived != "") {
    if(MESSAGE_FILTER_PREFIX == "" || messageReceived.startsWith(MESSAGE_FILTER_PREFIX)) {
      Serial.println("received: '" + messageReceived + "'");
    }      
  }
  
  if(Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    if(command.startsWith("pre ")) {
      MESSAGE_FILTER_PREFIX = command.substring(4);
      Serial.println("NEW MESSAGE FILTER: prefix '" + MESSAGE_FILTER_PREFIX + "'");
    } else if(command.equals("nofilter")) {
      MESSAGE_FILTER_PREFIX = "";
      Serial.println("MESSAGE FILTER DISABLED. SHOWING ALL MESSAGES THAT ARE RECEIVED.");
    } else if(command.startsWith("pipe ")) {
      String pipeStr = command.substring(5);
      Serial.println("LISTENING ON PIPE " + pipeStr);
      int pipeNr = pipeStr.toInt();
      comm.setReadingPipe(pipeNr);
    } else {
      Serial.println("unknown command: '" + command + "'");
    }
  }
}
