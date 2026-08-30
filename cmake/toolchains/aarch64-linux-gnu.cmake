# Generic ARM64 Embedded Linux toolchain template.
# Requires an ARM64 sysroot and a Qt 6 build for the target architecture.
# Example environment variables:
#   export AARCH64_SYSROOT=/opt/sysroots/aarch64
#   export AARCH64_QT_PREFIX=/opt/qt6-aarch64

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)

if(DEFINED ENV{AARCH64_SYSROOT})
    set(CMAKE_SYSROOT "$ENV{AARCH64_SYSROOT}")
endif()

if(DEFINED ENV{AARCH64_QT_PREFIX})
    list(PREPEND CMAKE_PREFIX_PATH "$ENV{AARCH64_QT_PREFIX}")
endif()

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
