#include "config.h"

#include "I2C.h"
#include "IMU.h"
#include "ESC.h"
#include "PID.h"
#include "PPMRX.h"

int8_t throttle, rudder, elevator, ailerons;

// Roll and pitch rates PIDs
PIDController rollRatePID = PIDController(
  PID_ROLL_PITCH_RATE_KP,
  PID_ROLL_PITCH_RATE_KI,
  PID_ROLL_PITCH_RATE_KD
);
PIDController pitchRatePID = PIDController(
  PID_ROLL_PITCH_RATE_KP,
  PID_ROLL_PITCH_RATE_KI,
  PID_ROLL_PITCH_RATE_KD
);

// Yaw rate PID
PIDController yawRatePID = PIDController(
  PID_YAW_RATE_KP,
  PID_YAW_RATE_KI,
  PID_YAW_RATE_KD
);

// Calibrate the ESCs controlling the motors
void initMotors()
{
  // Calibrate the max ESC PWM input
  for(int i=0 ; i<4 ; i++) ESC::setChannel(i, MOTOR_MAX);
  delay(3000);

  // Calibrate the min ESC PWM input (the motors stall below this value)
  for(int i=0 ; i<4 ; i++) ESC::setChannel(i, MOTOR_STALL);
  delay(2000);
}

// Stop all the motors
void stopMotors()
{
  // Stop all the motors
  for(int i=0 ; i<4 ; i++) ESC::setChannel(i, MOTOR_OFF);

  // Reset the PIDs
  rollRatePID.reset();
  pitchRatePID.reset();
  yawRatePID.reset();
}

// Update the motors to follow the sticks
void updateMotors()
{
  // Update the roll and pitch rates PIDs
  rollRatePID.update(
    MAX_ROLL_PITCH_RATE * ailerons / 100.0,
    IMU::getRollRate(),
    IMU::getLoopTime()
  );
  pitchRatePID.update(
    MAX_ROLL_PITCH_RATE * elevator / 100.0,
    IMU::getPitchRate(),
    IMU::getLoopTime()
  );

  // Update the yaw rate PID
  yawRatePID.update(
    MAX_YAW_RATE * rudder / 100.0,
    IMU::getYawRate(),
    IMU::getLoopTime()
  );

  // Compute the motor mix
  uint16_t motor[4];
  float base = MOTOR_IDLE + (THROTTLE_K * throttle);
  motor[MOTOR_FRONT] = base + pitchRatePID.getControl() - yawRatePID.getControl();
  motor[MOTOR_LEFT] = base + rollRatePID.getControl() + yawRatePID.getControl();
  motor[MOTOR_BACK] = base - pitchRatePID.getControl() - yawRatePID.getControl();
  motor[MOTOR_RIGHT] = base - rollRatePID.getControl() + yawRatePID.getControl();

  // Fix the motor mix range
  int16_t motorFix = 0;
  uint16_t motorMin = motor[0], motorMax = motor[0];
  for(int i=1 ; i<4 ; i++)
  {
    if(motor[i] < motorMin) motorMin = motor[i];
    if(motor[i] > motorMax) motorMax = motor[i];
  }
  if(motorMin < MOTOR_MIN) motorFix = MOTOR_MIN - motorMin;
  else if(motorMax > MOTOR_MAX) motorFix = MOTOR_MAX - motorMax;

  // Update the ESCs
  for(int i=0 ; i<4 ; i++) ESC::setChannel(i, motor[i] + motorFix);
}

void setup()
{
  ESC::init();
  initMotors();

  Serial.begin(115200);
  Serial.println("");
  Serial.println("---");

  I2C::begin();
  IMU::init();
  PPMRX::init();
}

void loop()
{
  // Read the IMU and the sticks
  bool readIMU = IMU::refresh();
  bool readPPMRX = PPMRX::refresh();

  // If we could read the sticks
  if(readPPMRX)
  {
    // Read the throttle
    throttle = PPMRX::getChannel(PPMRX_CHANNEL_THROTTLE);

    // If the throttle is below the threshold stop the motors
    if(throttle < THROTTLE_CUT) stopMotors();

    // If the throttle is above the threshold
    else
    {
      // Read the other sticks
      rudder = PPMRX::getChannel(PPMRX_CHANNEL_RUDDER);
      elevator = PPMRX::getChannel(PPMRX_CHANNEL_ELEVATOR);
      ailerons = PPMRX::getChannel(PPMRX_CHANNEL_AILERONS);

      // Center the sticks which are almost centered
      if ((rudder < RX_CENTER) && (rudder > -RX_CENTER)) rudder = 0;
      if ((elevator < RX_CENTER) && (elevator > -RX_CENTER)) elevator = 0;
      if ((ailerons < RX_CENTER) && (ailerons > -RX_CENTER)) ailerons = 0;

      // If we could read the IMU update the motors
      if(readIMU) updateMotors();
    }
  }

  // If could not read the sticks stop the motors
  else stopMotors();

  #ifdef IMU_DEBUG
    if(readIMU) IMU::debug();
  #endif
}
