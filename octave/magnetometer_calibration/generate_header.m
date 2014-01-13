addpath("../ellipsoid_fit/")

load data.csv;
[hard, soft, error] = magnetometer_calibration(data);

% generate the header
disp(sprintf("// Radius error standard deviation: %f", std(error)))
disp("")
disp(sprintf("#define MAGNET_HARD_X %f", hard(1)))
disp(sprintf("#define MAGNET_HARD_Y %f", hard(2)))
disp(sprintf("#define MAGNET_HARD_Z %f", hard(3)))
disp("")
disp(sprintf("#define MAGNET_SOFT_XX %f", soft(1,1)))
disp(sprintf("#define MAGNET_SOFT_XY %f", soft(1,2)))
disp(sprintf("#define MAGNET_SOFT_XZ %f", soft(1,3)))
disp(sprintf("#define MAGNET_SOFT_YX %f", soft(2,1)))
disp(sprintf("#define MAGNET_SOFT_YY %f", soft(2,2)))
disp(sprintf("#define MAGNET_SOFT_YZ %f", soft(2,3)))
disp(sprintf("#define MAGNET_SOFT_ZX %f", soft(3,1)))
disp(sprintf("#define MAGNET_SOFT_ZY %f", soft(3,2)))
disp(sprintf("#define MAGNET_SOFT_ZZ %f", soft(3,3)))
disp("")
disp("#define IMU_MAGNET_CALIBRATED")
