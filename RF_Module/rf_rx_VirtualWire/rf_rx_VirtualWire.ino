#include <VirtualWire.h>
const int led = 9;
 
const int receive_pin = 3;
void setup()
{
delay(1000);
 
vw_set_rx_pin(receive_pin);
 
vw_set_ptt_inverted(true);
vw_setup(2000);
vw_rx_start();
pinMode(led, OUTPUT);
}
 
void loop()
{
uint8_t buf[VW_MAX_MESSAGE_LEN];
uint8_t buflen = VW_MAX_MESSAGE_LEN;
if (vw_get_message(buf, &buflen))
{
if(buf[0]=='1')
 
digitalWrite(led , HIGH);
}
if(buf[0]=='0')
{
digitalWrite(led , LOW);
}
}
