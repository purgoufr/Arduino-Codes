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
#include <String.h>
#include <dht11.h>
#define DHT11PIN A0

dht11 DHT11;

nRF24L01p transmitter(9,10);
/* CSN - > 10, CE -> 9 defined */

float temp_variable;
float hum_variable;

static char temp[10];
static char hum[10];

void setup() {
  Serial.begin(9600);
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  /* SPI initialize */
  transmitter.channel(80);
  transmitter.TXaddress("Ugur");
  transmitter.init();
  /* Transmitter settings done */
}
void loop() {
  int chk = DHT11.read(DHT11PIN);

  temp_variable = (float)DHT11.temperature;
  hum_variable      = (float)DHT11.humidity;
  
  Serial.print("Humidity (%): ");
  Serial.println(hum_variable, 2);

  Serial.print("Temperature (Celcius): ");
  Serial.println(temp_variable, 2);
  
  dtostrf(hum_variable,5, 2, hum);
  
  dtostrf(temp_variable,5, 2, temp);
  /* float value converted to string value for temperature */
  
  transmitter.txPL(temp);
  transmitter.txPL(hum);
  boolean sendstate = transmitter.send(FAST);
  /* Temperature information transmitted to nRF24L01 */
  /* If there is a failure, sendstate's value will be false */
  if(sendstate==true){
        Serial.println("Data sent successfully");
  }else{
        Serial.println("Data can not sent!");
  }  
  delay(1000); 
}
