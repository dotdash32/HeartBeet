
#include <Arduino.h>
#include "keepAlive.h"

#define KeepAliveTime           1000 // [ms] post a message every 1 second
static unsigned long lastMsg = 0;


void keepAliveMessages(){
    if (KeepAliveTime < (millis()-lastMsg)) {
        Serial.println("Still Alive!");
        lastMsg = millis();
    }
}