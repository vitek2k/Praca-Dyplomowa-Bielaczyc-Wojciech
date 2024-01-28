
file(GLOB_RECURSE FIRMWARE_COMMON_SRC
       ${PROJECT_SOURCE_DIR}/*.cpp
       ${PROJECT_SOURCE_DIR}/*.hpp
)

add_library(Firmware_Common ${FIRMWARE_COMMON_SRC})

#https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html#using-esp-idf-in-custom-cmake-projects