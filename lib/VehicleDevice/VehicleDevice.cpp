#include "VehicleDevice.h"

VehicleDevice::VehicleDevice() : DeviceBase() {
    deviceType = PERSONAL_DEVICE; 
}

void VehicleDevice::alimentandoGPS() {
    while (SerialGPS.available() > 0) {
        gps.encode(SerialGPS.read());
    }
}

void VehicleDevice::buildSafetyPacket() {
    // com speed e course
    pckt.safetyPacket(deviceID, deviceType, deviceLatitude, deviceLongitude,
                     safetyPacket, speed, deviceCourse);
}

void VehicleDevice::buildMonitoringPacket() {
    pckt.monitoringPacket(deviceID, deviceType, deviceLatitude, deviceLongitude,
                          batteryLevel, last5positions, last5events, status, monitoringPacket);
}

void VehicleDevice::onReceiveDecoded() {
    Serial.println("Received Packet:");
    Serial.print("ID: "); Serial.println(pckt.safetyData.ID);
    Serial.print("Latitude: "); Serial.println(pckt.safetyData.lat, 6);
    Serial.print("Longitude: "); Serial.println(pckt.safetyData.lng, 6);
}
