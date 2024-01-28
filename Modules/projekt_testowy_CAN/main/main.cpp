#include "MQTT/Lib_mqtt.hpp"
#include "CAN/Can_Interface.hpp"
#include "wifi/Lib_wifi.hpp"

#include "CAN/CAN_Defines.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "esp_timer.h"

extern "C" void app_main(void)
{
  CanInit(TX_CAN_PIN, RX_CAN_PIN);
  // init_wifi();
  // mqtt_app_start();

  initConversationSlaveCAN(6, ELECTRIC_ID);
  initConversationSlaveCAN(5, DATA_ID);


    twai_message_t rx_message;
    uint8_t data_lenght = 2;
// electric module
  do{
      receiveMessage(&rx_message, 2000);
  }while (rx_message.data[0] != COLLECT_DATA);

  uint8_t voltage[] = {VOLTAGE_VALUE, 0, 6, 6};
  CanSendMessage(ELECTRIC_ID, voltage, 4);

  uint8_t current[] = {CURRENT_VALUE, 0, 5, 5};
  CanSendMessage(ELECTRIC_ID, current, 4);

  uint8_t ambient_temperature[] = {AMBIENT_TEMPERATURE, 5};
  CanSendMessage(ELECTRIC_ID, ambient_temperature, data_lenght);

  uint8_t err[] = {ERROR, 5};
  CanSendMessage(ELECTRIC_ID, err, data_lenght);

  uint8_t h2_leak[] = {H2_LEAK, 5};
  CanSendMessage(ELECTRIC_ID, h2_leak, data_lenght);

  uint8_t cell_temp[] = {CELL_TEMP, 5};
  CanSendMessage(ELECTRIC_ID, cell_temp, data_lenght);

  
// data module
  do{
      receiveMessage(&rx_message, 2000);
  }while (rx_message.data[0] != COLLECT_DATA);

  uint8_t temp[] = {TEMPERATURE, 0, 5, 5};
  CanSendMessage(DATA_ID, temp, 4);


  uint8_t hydro[] = {HYDROGEN_USAGE, 5, 5};
  CanSendMessage(DATA_ID, hydro, 3);

  uint8_t hum[] = {HUMIDITY, 5};
  CanSendMessage(DATA_ID, hum, data_lenght);

  uint8_t err1[] = {ERROR, 5};
  CanSendMessage(DATA_ID, err1, data_lenght);

  uint8_t speed[] = {SPEED, 5};
  CanSendMessage(DATA_ID, speed, data_lenght);



}
