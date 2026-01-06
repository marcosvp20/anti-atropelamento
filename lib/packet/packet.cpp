#include "packet.h"

void packet::safetyPacket(uint8_t ID, uint8_t deviceType, float latitude, float longitude, uint8_t *returnPacket) {
    returnPacket[0] = SAFETY_PACKET;
    returnPacket[1] = ID;
    returnPacket[2] = deviceType;

    memccpy(&returnPacket[3], &latitude, 0, sizeof(float));
    memccpy(&returnPacket[7], &longitude, 0, sizeof(float));
}

void packet::monitoringPacket(uint8_t ID, uint8_t deviceType, float latitude, float longitude, uint8_t batteryLevel, float last5positions[5][2], uint8_t last5events[5], uint8_t status, uint8_t *returnPacket) {
    returnPacket[0] = MONITORING_PACKET;
    returnPacket[1] = ID;
    returnPacket[2] = deviceType;

    memccpy(&returnPacket[3], &latitude, 0, sizeof(float));
    memccpy(&returnPacket[7], &longitude, 0, sizeof(float));
    returnPacket[11] = batteryLevel;

    for (int i = 0; i < 5; i++) {
        memccpy(&returnPacket[12 + i * 8], &last5positions[i][0], 0, sizeof(float));
        memccpy(&returnPacket[16 + i * 8], &last5positions[i][1], 0, sizeof(float));
    }

    for (int i = 0; i < 5; i++) {
        returnPacket[52 + i] = last5events[i];
    }

    returnPacket[57] = status;
}

void packet::decodePacket(uint8_t *receivedPacket) {
    uint8_t packetID = receivedPacket[0];
    uint8_t ID = receivedPacket[1];
    uint8_t deviceType = receivedPacket[2];

    float latitude;
    float longitude;

    memccpy(&latitude, &receivedPacket[3], 0, sizeof(float));
    memccpy(&longitude, &receivedPacket[7], 0, sizeof(float));

    if (packetID == SAFETY_PACKET) {
        Serial.println("Decoding Safety Packet...");
        safetyPacketData.packetID = packetID;
        safetyPacketData.ID = ID;
        safetyPacketData.deviceType = deviceType;
        safetyPacketData.latitude = latitude;
        safetyPacketData.longitude = longitude;

    } else if (packetID == MONITORING_PACKET) {
        Serial.println("Decoding Monitoring Packet...");
        uint8_t batteryLevel = receivedPacket[11];
        float last5positions[5][2];
        uint8_t last5events[5];
        uint8_t status;

        for (int i = 0; i < 5; i++) {
            memccpy(&last5positions[i][0], &receivedPacket[12 + i * 8], 0, sizeof(float));
            memccpy(&last5positions[i][1], &receivedPacket[16 + i * 8], 0, sizeof(float));
        }

        for (int i = 0; i < 5; i++) {
            last5events[i] = receivedPacket[52 + i];
        }

        status = receivedPacket[57];
        monitoringPacketData.packetID = packetID;
        monitoringPacketData.ID = ID;
        monitoringPacketData.deviceType = deviceType;
        monitoringPacketData.latitude = latitude;
        monitoringPacketData.longitude = longitude;
        monitoringPacketData.batteryLevel = batteryLevel;
        for (int i = 0; i < 5; i++) {
            monitoringPacketData.last5positions[i][0] = last5positions[i][0];
            monitoringPacketData.last5positions[i][1] = last5positions[i][1];
        }
        for (int i = 0; i < 5; i++) {
            monitoringPacketData.last5events[i] = last5events[i];
        }
        monitoringPacketData.status = status;
    }
}
