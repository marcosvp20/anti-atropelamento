#ifndef PACKET_H
#define PACKET_H
#include <string>
#include <Arduino.h>
using namespace std;

#define PERSONAL_DEVICE 1
#define VEHICLE_DEVICE 2

#define SAFETY_PACKET 1
#define MONITORING_PACKET 2

#define SAFETY_PACKET_SIZE 11
#define MONITORING_PACKET_SIZE 58

struct safetyData {
    uint8_t packetID;
    uint8_t ID;
    uint8_t deviceType;
    float latitude;
    float longitude;
    //trocar lat e lng float por int32_t
    // int32_t  lat;          // 4 bytes (coordenada multiplicada por 10^6 para evitar float)
    // int32_t  lng;          // 4 bytes
    uint16_t course;        // 2 bytes (0-360 em centésimos)
    uint8_t  RiskRadius;    // 1 byte  (em metros)
    uint8_t  DangerMask;// 1 byte  (A máscara de bits)
};
struct monitoringData {
    uint8_t packetID;
    uint8_t ID;
    uint8_t deviceType;
    float latitude;
    float longitude;
    //trocar lat e lng float por int32_t
    // int32_t  lat;          // 4 bytes (coordenada multiplicada por 10^6 para evitar float)
    // int32_t  lng;          // 4 bytes
    uint8_t batteryLevel;
    float last5positions[5][2];
    uint8_t last5events[5];
    uint8_t status;
};

class packet {
    public:
    void safetyPacket(uint8_t ID, uint8_t deviceType, float latitude, float longitude, uint8_t *returnPacket);
    void monitoringPacket(uint8_t ID, uint8_t deviceType, float latitude, float longitude, uint8_t batteryLevel, float last5positions[5][2], uint8_t last5events[5], uint8_t status, uint8_t *returnPacket);
    void decodePacket(uint8_t *receivedPacket);

    safetyData safetyPacketData;
    monitoringData monitoringPacketData;
};
#endif