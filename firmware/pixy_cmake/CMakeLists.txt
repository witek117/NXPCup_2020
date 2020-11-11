cmake_minimum_required(VERSION 3.1)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(TOOLCHAIN_PREFIX "arm-none-eabi-"  CACHE INTERNAL "BOB selected Toolchain prefix" FORCE)

set(DEFINITIONS_M0 CORE_M0 __USE_LPCOPEN CPP_USE_HEAP __LPC43XX__ NO_BOARD_LIB __MULTICORE_M0APP CORE_M0APP IPC_SLAVE)
set(DEFINITIONS_M4 CORE_M4 __USE_LPCOPEN CPP_USE_HEAP __LPC43XX__ NO_BOARD_LIB __MULTICORE_M0APP __MULTICORE_MASTER __MULTICORE_MASTER_SLAVE_M0APP IPC_MASTER)

set(FLAGS_M4 -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp)
set(FLAGS_M0 -mcpu=cortex-m0)
set(FLAGS_COMMON -mthumb -ffunction-sections -fdata-sections  -Wl,--gc-sections -Os -fno-common -g3 -fmessage-length=0 -fno-builtin -fno-rtti -fno-exceptions -fmerge-constants -fstack-usage)

set(CPU_COMPILE_FLAGS_M0 "-mthumb -mcpu=cortex-m0 -ffunction-sections -fdata-sections  -Wl,--gc-sections")
set(CPU_COMPILE_FLAGS_M0 "${CPU_COMPILE_FLAGS_M0}   -DCORE_M0 -D__USE_LPCOPEN -DCPP_USE_HEAP -D__LPC43XX__ -DNO_BOARD_LIB -D__MULTICORE_M0APP -DCORE_M0APP")
set(CPU_COMPILE_FLAGS_M0 "${CPU_COMPILE_FLAGS_M0} -Og -fno-common -g3 -fmessage-length=0 -fno-builtin -fno-rtti -fno-exceptions -fmerge-constants -fstack-usage")

set(CPU_COMPILE_FLAGS_M4 "-mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -ffunction-sections -fdata-sections -Wl,--gc-sections")
set(CPU_COMPILE_FLAGS_M4 "${CPU_COMPILE_FLAGS_M4}  -DCORE_M4 -D__USE_LPCOPEN -DCPP_USE_HEAP -D__LPC43XX__ -DNO_BOARD_LIB -DLPC43_MULTICORE_M0APP -D__MULTICORE_MASTER -D__MULTICORE_MASTER_SLAVE_M0APP")
set(CPU_COMPILE_FLAGS_M4 "${CPU_COMPILE_FLAGS_M4} -Og -fno-common -g3  -fmessage-length=0 -fno-builtin -fno-rtti -fno-exceptions -fsingle-precision-constant -fmerge-constants -fstack-usage")

set(CMAKE_ASM_COMPILER "${TOOLCHAIN_PREFIX}gcc" CACHE STRING "ASM Compiler selected by BOB" FORCE)
set(CMAKE_C_COMPILER "${TOOLCHAIN_PREFIX}gcc" CACHE STRING "C Compiler selected by BOB" FORCE)
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_PREFIX}g++" CACHE STRING "C++ Compiler selected by BOB" FORCE)
set(CMAKE_AR "${TOOLCHAIN_PREFIX}gcc-ar" CACHE STRING "Archiver Compiler selected by BOB" FORCE)
set(CMAKE_OBJCOPY "${TOOLCHAIN_PREFIX}objcopy" CACHE STRING "Objcopy Compiler selected by BOB" FORCE)

set(CMAKE_C_FLAGS ${CPU_COMPILE_FLAGS} CACHE STRING "CFLAGS" FORCE)
set(CMAKE_CXX_FLAGS ${CPU_COMPILE_FLAGS} CACHE STRING "CXXFLAGS" FORCE)
set(CMAKE_ASM_FLAGS ${CPU_COMPILE_FLAGS} CACHE STRING "ASMFLAGS" FORCE)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")  # reasonable and standard
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wformat=2") # warn if float is implicit promoted to double
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-parameter")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-variable")


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra") # reasonable and standard
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow") # warn the user if a variable declaration shadows one from a parent context
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor") # warn the user if a class with virtual functions has a non-virtual destructor. This helps catch hard to track down memory errors
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-align") # warn for potential performance problem casts
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused") # warn on anything being unused
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Woverloaded-virtual") # warn if you overload (not override) a virtual function
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpedantic") # warn if non-standard C++ is used
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion") # warn on type conversions that may lose data
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsign-conversion") # warn on sign conversions
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wduplicated-cond") # warn if if / else chain has duplicated conditions
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wduplicated-branches") # warn if if / else branches have duplicated code
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wlogical-op") # warn about logical operations being used where bitwise were probably wanted
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnull-dereference") # warn if a null dereference is detected
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wdouble-promotion") # warn if float is implicit promoted to double
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat=2") # warn if float is implicit promoted to double
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-variable")



set(OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/build/LPC)
set(REPORTS_PATH ${OUTPUT_PATH}/reports)

file(MAKE_DIRECTORY ${OUTPUT_PATH})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_PATH}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_PATH}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_PATH}/bin)


project(PIXY C CXX ASM)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)


add_subdirectory(libpixy_m0)
add_subdirectory(main_m0)

add_subdirectory(libpixy_m4)
add_subdirectory(main_m4)
