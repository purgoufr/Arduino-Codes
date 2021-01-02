#pragma once
#include <Arduino.h>

#define TEMP_MIN_LIMIT      0
#define TEMP_CONTROL_LIMIT  20
#define TEMP_MAX_LIMIT      50
#define HUM_RAIN_LIMIT      80

#define PWM_MIN     0
#define PWM_MAKS    64//200
#define PWM_STEP    4//10

#define PROX_MIN_LIMIT      5
#define PROX_CONTROL_LIMIT  25

#define ACC_MAX_VELOCITY    100



typedef enum 
{
    FAIL,
    PASS
}Check_t;

typedef enum
{
    MOTOR_STOP,
    MOTOR_BRAKE,
    MOTOR_FORWARD,
    MOTOR_BACKWARD
}Motor_state_t;

typedef union 
{
    struct
    {
        unsigned int right      : 1;
        unsigned int middle     : 1;
        unsigned int left       : 1;
        unsigned int reserved   : 13;
    }state_t;
    unsigned int all;
}Direction_t;

class Sensor_msgs_t
{
public:
    Sensor_msgs_t()
    {
        delay_ms = 500;
        time_ms = 1.0 / static_cast<float>(delay_ms);
    }

    byte dht_temp ;
    byte dht_hum  ;

    float accX, accY, accZ;
    float velocity;
    float time_ms;

    int distance; 
    int delay_ms;   
    
    Direction_t dir;

    int target_pwm1;
    int target_pwm2;
    int real_pwm1;
    int real_pwm2;

    Motor_state_t motor_state;


};

///////////////////////////////////////////////////////
/*Function Prototypes*/
void sensor_init(void);
void dht_run(Sensor_msgs_t *sens);
void mpu6050_run(Sensor_msgs_t *sens);
void proximity_run(Sensor_msgs_t *sens);
void direction_finding_run(Sensor_msgs_t *sens);
void take_sensor_info(Sensor_msgs_t *sens);
Check_t check_sensor_state(Sensor_msgs_t *sens);
void print_sensor(Sensor_msgs_t *sens);
///////////////////////////////////////////////////////