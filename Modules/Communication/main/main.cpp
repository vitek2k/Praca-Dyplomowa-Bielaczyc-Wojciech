#include "MQTT/Lib_mqtt.hpp"
#include "CAN/Can_Interface.hpp"
#include "wifi/Lib_wifi.hpp"

#include "CAN/CAN_Defines.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "esp_timer.h"

extern "C" void app_main(void)
{
  initConversationMasterCAN();
  // init_wifi();
  // mqtt_app_start();

  printf("electric_module_frame_number: %u \n", electric_module_frame_number);
  printf("data_module_frame_number: %u \n", data_module_frame_number);
  
  
  uint32_t time = (uint32_t)(esp_timer_get_time() / 1000ULL);
  collectData();
  time = (uint32_t)(esp_timer_get_time() / 1000ULL) - time;
  printf("\n Czas: %u \n \n", (unsigned int)time);

printf("voltage: %u \n", (unsigned int)ElectricModuleData.voltage[0]);
printf("current: %u \n", (unsigned int)ElectricModuleData.current[0]);
printf("ambient_temperature: %u \n", (unsigned int)ElectricModuleData.ambient_temperature);
printf("h2_leak: %u \n", (unsigned int)ElectricModuleData.h2_leak);
printf("error: %u \n", (unsigned int)ElectricModuleData.error);

printf("temperature: %u \n", (unsigned int)DataModuleData.temperature[0]);
printf("hydrogen_usage: %u \n", (unsigned int)DataModuleData.hydrogen_usage);
printf("humidity: %u \n", (unsigned int)DataModuleData.humidity);
printf("speed: %u \n", (unsigned int)DataModuleData.speed);
printf("error: %u \n", (unsigned int)DataModuleData.error);
}
