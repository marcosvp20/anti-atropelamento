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

double VehicleDevice::getLatitude(){
    deviceLatitude = gps.location.lat();
    return deviceLatitude;
}

void VehicleDevice::setLatitude(double latitude) {
    Serial.println("Setting latitude to: " + String(latitude));
    deviceLatitude = latitude;
}

double VehicleDevice::getLongitude(){
    deviceLongitude = gps.location.lng();
    return deviceLongitude;
}

void VehicleDevice::setLongitude(double longitude) {
    Serial.println("Setting longitude to: " + String(longitude));
    deviceLongitude = longitude;
}

double VehicleDevice::getSpeed() {
    return speed;
}

void VehicleDevice::setSpeed(double speedValue) {
    speed = speedValue;
}

double VehicleDevice::getCourse() {
    return deviceCourse;
}

void VehicleDevice::setCourse(double courseValue) {
    deviceCourse = courseValue;
}


void VehicleDevice::sendSafety() {
    // lora.SpreadingFactor(7);
    // pckt.safetyPacket(deviceID, deviceType, deviceLatitude, deviceLongitude, safetyPacket);
    lora.sendData(safetyPacket, SAFETY_PACKET_SIZE);
}
void VehicleDevice::sendMonitoring() {
    // lora.SpreadingFactor(9);
    // pckt.monitoringPacket(deviceID, deviceType, deviceLatitude, deviceLongitude, batteryLevel, last5positions, last5events, status, monitoringPacket);
    lora.sendData(monitoringPacket, MONITORING_PACKET_SIZE);
}
bool VehicleDevice::receive() {

    lora.SpreadingFactor(7);
    if(lora.receiveData(receivedPacket, MONITORING_PACKET_SIZE, 1000)) {
    pckt.decodePacket(receivedPacket);
    Serial.println("Received Packet:");
    Serial.print("ID: ");
    Serial.println(pckt.safetyData.ID);
    Serial.print("Latitude: ");
    Serial.println(pckt.safetyData.lat, 6);
    Serial.print("Longitude: ");
    Serial.println(pckt.safetyData.lng, 6);
    return true;
    }
    return false;
}

bool VehicleDevice::isChannelBusy(int channel) {
    if(channel == SAFETY_CHANNEL) {
        lora.SpreadingFactor(7);
        pckt.safetyPacket(deviceID, deviceType, deviceLatitude, deviceLongitude, safetyPacket, speed, deviceCourse);
    } else if(channel == MONITORING_CHANNEL) {
        lora.SpreadingFactor(9);
        pckt.monitoringPacket(deviceID, deviceType, deviceLatitude, deviceLongitude, batteryLevel, last5positions, last5events, status, monitoringPacket);
    }
    return lora.isChannelBusy();
}

void VehicleDevice::sendAlert(uint8_t alertType, uint8_t targetID) {
    // Construir o pacote de alerta
    uint8_t alertPacket[ADVERTISE_PACKET_SIZE]; // Tamanho arbitrário, ajuste conforme necessário
    pckt.advertisePacket(alertType, targetID, alertPacket);    // Enviar o pacote de alerta
    lora.sendData(alertPacket, sizeof(alertPacket));
}