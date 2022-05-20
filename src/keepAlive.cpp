
#include <Arduino.h>
#include "keepAlive.h"
#include <avr/wdt.h>


#define KeepAliveTime   1000 // [ms] post a message every 1 second
static unsigned long lastMsg = 0;

#define WDT_Timeout     WDTO_500MS // 2 s, several options


void keepAliveMessages(){
    if (KeepAliveTime < (millis()-lastMsg)) {
        Serial.println("Still Alive!");
        lastMsg = millis();
    }
}

void setupWatchdog(void){
    wdt_enable(WDT_Timeout);
}

void feedWatchdog(void) {
    wdt_reset();
}