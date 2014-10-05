#include "MultiWiiBoard_config.h"
#include "OrangeRX_config.h"

#define ATTITUDE_MODE

// IMU configuration
#define MPU6050_DLPF_SETTING 6 // 5Hz cut-off
#define IMU_ANGLES

// Threshold under which the sticks are considered centered
#define RX_CENTER 5

/* --- Motors --- */

#define MOTOR_FRONT 3
#define MOTOR_LEFT 0
#define MOTOR_BACK 1
#define MOTOR_RIGHT 2

/* --- Throttle --- */

#define THROTTLE_CUT -80
#define THROTTLE_K 5.0

/* --- Controllers --- */

// Maximum PID targets
#define MAX_ROLL_PITCH 0.39269908169
#define MAX_ROLL_PITCH_RATE 1.57079632679 // Only used in rate mode
#define MAX_YAW_RATE 3.14159265359

// Roll and pitch rates PIDs
#define PID_ROLL_PITCH_RATE_KP 150.0
#define PID_ROLL_PITCH_RATE_KI 0.0
#define PID_ROLL_PITCH_RATE_KD 8.0

// Roll and pitch angles PIDs
#define PID_ROLL_PITCH_KP 5.0
#define PID_ROLL_PITCH_KI 20.0
#define PID_ROLL_PITCH_KD 0.0

// Yaw rate PID
#define PID_YAW_RATE_KP 500.0
#define PID_YAW_RATE_KI 0.0
#define PID_YAW_RATE_KD 0.0
