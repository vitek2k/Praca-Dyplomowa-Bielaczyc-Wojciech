
# Source files                            
set(SERVER_CONNECTION_SOURCE "../Source/wifi/Lib_wifi.cpp"
                             "../Source/MQTT/Lib_mqtt.cpp")
set(FIRMWARE_COMMON_SOURCE ../../../Firmware_Common/CAN/Can_Interface.cpp)


# Include directories
set(COMMUNICATION_MODULE ../Source)
set(FIRMWARE_COMMON_INCLUDE ../../../Firmware_Common)


set(SOURCE_FILES ${SERVER_CONNECTION_SOURCE}
                 ${FIRMWARE_COMMON_SOURCE}
                 ${CAN_COMMUNICATION_MODULE_SOURCE})

set(INCLUDE_FILES ${COMMUNICATION_MODULE}
                  ${FIRMWARE_COMMON_INCLUDE})

idf_component_register(SRCS ${SOURCE_FILES} "main.cpp"
                        INCLUDE_DIRS . ${INCLUDE_FILES})

