#ifndef VEHICLEDEVICE_H
#define VEHICLEDEVICE_H

#include "packet.h"
#include <Arduino.h>
#include "cmslora.h"

#define MONITORING_CHANNEL 1
#define SAFETY_CHANNEL 2

class VehicleDevice {
  public:
    VehicleDevice();

    void setup();

    uint8_t getID() const;
    void setID(uint8_t id);

    int32_t getLatitude() const;
    void setLatitude(int32_t latitude);

    int32_t getLongitude() const;
    void setLongitude(int32_t longitude);

    unsigned long getSpeed() const;
    void setSpeed(unsigned long speed);

    unsigned long getCourse() const;
    void setCourse(unsigned long courseValue);

    void sendSafety();
    void sendMonitoring();
    bool receive();

    bool isChannelBusy(int channel);

    void sendAlert(uint8_t alertType, uint8_t targetID);

  private:
    uint8_t deviceID;
    int32_t deviceLatitude;
    int32_t deviceLongitude;
    unsigned long speed;
    unsigned long course;
    uint8_t batteryLevel;
    uint8_t status;
    uint8_t deviceType = VEHICLE_DEVICE;
    int32_t last5positions[5][2];
    uint8_t last5events[5];
    packet pckt;
    uint8_t safetyPacket[SAFETY_PACKET_SIZE];
    uint8_t monitoringPacket[MONITORING_PACKET_SIZE];
    uint8_t receivedPacket[MONITORING_PACKET_SIZE];
    CMSLoRa lora;
};

#endif