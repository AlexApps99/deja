#pragma once

// MadgwickAHRS class. Implementation of Madgwick's IMU and AHRS algorithms.
// See: http://www.x-io.co.uk/node/8#open_source_ahrs_and_imu_algorithms
class MadgwickAHRS {
   public:
    using Num = double;
    // sample period
    Num sample_period;

    // algorithm gain beta
    const Num beta;

    // quaternion output
    Num quaternion[4] = {1.0, 0.0, 0.0, 0.0};

    inline MadgwickAHRS(Num sample_period, Num beta = 1.0)
        : sample_period(sample_period), beta(beta) {}

    // Algorithm AHRS update method. Gyroscope measurements in radians/s, others
    // are normalized so any units
    void Update(Num gx, Num gy, Num gz, Num ax, Num ay, Num az, Num mx, Num my,
                Num mz);
};