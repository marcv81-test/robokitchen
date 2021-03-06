#define I2C_SPEED 400000

#define IMU_ACCEL_X MPU6050::getAccelX()
#define IMU_ACCEL_Y MPU6050::getAccelY()
#define IMU_ACCEL_Z MPU6050::getAccelZ()
#define IMU_MAGNET_X -HMC5883L::getMagnetY()
#define IMU_MAGNET_Y HMC5883L::getMagnetX()
#define IMU_MAGNET_Z HMC5883L::getMagnetZ()
#define IMU_GYRO_X MPU6050::getGyroX()
#define IMU_GYRO_Y MPU6050::getGyroY()
#define IMU_GYRO_Z MPU6050::getGyroZ()

#define IMU_GYRO_CALIBRATION

#define IMU_ACCEL_ENABLE
#define IMU_ACCEL_COEFF 0.01

#define IMU_MAGNET_ENABLE
#define IMU_MAGNET_COEFF 0.1
