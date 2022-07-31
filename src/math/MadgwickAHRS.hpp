#pragma once

// MadgwickAHRS class. Implementation of Madgwick's IMU and AHRS algorithms.
// See: http://www.x-io.co.uk/node/8#open_source_ahrs_and_imu_algorithms
class MadgwickAHRS {
   public:
    // sample period
    float sample_period;

    // algorithm gain beta
    const float beta;

    // quaternion output
    float quaternion[4] = {1.0, 0.0, 0.0, 0.0};

    inline MadgwickAHRS(float sample_period, float beta = 1.0)
        : sample_period(sample_period), beta(beta) {}

    // Algorithm AHRS update method. Requires only gyroscope and accelerometer
    // Gyroscope measurements in radians/s.
    void Update(float gx, float gy, float gz, float ax, float ay, float az,
                float mx, float my, float mz);

    // Algorithm IMU update method. Requires only gyroscope and accelerometer
    // Gyroscope measurements in radians/s.
    void Update(float gx, float gy, float gz, float ax, float ay, float az);
};