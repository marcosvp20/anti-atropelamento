#ifndef PERSONALDEVICE_H
#define PERSONALDEVICE_H

#include "packet.h"
#include "cmslora.h"
#include <Arduino.h>

#define MONITORING_CHANNEL 1
#define SAFETY_CHANNEL 2


class PersonalDevice {
  public:
    PersonalDevice();

    void setup();

    uint8_t getID() const;
    void setID(uint8_t id);

    float getLatitude() const;
    void setLatitude(float latitude);

    float getLongitude() const;
    void setLongitude(float longitude);

    float getAccelerationX() const;
    void setAccelerationX(float ax);

    float getAccelerationY() const;
    void setAccelerationY(float ay);

    void setSpeed(float speedValue);
    float getSpeed() const;

    void sendSafety();
    void sendMonitoring();
    bool receive();

    bool isChannelBusy(int channel);

    void updateFromBluetooth(String rawData);

    float calculateDistance(float targetLat, float targetLng);

  private:
    uint8_t deviceID;
    float deviceLatitude;
    float deviceLongitude;
    uint8_t batteryLevel;
    uint8_t status;
    uint8_t deviceType = PERSONAL_DEVICE;
    float last5positions[5][2];
    uint8_t last5events[5];
    packet pckt;
    uint8_t safetyPacket[SAFETY_PACKET_SIZE];
    uint8_t monitoringPacket[MONITORING_PACKET_SIZE];
    uint8_t receivedPacket[MONITORING_PACKET_SIZE];
    float accelerationX;
    float accelerationY;
    float speed;
    CMSLoRa lora;

    float toRadians(float degree);
};

#endif