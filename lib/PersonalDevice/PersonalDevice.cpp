#include "PersonalDevice.h"
PersonalDevice::PersonalDevice()
    : deviceID(0), deviceLatitude(0.0), deviceLongitude(0.0), batteryLevel(100), status(0) {
    for (int i = 0; i < 5; i++) {
        last5positions[i][0] = 0.0;
        last5positions[i][1] = 0.0;
        last5events[i] = 0;
    }

    
}
void PersonalDevice::setup() {
    lora.begin();
    lora.SpreadingFactor(7);
}
uint8_t PersonalDevice::getID() const {
    return deviceID;
}
void PersonalDevice::setID(uint8_t id) {
    deviceID = id;
}
float PersonalDevice::getLatitude() const {
    return deviceLatitude;
}
void PersonalDevice::setLatitude(float latitude) {
    deviceLatitude = latitude;
}
float PersonalDevice::getLongitude() const {
    return deviceLongitude;
}
void PersonalDevice::setLongitude(float longitude) {
    deviceLongitude = longitude;
}
float PersonalDevice::getAccelerationX() const {
    return accelerationX;
}
void PersonalDevice::setAccelerationX(float ax) {
    accelerationX = ax;
}
float PersonalDevice::getAccelerationY() const {
    return accelerationY;
}
void PersonalDevice::setAccelerationY(float ay) {
    accelerationY = ay;
}
void PersonalDevice::sendSafety() {
    lora.sendData(safetyPacket, SAFETY_PACKET_SIZE);
}
void PersonalDevice::sendMonitoring() {
    lora.sendData(monitoringPacket, MONITORING_PACKET_SIZE);
}
bool PersonalDevice::receive() {
    lora.SpreadingFactor(7);
    if(lora.receiveData(receivedPacket, MONITORING_PACKET_SIZE)) {
    pckt.decodePacket(receivedPacket);
    return true;
    }
    return false;

}
bool PersonalDevice::isChannelBusy(int channel) {
    if(channel == SAFETY_CHANNEL) {
        // talvez seja mais apropriado fazer o pacote aqui para que o envio seja logo após a verificação do canal
        lora.SpreadingFactor(7);
        pckt.safetyPacket(deviceID, deviceType, deviceLatitude, deviceLongitude, safetyPacket);
    } else if(channel == MONITORING_CHANNEL) {
        lora.SpreadingFactor(9);
        pckt.monitoringPacket(deviceID, deviceType, deviceLatitude, deviceLongitude, batteryLevel, last5positions, last5events, status, monitoringPacket);
    }
    return lora.isChannelBusy();
}