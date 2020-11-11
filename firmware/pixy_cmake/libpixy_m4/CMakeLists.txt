set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CPU_COMPILE_FLAGS_M4}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CPU_COMPILE_FLAGS_M4}")



file(GLOB_RECURSE SOURCES "*.c" "*.cpp")

set(SOURCES ${SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/../common/src/debug_frmwrk.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../common/src/ipc_mbx.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../common/src/lpc43xx_adc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../common/src/lpc43xx_cgu.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../common/src/lpc43xx_i2c.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../common/src/lpc43xx_scu.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../common/src/lpc43xx_ssp.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../common/src/lpc43xx_uart.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../common/src/lpc43xx_timer.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../common/src/system_LPC43xx.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../common_up/src/calc.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../common_up/src/chirp.cpp

        )


add_library(libpixy_m4 ${SOURCES})
target_compile_definitions(libpixy_m4 PUBLIC ${DEFINITIONS_M4})

# without warnings
foreach(file ${SOURCES})
    set_source_files_properties(${file} PROPERTIES COMPILE_FLAGS "-w")
endforeach(file)

target_include_directories(libpixy_m4 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_include_directories(libpixy_m4 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../common/inc)
target_include_directories(libpixy_m4 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../common_up/inc)