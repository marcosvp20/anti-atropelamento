#include "VehicleDevice.h"
VehicleDevice device;
int i = 0;
void setup() {
    Serial.begin(9600);
    device.setup();
    device.setID(2);
    device.setLatitude(37.7749);
    device.setLongitude(-122.4194);
    device.setSpeed(60.0);
}
void loop() {
    device.receive();
    if(device.isChannelBusy(SAFETY_CHANNEL) && i < 5)
    {
        Serial.println("Channel is busy, waiting... a random time");
        delay(random(500, 1000));
    }
    else
    {
        Serial.println("Sending Safety Packet...");
        device.sendSafety();
    }
    if(i == 5) {
        if(device.isChannelBusy(MONITORING_CHANNEL))
        {
            Serial.println("Channel is busy, waiting... a random time");
            delay(random(500, 1000));
        }
        else
        {
            Serial.println("Sending Monitoring Packet...");
            device.sendMonitoring();
            i = 0;
        }
        
    }
    i++;
}

