#ifndef PERSONALDEVICE_H
#define PERSONALDEVICE_H

#include "packet.h"
#include "cmslora.h"
#include <Arduino.h>

#define MONITORING_CHANNEL 1
#define SAFETY_CHANNEL 2

#define CONFIG_SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CONFIG_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

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

    void sendSafety();
    void sendMonitoring();
    bool receive();

    bool isChannelBusy(int channel);

    void updateFromBluetooth(String rawData);

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
    CMSLoRa lora;
};

#endif