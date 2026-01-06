#include "VehicleDevice.h"
VehicleDevice::VehicleDevice()
    : deviceID(0), deviceLatitude(0.0), deviceLongitude(0.0), speed(0.0), batteryLevel(100), status(0) {
    for (int i = 0; i < 5; i++) {
        last5positions[i][0] = 0.0;
        last5positions[i][1] = 0.0;
        last5events[i] = 0;
    }
}

void VehicleDevice::setup() {
    lora.begin();
    lora.SpreadingFactor(7);
}
uint8_t VehicleDevice::getID() const {
    return deviceID;
}
void VehicleDevice::setID(uint8_t id) {
    deviceID = id;
}
float VehicleDevice::getLatitude() const {
    return deviceLatitude;
}
void VehicleDevice::setLatitude(float latitude) {
    deviceLatitude = latitude;
}
float VehicleDevice::getLongitude() const {
    return deviceLongitude;
}
void VehicleDevice::setLongitude(float longitude) {
    deviceLongitude = longitude;
}
float VehicleDevice::getSpeed() const {
    return speed;
}
void VehicleDevice::setSpeed(float speedValue) {
    speed = speedValue;
}
void VehicleDevice::sendSafety() {
    lora.SpreadingFactor(7);
    pckt.safetyPacket(deviceID, deviceType, deviceLatitude, deviceLongitude, safetyPacket);
    lora.sendData(safetyPacket, SAFETY_PACKET_SIZE);
}
void VehicleDevice::sendMonitoring() {
    lora.SpreadingFactor(9);
    pckt.monitoringPacket(deviceID, deviceType, deviceLatitude, deviceLongitude, batteryLevel, last5positions, last5events, status, monitoringPacket);
    lora.sendData(monitoringPacket, MONITORING_PACKET_SIZE);
}
bool VehicleDevice::receive() {
    lora.SpreadingFactor(7);
    if(lora.receiveData(receivedPacket, MONITORING_PACKET_SIZE)) {
    pckt.decodePacket(receivedPacket);
    return true;
    }
    return false;
}

bool VehicleDevice::isChannelBusy(int channel) {
    if(channel == SAFETY_CHANNEL) {
        lora.SpreadingFactor(7);
        pckt.safetyPacket(deviceID, deviceType, deviceLatitude, deviceLongitude, safetyPacket);
    } else if(channel == MONITORING_CHANNEL) {
        lora.SpreadingFactor(9);
        pckt.monitoringPacket(deviceID, deviceType, deviceLatitude, deviceLongitude, batteryLevel, last5positions, last5events, status, monitoringPacket);
    }
    return lora.isChannelBusy();
}
