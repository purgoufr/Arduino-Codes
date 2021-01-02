#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <SimpleDHT.h>
#include "I2Cdev.h"
#include "MPU6050.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

///////////////////////////////////////////////////////
#define MOTOR_1_PIN_1     2
#define MOTOR_1_PIN_2     3
#define MOTOR_2_PIN_1     4
#define MOTOR_2_PIN_2     5
#define MOTOR_1_PIN_PWM   9
#define MOTOR_2_PIN_PWM   10
#define PROX_TRIG_PIN     6
#define PROX_ECHO_PIN     7
#define DHT_PIN           8
#define MPU_INT_PIN       12
///////////////////////////////////////////////////////
SimpleDHT11 dht11(DHT_PIN);
float dht_temp = 0;
float dht_hum  = 0;

#define OUTPUT_READABLE_ACCELGYRO
MPU6050 accelgyro;
float accX, accY, accZ;
// float mpu_temp = 0;

SemaphoreHandle_t mutex;
///////////////////////////////////////////////////////
/*Function Prototypes*/
void pin_mode_init(void);
void motor_set_pwm(int pwm1, int pwm2);
void motor_turn_right(void);
void motor_turn_left(void);
static void dht_run(void* pvParameters);
static void mpu6050_run(void* pvParameters);
static void print_data_run(void* pvParameters);
static void MyIdleTask(void* pvParameters);
///////////////////////////////////////////////////////
void setup()
{ 
  pin_mode_init();

  Serial.begin(115200);
  
  mutex = xSemaphoreCreateMutex();
  if (mutex != NULL) {
    Serial.println("Mutex created");
  }

  /*MPU6050*/
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin();
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true);
  #endif
  
  Serial.println("Initializing I2C devices...");
  accelgyro.initialize();
  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  
  delay(500);

  /*Tasks*/
  xTaskCreate(dht_run, "dht_task", 128, NULL, 2, NULL);
  xTaskCreate(mpu6050_run, "mpu6050_task", 128, NULL, 2, NULL);
  xTaskCreate(print_data_run, "print_data_task", 128, NULL, 1, NULL);
  xTaskCreate(MyIdleTask, "IdleTask", 64, NULL, 0, NULL);
}
 
void loop()
{

}

void pin_mode_init(void)
{
  pinMode(MOTOR_1_PIN_1, OUTPUT);
  pinMode(MOTOR_1_PIN_2, OUTPUT);
  pinMode(MOTOR_2_PIN_1, OUTPUT);
  pinMode(MOTOR_2_PIN_2, OUTPUT);
  pinMode(MOTOR_1_PIN_PWM, OUTPUT); 
  pinMode(MOTOR_2_PIN_PWM, OUTPUT);
  pinMode(PROX_TRIG_PIN, OUTPUT); 
  pinMode(PROX_ECHO_PIN, INPUT); 
}

void motor_set_pwm(int pwm1, int pwm2)
{
  analogWrite(MOTOR_1_PIN_PWM, pwm1); //ENA pin
  analogWrite(MOTOR_2_PIN_PWM, pwm2); //ENB pin
}

void motor_turn_right(void)
{
  digitalWrite(MOTOR_1_PIN_1, HIGH);
  digitalWrite(MOTOR_1_PIN_2, LOW);

  digitalWrite(MOTOR_2_PIN_1, HIGH);
  digitalWrite(MOTOR_2_PIN_2, LOW);
}

void motor_turn_left(void)
{
  digitalWrite(MOTOR_1_PIN_1, LOW);
  digitalWrite(MOTOR_1_PIN_2, HIGH);

  digitalWrite(MOTOR_2_PIN_1, LOW);
  digitalWrite(MOTOR_2_PIN_2, HIGH);
}


static void dht_run(void* pvParameters)
{
 while (true){
  vTaskDelay(2000 / portTICK_PERIOD_MS);//2000 ms

  int err = SimpleDHTErrSuccess;
  if (xSemaphoreTake(mutex, 100) == pdTRUE) {
    if ((err = dht11.read(&dht_temp, &dht_hum, NULL)) != SimpleDHTErrSuccess) {
      Serial.print("Read DHT11 failed, err="); Serial.println(err);
    }
    xSemaphoreGive(mutex);
  }
 }
}
 
static void mpu6050_run(void* pvParameters)
{
  int16_t ax, ay, az;
  int16_t gx, gy, gz;
  while (true) {
    vTaskDelay(2000 / portTICK_PERIOD_MS);//1000 ms

    if (xSemaphoreTake(mutex, 100) == pdTRUE)
    {
      accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
      // mpu_temp = accelgyro.getTemperature();
      xSemaphoreGive(mutex);
    }

    // mpu_temp = (mpu_temp + 12412.0) * 0.00294118;
    accX = ((float)ax) * 0.000061035;
    accY = ((float)ay) * 0.000061035;
    accZ = ((float)az) * 0.000061035;
  }
}

static void print_data_run(void* pvParameters)
{
  while (true){
    vTaskDelay(2000 / portTICK_PERIOD_MS);//2000 ms
    Serial.print("a/g:\t");
    Serial.print(accX); Serial.print("\t");
    Serial.print(accY); Serial.print("\t");
    Serial.print(accZ); Serial.print("\t");
    // Serial.print(mpu_temp); Serial.print("°C \t");
    Serial.print(dht_hum); Serial.print("%\t");
    Serial.print(dht_temp); Serial.print("°C \t");
    Serial.println();
  }
}
 
static void MyIdleTask(void* pvParameters)
{
 while (true){
    vTaskDelay(3000 / portTICK_PERIOD_MS);//3000 ms
    Serial.println(F("Idle state"));
 }
 
}
