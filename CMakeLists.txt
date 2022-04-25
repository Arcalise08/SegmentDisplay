cmake_minimum_required (VERSION 3.8)

include(C:/VSARM/sdk/pico/pico-sdk/external/pico_sdk_import.cmake)

# Add source to this project's executable.
# add_executable (picoPika "picoPika.cpp" "picoPika.h" realPicoPika.cpp realPicoPika.h)
add_executable (test test.cpp)



# Create C header file with the name <pio program>.pio.h

# pull in common dependencies
target_link_libraries(test pico_stdlib hardware_pio hardware_spi)

project(test)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()
pico_add_extra_outputs(test)
