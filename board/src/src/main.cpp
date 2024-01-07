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
    #if 0
    static struct gps_state lastState = { 0, };
    static int32_t lastAccFiltered = 0;
    static int32_t lastAcc = 0;
    static int32_t lastSpeedFiltered = 0;

    struct gps_state state;

    if (gps_loop(&state) == 0) { 
        if (lastState.gps_time == 0) { /* Przewsze wywołanie, zapisujemy dane i ponawiamy */
        lastState = state;
        return;
        }

        int32_t dt = state.gps_time - lastState.gps_time; /* Obliczamy ile czasu upłyneło w ms */
        int32_t dv = state.gps_speed - lastState.gps_speed; 
        int32_t acc = (dv * 1000) / dt;

        /* Filtr dla przyśpieszenia */
        int32_t accFiltered = (ACC_FILTER_A * lastAccFiltered + ACC_FILTER_B * acc + ACC_FILTER_B * lastAcc) / (ACC_FILTER_A + 2 * ACC_FILTER_B);
        
        /* Filtr dla prędkości */
        int32_t speedCalc = (ACC_FILTER_A * lastSpeedFiltered + ACC_FILTER_B * state.gps_speed + ACC_FILTER_B * lastState.gps_speed) / (ACC_FILTER_A + 2 * ACC_FILTER_B);

        /* Miksujemy prędkośc przefiltrowaną ze zmierzoną */
        int32_t speedFiltered = (state.gps_speed * 20 + speedCalc * 80) / 100;

        uprintf("time=%u.%us, gps_speed=%umm/s, speed=%dmm/s, a=%dmm/s^2, af=%dmm/s^2\r\n", state.gps_time / 1000, state.gps_time % 1000 / 100, 
                state.gps_speed, speedFiltered, acc, accFiltered);

        lastSpeedFiltered = speedFiltered;
        lastAccFiltered = accFiltered;
        lastAcc = acc;
        lastState = state;    
    }
    #endif
    gps_loop();  
}
