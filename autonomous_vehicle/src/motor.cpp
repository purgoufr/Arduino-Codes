#include <Arduino.h>
#include "motor.h"


template<typename T>
T limit_number(T *num, const T &lower, const T &upper)
{
  if(*num < lower)
    *num = lower;
  else if(*num > upper)
    *num = upper;
  else
    return *num;

  return *num;
}

void motor_set_pwm(const int pwm1, const int pwm2)
{
  analogWrite(MOTOR_1_PIN_PWM, pwm1); //ENA pin
  analogWrite(MOTOR_2_PIN_PWM, pwm2); //ENB pin
}

void motor_stop(Motor_state_t &state)
{
  motor_set_pwm(0, 0);
  state = MOTOR_STOP;
}

void motor_brake(Motor_state_t &state)
{
  digitalWrite(MOTOR_1_PIN_1, HIGH);
  digitalWrite(MOTOR_1_PIN_2, HIGH);

  digitalWrite(MOTOR_2_PIN_1, HIGH);
  digitalWrite(MOTOR_2_PIN_2, HIGH);

  state = MOTOR_BRAKE;
}

void motor_move_forward(Motor_state_t &state)
{
  digitalWrite(MOTOR_1_PIN_1, HIGH);
  digitalWrite(MOTOR_1_PIN_2, LOW);

  digitalWrite(MOTOR_2_PIN_1, HIGH);
  digitalWrite(MOTOR_2_PIN_2, LOW);

  state = MOTOR_FORWARD;
}

void motor_move_backward(Motor_state_t &state)
{
  digitalWrite(MOTOR_1_PIN_1, LOW);
  digitalWrite(MOTOR_1_PIN_2, HIGH);

  digitalWrite(MOTOR_2_PIN_1, LOW);
  digitalWrite(MOTOR_2_PIN_2, HIGH);

  state = MOTOR_BACKWARD;
}

void generate_pwm(Sensor_msgs_t *sens)
{
  if(sens->dht_temp > TEMP_MIN_LIMIT && sens->dht_temp < TEMP_CONTROL_LIMIT)
  {
    sens->target_pwm1 = sens->dht_temp * 10;
    sens->target_pwm2 = sens->dht_temp * 10;
  }
  else if(sens->dht_temp > TEMP_CONTROL_LIMIT && sens->dht_temp < TEMP_MAX_LIMIT)
  {
    sens->target_pwm1 = PWM_MAKS;
    sens->target_pwm2 = PWM_MAKS;
  }
  else
  {
    sens->target_pwm1 = PWM_MIN;
    sens->target_pwm2 = PWM_MIN;
  }

  if(sens->dht_hum > HUM_RAIN_LIMIT )
  {
    sens->target_pwm1 = sens->target_pwm1 >> 1;
    sens->target_pwm2 = sens->target_pwm2 >> 1;
  }

  if(sens->distance > PROX_MIN_LIMIT && sens->distance < PROX_CONTROL_LIMIT)
  {
    sens->target_pwm1 -= (PROX_CONTROL_LIMIT - sens->distance) * 10;
    sens->target_pwm2 -= (PROX_CONTROL_LIMIT - sens->distance) * 10;
  }
  else if(sens->distance > PROX_CONTROL_LIMIT)
  {
    // Serial.println("Safe Distance");
  }
  else
  {
    sens->target_pwm1 = PWM_MIN;
    sens->target_pwm2 = PWM_MIN;
    Serial.println("Unsafe Distance");
  }
}

void pwm_tunning (int *real_pwm, int *target_pwm)
{
  if(*real_pwm < *target_pwm) 
  {
    *real_pwm += PWM_STEP;
  }
  else if(*real_pwm > *target_pwm)
  {
    *real_pwm -= PWM_STEP;
  }
  else
  {
    // Serial.print("target reached!\r\n");
  }

  limit_number(real_pwm, PWM_MIN, PWM_MAKS);
}

void move_motor(Sensor_msgs_t *sens)
{
  direction_finding_run(sens);

  switch (sens->dir.all)
  {
  case 1:
  {
    motor_move_forward(sens->motor_state);
    // //GO LEFT
    // sens->real_pwm1 -= sens->real_pwm1 >> 3;
    sens->real_pwm2 -= sens->real_pwm2 >> 3;
    break;
  }
  case 2:
  {
    motor_move_forward(sens->motor_state);
    //GO FORWARD
    break;
  }
  case 3:
  {
    motor_move_forward(sens->motor_state);
    // //GO LEFT
    // sens->real_pwm1 -= sens->real_pwm1 >> 4;
    sens->real_pwm2 -= sens->real_pwm2 >> 4;
    break;
  }
  case 4:
  {
    motor_move_forward(sens->motor_state);
    // //GO RIGHT
    // sens->real_pwm2 -= sens->real_pwm2 >> 3;
    sens->real_pwm1 -= sens->real_pwm1 >> 3;
    break;
  }
  case 5:
  {
    //UB
    motor_stop(sens->motor_state);
    break;
  }
  case 6:
  {
    motor_move_forward(sens->motor_state);
    // //GO RIGHT
    // sens->real_pwm2 -= sens->real_pwm2 >> 4;
    sens->real_pwm1 -= sens->real_pwm1 >> 4;
    break;
  }
  case 7:
  {
    motor_move_forward(sens->motor_state);
    break;
  }
  
  default:
    motor_move_backward(sens->motor_state);
    break;
  }

  motor_set_pwm(sens->real_pwm1, sens->real_pwm2);
}

void motor_control(Sensor_msgs_t *sens)
{
  take_sensor_info(sens);
  generate_pwm(sens);
  if(check_sensor_state(sens) == FAIL)
  {
    sens->target_pwm1 = 0;
    sens->target_pwm2 = 0;
  }
  pwm_tunning(&sens->real_pwm1, &sens->target_pwm1);
  pwm_tunning(&sens->real_pwm2, &sens->target_pwm2);
  move_motor(sens);

  // motor_move_forward(sens->motor_state);
  // motor_set_pwm(50, 60);


}
