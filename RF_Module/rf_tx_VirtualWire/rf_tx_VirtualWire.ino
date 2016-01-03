#include <VirtualWire.h>
const int transmit_pin = 2;
const int buton = 9;
int butondurumu = 0;
 
void setup()
{
delay(1000);
vw_set_tx_pin(transmit_pin);
vw_set_ptt_inverted(true);
vw_setup(2000);
pinMode(buton, INPUT);
}
 
void loop()
{
butondurumu = digitalRead(buton);
if (butondurumu == HIGH)
{
 
char msg[1] = {'1'};
vw_send((uint8_t *)msg, 1);
vw_wait_tx();
 
delay (100);
}
 
else if (butondurumu==LOW) {
char msg[1] = {'0'};
vw_send((uint8_t *)msg, 1);
vw_wait_tx();
 
delay (100);
}
}
