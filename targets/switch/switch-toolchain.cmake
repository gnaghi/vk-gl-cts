# VK-GL-CTS Nintendo Switch Toolchain
# Included BEFORE project() via DEQP_TARGET_TOOLCHAIN
#
# Usage:
#   cmake .. -DDEQP_TARGET=switch -DDEQP_TARGET_TOOLCHAIN=switch-toolchain

# Include devkitPro's official Switch CMake toolchain
# This sets up aarch64-none-elf-gcc/g++, Switch platform flags, elf2nro, etc.
include($ENV{DEVKITPRO}/cmake/Switch.cmake)

# Override delibs platform detection for Switch
# Switch uses libnx which provides POSIX-compatible pthreads, semaphores, etc.
set(DE_OS "DE_OS_UNIX")
set(DE_COMPILER "DE_COMPILER_GCC")
set(DE_CPU "DE_CPU_ARM_64")

# Switch cannot produce standard executables for try_compile
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
