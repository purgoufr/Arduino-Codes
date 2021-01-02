#include "motor.h"

Sensor_msgs_t sensor_msgs;

int timer2_counter = 0; 
int gl_counter = 0;

static void task_run(int &cnt);

void setup()
{ 
  pin_mode_init();

  Serial.begin(115200);
  while (!Serial) ;
  
  sensor_init();

  noInterrupts();          
  TCCR2A = 0;
  TCCR2B = 0;
  timer2_counter = 59286;   // preload timer 65536-16MHz/256/2Hz (34286 for 0.5sec) (59286 for 0.1sec)
  TCNT2 = timer2_counter;   // preload timer
  TCCR2B |= (1 << CS22);    // 256 prescaler 
  TIMSK2 |= (1 << TOIE2);   // enable timer overflow interrupt
  interrupts();             

}
 
void loop()
{
  task_run(gl_counter);
}



ISR(TIMER2_OVF_vect)        // interrupt service routine - tick every 0.1sec
{
  TCNT2 = timer2_counter;  
  gl_counter++;
}

static void task_run(int &cnt)
{
  int temp_cnt = cnt;
  if(temp_cnt % 2 == 0)
  {
    motor_control(&sensor_msgs);
  }
  if(temp_cnt % 10 == 0)
  {
    print_sensor(&sensor_msgs);
  }
}
