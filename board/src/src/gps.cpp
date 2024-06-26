#include <gps.h>
#include <Arduino.h>
#include <uprintf.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>

static SFE_UBLOX_GNSS _ublox_gnss;

HardwareSerial _gps_serial(USART2);


void gps_init(void) {
    //uint8_t flags, odoCfg, cogMaxSpeed, cogMaxPosAcc, velLpGain, cogLpGain;

    uprintf("GPS: Init\r\n");

    do {
        uprintf("GPS: Trying 38400 baudrate\r\n");
        _gps_serial.begin(38400);

        if (_ublox_gnss.begin(_gps_serial)) {
            break;
        }

        delay(100);
        uprintf("GPS: Trying 9600 baudrate\r\n");
        _gps_serial.begin(9600);
        if (_ublox_gnss.begin(_gps_serial)) {
            uprintf("GPS: Connected, switching to 38400 baudrate\r\n");
            _ublox_gnss.setSerialRate(38400);
            delay(100);
        }
        else {
            delay(2000);
        }
    } while(1);

    uprintf("GPS: Connected\r\n");

    _ublox_gnss.setUART1Output(COM_TYPE_UBX);
    _ublox_gnss.setMeasurementRate(250); /* 4Hz */
    _ublox_gnss.setNavigationRate(1);
    _ublox_gnss.setDynamicModel(DYN_MODEL_AUTOMOTIVE);
    _ublox_gnss.setAutoPVT(true);
    

    uprintf("GPS: Ready\r\n");
}

void gps_loop(void) {
#if 0
    static uint32_t speedHistory[3] = { 0, 0, 0};
    static uint32_t timeHistory[3] = { 0, 0, 0};
    static uint32_t prevSpeed = 0, prevSpeedFiltered = 0;
    uint32_t speed, speedFiltered;
#endif 

    if (_ublox_gnss.getPVT()) {
        uint32_t currentTime = _ublox_gnss.getTimeOfWeek();
        uint32_t currentSpeed = _ublox_gnss.getGroundSpeed();
#if 0
        /* Rotowanie danych */
        for(int i = 0; i < 3 - 1; i++) {
            speedHistory[i] = speedHistory[i + 1];
            timeHistory[i] = timeHistory[i + 1];
        }

        /* Na koniec kolejki wrzucamy bierzące pomiary */
        speedHistory[2] = currentSpeed;
        timeHistory[2] = currentTime;
        
        /* Liczymy średnia i podajemy jako wynik */
        uint32_t sum = 0;
        for(int i = 0; i < 3; i++) {
            sum += speedHistory[i];
        }
        
        /* Srednia z 3 próbek */
        speed = sum / 3;

        /* Filtr dolnoprzepustowy */        
        speedFiltered = (50 * prevSpeedFiltered + 25 * speed + 25 * prevSpeed) / 100;

        /* Wypisujemy o 0.2sek do tyłu, żeby zniwelować opóźnienie na filtrze */
        //uprintf("time=%u, speedFiltered=%u, speed=%u, speedraw=%u\r\n", timeHistory[0], speedFiltered, prevSpeed, speedHistory[0]);
#endif
        //uprintf("+%u;%u;%u;%u\r\n", timeHistory[0], speedFiltered, _ublox_gnss.getSIV(), _ublox_gnss.getFixType());
        uprintf("+%u;%u;%u;%u\r\n", currentTime, currentSpeed, _ublox_gnss.getSIV(), _ublox_gnss.getFixType());

#if 0
        prevSpeed = speed;
        prevSpeedFiltered = speedFiltered;
#endif 
    }
}
