#include "pins.h"

void pin_mode_init(void)
{
  pinMode(MOTOR_1_PIN_1,    OUTPUT);
  pinMode(MOTOR_1_PIN_2,    OUTPUT);
  pinMode(MOTOR_2_PIN_1,    OUTPUT);
  pinMode(MOTOR_2_PIN_2,    OUTPUT);
  pinMode(MOTOR_1_PIN_PWM,  OUTPUT); 
  pinMode(MOTOR_2_PIN_PWM,  OUTPUT);
  pinMode(PROX_TRIG_PIN,    OUTPUT); 
  pinMode(PROX_ECHO_PIN,    INPUT); 
  pinMode(SENS_RIGHT_PIN,   INPUT); 
  pinMode(SENS_MIDDLE_PIN,  INPUT); 
  pinMode(SENS_LEFT_PIN,    INPUT); 
}