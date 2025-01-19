add_executable(udp_basic
    drone_udp.c 
       
	)
target_compile_definitions(udp_basic PRIVATE
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
        )
target_include_directories(udp_basic PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
        )
target_link_libraries(udp_basic
        pico_cyw43_arch_lwip_threadsafe_background
        pico_stdlib
        )

# enable usb output, disable uart output
pico_enable_stdio_usb(udp_basic 1)
pico_enable_stdio_uart(udp_basic 0)

pico_add_extra_outputs(udp_basic)
