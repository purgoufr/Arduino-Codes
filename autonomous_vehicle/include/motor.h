#pragma once
#include "pins.h"
#include "sensors.h"
#include "sensor_lib.h"

void motor_set_pwm(const int pwm1, const int pwm2);
void motor_stop(Motor_state_t &state);
void motor_brake(Motor_state_t &state);
void motor_move_forward(Motor_state_t &state);
void motor_move_backward(Motor_state_t &state);
void generate_pwm(Sensor_msgs_t *sens);
void motor_control(Sensor_msgs_t *sens);