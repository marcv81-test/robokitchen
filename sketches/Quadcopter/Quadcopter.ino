#include "config.h"

#include "I2C.h"
#include "IMU.h"
#include "ESC.h"
#include "PID.h"
#include "PPMRX.h"
#include "QuadMotor.h"

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

#ifdef ATTITUDE_MODE

  // Roll and pitch angles PIDs
  PIDController rollPID = PIDController(
    PID_ROLL_PITCH_KP,
    PID_ROLL_PITCH_KI,
    PID_ROLL_PITCH_KD
  );
  PIDController pitchPID = PIDController(
    PID_ROLL_PITCH_KP,
    PID_ROLL_PITCH_KI,
    PID_ROLL_PITCH_KD
  );

#endif

// Stop all the motors
void stopMotors()
{
  QuadMotor::stop();

  // Reset the PIDs
  rollRatePID.reset();
  pitchRatePID.reset();
  yawRatePID.reset();
  #ifdef ATTITUDE_MODE
    rollPID.reset();
    pitchPID.reset();
  #endif
}

// Update the motors to follow the sticks
void updateMotors()
{
  #ifdef ATTITUDE_MODE

    // Update the roll and pitch angles PIDs
    rollPID.update(
      MAX_ROLL_PITCH * ailerons / 100.0,
      IMU::getRoll(),
      IMU::getLoopTime()
    );
    pitchPID.update(
      MAX_ROLL_PITCH * elevator / 100.0,
      IMU::getPitch(),
      IMU::getLoopTime()
    );

    // Update the roll and pitch rates PIDs
    rollRatePID.update(
      rollPID.getControl(),
      IMU::getRollRate(),
      IMU::getLoopTime()
    );
    pitchRatePID.update(
      pitchPID.getControl(),
      IMU::getPitchRate(),
      IMU::getLoopTime()
    );

  #else

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

  #endif

  // Update the yaw rate PID
  yawRatePID.update(
    MAX_YAW_RATE * rudder / 100.0,
    IMU::getYawRate(),
    IMU::getLoopTime()
  );

  QuadMotor::mix(
    THROTTLE_K * throttle,
    rollRatePID.getControl(),
    pitchRatePID.getControl(),
    yawRatePID.getControl()
  );
}

void setup()
{
  ESC::init();
  QuadMotor::init();

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
      if(readIMU)
      {
        updateMotors();
        #ifdef IMU_DEBUG
          IMU::debug();
        #endif
        #ifdef PID_DEBUG
          rollPIDDebug();
        #endif
      }
    }
  }

  // If could not read the sticks stop the motors
  else stopMotors();
}

#ifdef PID_DEBUG
  void rollPIDDebug()
  {
    Serial.print("Roll:");
    Serial.print(ESC::getChannel(MOTOR_LEFT));
    Serial.print(",");
    Serial.print(ESC::getChannel(MOTOR_RIGHT));
    Serial.print(",");
    Serial.print(IMU::getRollRate(), 3);
    Serial.print(",");
    Serial.print(IMU::getLoopTime(), 5);
    Serial.println("");
  }
#endif
