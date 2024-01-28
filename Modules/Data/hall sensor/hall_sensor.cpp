#include "hall_sensor.h"
#include "sdkconfig.h"

SemaphoreHandle_t HallSensor::rotationsMutex = nullptr;
volatile long HallSensor::rotationsSensor1 = 0;
volatile long HallSensor::rotationsSensor2 = 0;

unsigned long long HallSensor::lastVelocityMeasure = 0;
unsigned long long HallSensor::measureTime = 1000;

float HallSensor::velocity1 = 0;
float HallSensor::velocity2 = 0;

HallSensor::HallSensor() {}

void HallSensor::addRotationSensor(void *arg) {
    long *rotationsSensor = (long *)arg;

    if (xSemaphoreTake(rotationsMutex, portMAX_DELAY)) {
        (*rotationsSensor)++;
        xSemaphoreGive(rotationsMutex);
    }
}

void HallSensor::calculateVelocity() {
    if (xSemaphoreTake(rotationsMutex, portMAX_DELAY)) {
        unsigned long long currentTime = esp_timer_get_time() / 1000;

        if (lastVelocityMeasure + measureTime < currentTime) {
            float velocity1Local = (rotationsSensor1 * WHEEL_DIAMETER) / (currentTime - lastVelocityMeasure);
            float velocity2Local = (rotationsSensor2 * WHEEL_DIAMETER) / (currentTime - lastVelocityMeasure);
            lastVelocityMeasure = currentTime;
            rotationsSensor1 = 0;
            rotationsSensor2 = 0;

#ifdef DEBUG
            ESP_LOGI("HallSensor", "Velocity Sensor 1: %.2f km/h", velocity1Local);
            ESP_LOGI("HallSensor", "Velocity Sensor 2: %.2f km/h", velocity2Local);
#endif

            if (fabs(velocity1Local) > 0.01 && fabs(velocity2Local) > 0.01) {
                float averageVelocity = (velocity1Local + velocity2Local) / 2;
                ESP_LOGI("HallSensor", "Average Velocity: %.2f km/h", averageVelocity);
            } else if (fabs(velocity1Local) > 0.01) {
                ESP_LOGI("HallSensor", "Final 1 Velocity: %.2f km/h", velocity1Local);
            } else if (fabs(velocity2Local) > 0.01) {
                ESP_LOGI("HallSensor", "Final 2 Velocity: %.2f km/h", velocity2Local);
            }
        }

        xSemaphoreGive(rotationsMutex);
    }
}

void HallSensor::setup() {
    gpio_config_t io_conf_1;
    io_conf_1.pin_bit_mask = (1ULL << VELOCITY_MEASURE_PIN_1);
    io_conf_1.mode = GPIO_MODE_INPUT;
    io_conf_1.intr_type = GPIO_INTR_ANYEDGE;
    io_conf_1.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf_1.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf_1);

    gpio_config_t io_conf_2;
    io_conf_2.pin_bit_mask = (1ULL << VELOCITY_MEASURE_PIN_2);
    io_conf_2.mode = GPIO_MODE_INPUT;
    io_conf_2.intr_type = GPIO_INTR_ANYEDGE;
    io_conf_2.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf_2.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf_2);

    rotationsMutex = xSemaphoreCreateMutex();

    gpio_install_isr_service(0);
    gpio_isr_handler_add(VELOCITY_MEASURE_PIN_1, addRotationSensor, (void *)&rotationsSensor1);
    gpio_isr_handler_add(VELOCITY_MEASURE_PIN_2, addRotationSensor, (void *)&rotationsSensor2);

#ifdef DEBUG
    esp_log_level_set("HallSensor", ESP_LOG_INFO);
#endif
}

void HallSensor::loop() {
    const TickType_t delay = pdMS_TO_TICKS(10);
    static TickType_t lastWakeTime = xTaskGetTickCount();

    calculateVelocity();
    vTaskDelayUntil(&lastWakeTime, delay);
}

