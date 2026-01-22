#include "VehicleDevice.h"

static inline int circularDistDeg(int a, int b) {
    int d = abs(a - b);
    return (d > 180) ? (360 - d) : d;
}

VehicleDevice::VehicleDevice() : DeviceBase() {
    deviceType = 1; 
    for (int i = 0; i < 360; i++) bubble360_[i] = 0.0f;
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

void VehicleDevice::buildDynamicBubble360(
    float headingDeg,
    float baseRadius,
    float maxExtra,
    int   qntdSetores,
    float exponent
) {
  int center = (int)lroundf(headingDeg);
  center %= 360;
  if (center < 0) center += 360;

  if (qntdSetores < 0) qntdSetores = 0;
  if (qntdSetores > 180) qntdSetores = 180;

  const float invW = (qntdSetores > 0) ? (1.0f / (float)qntdSetores) : 0.0f;

  for (int i = 0; i < 360; i++) {
    int d = circularDistDeg(i, center);

    float extra = 0.0f;

    if (qntdSetores == 0) {
      extra = (d == 0) ? maxExtra : 0.0f;
    } else if (d <= qntdSetores) {
      float t = d * invW;                  
      float gain = cosf(t * (PI * 0.5f));  
      if (exponent > 0.0f) gain = powf(gain, exponent);
      extra = maxExtra * gain;
    }

    bubble360_[i] = baseRadius + extra;
  }

    //vehicle.buildDynamicBubble360(heading, 8.0f, 12.0f, 45, 2.0f);
}
