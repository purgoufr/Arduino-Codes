#pragma once
#include <Arduino.h>
///////////////////////////////////////////////////////
#define MOTOR_1_PIN_1     14//2
#define MOTOR_1_PIN_2     15//3
#define MOTOR_2_PIN_1     17//4
#define MOTOR_2_PIN_2     16//5
#define MOTOR_1_PIN_PWM   9
#define MOTOR_2_PIN_PWM   10
#define PROX_TRIG_PIN     6
#define PROX_ECHO_PIN     7
#define DHT_PIN           8

#define SENS_RIGHT_PIN    2
#define SENS_MIDDLE_PIN   3
#define SENS_LEFT_PIN     4
///////////////////////////////////////////////////////
void pin_mode_init(void);
///////////////////////////////////////////////////////
