#include "PersonalDevice.h"

PersonalDevice::PersonalDevice() : DeviceBase() {
    deviceType = 2;
}

void PersonalDevice::buildSafetyPacket() {
  pckt.safetyPacket(deviceID, deviceType, deviceLatitude, deviceLongitude, safetyPacket, 0.0, 0.0);
}

void PersonalDevice::buildMonitoringPacket() {
    pckt.monitoringPacket(deviceID, deviceType, deviceLatitude, deviceLongitude,
                          batteryLevel, last5positions, last5events, status, monitoringPacket);
}

void PersonalDevice::onReceiveDecoded() {
    Serial.println("Received Packet:");
    Serial.print("ID: "); Serial.println(pckt.getDeviceID());
    Serial.print("Latitude: "); Serial.println(pckt.getLat(), 6);
    Serial.print("Longitude: "); Serial.println(pckt.getLng(), 6);
}

int PersonalDevice::isValidSend(float targetLat, float targetLng) {

  const uint32_t RESET_MS = 5000;
  uint32_t agora = millis();
  if (agora - lastMinResetMs >= RESET_MS) {
    minDistance = 1e9;
    lastMinResetMs = agora;
  }

  float distance = calculateDistance(targetLat, targetLng);

  if (distance < minDistance) {
    minDistance = distance;
  }

  if (minDistance < 30.0f) {
    return 1;
  } else if (minDistance <= 50.0f) {
    return 2;
  } else {  
    return 3;
  }
}
