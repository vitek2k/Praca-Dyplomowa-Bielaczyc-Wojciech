#ifndef MQTT_HPP
#define MQTT_HPP

#include "esp_log.h"
#include "mqtt_client.h"
#include "esp_tls.h"
#include "esp_ota_ops.h"
#include <sys/param.h>

#include "Credentials.h"


#define CONFIG_BROKER_BIN_SIZE_TO_SEND 20000
#define TOPIC_NAME "/Telemetry"

static const char *MQTTS_TAG = "MQTTS_EXAMPLE";


/*
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);


void mqtt_app_start(void);


#endif //MQTT_HPP