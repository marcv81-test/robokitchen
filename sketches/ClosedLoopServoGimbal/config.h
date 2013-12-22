#include "IMU_config.h"
#include "Servo_config.h"

#define IMU_GYRO_CALIB_SAMPLE_SIZE 250

#define IMU_ACCEL_ENABLE
#define IMU_ACCEL_COEFF 0.01

#define I2C_SPEED 400000

#define PID_KP 1000.0
#define PID_KI 40.0
#define PID_KD 0.0
#define PID_CONTROL_MAX 650.0
