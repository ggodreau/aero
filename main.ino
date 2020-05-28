
/*
 * Active Aero Code
 */



#include <Adafruit_LSM303_Accel.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

int upbut = 2;//for dig pin 2... not really needed anymore since the control is based off the IMU
int Actup = 3; //for dig pin 3
int dwnbut = 4; //for dig pin 4 ... not really needed anymore since the control is based off the IMU
int Actdwn = 5; //for dig pin 5
int sensorVal = 5; //analog pin input to read potentiometer
int DRS = 800; //downforce potentiometer setting
int DF = 700; //fully stowed potentiometer setting
int AB = 600; //air brake potentiometer setting
int band = 25; //the hystersis or error band allowed. This is now calibratable. 
int Ay = 4; //m/s threshold to activate DF
int dAx = -7; //m/s threshold to activate AB
int aAx = 4; //m/s threshold to activate DRS
int targetH = 700; //target pot reading
int targetL = 680; //target pot reading
int count=0;

/* Assign a unique ID to this sensor at the same time */
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);

void displaySensorDetails(void) {
  sensor_t sensor;
  accel.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print("Sensor:       ");
  Serial.println(sensor.name);
  Serial.print("Driver Ver:   ");
  Serial.println(sensor.version);
  Serial.print("Unique ID:    ");
  Serial.println(sensor.sensor_id);
  Serial.print("Max Value:    ");
  Serial.print(sensor.max_value);
  Serial.println(" m/s^2");
  Serial.print("Min Value:    ");
  Serial.print(sensor.min_value);
  Serial.println(" m/s^2");
  Serial.print("Resolution:   ");
  Serial.print(sensor.resolution);
  Serial.println(" m/s^2");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

void setup(void) {

    // Initialize dig pins as outputs and inputs:
  pinMode(upbut, INPUT_PULLUP);
  pinMode(dwnbut, INPUT_PULLUP); //this indicated the button is a pull DOWN to GND
  pinMode(Actup, OUTPUT);
  pinMode(Actdwn, OUTPUT);

  Serial.begin(9600);
  
#ifndef ESP8266
  while (!Serial)
    ; // will pause Zero, Leonardo, etc until serial console opens
#endif
  Serial.begin(9600);
  Serial.println("Accelerometer Test");
  Serial.println("");

  /* Initialise the sensor */
  if (!accel.begin()) {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while (1)
      ;
  }

  /* Display some basic information on this sensor */
  displaySensorDetails();

  accel.setRange(LSM303_RANGE_4G);
  Serial.print("Range set to: ");
  lsm303_accel_range_t new_range = accel.getRange();
  switch (new_range) {
  case LSM303_RANGE_2G:
    Serial.println("+- 2G");
    break;
  case LSM303_RANGE_4G:
    Serial.println("+- 4G");
    break;
  case LSM303_RANGE_8G:
    Serial.println("+- 8G");
    break;
  case LSM303_RANGE_16G:
    Serial.println("+- 16G");
    break;
  }

  accel.setMode(LSM303_MODE_NORMAL);
  Serial.print("Mode set to: ");
  lsm303_accel_mode_t new_mode = accel.getMode();
  switch (new_mode) {
  case LSM303_MODE_NORMAL:
    Serial.println("Normal");
    break;
  case LSM303_MODE_LOW_POWER:
    Serial.println("Low Power");
    break;
  case LSM303_MODE_HIGH_RESOLUTION:
    Serial.println("High Resolution");
    break;
  }

}

void loop(void) {
  /* Get a new sensor event */
  sensors_event_t event;
  accel.getEvent(&event);

  /* Display the results (acceleration is measured in m/s^2) */
  Serial.print("X: ");
  Serial.print(event.acceleration.x);
  Serial.print("  ");
  Serial.print("Y: ");
  Serial.print(event.acceleration.y);
  Serial.print("  ");
  Serial.print("Z: ");
  Serial.print(event.acceleration.z);
  Serial.print("  ");
  Serial.println("m/s^2");

if(abs(event.acceleration.y) > Ay){
  targetH = DF + band;
  targetL = DF - band;
  count = 0;
}
else if(event.acceleration.x < dAx){
  targetH = AB + band;
  targetL = AB - band;
  count = 0;
}
else if(event.acceleration.x > aAx){
  targetH = DRS + band;
  targetL = DRS - band;
  count = 0;
}
else if(count > 20){
  targetH = DRS + band;
  targetL = DRS - band;
}
else {
  count++;
}

int reading = analogRead(sensorVal); 
Serial.print("reading: ");
Serial.println(reading);
Serial.print("targetH: ");
Serial.println(targetH);
Serial.print("targetL: ");
Serial.println(targetL);
Serial.print("count: ");
Serial.println(count);


if(reading > targetH)
{digitalWrite(Actup, HIGH);}
else if(reading < targetH)
{digitalWrite(Actup, LOW);}

if(reading < targetL)
{digitalWrite(Actdwn, HIGH);}
else if(reading > targetL)
{digitalWrite(Actdwn, LOW);}

  /* Delay before the next sample */
  delay(1000);
}
