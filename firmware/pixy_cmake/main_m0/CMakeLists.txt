set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CPU_COMPILE_FLAGS_M0}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CPU_COMPILE_FLAGS_M0}")

file(GLOB_RECURSE SOURCES "*.c" "*.cpp" "*.s")

# without warnings
foreach(file ${SOURCES})
    set_source_files_properties(${file} PROPERTIES COMPILE_FLAGS "-w")
endforeach(file)

add_executable(main_m0 ${SOURCES})

target_include_directories(libpixy_m0 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_include_directories(libpixy_m0 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/RTE/_main_m0)
target_include_directories(libpixy_m0 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../common_up/inc)
target_include_directories(libpixy_m0 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../common/inc)


set_target_properties(main_m0 PROPERTIES LINK_FLAGS
        "-T ${CMAKE_CURRENT_SOURCE_DIR}/pixy_board_m0_Debug.ld  ${CORE_M0_FLAGS} -L ${CMAKE_CURRENT_SOURCE_DIR}/Debug -Xlinker -Map=main_m0.map -Xlinker -print-memory-usage -flto -Wl,--gc-sections -specs=nano.specs -flto -lc")

target_link_libraries(main_m0 libpixy_m0)
target_compile_definitions(main_m0 PUBLIC ${DEFINITIONS_M0})

add_custom_command(TARGET main_m0
        POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/main_m0 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/main_m0.elf
        COMMAND arm-none-eabi-objcopy
        --target elf32-littlearm
        --verbose
        --strip-all
        --remove-section=.ARM.attributes
        --redefine-sym __vectors_start__=__vectors_start___core_m0app
        --keep-symbol __vectors_start___core_m0app
        --redefine-sym _data=__start_data_core_m0app
        --keep-symbol __start_data_core_m0app
        --redefine-sym _edata=__end_data_core_m0app
        --keep-symbol __end_data_core_m0app
        --remove-section=".bss*"
        --remove-section=".noinit*"
        --rename-section .ARM.exidx=".core_m0app.ARM.exidx"
        --rename-section .ARM.extab=".core_m0app.ARM.extab"
        --rename-section .text=".core_m0app"
        --rename-section .data=".core_m0app.data"
        --rename-section .data_RAM2=".core_m0app.data_RAM2"
        --rename-section .data_RAM3=".core_m0app.data_RAM3"
        --rename-section .data_RAM4=".core_m0app.data_RAM4"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/main_m0" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/main_m0.o")