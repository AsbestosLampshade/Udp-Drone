cmake_minimum_required(VERSION 3.12)
set(PICO_TARGET rp2040)
set(PICO_BOARD pico_w)
set(PICO_BUILD Release)
set(WIFI_SSID Airtel_geor_5270)
set(WIFI_PASSWORD air38530)

# Pull in SDK (must be before project)
include(/home/alfaugus/raspberry/pico/pico-examples/pico_sdk_import.cmake)
include(/home/alfaugus/raspberry/pico/pico-examples/pico_extras_import_optional.cmake)

project(picow_tcpip_server_background C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "2.0.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 2.0.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})

# If you want debug output from USB (pass -DPICO_STDIO_USB=1) this ensures you don't lose any debug output while USB is set up
if (NOT DEFINED PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS)
    set(PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS 3000)
endif()

# Initialize the SDK
pico_sdk_init()

include(/home/alfaugus/raspberry/pico/pico-examples/example_auto_set_url.cmake)

function(add_subdirectory_exclude_platforms NAME)
    if (ARGN)
        if (PICO_PLATFORM IN_LIST ARGN)
            message("Skipping ${NAME} example which is unsupported on this platform")
            return()
        endif()
        foreach(PATTERN IN LISTS ARGN)
            string(REGEX MATCH "${PATTERN}" MATCHED "${PICO_PLATFORM}")
            if (MATCHED)
                message("Skipping ${NAME} example which is unsupported on this platform")
                return()
            endif()
        endforeach()
    endif()
    add_subdirectory(${NAME})
endfunction()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        )
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wno-maybe-uninitialized)
endif()
###############################################
##########Cmake 2
if (PICO_CYW43_SUPPORTED) # set by PICO_BOARD=pico_w
    if (NOT TARGET pico_cyw43_arch)
        message("Skipping Pico W examples as support is not available")
    else()

        if (DEFINED ENV{WIFI_SSID} AND (NOT WIFI_SSID))
            set(WIFI_SSID $ENV{WIFI_SSID})
            message("Using WIFI_SSID from environment ('${WIFI_SSID}')")
        endif()

        if (DEFINED ENV{WIFI_PASSWORD} AND (NOT WIFI_PASSWORD))
            set(WIFI_PASSWORD $ENV{WIFI_PASSWORD})
            message("Using WIFI_PASSWORD from environment")
        endif()

        set(WIFI_SSID "${WIFI_SSID}" CACHE INTERNAL "WiFi SSID for examples")
        set(WIFI_PASSWORD "${WIFI_PASSWORD}" CACHE INTERNAL "WiFi password for examples")

#
#        add_subdirectory(wifi)
#        if (NOT TARGET pico_btstack_base)
#            message("Skipping Pico W Bluetooth examples as support is not available")
#        else()
#            add_subdirectory(bt)
#        endif()
    endif()
endif()
########################################################
##############Cmake 3
set(WIFI_SSID "${WIFI_SSID}" CACHE INTERNAL "WiFi SSID for examples")
set(WIFI_PASSWORD "${WIFI_PASSWORD}" CACHE INTERNAL "WiFi password for examples")

add_subdirectory_exclude_platforms(tcp_server)

add_subdirectory_exclude_platforms(drone_tcp)

add_subdirectory_exclude_platforms(pwm)

add_subdirectory_exclude_platforms(bms)

add_subdirectory_exclude_platforms(accel_gyro)

add_subdirectory_exclude_platforms(udp)

add_subdirectory_exclude_platforms(udp_basic)
