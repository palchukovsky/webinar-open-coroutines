cmake_minimum_required(VERSION 3.5)
project(corutines)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------------------------------------------------------
# 3rd party libraries
#
find_package(Threads)
find_package(Boost 1.70.0 REQUIRED
  COMPONENTS system thread coroutine context)
find_package(OpenSSL REQUIRED)

# -----------------------------------------------------------------------------
# Common code to support examples
#
add_library(lib STATIC
  lib/download.hpp lib/download.cpp
  lib/download_async.hpp lib/download_async.cpp
  lib/download_result.hpp
  lib/client.hpp lib/client.cpp
  lib/main.cpp)
target_link_libraries(lib
  PUBLIC ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})

target_compile_features(lib PUBLIC cxx_std_20)
set_target_properties(lib PROPERTIES CXX_STANDARD_REQUIRED ON)

# -----------------------------------------------------------------------------
# Executables with examples
#
function(add_my_executable TARGET)
  add_executable(${TARGET} ${TARGET}.cpp)
  target_link_libraries(${TARGET}
    ${CMAKE_THREAD_LIBS_INIT}
    lib)
  target_compile_features(${TARGET} PUBLIC cxx_std_20)
  set_target_properties(${TARGET} PROPERTIES CXX_STANDARD_REQUIRED ON)
endfunction()

add_my_executable(1_sync)

add_my_executable(2-1_multi-thread)
add_my_executable(2-2_multi-thread-async)
add_my_executable(2-3_sync-async)

add_my_executable(3-2_co-simple)
add_my_executable(3-3_co-simple-sequence)
add_my_executable(3-4_co-download-sync)
add_my_executable(3-5_co-download-async)

add_my_executable(4-1_boost-multi-thread-server)
add_my_executable(4-2_boost-co-server)

add_my_executable(5-1_co-generator)
add_my_executable(5-2_co-generator-sequence)
