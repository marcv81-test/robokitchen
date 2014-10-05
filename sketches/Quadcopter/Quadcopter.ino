#include "config.h"

#include "I2C.h"
#include "IMU.h"
#include "ESC.h"
#include "PID.h"
#include "PPMRX.h"

int8_t throttle, rudder, elevator, ailerons;
float rollTarget, pitchTarget, yawTarget;

#ifdef PID_DEBUG
  float totalTime = 0.0;
#endif

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
  // Stop all the motors
  ESC::stop();

  // Reset the PIDs
  rollRatePID.reset();
  pitchRatePID.reset();
  yawRatePID.reset();
  #ifdef ATTITUDE_MODE
    rollPID.reset();
    pitchPID.reset();
  #endif

  #ifdef PID_DEBUG
    totalTime = 0.0;
  #endif
}

// Update the motors to follow the sticks
void updateMotors()
{
  #ifdef ATTITUDE_MODE

    // Update the roll and pitch angles PIDs
    rollTarget = MAX_ROLL_PITCH * ailerons / 100.0;
    rollPID.update(
      rollTarget,
      IMU::getRoll(),
      IMU::getLoopTime()
    );
    pitchTarget = MAX_ROLL_PITCH * elevator / 100.0;
    pitchPID.update(
      pitchTarget,
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
    rollTarget = MAX_ROLL_PITCH_RATE * ailerons / 100.0;
    rollRatePID.update(
      rollTarget,
      IMU::getRollRate(),
      IMU::getLoopTime()
    );
    pitchTarget = MAX_ROLL_PITCH_RATE * elevator / 100.0;
    pitchRatePID.update(
      pitchTarget,
      IMU::getPitchRate(),
      IMU::getLoopTime()
    );

  #endif

  // Update the yaw rate PID
  yawTarget = MAX_YAW_RATE * rudder / 100.0;
  yawRatePID.update(
    yawTarget,
    IMU::getYawRate(),
    IMU::getLoopTime()
  );

  // Compute the motor mix
  int16_t motor[4];
  motor[MOTOR_FRONT] = MOTOR_IDLE + pitchRatePID.getControl() - yawRatePID.getControl();
  motor[MOTOR_LEFT] = MOTOR_IDLE + rollRatePID.getControl() + yawRatePID.getControl();
  motor[MOTOR_BACK] = MOTOR_IDLE - pitchRatePID.getControl() - yawRatePID.getControl();
  motor[MOTOR_RIGHT] = MOTOR_IDLE - rollRatePID.getControl() + yawRatePID.getControl();

  // Limit the motors
  for(int i=0 ; i<4 ; i++)
  {
    if(motor[i] < MOTOR_MIN) motor[i] = MOTOR_MIN;
    else if(motor[i] > MOTOR_MAX) motor[i] = MOTOR_MAX;
  }

  // Calculate the throttle contribution
  int16_t throttleFix = THROTTLE_K * throttle;
  int16_t motorMin = motor[0], motorMax = motor[0];
  for(int i=1 ; i<4 ; i++)
  {
    if(motor[i] < motorMin) motorMin = motor[i];
    else if(motor[i] > motorMax) motorMax = motor[i];
  }
  if(motorMin + throttleFix < MOTOR_MIN) throttleFix = MOTOR_MIN - motorMin;
  else if(motorMax + throttleFix > MOTOR_MAX) throttleFix = MOTOR_MAX - motorMax;

  // Update the ESCs
  for(int i=0 ; i<4 ; i++) ESC::setChannel(i, motor[i] + throttleFix);
}

void setup()
{
  ESC::init();

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

      // If we could read the IMU update the motors
      if(readIMU)
      {
        updateMotors();

        #ifdef IMU_DEBUG
          IMU::debug();
        #endif
        #ifdef PID_DEBUG
          totalTime += IMU::getLoopTime();
          debugPID();
        #endif
      }
    }
  }

  // If could not read the sticks stop the motors
  else stopMotors();
}

#ifdef PID_DEBUG
  void debugPID()
  {
    // Motors
    Serial.print(ESC::getChannel(MOTOR_FRONT));
    Serial.print(",");
    Serial.print(ESC::getChannel(MOTOR_LEFT));
    Serial.print(",");
    Serial.print(ESC::getChannel(MOTOR_BACK));
    Serial.print(",");
    Serial.print(ESC::getChannel(MOTOR_RIGHT));
    Serial.print(",");

    // Attitude mode
    #ifdef ATTITUDE_MODE

      // Angular positions
      Serial.print(rollTarget, 3);
      Serial.print(",");
      Serial.print(IMU::getRoll(), 3);
      Serial.print(",");
      Serial.print(pitchTarget, 3);
      Serial.print(",");
      Serial.print(IMU::getPitch(), 3);
      Serial.print(",");

      // Angular rates
      Serial.print(rollPID.getControl(), 3);
      Serial.print(",");
      Serial.print(IMU::getRollRate(), 3);
      Serial.print(",");
      Serial.print(pitchPID.getControl(), 3);
      Serial.print(",");
      Serial.print(IMU::getPitchRate(), 3);
      Serial.print(",");

    #endif

    // Manual mode
    #ifndef ATTITUDE_MODE

      // Angular rates
      Serial.print(rollTarget, 3);
      Serial.print(",");
      Serial.print(IMU::getRollRate(), 3);
      Serial.print(",");
      Serial.print(pitchTarget, 3);
      Serial.print(",");
      Serial.print(IMU::getPitchRate(), 3);
      Serial.print(",");

    #endif

    // Yaw
    Serial.print(yawTarget, 3);
    Serial.print(",");
    Serial.print(IMU::getYawRate(), 3);
    Serial.print(",");

    // Time
    Serial.print(totalTime, 5);
    Serial.println("");
  }
#endif
