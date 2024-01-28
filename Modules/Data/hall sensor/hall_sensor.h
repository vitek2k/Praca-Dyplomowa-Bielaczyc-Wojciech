#ifndef HALL_SENSOR_H
#define HALL_SENSOR_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <freertos/semphr.h>
#include <esp_log.h>
#include <esp_timer.h>
#include <cmath>

#define VELOCITY_MEASURE_PIN_1 GPIO_NUM_2
#define VELOCITY_MEASURE_PIN_2 GPIO_NUM_4
#define WHEEL_DIAMETER 260.0
#define DEBUG 1

class HallSensor {
public:
    HallSensor();
    void setup();
    void loop();

private:
    static void addRotationSensor(void *arg);
    void calculateVelocity();

    static SemaphoreHandle_t rotationsMutex;
    static volatile long rotationsSensor1;
    static volatile long rotationsSensor2;

    static unsigned long long lastVelocityMeasure;
    static unsigned long long measureTime;

    static float velocity1;
    static float velocity2;
};

#endif
