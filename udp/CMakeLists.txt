add_executable(udp
    drone_udp.c 
       
	)
target_compile_definitions(udp PRIVATE
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
        )
target_include_directories(udp PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
        )
target_link_libraries(udp
        pico_cyw43_arch_lwip_threadsafe_background
        pico_stdlib
        )

# enable usb output, disable uart output
pico_enable_stdio_usb(udp 1)
pico_enable_stdio_uart(udp 0)

pico_add_extra_outputs(udp)
