#include <Arduino.h>
#include "I2Cdev.h"

#include "MPU6050_6Axis_MotionApps20.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

MPU6050 mpu;

#define OUTPUT_READABLE_YAWPITCHROLL

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

const int16_t accel_offset[3] = { -1881, -3615, 1341 };
const int16_t gyro_offset[3] = { 51, -7, 13 };

uint32_t last_us;

void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin(PB6, PB7);
        // Wire.setClock(1000000); // 400kHz I2C clock. Comment this line if having compilation difficulties
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    Serial.begin(115200);

    mpu.initialize();

    if (!mpu.testConnection()) {
      
      while(1) {
        Serial.println(F("MPU6050 connection failed"));
        delay(500);
      }
    }    

    devStatus = mpu.dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity
    mpu.setXGyroOffset(gyro_offset[0]);
    mpu.setYGyroOffset(gyro_offset[1]);
    mpu.setZGyroOffset(gyro_offset[2]);
    mpu.setXAccelOffset(accel_offset[0]);
    mpu.setYAccelOffset(accel_offset[1]);
    mpu.setZAccelOffset(accel_offset[2]);

    if (devStatus == 0) {
        // mpu.CalibrateAccel(6);
        // mpu.CalibrateGyro(6);
        mpu.setDMPEnabled(true);
        dmpReady = true;
    } else {
        Serial.print(F("DMP Initialization failed"));
    }
    last_us = micros();
}

void updateYPR() {
  if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
    // mpu.dmpGetQuaternion(&q, fifoBuffer);
    // mpu.dmpGetGravity(&gravity, &q);
    // mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
  }
}

void loop() {
  static unsigned long ts = micros();
  static unsigned long counter = 0;
  unsigned long now = micros();
  counter++;
  if (now - ts >= 1000000) {
    Serial.println(counter);
    counter = 0;
    ts = now;
  }
  updateYPR();
}
