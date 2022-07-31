#include <hardware/gpio.h>
#include <inv_mpu.h>
#include <pico/binary_info.h>
#include <pico/sync.h>

#include <memory>

#include "../common.hpp"
#include "i2c.hpp"

bi_decl(bi_1pin_with_name(PIN_MPU9250_INT, "MPU9250 INT"));

// You can only have ONE INSTANCE of this at a time
// TODO modify library to fix that

// I can't use FIFO because it doesn't seem to support compass
// however, 0x49/73 seems to be allowable on the FIFO?
// I need to test it, obviously (not a priority)
class MPU9250 {
   public:
    static inline MPU9250& instance() { return *mpu9250; }
    static inline void setup(I2C& i2c, u32 int_p) {
        if (!mpu9250) mpu9250 = new MPU9250(i2c, int_p);
    }
    static inline void destroy() {
        if (mpu9250) {
            delete mpu9250;
            mpu9250 = nullptr;
        }
    }

    ~MPU9250() {}

   private:
    MPU9250(I2C& i2c, u32 int_p) : i2c(i2c), int_p(int_p) {
        // TODO error checking
        mpu_init(i2c.i2c);
        // mpu_set_gyro_fsr(2000);
        // mpu_set_accel_fsr(2);
        // mpu_set_lpf(42);
        // mpu_set_sample_rate(50);
        // No compass :(
        mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);
        mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS);

        // int mpu_get_gyro_reg(short *data, unsigned long *timestamp);
        // int mpu_get_accel_reg(short *data, unsigned long *timestamp);
        // int mpu_get_compass_reg(short *data, unsigned long *timestamp);
        // int mpu_get_temperature(long *data, unsigned long *timestamp);

        // int mpu_read_fifo(short *gyro, short *accel, unsigned long
        // *timestamp,
        //                   unsigned char *sensors, unsigned char *more);
        // int mpu_read_fifo_stream(unsigned short length, unsigned char *data,
        //                          unsigned char *more);
        // int mpu_reset_fifo(void);
    }

    static void interrupt_handler() {
        if (mpu9250) {
        }
    }

    I2C& i2c;
    u32 int_p;

    static MPU9250* mpu9250;
};

MPU9250* MPU9250::mpu9250 = nullptr;