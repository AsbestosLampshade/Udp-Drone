add_executable(drone_tcp
        drone_udp.c
        auxiliary_funcs.h
        auxiliary_funcs.c
		gyro.h
		gyro.c
        )
target_compile_definitions(drone_tcp PRIVATE
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
        )
target_include_directories(drone_tcp PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
        )
target_link_libraries(drone_tcp
        pico_cyw43_arch_lwip_threadsafe_background
        pico_stdlib
        hardware_pwm
	hardware_i2c
        hardware_adc
        )

# enable usb output, disable uart output
pico_enable_stdio_usb(drone_tcp 1)
pico_enable_stdio_uart(drone_tcp 0)

pico_add_extra_outputs(drone_tcp)
