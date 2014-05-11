#include "MultiWiiBoard_config.h"
#include "OrangeRX_config.h"

// IMU configuration
#define MPU6050_DLPF_SETTING 6 // 5Hz cut-off
#define IMU_ANGLES

// Threshold under which the sticks are considered centered
#define RX_CENTER 5

/* --- Motors --- */

#define MOTOR_OFF 100
#define MOTOR_STALL 150
#define MOTOR_MIN 200
#define MOTOR_IDLE 1000
#define MOTOR_MAX 2000

#define MOTOR_FRONT 3
#define MOTOR_LEFT 0
#define MOTOR_BACK 1
#define MOTOR_RIGHT 2

/* --- Throttle --- */

#define THROTTLE_CUT -80
#define THROTTLE_K 5.0

/* --- Controllers --- */

// Maximum PID targets
#define MAX_ROLL_PITCH_RATE 1.57079632679
#define MAX_YAW_RATE 3.14159265359

// Roll and pitch rates PIDs
#define PID_ROLL_PITCH_RATE_KP 150.0
#define PID_ROLL_PITCH_RATE_KI 0.0
#define PID_ROLL_PITCH_RATE_KD 8.0

// Yaw rate PID
#define PID_YAW_RATE_KP 500.0
#define PID_YAW_RATE_KI 0.0
#define PID_YAW_RATE_KD 0.0
