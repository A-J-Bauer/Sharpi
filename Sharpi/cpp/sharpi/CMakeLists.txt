# CMakeList.txt : CMake project for Sharpi

cmake_minimum_required (VERSION 3.19)

# create sharpi.so
add_library(sharpi SHARED    
    sharpi.h
    sharpi.cpp
    DevSpi.h
    DevSpi.cpp
    DevI2c.h
    DevI2c.cpp
    DevGpio.h
    DevGpio.cpp
    Display.h    
    DisplayPcd8544.h
    DisplayPcd8544.cpp
    DisplaySsd1351.h
    DisplaySsd1351.cpp
    DisplayTm1637.h
    DisplayTm1637.cpp
    DisplayDrm.h
    DisplayDrm.cpp
    UsbWorker.h
    UsbWorker.cpp
    SysPwm.h
    SysPwm.cpp
    Info.h
    Info.cpp
    Adc.h
    AdcMcp3008.h
    AdcMcp3008.cpp
    Sensor.h
    SensorAmg8833.h
    SensorAmg8833.cpp
)

target_include_directories(sharpi
PUBLIC    
    "include"
    "skia"
    "gpiod/include"
    "include/libdrm"
    "i2c/include"    
)

target_link_directories(sharpi
PUBLIC
    "skia/lib"
    "gpiod/lib"
    "i2c/lib"
)

target_link_libraries(sharpi 
# static .a
pthread
jpeg
png
z
compression_utils_portable
dng_sdk
particles
pathkit
piex
sfntly
skia
skottie
skresources
sksg
skshaper
skunicode
svg
freetype
fontconfig
gpiod
i2c
# shared .so
drm
)

set_target_properties(sharpi PROPERTIES
LIBRARY_OUTPUT_DIRECTORY "../../so"
)

# -------------------------------

add_executable(capi
    sharpi.cpp
    sharpi.h
    main.cpp
)

target_link_libraries(capi 
    sharpi
)

set_target_properties(capi
PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../../bin"
)


