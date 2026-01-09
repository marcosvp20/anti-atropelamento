#include "packet.h"

void packet::safetyPacket(uint8_t ID, uint8_t deviceType, float latitude, float longitude, 
                          uint16_t course, uint8_t riskRadius, uint8_t dangerMask, uint8_t *returnPacket) {
    
    returnPacket[0] = SAFETY_PACKET;
    returnPacket[1] = ID;
    returnPacket[2] = deviceType;

    // Mantendo sua lógica de memcpy para as coordenadas (4 bytes cada)
    memcpy(&returnPacket[3], &latitude, sizeof(float));
    memcpy(&returnPacket[7], &longitude, sizeof(float));

    // Serialização do Course (uint16_t - 2 bytes)
    // Quebramos em High Byte e Low Byte
    returnPacket[11] = (uint8_t)(course >> 8);
    returnPacket[12] = (uint8_t)(course & 0xFF);

    // Campos de Risco (1 byte cada)
    returnPacket[13] = riskRadius;
    returnPacket[14] = dangerMask;
}

void packet::monitoringPacket(uint8_t ID, uint8_t deviceType, float latitude, float longitude, uint8_t batteryLevel, float last5positions[5][2], uint8_t last5events[5], uint8_t status, uint8_t *returnPacket) {
    returnPacket[0] = MONITORING_PACKET;
    returnPacket[1] = ID;
    returnPacket[2] = deviceType;

    memcpy(&returnPacket[3], &latitude, sizeof(float));
    memcpy(&returnPacket[7], &longitude, sizeof(float));
    returnPacket[11] = batteryLevel;

    for (int i = 0; i < 5; i++) {
        memcpy(&returnPacket[12 + i * 8], &last5positions[i][0], sizeof(float));
        memcpy(&returnPacket[16 + i * 8], &last5positions[i][1], sizeof(float));
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

    memcpy(&latitude, &receivedPacket[3], sizeof(float));
    memcpy(&longitude, &receivedPacket[7], sizeof(float));

    if (packetID == SAFETY_PACKET) {
        Serial.println("Decoding Safety Packet (Enhanced)...");
        safetyPacketData.packetID = packetID;
        safetyPacketData.ID = ID;
        safetyPacketData.deviceType = deviceType;
        safetyPacketData.latitude = latitude;
        safetyPacketData.longitude = longitude;

        // --- NOVOS CAMPOS DE RISCO ---
        // Reconstrói o uint16_t (2 bytes) do curso
        safetyPacketData.course = (uint16_t)((receivedPacket[11] << 8) | receivedPacket[12]);
        
        // Lê os bytes únicos de raio e máscara
        safetyPacketData.RiskRadius = receivedPacket[13];
        safetyPacketData.DangerMask = receivedPacket[14];

        // Logs de Debug
        Serial.println("ID: " + String(safetyPacketData.ID));
        Serial.println("Course: " + String(safetyPacketData.course / 100.0) + "°");
        Serial.println("Risk Radius: " + String(safetyPacketData.RiskRadius) + "m");
        Serial.print("Danger Mask: 0b"); Serial.println(safetyPacketData.DangerMask, BIN);
        Serial.println();

    } else if (packetID == MONITORING_PACKET) {
        Serial.println("Decoding Monitoring Packet...");
        uint8_t batteryLevel = receivedPacket[11];
        float last5positions[5][2];
        uint8_t last5events[5];
        uint8_t status;

        for (int i = 0; i < 5; i++) {
            memcpy(&last5positions[i][0], &receivedPacket[12 + i * 8], sizeof(float));
            memcpy(&last5positions[i][1], &receivedPacket[16 + i * 8], sizeof(float));
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

        Serial.println("ID: " + String(ID));
        Serial.println("Device Type: " + String(deviceType));
        Serial.println("Latitude: " + String(latitude, 6));
        Serial.println("Longitude: " + String(longitude, 6));
        Serial.println("Battery Level: " + String(batteryLevel) + "%");
        Serial.println("Last 5 Positions:");
        for (int i = 0; i < 5; i++) {
            Serial.println("  Position " + String(i + 1) + ": (" + String(last5positions[i][0], 6) + ", " + String(last5positions[i][1], 6) + ")");
        }
        Serial.println("Last 5 Events:");
        for (int i = 0; i < 5; i++) {
            Serial.println("  Event " + String(i + 1) + ": " + String(last5events[i]));
        }
        Serial.println("Status: " + String(status));
        Serial.println();
    }
}