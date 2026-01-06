#include "PersonalDevice.h"
PersonalDevice device;
int i = 0;
void setup() {
    Serial.begin(9600);
    device.setup();
    device.setID(1);
    device.setLatitude(37.7749);
    device.setLongitude(-122.4194);
    device.setAccelerationX(0.5);
    device.setAccelerationY(1.0);
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
        Serial.println("Sending Monitoring Packet...");
        if(device.isChannelBusy(MONITORING_CHANNEL))
        {
            Serial.println("Channel is busy, waiting... a random time");
            delay(random(500, 1000));
        }
        else
        {
            device.sendMonitoring();
        }
        i = 0;
    }
    i++;
}

// void loop()
// {
//     device.sendSafety();
//     delay(1000);
// }
