cmake_minimum_required(VERSION 3.12)
project(Quiver)

set(CMAKE_CXX_STANDARD 17)

unset(CMAKE_C_COMPILER CACHE)
unset(CMAKE_CXX_COMPILER CACHE)

option(Android "Cross-arch build executable for android" OFF)
if(Android)
	set(ANDROID_ABI arm64-v8a)
	set(ANDROID_STL c++_static)
	set(ANDROID_PLATFORM 21)
	set(CMAKE_BUILD_TYPE Release)
	include(/home/user/android-ndk-r20b/build/cmake/android.toolchain.cmake)
	set(CMAKE_CXX_FLAGS "-w")
	#set(CMAKE_SYSTEM_PROCESSOR aarch64)
	set(CMAKE_C_COMPILER /home/user/android-ndk-r20b/toolchains/llvm/prebuilt/linux-x86_64/bin/armv7a-linux-androideabi21-clang)
	set(CMAKE_CXX_COMPILER /home/user/android-ndk-r20b/toolchains/llvm/prebuilt/linux-x86_64/bin/armv7a-linux-androideabi21-clang++)
	#set(CMAKE_EXE_LINKER_FLAGS "-static")
	add_definitions(-DAndroid -Dz_size_t=size_t)
	set(PNG_SHARED OFF)
	set(PNG_TESTS OFF)
	set(PNG_BUILD_ZLIB ON)
	add_subdirectory(libpng)
endif()

add_subdirectory(jsoncpp-1.8.4)

#set(CMAKE_EXE_LINKER_FLAGS -static)

include_directories(arg-parser/include jsoncpp-1.8.4/include)
add_executable(Quiver src/main.cpp)
target_link_libraries(Quiver jsoncpp_lib_static png z)
