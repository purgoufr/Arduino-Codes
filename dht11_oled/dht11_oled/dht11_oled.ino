#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Time.h>


#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

U8G2_SH1106_128X32_VISIONOX_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); 


int timer1_counter = 0;
int timer2_counter = 0;
int gl_counter = 0; 

float tempC = 0;

int tick = 0;
int hour = 9;
int minute = 8;
int second = 7;

int time_ok = 0;

void setup(void) {
  sensors.begin();
  
  u8g2.begin();
  
  Serial.begin(115200);
  while (!Serial) ;
  
  noInterrupts();    
  //Timer-1    
  TCCR1A = 0;
  TCCR1B = 0;
  timer1_counter = 3035;   // preload timer 65536-16MHz/256/2Hz (34286 for 0.5sec) (59286 for 0.1sec) (3035 for 1sec) 
  TCNT1 = timer1_counter;   // preload timer
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt  
  //Timer-2    
  TCCR2A = 0;
  TCCR2B = 0;
  timer2_counter = 124;   // preload timer 256-16MHz/256/2Hz (124 for 500 Hz - 0.002 sec)
  TCNT2 = timer2_counter;   // preload timer
  TCCR2B |= (1 << CS22);    // 256 prescaler 
  TIMSK2 |= (1 << TOIE2);   // enable timer overflow interrupt
  interrupts();  
}

void loop(void) 
{
  task_run(gl_counter);
}

void read_temp_sens(void)
{
  sensors.requestTemperatures(); // Send the command to get temperatures
  tempC = sensors.getTempCByIndex(0);
}

void run_OLED(const float temp_s, const int hour_s, const int minute_s, const int second_s)
{
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_helvB12_tf);//u8g2_font_profont22_mr); // choose a suitable font
  u8g2.setCursor(10, 16);
  u8g2.print(temp_s);
  u8g2.print(" C"); 
  u8g2.sendBuffer();

  u8g2.setFont(u8g2_font_7x14_mr );//u8g2_font_8x13B_tf ); // choose a suitable font
  u8g2.setCursor(10, 32);
  if(hour_s < 10)
    u8g2.print("0");
  u8g2.print(hour_s);
  u8g2.print(":"); 
  if(minute_s < 10)
    u8g2.print("0");
  u8g2.print(minute_s);
  u8g2.print(":"); 
  if(second_s < 10)
    u8g2.print("0");
  u8g2.print(second_s);
  u8g2.print("  DEFNEM");
  u8g2.sendBuffer();
}


ISR(TIMER1_OVF_vect)        // interrupt service routine - tick every 1sec
{
  TCNT1 = timer1_counter;  
  tick++;
}

ISR(TIMER2_OVF_vect)        // interrupt service routine - tick every 0.002sec
{
  TCNT2 = timer2_counter;  
  gl_counter++;
}

void set_time(int &hour_s, int &minute_s)
{
  int time_idx = 0;
  String h_m_str = Serial.readString();

  if(h_m_str.indexOf("clock") != -1)
  {
    h_m_str.replace("clock", "");
    Serial.println("New string: " + h_m_str);
    hour_s = h_m_str.substring(0,2).toInt();
    minute_s = h_m_str.substring(2,4).toInt();
    Serial.print("hour: ");
    Serial.println(hour_s);
    Serial.print("minute: ");
    Serial.println(minute_s);
    time_ok = 1;
  }
}

void start_time(int &hour_s, int &minute_s, int &second_s)
{
  second_s = tick;

  if(second_s > 59)
  {
    tick = 0;
    minute_s++;
  }
  if(minute_s > 59)
  {
    minute_s = 0;
    hour_s++;
  }
  if(hour_s > 23)
  {
    hour_s = 0;
  }
}

static void task_run(int &cnt)
{
  int temp_cnt = cnt;

  //1 Hz 
  if(temp_cnt % 500 == 0)
  {
    start_time(hour, minute, second);
    read_temp_sens();
    run_OLED(tempC, hour, minute, second);
  }

  if(time_ok == 0)
  {
    set_time(hour, minute);
  }
  
}

