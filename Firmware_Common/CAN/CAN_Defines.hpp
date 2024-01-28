#ifndef CAN_DEFINES_HPP
#define CAN_DEFINES_HPP

#include "hal/gpio_types.h"
#include "Inc/Pin_Defines.hpp"


#define CAN_FAIL 1
#define CAN_SUCCESS 0

// identyfikatory glownych moulow
#define COMMUNICATION_ID 0x0
#define ELECTRIC_ID 0x1
#define DATA_ID 0x2

// identyfikatory modulow pomiarowych
#define ELECTRIC_SENSOR_1 
#define ELECTRIC_SENSOR_2 
#define ELECTRIC_SENSOR_3 
#define ELECTRIC_SENSOR_4 
#define ELECTRIC_SENSOR_5 

// kody wiadomości 
//modol komunikacji
#define COLLECT_DATA 0x0
#define INIT_CONVERSATION_MASTER 0x1
#define REQUEST_MODULE 0x2

#define INIT_CONVERSATION_SLAVE 0x3

//modol pomiarow elektrycznych
#define VOLTAGE_VALUE 0x4
#define CURRENT_VALUE 0x5
#define AMBIENT_TEMPERATURE 0x6
#define H2_LEAK 0x7
#define CELL_TEMP 0xA
#define ELECTRIC_MESSURMENTS_TIME 0x11

//Modul pozostalych pomiarow
#define TEMPERATURE 0xB
#define HYDROGEN_USAGE 0xC
#define HUMIDITY 0xD
#define ERROR 0xE
#define SPEED 0xF
#define DATA_MESSURMENTS_TIME 0x12


//Errors
#define COULDNT_COLLECT_DATA 0x1

// variable contains number of mesurements made by specific module per interval
#define TRANSMITION 1
#define RECEIVE 2

#define TX_QUEUE_LEN 20
#define RX_QUEUE_LEN 20

extern uint8_t electric_module_frame_number;
extern uint8_t data_module_frame_number;


#endif