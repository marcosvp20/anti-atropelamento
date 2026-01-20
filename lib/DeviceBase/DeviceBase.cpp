#include "DeviceBase.h"

DeviceBase::DeviceBase() {
    for (int i = 0; i < 5; i++) {
        last5positions[i][0] = 0;
        last5positions[i][1] = 0;
        last5events[i] = 0;
    }
}

void DeviceBase::setup() {
    lora.begin();
    lora.SpreadingFactor(7);
}

uint8_t DeviceBase::getID() const { return deviceID; }
void DeviceBase::setID(uint8_t id) { deviceID = id; }

double DeviceBase::getLatitude() const { return deviceLatitude; }
void DeviceBase::setLatitude() { deviceLatitude = gps.location.lat(); }

double DeviceBase::getLongitude() const { return deviceLongitude; }
void DeviceBase::setLongitude() { deviceLongitude = gps.location.lng(); }

double DeviceBase::getSpeed() const { return speed; }
void DeviceBase::setSpeed() { speed = gps.speed.kmph(); }

double DeviceBase::getCourse() const { return deviceCourse; }
void DeviceBase::setCourse() { deviceCourse = gps.course.deg(); }

int DeviceBase::getSatValue() {
    return gps.satellites.value();
}

bool DeviceBase::getSatValid() {
    return gps.satellites.isValid();
}

double DeviceBase::getHdop() const { return deviceHdop; }
void DeviceBase::setHdop() { deviceHdop = gps.hdop.hdop(); }


void DeviceBase::sendSafety() {
    lora.sendData(safetyPacket, SAFETY_PACKET_SIZE);
}

void DeviceBase::sendMonitoring() {
    lora.sendData(monitoringPacket, MONITORING_PACKET_SIZE);
}

bool DeviceBase::receive() {
    lora.SpreadingFactor(safetySF());
    if (lora.receiveData(receivedPacket, MONITORING_PACKET_SIZE, 1000)) {
        pckt.decodePacket(receivedPacket);
        onReceiveDecoded();
        return true;
    }
    return false;
}

bool DeviceBase::isChannelBusy(int channel) {
    if (channel == SAFETY_CHANNEL) {
        lora.SpreadingFactor(safetySF());
        buildSafetyPacket();
    } else if (channel == MONITORING_CHANNEL) {
        lora.SpreadingFactor(monitoringSF());
        buildMonitoringPacket();
    }
    return lora.isChannelBusy();
}

void DeviceBase::updateFromBluetooth(String rawData) {
    int firstSemi = rawData.indexOf(';');
    int secondSemi = rawData.indexOf(';', firstSemi + 1);

    if (firstSemi != -1 && secondSemi != -1) {
        uint8_t novoID = (uint8_t)rawData.substring(0, firstSemi).toInt();
        float novaLat = rawData.substring(firstSemi + 1, secondSemi).toFloat();
        float novaLng = rawData.substring(secondSemi + 1).toFloat();

        setID(novoID);
        setLatitude();
        setLongitude();
        Serial.println("\n>>> Dados atualizados via BLE.");
    }
}

float DeviceBase::calculateDistance(float targetLat, float targetLng) {
    return gps.distanceBetween(deviceLatitude, deviceLongitude, targetLat, targetLng);
}

void DeviceBase::sendAlert(uint8_t alertType, uint8_t targetID) {
    uint8_t alertPacket[ADVERTISE_PACKET_SIZE];
    pckt.advertisePacket(alertType, targetID, alertPacket);
    lora.sendData(alertPacket, sizeof(alertPacket));
}

double DeviceBase::getRadius(int index) const {
    if (index >=0 && index < 3) {
        return deviceRadius[index];
    }
    return 0.0;
}

void DeviceBase::setRadius(double hdop) {
    for (int i = 0; i < 3; i++) {
        if (i == 0) {
            deviceRadius[i] = hdop * 1.5;
        } else if (i == 1) {
            deviceRadius[i] = (hdop * 1.5) + 9.0;
        } else if (i == 2) {
            deviceRadius[i] = (hdop * 1.5) + 25.0;
        }
    }
}