cmake_minimum_required(VERSION 3.10)
project(inet)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_BUILD_TYPE Release)

set(SYSROOT "${ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/sysroot")

include_directories(BEFORE SYSTEM
    "${ANDROID_NDK}/sources/third_party/openssl/include"
    "${SYSROOT}/usr/include/c++/v1"
    "${SYSROOT}/usr/include"
)

include_directories("include")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv8-a -Os -ffunction-sections -fdata-sections")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections,--strip-all")

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

set(SRC
    "main.cpp"
)

add_library(inet SHARED ${SRC})

target_link_libraries(inet
    ssl
    crypto
    log
)

target_compile_definitions(inet PRIVATE DEBUGLOGGING=0 CPPHTTPLIB_OPENSSL_SUPPORT=1)