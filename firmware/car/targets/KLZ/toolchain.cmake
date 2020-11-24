INCLUDE(CMakeForceCompiler)

set(CMAKE_SYSTEM_NAME Generic)

find_program(CC NAMES arm-none-eabi-gcc PATHS $ENV{ARM_TOOLCHAIN})
find_program(CXX NAMES arm-none-eabi-g++ PATHS $ENV{ARM_TOOLCHAIN})

find_program(CMAKE_GCOV NAMES arm-none-eabi-gcov PATHS $ENV{ARM_TOOLCHAIN})
find_program(CMAKE_AR NAMES arm-none-eabi-gcc-ar PATHS $ENV{ARM_TOOLCHAIN})
find_program(CMAKE_RANLIB NAMES arm-none-eabi-gcc-ranlib PATHS $ENV{ARM_TOOLCHAIN})
find_program(CMAKE_OBJCOPY NAMES arm-none-eabi-objcopy PATHS $ENV{ARM_TOOLCHAIN})
find_program(CMAKE_OBJDUMP NAMES arm-none-eabi-objdump PATHS $ENV{ARM_TOOLCHAIN})
find_program(CMAKE_GCC_SIZE NAMES arm-none-eabi-size PATHS $ENV{ARM_TOOLCHAIN})
find_program(CMAKE_MAKE_PROGRAM NAMES make PATHS $ENV{ARM_TOOLCHAIN})

find_program(JLINK NAMES JLink JLinkExe PATHS $ENV{ARM_TOOLCHAIN} $ENV{JLINK_PATH})
find_program(JLINKSWO NAMES JLinkSWOViewerCL PATHS $ENV{ARM_TOOLCHAIN} $ENV{JLINK_PATH})
find_program(JLinkGDBServer NAMES JLinkGDBServerCL PATHS $ENV{ARM_TOOLCHAIN} $ENV{JLINK_PATH})

set(CMAKE_EXECUTABLE_FORMAT ELF)

find_program(CLANG_FORMAT NAMES clang-format HINTS $ENV{CLANG_PATH})

message(STATUS "Using C compiler from ${CMAKE_C_COMPILER}")
message(STATUS "Using C++ compiler from ${CMAKE_CXX_COMPILER}")