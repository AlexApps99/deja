cmake_minimum_required(VERSION 3.13)

# initialize pico-sdk from submodule
# note: this must happen before project()
include(ext/pico-sdk/pico_sdk_init.cmake)

project(deja CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# RF24
include(ext/RF24/CMakeLists.txt)

# nanopb
set(NANOPB_SRC_ROOT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/ext/nanopb)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${NANOPB_SRC_ROOT_FOLDER}/extra)
find_package(Nanopb REQUIRED)
set(NANOPB_OPTIONS "--cpp-descriptors")
nanopb_generate_cpp(PROTO_SRCS PROTO_HDRS RELPATH . msg.proto)

file(GLOB_RECURSE drone_SRC CONFIGURE_DEPENDS "src/*.h" "src/*.c" "src/*.hpp" "src/*.cpp")

add_executable(drone
    ${drone_SRC}
    ext/motion_driver/inv_mpu.c
    ext/motion_driver/inv_mpu.h
    ext/BMP2-Sensor-API/bmp2.c
    ext/BMP2-Sensor-API/bmp2.h
    ext/BMP2-Sensor-API/bmp2_defs.h
    ${PROTO_SRCS} ${PROTO_HDRS}
)
target_include_directories(drone PRIVATE
    ${NANOPB_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
    ext/motion_driver/
    ext/BMP2-Sensor-API/
)

target_compile_definitions(drone PRIVATE
    NUM_MOTORS=6

    PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS=10000

    PB_ENABLE_MALLOC=1
    PB_CONVERT_DOUBLE_FLOAT=1

    BMP2_64BIT_COMPENSATION=1

    PICO_DEFAULT_UART=0
    PICO_DEFAULT_I2C=0
    PICO_DEFAULT_SPI=0

    PIN_PWM0=0
    PICO_DEFAULT_I2C_SCL_PIN=1
    PIN_PWM1=2
    PIN_HC_SR04_TRIG=3
    PIN_PWM2=4
    PIN_UART1_RX=5
    PIN_PWM3=6
    PIN_HC_SR04_ECHO=7
    PIN_UART1_TX=8
    PIN_PWM4=9
    PIN_PWM5=10
    PIN_NRF24_IRQ=11
    PIN_PWM6=12
    PIN_NRF24_CE=13
    PIN_PWM7=14
    PIN_ACTION=15
    PICO_DEFAULT_UART_TX_PIN=16
    PICO_DEFAULT_UART_RX_PIN=17
    PICO_DEFAULT_SPI_SCK_PIN=18
    PICO_DEFAULT_SPI_TX_PIN=19
    PICO_DEFAULT_SPI_RX_PIN=20
    PICO_DEFAULT_SPI_CSN_PIN=21
    PIN_MPU9250_INT=22
    PIN_PM_VOLTAGE=26
    PIN_PM_CURRENT=27
    PICO_DEFAULT_I2C_SDA_PIN=28
)

pico_generate_pio_header(drone ${CMAKE_CURRENT_LIST_DIR}/hc_sr04.pio)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(drone
    pico_stdlib
    pico_stdio
    hardware_spi
    hardware_pwm
    hardware_pio
    hardware_adc
    hardware_i2c
    pico_stdio_usb
    RF24
)

pico_enable_stdio_usb(drone 1)
pico_enable_stdio_uart(drone 0)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(drone)
