#include <uprintf.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <Arduino.h>

HardwareSerial Serial1(USART1);

void uprintf(const char * fmt, ...) {
	va_list va;
	va_start(va, fmt);	
	Serial.vprintf(fmt, va);
	Serial.flush();
	va_end(va);
}

void uprintf_init(void) {    
    Serial = Serial1;
    Serial.begin(115200);  
}
