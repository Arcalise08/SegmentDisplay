cmake_minimum_required (VERSION 3.8)

include(C:/VSARM/sdk/pico-sdk/external/pico_sdk_import.cmake)
add_executable (segmentDisplay segmentDisplay.cpp)

target_link_libraries(segmentDisplay pico_stdlib hardware_pio hardware_spi)

project(segmentDisplay)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()
pico_add_extra_outputs(segmentDisplay)
