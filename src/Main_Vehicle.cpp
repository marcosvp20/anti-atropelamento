#include "VehicleDevice.h"
#include "SimpleTimer.h"

#define SAFETY_TIMER 5000
#define MONITORING_TIMER 60000

VehicleDevice device;
SimpleTimer safetyTimer(SAFETY_TIMER);
SimpleTimer monitoringTimer(MONITORING_TIMER);

void setup() {
    Serial.begin(9600);
    device.setup();
    device.setID(2);
    device.setLatitude(377749);
    device.setLongitude(-1224194);
    device.setSpeed(60.4321);
    device.setCourse(34.1234);

}
void loop() {
    device.receive();
    if(safetyTimer.isReady()){
        if(device.isChannelBusy(SAFETY_CHANNEL))
        {
            Serial.println("Channel is busy, waiting... a random time");
            safetyTimer.setInterval(random(500, 1000));
            safetyTimer.reset();
        }
        else
        {
            Serial.println("Sending Safety Packet...");
            //device.sendSafety();
            device.sendAlert(ALERT_INTERLOCK, 1);
            
            safetyTimer.reset();
        }
    }
    if(monitoringTimer.isReady()){
        if(device.isChannelBusy(MONITORING_CHANNEL))
        {
            Serial.println("Channel is busy, waiting... a random time");
            monitoringTimer.setInterval(random(500, 1000));
            monitoringTimer.reset();
        }
        else
        {
            Serial.println("Sending Monitoring Packet...");
            device.sendMonitoring();
            monitoringTimer.reset();
        }
}
}

