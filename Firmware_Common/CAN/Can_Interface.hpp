#ifndef CAN_INTERFACE_HPP
#define CAN_INTERFACE_HPP

#include "driver/twai.h"
#include "esp_log.h"
#include "CAN/CAN_Defines.hpp"


struct ElectricModule {
    uint16_t voltage[5];
    uint16_t current[5];
    uint8_t ambient_temperature;
    uint8_t h2_leak;
    uint8_t cell_temp;
    uint8_t error;
    uint8_t time;
}extern ElectricModuleData;

struct DataModule {
    uint16_t temperature[5];
    uint16_t hydrogen_usage;
    uint8_t humidity;
    // uint8_t GPS;
    uint8_t speed;
    uint8_t error;
    uint8_t time;
}extern DataModuleData;


// task tag
static const char *CAN_TAG = "CAN";


// innitialize CAN controller
// accepts all incoming messegess by default
void CanInit(gpio_num_t tx_pin, gpio_num_t rx_pin, uint32_t acceptance_code = 0, uint32_t acceptance_mask = 0xFFFFFFFF);

 
// Checks TWAI driver state
uint8_t CheckStateCAN(uint8_t func);


// Sends single CAN messege
uint8_t CanSendMessage(uint32_t module_identifier, uint8_t* data, uint8_t data_lenght);


// Check receive buffer (blocks program for 500ms)
uint8_t receiveMessage(twai_message_t *rx_message, uint16_t ticks_to_wait = 500);



/////////////////////////////////////////////////////////////
// communication module
/////////////////////////////////////////////////////////////

/* Na razie wysyła wiadomość o inicjalizacji i gdy dostanie wiadomość zwrotną od slava to zapisuje liczbe pomiarow
sprawdzic poprawność
dodać diode informującą o trwającejinicjalizacji*/
/*
* communicates with single module
* detailes descriebed in datasheet
*
*  @param module_id Id of module we want to init conversation with
*  @return Info about succsess/fail
*/
uint8_t initConversationMasterSlaveCAN(uint8_t module_id);


/*
* initialize whole System
* if fail, System should fall into error mode (infinite loop)
*  @return Info about succsess/fail
*/
uint8_t initConversationMasterCAN();


/*
* sprawdza dane z modulu pomiarowego i zapisuje je do odpowiedniej struktory
*  @return Info about succsess/fail
*/
uint8_t saveCanDataFrame(twai_message_t rx_message);


/*
* sprawdza dane z modulu elektrycznego i zapisuje je do odpowiedniej struktory
*  @return Info about succsess/fail
*/
uint8_t saveCanElecticFrame(twai_message_t rx_message);


/*
* gets all data frames from single module
*  @return Info about succsess/fail
*/
uint8_t getDataFromSingleModule(uint8_t module_id);


/*
* gets all data frames from single module
*  @return Info about succsess/fail
*/
uint8_t getDataFromSingleModule(uint8_t module_id);


/*
* collects data from all modules
*  @return Info about succsess/fail
*/
uint8_t collectData();



/////////////////////////////////////////////////////////////
// electric and data modules
/////////////////////////////////////////////////////////////

/*
*  wait for master init and
*  send init data from slave module
*  @return Info about succsess/fail
*/
void initConversationSlaveCAN(uint8_t number_of_messurements, uint8_t module_id);


#endif //CAN_INTERFACE_HPP