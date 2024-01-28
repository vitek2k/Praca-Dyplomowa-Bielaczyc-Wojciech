#ifndef LIB_WIFI_HPP
#define LIB_WIFI_HPP
  
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "Credentials.h"

////////////////////////
// DEFINES 
////////////////////////
#define WIFI_SUCCESS 1 << 0
#define WIFI_FAILURE 1 << 1
#define MAX_FAILURES 10


////////////////////////
// GLOBAL VARIABLES
////////////////////////
// event group to contain status information
static EventGroupHandle_t wifi_event_group;

// retry tracker
static int s_retry_num = 0;

// task tag
static const char *WIFI_TAG = "WIFI";


////////////////////////
// FUNCTIONS 
////////////////////////
//event handler for wifi events
void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data);

//event handler for ip events
void ip_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data);

// connect to wifi and return the result
esp_err_t connect_wifi();

// innitialize wifi connection
esp_err_t init_wifi();

#endif //LIB_WIFI_HPP