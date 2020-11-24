add_library(NXP_hal STATIC  ${CMAKE_CURRENT_LIST_DIR}/NXP_adc.cpp 
                            ${CMAKE_CURRENT_LIST_DIR}/NXP_camera.cpp
                            ${CMAKE_CURRENT_LIST_DIR}/NXP_display.cpp
                            ${CMAKE_CURRENT_LIST_DIR}/NXP_DMA.cpp
                            ${CMAKE_CURRENT_LIST_DIR}/NXP_gpio.cpp
                            ${CMAKE_CURRENT_LIST_DIR}/NXP_I2C.cpp
                            ${CMAKE_CURRENT_LIST_DIR}/NXP_Kitty.cpp
                            ${CMAKE_CURRENT_LIST_DIR}/NXP_menu.cpp
                            ${CMAKE_CURRENT_LIST_DIR}/NXP_motor.cpp
                            ${CMAKE_CURRENT_LIST_DIR}/NXP_PIT.cpp
                            ${CMAKE_CURRENT_LIST_DIR}/NXP_pwm.cpp
                            ${CMAKE_CURRENT_LIST_DIR}/NXP_servo.cpp
                            ${CMAKE_CURRENT_LIST_DIR}/NXP_uart.cpp
                            ${CMAKE_CURRENT_LIST_DIR}/VL53L0X.cpp
                            ${CMAKE_CURRENT_LIST_DIR}/NXP_pixy.cpp 
                            )
target_include_directories(NXP_hal PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(NXP_hal drivers debug_module halina board algorithms) 