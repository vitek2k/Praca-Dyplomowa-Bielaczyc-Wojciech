
# Source files                            
set(FIRMWARE_COMMON_SOURCE ../../../Firmware_Common/CAN/Can_Interface.cpp)


# Include directories
set(DATA_MODULE ../Source)
set(FIRMWARE_COMMON_INCLUDE ../../../Firmware_Common)


set(SOURCE_FILES ${FIRMWARE_COMMON_SOURCE})

set(INCLUDE_FILES ${DATA_MODULE}
                  ${FIRMWARE_COMMON_INCLUDE})

idf_component_register(SRCS ${SOURCE_FILES} "main.cpp"
                        INCLUDE_DIRS . ${INCLUDE_FILES})

