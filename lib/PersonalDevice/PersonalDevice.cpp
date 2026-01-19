#include "PersonalDevice.h"

PersonalDevice::PersonalDevice() : DeviceBase() {
    deviceType = PERSONAL_DEVICE;
}

void PersonalDevice::buildSafetyPacket() {
    pckt.safetyPacket(deviceID, deviceType, deviceLatitude, deviceLongitude, safetyPacket);
}

void PersonalDevice::buildMonitoringPacket() {
    pckt.monitoringPacket(deviceID, deviceType, deviceLatitude, deviceLongitude,
                          batteryLevel, last5positions, last5events, status, monitoringPacket);
}
