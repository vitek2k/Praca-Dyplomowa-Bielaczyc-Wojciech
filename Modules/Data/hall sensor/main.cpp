#include "hall_sensor.h"

HallSensor hallSensorInstance;

extern "C" void app_main() {
    hallSensorInstance.setup();

    while (true) {
        hallSensorInstance.loop();
    }
}
