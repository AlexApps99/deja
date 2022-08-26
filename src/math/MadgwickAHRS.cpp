#include "MadgwickAHRS.hpp"

#include <math.h>

#include "../common.hpp"

// handle NaN, use reciprocal for division
#define rsqrt(v)              \
    Ldebug(">%f", v);         \
    if (v < 1e-10) {          \
        return;               \
    } else {                  \
        v = sqrt(v);          \
        Ldebug("~%f", v);     \
        v = 1. / v;           \
        Ldebug(".%f", v);     \
        if (v > 1e10) return; \
    }

void MadgwickAHRS::Update(Num gx, Num gy, Num gz, Num ax, Num ay, Num az,
                          Num mx, Num my, Num mz) {
    Num q1 = quaternion[0], q2 = quaternion[1], q3 = quaternion[2],
        q4 = quaternion[3];  // short name local variable for readability
    Ldebug("%f %f %f, %f %f %f, %f %f %f (%f %f %f %f)", gx, gy, gz, ax, ay, az,
           mx, my, mz, q1, q2, q3, q4);
    Num norm;
    Num hx, hy, _2bx, _2bz;
    Num s1, s2, s3, s4;
    Num qDot1, qDot2, qDot3, qDot4;

    // Auxiliary variables to avoid repeated arithmetic
    Num _2q1mx;
    Num _2q1my;
    Num _2q1mz;
    Num _2q2mx;
    Num _4bx;
    Num _4bz;
    Num _2q1 = 2. * q1;
    Num _2q2 = 2. * q2;
    Num _2q3 = 2. * q3;
    Num _2q4 = 2. * q4;
    Num _2q1q3 = 2. * q1 * q3;
    Num _2q3q4 = 2. * q3 * q4;
    Num q1q1 = q1 * q1;
    Num q1q2 = q1 * q2;
    Num q1q3 = q1 * q3;
    Num q1q4 = q1 * q4;
    Num q2q2 = q2 * q2;
    Num q2q3 = q2 * q3;
    Num q2q4 = q2 * q4;
    Num q3q3 = q3 * q3;
    Num q3q4 = q3 * q4;
    Num q4q4 = q4 * q4;

    // Normalise accelerometer measurement
    norm = ax * ax + ay * ay + az * az;
    rsqrt(norm);
    ax *= norm;
    ay *= norm;
    az *= norm;

    // Normalise magnetometer measurement
    norm = mx * mx + my * my + mz * mz;
    rsqrt(norm);
    mx *= norm;
    my *= norm;
    mz *= norm;

    // Reference direction of Earth's magnetic field
    _2q1mx = 2. * q1 * mx;
    _2q1my = 2. * q1 * my;
    _2q1mz = 2. * q1 * mz;
    _2q2mx = 2. * q2 * mx;
    hx = mx * q1q1 - _2q1my * q4 + _2q1mz * q3 + mx * q2q2 + _2q2 * my * q3 +
         _2q2 * mz * q4 - mx * q3q3 - mx * q4q4;
    hy = _2q1mx * q4 + my * q1q1 - _2q1mz * q2 + _2q2mx * q3 - my * q2q2 +
         my * q3q3 + _2q3 * mz * q4 - my * q4q4;
    _2bx = sqrt(hx * hx + hy * hy);
    _2bz = -_2q1mx * q3 + _2q1my * q2 + mz * q1q1 + _2q2mx * q4 - mz * q2q2 +
           _2q3 * my * q4 - mz * q3q3 + mz * q4q4;
    _4bx = 2. * _2bx;
    _4bz = 2. * _2bz;

    // Gradient decent algorithm corrective step
    s1 = -_2q3 * (2. * q2q4 - _2q1q3 - ax) + _2q2 * (2. * q1q2 + _2q3q4 - ay) -
         _2bz * q3 * (_2bx * (.5 - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) +
         (-_2bx * q4 + _2bz * q2) *
             (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) +
         _2bx * q3 * (_2bx * (q1q3 + q2q4) + _2bz * (.5 - q2q2 - q3q3) - mz);
    s2 = _2q4 * (2. * q2q4 - _2q1q3 - ax) + _2q1 * (2. * q1q2 + _2q3q4 - ay) -
         4. * q2 * (1. - 2. * q2q2 - 2. * q3q3 - az) +
         _2bz * q4 * (_2bx * (.5 - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) +
         (_2bx * q3 + _2bz * q1) *
             (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) +
         (_2bx * q4 - _4bz * q2) *
             (_2bx * (q1q3 + q2q4) + _2bz * (.5 - q2q2 - q3q3) - mz);
    s3 = -_2q1 * (2. * q2q4 - _2q1q3 - ax) + _2q4 * (2. * q1q2 + _2q3q4 - ay) -
         4. * q3 * (1. - 2. * q2q2 - 2. * q3q3 - az) +
         (-_4bx * q3 - _2bz * q1) *
             (_2bx * (.5 - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) +
         (_2bx * q2 + _2bz * q4) *
             (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) +
         (_2bx * q1 - _4bz * q3) *
             (_2bx * (q1q3 + q2q4) + _2bz * (.5 - q2q2 - q3q3) - mz);
    s4 = _2q2 * (2. * q2q4 - _2q1q3 - ax) + _2q3 * (2. * q1q2 + _2q3q4 - ay) +
         (-_4bx * q4 + _2bz * q2) *
             (_2bx * (.5 - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) +
         (-_2bx * q1 + _2bz * q3) *
             (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) +
         _2bx * q2 * (_2bx * (q1q3 + q2q4) + _2bz * (.5 - q2q2 - q3q3) - mz);
    // normalise step magnitude
    norm = s1 * s1 + s2 * s2 + s3 * s3 + s4 * s4;
    rsqrt(norm);
    s1 *= norm;
    s2 *= norm;
    s3 *= norm;
    s4 *= norm;

    // Compute rate of change of quaternion
    qDot1 = .5 * (-q2 * gx - q3 * gy - q4 * gz) - beta * s1;
    qDot2 = .5 * (q1 * gx + q3 * gz - q4 * gy) - beta * s2;
    qDot3 = .5 * (q1 * gy - q2 * gz + q4 * gx) - beta * s3;
    qDot4 = .5 * (q1 * gz + q2 * gy - q3 * gx) - beta * s4;

    // Integrate to yield quaternion
    q1 += qDot1 * sample_period;
    q2 += qDot2 * sample_period;
    q3 += qDot3 * sample_period;
    q4 += qDot4 * sample_period;
    // normalise quaternion
    norm = q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4;
    rsqrt(norm);
    quaternion[0] = q1 * norm;
    quaternion[1] = q2 * norm;
    quaternion[2] = q3 * norm;
    quaternion[3] = q4 * norm;
}