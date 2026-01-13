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

    float getLatitude() const;
    void setLatitude();

    float getLongitude() const;
    void setLongitude();

    float getSpeed() const;
    void setSpeed(float speed);
    
    float getCourse() const;
    void setCourse();

    void sendSafety();
    void sendMonitoring();
    bool receive();

    bool isChannelBusy(int channel);

    uint8_t calculateRiskMask(float steeringAngle);
    void updateSteeringFromCourse();

    float course; // Direção atual do veículo (0-360)
    uint8_t currentRiskMask;
    uint8_t riskRadius;

  private:
    uint8_t deviceID;
    float deviceLatitude;
    float deviceLongitude;
    float speed;
    float lastCourse; // Armazena o curso da última execução para cálculo de delta
    float steeringAngle;
    uint8_t batteryLevel;
    uint8_t status;
    uint8_t deviceType = VEHICLE_DEVICE;
    float last5positions[5][2];
    uint8_t last5events[5];
    
    packet pckt;
    uint8_t safetyPacket[SAFETY_PACKET_SIZE];
    uint8_t monitoringPacket[MONITORING_PACKET_SIZE];
    uint8_t receivedPacket[MONITORING_PACKET_SIZE];
    CMSLoRa lora;
};

#endif