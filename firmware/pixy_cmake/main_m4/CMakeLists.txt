set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CPU_COMPILE_FLAGS_M4}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CPU_COMPILE_FLAGS_M4}")

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAGS_M4} ${FLAGS_COMMON} ${DEFINITIONS_M4}")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS_M4} ${FLAGS_COMMON} ${DEFINITIONS_M4}")

file(GLOB_RECURSE SOURCES "*.c" "*.cpp")


set(SOURCES ${SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/../common_up/src/qqueue.cpp
        )


# without warnings
#foreach(file ${SOURCES})
#    set_source_files_properties(${file} PROPERTIES COMPILE_FLAGS "-w")
#endforeach(file)

set_source_files_properties(src/syscalls.c PROPERTIES COMPILE_FLAGS "-w")

SET(M0_OBJ ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/main_m0.o)
SET_SOURCE_FILES_PROPERTIES(${M0_OBJ} PROPERTIES EXTERNAL_OBJECT true GENERATED true)


add_executable(pixy ${SOURCES} ${M0_OBJ})
target_compile_definitions(pixy PUBLIC ${DEFINITIONS_M4})


target_include_directories(pixy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_include_directories(pixy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../common_up/inc)
target_include_directories(pixy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../common/inc)


#target_compile_options(pixy PRIVATE ${FLAGS_M4} ${FLAGS_COMMON})
#target_compile_definitions(pixy PRIVATE ${DEFINITIONS_M4})

target_include_directories(pixy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
#${CORE_M4_FLAGS}
set_target_properties(pixy PROPERTIES LINK_FLAGS
        "-T ${CMAKE_CURRENT_SOURCE_DIR}/pixy_board_Debug.ld  -Xlinker -Map=pixy.map -Xlinker -print-memory-usage -flto -Wl,--gc-sections -specs=nano.specs -flto -lc")

#set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "${FLAGS_COMMON}")

target_link_libraries(pixy libpixy_m4 )

add_custom_command(TARGET pixy
        POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -v -O ihex "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/pixy" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/pixy_firmware.hex"
        COMMAND ${CMAKE_OBJCOPY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/pixy ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/pixy_firmware.elf)

add_dependencies(pixy main_m0)