#ifndef QUAD_MOTOR_H
#define QUAD_MOTOR_H

#define MOTOR_OFF 100
#define MOTOR_STALL 150
#define MOTOR_MIN 200
#define MOTOR_IDLE 1000
#define MOTOR_MAX 2000

#define MOTOR_FRONT 3
#define MOTOR_LEFT 0
#define MOTOR_BACK 1
#define MOTOR_RIGHT 2

class QuadMotor
{
  public:
    static void init();
    static void stop();
    static void mix(float throttle, float roll, float pitch, float yaw);
};

#endif // QUAD_MOTOR_H
