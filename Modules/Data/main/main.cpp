#include "CAN/Can_Interface.hpp"
#include "CAN/CAN_Defines.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_timer.h"

extern "C" void app_main(void)
{
  CanInit(TX_CAN_PIN, RX_CAN_PIN);
  initConversationSlaveCAN(5, DATA_ID);

}
