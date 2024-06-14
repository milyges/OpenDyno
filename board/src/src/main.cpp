#include <Arduino.h>
#include <uprintf.h>
//#include <mpu.h>
#include <gps.h>
#include <stdlib.h>

#define ACC_FILTER_A    90
#define ACC_FILTER_B    ((100 - ACC_FILTER_A) / 2)

void setup() {  
    uprintf_init();
    delay(500);

    uprintf("OpenDyno Board start\r\n");

    gps_init();
}

void loop() {
    gps_loop();  
}
