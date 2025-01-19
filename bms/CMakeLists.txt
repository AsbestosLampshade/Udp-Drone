add_executable(bms bms.c)

target_link_libraries(bms pico_stdlib hardware_adc)

# enable uart output, disable usb output
pico_enable_stdio_uart(bms 0)
pico_enable_stdio_usb(bms 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(bms)
