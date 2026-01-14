#ifndef PERSONALDEVICE_H
#define PERSONALDEVICE_H

#include "packet.h"
#include "cmslora.h"
#include <Arduino.h>
#include "LoRaBoards.h"
#include <TinyGPS++.h>

#define MONITORING_CHANNEL 1
#define SAFETY_CHANNEL 2

class PersonalDevice {
  public:
    PersonalDevice();

    void setup();

    uint8_t getID() const;
    void setID(uint8_t id);

    double getLatitude();
    void setLatitude(double latitude);

    double getLongitude();
    void setLongitude(double longitude);

    double getSpeed();
    void setSpeed(double speedValue);
    
    float getAccelerationX() const;
    void setAccelerationX(float ax);

    float getAccelerationY() const;
    void setAccelerationY(float ay);

    void sendSafety();
    void sendMonitoring();
    bool receive();

    bool isChannelBusy(int channel);

    void updateFromBluetooth(String rawData);

    float calculateDistance(float targetLat, float targetLng);

    void sendAlert(uint8_t alertType, uint8_t targetID);

    double getCourse();
    void setCourse(double course);

  private:
    uint8_t deviceID;
    double deviceLatitude;
    double deviceLongitude;
    uint8_t batteryLevel;
    uint8_t status;
    uint8_t deviceType = PERSONAL_DEVICE;
    int32_t last5positions[5][2];
    uint8_t last5events[5];
    packet pckt;
    uint8_t safetyPacket[SAFETY_PACKET_SIZE];
    uint8_t monitoringPacket[MONITORING_PACKET_SIZE];
    uint8_t receivedPacket[MONITORING_PACKET_SIZE];
    double speed;
    float accelerationX;
    float accelerationY;
    CMSLoRa lora;
    TinyGPSPlus gps;
    double deviceCourse;


    float toRadians(float degree);
};

#endif