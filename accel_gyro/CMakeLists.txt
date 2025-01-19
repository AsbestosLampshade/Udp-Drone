set(i2c_default i2c0)
set(PICO_DEFAULT_I2C_SDA_PIN 6)
set(PICO_DEFAULT_I2C_SCL_PIN 7) 

add_executable(gyro
    gyro_original.c
    )

# pull in common dependencies
target_link_libraries(gyro pico_stdlib hardware_i2c)

pico_enable_stdio_usb(gyro 1)
pico_enable_stdio_uart(gyro 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(gyro)

