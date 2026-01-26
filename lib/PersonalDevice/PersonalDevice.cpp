#include "PersonalDevice.h"

PersonalDevice::PersonalDevice() : DeviceBase() {
    deviceType = 2;
}

void PersonalDevice::buildSafetyPacket() {
    pckt.safetyPacket(deviceID, deviceType, deviceLatitude, deviceLongitude, safetyPacket);
}

void PersonalDevice::buildMonitoringPacket() {
    pckt.monitoringPacket(deviceID, deviceType, deviceLatitude, deviceLongitude,
                          batteryLevel, last5positions, last5events, status, monitoringPacket);
}

int PersonalDevice::isValidSend(float targetLat, float targetLng) {
  if (!gps.location.isValid()) return 0;

  float distance = calculateDistance(targetLat, targetLng);

  if (distance < 30.0f) return 1;
  if (distance <= 50.0f) return 2;
  return 3;
}