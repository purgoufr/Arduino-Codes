#include "sensors.h"
#include "sensor_lib.h"

SimpleDHT11 dht11(DHT_PIN);
MPU6050 accelgyro;

void sensor_init(void)
{
  Wire.begin();
  
  Serial.println("Initializing I2C devices...");
  accelgyro.initialize();
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
}

void dht_run(Sensor_msgs_t *sens)
{
  dht11.read(&sens->dht_temp, &sens->dht_hum, NULL);

}
 
void mpu6050_run(Sensor_msgs_t *sens)
{
  int16_t ax, ay, az;
  int16_t gx, gy, gz;
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    // mpu_temp = accelgyro.getTemperature();
    // mpu_temp = (mpu_temp + 12412.0) * 0.00294118;
  sens->accX = ((float)ax) * 0.000061035;
  sens->accY = ((float)ay) * 0.000061035;
  sens->accZ = ((float)az) * 0.000061035;

  sens->velocity = sens->accY * sens->time_ms; 
}

void proximity_run(Sensor_msgs_t *sens)
{
  long duration; // variable for the duration of sound wave travel

  digitalWrite(PROX_TRIG_PIN, LOW);
  delay(2);
  digitalWrite(PROX_TRIG_PIN, HIGH);
  delay(10);
  digitalWrite(PROX_TRIG_PIN, LOW);

  duration = pulseIn(PROX_ECHO_PIN, HIGH);
  sens->distance = duration * 0.017;
}

void direction_finding_run(Sensor_msgs_t *sens)
{
  sens->dir.all = 0;
  sens->dir.state_t.right  = digitalRead(SENS_RIGHT_PIN);
  sens->dir.state_t.middle = digitalRead(SENS_MIDDLE_PIN);
  sens->dir.state_t.left   = digitalRead(SENS_LEFT_PIN);
}  

void take_sensor_info(Sensor_msgs_t *sens)
{
  dht_run(sens);
  mpu6050_run(sens);
  proximity_run(sens);
  direction_finding_run(sens);
}

Check_t check_sensor_state(Sensor_msgs_t *sens)
{
  if(sens->dht_temp < 0   || 
     sens->dht_hum  > 99  ||
     sens->distance < 5   )
     return FAIL;
  else
    return PASS;
}

void print_sensor(Sensor_msgs_t *sens)
{
  Serial.print(sens->accX);                Serial.print("\t");
  Serial.print(sens->accY);                Serial.print("\t");
  Serial.print(sens->accZ);                Serial.print("\t");
  Serial.print(sens->dht_hum);             Serial.print("%\t");
  Serial.print(sens->dht_temp);            Serial.print("°C\t");
  Serial.print(sens->distance);            Serial.print("cm\t");
  Serial.print(sens->dir.state_t.right);   Serial.print("R\t");
  Serial.print(sens->dir.state_t.middle);  Serial.print("M\t");
  Serial.print(sens->dir.state_t.left);    Serial.print("L\t");
  Serial.print(sens->dir.all);             Serial.print("\t");
  Serial.print(sens->real_pwm1);           Serial.print("pwm1  ");                   
  Serial.print(sens->real_pwm2);           Serial.print("pwm2  "); 
  Serial.print(sens->motor_state);         Serial.print(" state");
  Serial.print("\r\n");
}

