set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CPU_COMPILE_FLAGS_M0}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CPU_COMPILE_FLAGS_M0}")

file(GLOB_RECURSE SOURCES "*.c" "*.cpp")

set(SOURCES ${SOURCES}
${CMAKE_CURRENT_SOURCE_DIR}/../common/src/debug_frmwrk.c
${CMAKE_CURRENT_SOURCE_DIR}/../common/src/ipc_mbx.c
${CMAKE_CURRENT_SOURCE_DIR}/../common/src/lpc43xx_uart.c
        )

add_library(libpixy_m0 ${SOURCES})
target_compile_definitions(libpixy_m0 PUBLIC ${DEFINITIONS_M0})

# without warnings
foreach(file ${SOURCES})
    set_source_files_properties(${file} PROPERTIES COMPILE_FLAGS "-w")
endforeach(file)

target_include_directories(libpixy_m0 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_include_directories(libpixy_m0 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/RTE/_libpixy_m0)
target_include_directories(libpixy_m0 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../common_up/inc)
target_include_directories(libpixy_m0 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../common/inc)
