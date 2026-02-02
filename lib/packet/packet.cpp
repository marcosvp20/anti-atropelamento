#include "packet.h"

// Definição das variáveis globais de dados decodificados
SafetyData safetyPacketData;
MonitoringData monitoringPacketData;
AdvertiseData advertisePacketData;

// --- FUNÇÕES DE MAPEAMENTO ---
uint8_t packet::mapDoubleToUint8(double value) {
    // Mapeamento genérico 0-360 -> 0-255 (ideal para curso)
    return (uint8_t)((value * 255) / 360); 
}

int32_t packet::mapDoubleToInt32(double value) {
    // Mapeamento genérico para latitude/longitude em micrograus
    return (int32_t)(value * 1000000);
}

float packet::mapUint8ToFloat(uint8_t value) {
    // Retorno genérico 0-255 -> 0-360
    return ((float)value * 360.0) / 255.0;
}

// --- CONSTRUTORES DE PACOTE ---

void packet::safetyPacket(uint8_t ID, uint8_t deviceType, double latitude, double longitude, uint8_t *returnPacket, double speed, double course) {
    SafetyPayload pkt;

    pkt.packetType = SAFETY_PACKET; // Usa o define
    pkt.id = ID;
    pkt.deviceType = deviceType;
    pkt.lat = mapDoubleToInt32(latitude);
    pkt.lng = mapDoubleToInt32(longitude);

    if (deviceType == VEHICLE_DEVICE) {
        // Assume que speed já foi tratado ou é pequeno o suficiente para caber em uint8
        // Se speed for Centi-Knots (biblioteca), converta antes de chamar essa função!
        pkt.speed = mapDoubleToUint8(speed); // Usa sua função de mapa
        
        // Usa sua função de mapa para o curso
        pkt.course = mapDoubleToUint8(course); 
    } else {
        pkt.speed = 0;
        pkt.course = 0;
    }

    // Copia a estrutura segura para o array de bytes
    memcpy(returnPacket, &pkt, sizeof(SafetyPayload));
}

void packet::monitoringPacket(uint8_t ID,  uint8_t deviceType, double latitude, double longitude, uint8_t batteryLevel, int32_t last5positions[5][2], uint8_t last5events[5], uint8_t status, uint8_t *returnPacket) {
    MonitoringPayload pkt;

    pkt.packetType = MONITORING_PACKET;
    pkt.id = ID;
    pkt.deviceType = deviceType;
    pkt.lat = latitude;
    pkt.lng = longitude;
    pkt.batteryLevel = batteryLevel;

    // Copia a matriz de posições inteira de uma vez (seguro pois ambos são int32_t)
    memcpy(pkt.last5positions, last5positions, sizeof(pkt.last5positions));
    
    // Copia os eventos
    memcpy(pkt.last5events, last5events, sizeof(pkt.last5events));
    
    pkt.status = status;

    memcpy(returnPacket, &pkt, sizeof(MonitoringPayload));
}

void packet::advertisePacket(uint8_t ID, uint8_t deviceID, uint8_t *returnPacket) {
    AdvertisePayload pkt;
    
    pkt.packetType = ADVERTISE_PACKET;
    pkt.id = ID;
    pkt.deviceID = deviceID;

    memcpy(returnPacket, &pkt, sizeof(AdvertisePayload));
}

// --- DECODIFICADOR ---

uint8_t packet::decodePacket(uint8_t *receivedPacket) {
    uint8_t packetID = receivedPacket[0];

    if (packetID == SAFETY_PACKET) {
        // Casting direto para a estrutura (Segurança total de memória)
        SafetyPayload *pkt = (SafetyPayload*)receivedPacket;

        Serial.println("Decoding Safety Packet...");
        safetyPacketData.packetID = pkt->packetType;
        safetyPacketData.ID = pkt->id;
        safetyPacketData.deviceType = pkt->deviceType;
        safetyPacketData.lat = pkt->lat;
        safetyPacketData.lng = pkt->lng;

        Serial.println("ID: " + String(safetyPacketData.ID));
        Serial.println("Device Type: " + String(safetyPacketData.deviceType));
        Serial.println("Latitude: " + String(safetyPacketData.lat));
        Serial.println("Longitude: " + String(safetyPacketData.lng));

        if (pkt->deviceType == VEHICLE_DEVICE) {
            safetyPacketData.speed = mapUint8ToFloat(pkt->speed); // Lê direto
            safetyPacketData.course = mapUint8ToFloat(pkt->course); // Desmapeia
            
            Serial.println("Speed: " + String(safetyPacketData.speed));
            Serial.println("Course: " + String(safetyPacketData.course) + " degrees");
        } else {
            safetyPacketData.speed = 0;
            safetyPacketData.course = 0;
        }
        Serial.println();

    } else if (packetID == MONITORING_PACKET) {
        MonitoringPayload *pkt = (MonitoringPayload*)receivedPacket;

        Serial.println("Decoding Monitoring Packet...");
        monitoringPacketData.packetID = pkt->packetType;
        monitoringPacketData.ID = pkt->id;
        monitoringPacketData.deviceType = pkt->deviceType;
        monitoringPacketData.lat = pkt->lat;
        monitoringPacketData.lng = pkt->lng;
        monitoringPacketData.batteryLevel = pkt->batteryLevel;
        monitoringPacketData.status = pkt->status;

        // Copia arrays de volta para a struct global
        memcpy(monitoringPacketData.last5positions, pkt->last5positions, sizeof(pkt->last5positions));
        memcpy(monitoringPacketData.last5events, pkt->last5events, sizeof(pkt->last5events));

        Serial.println("ID: " + String(monitoringPacketData.ID));
        Serial.println("Battery Level: " + String(monitoringPacketData.batteryLevel) + "%");
        
        Serial.println("Last 5 Positions:");
        for (int i = 0; i < 5; i++) {
            // Conversão para visualização (float) acontece APENAS aqui no print
            float latPos = monitoringPacketData.last5positions[i][0] / 1000000.0;
            float lngPos = monitoringPacketData.last5positions[i][1] / 1000000.0;
            
            Serial.println(" Position " + String(i + 1) + ": (" + String(latPos, 6) + ", " + String(lngPos, 6) + ")");
        }
        Serial.println();

    } else if (packetID == ADVERTISE_PACKET) {
        AdvertisePayload *pkt = (AdvertisePayload*)receivedPacket;
        
        Serial.println("Decoding Advertise Packet...");
        advertisePacketData.deviceID = pkt->deviceID;
        advertisePacketData.ID = pkt->id;
        
        Serial.println("Target Device ID: " + String(advertisePacketData.deviceID));
        if(advertisePacketData.ID == ALERT_ADVERTISE) {
            Serial.println("Alert Type: ADVERTISE");
        } else if(advertisePacketData.ID == ALERT_INTERLOCK) {
            Serial.println("Alert Type: INTERLOCK");
        }
        Serial.println();
    }
    return packetID;
}
uint8_t packet::getPacketID() {
    return safetyPacketData.packetID;
}
uint8_t packet::getDeviceID() {
    return safetyPacketData.ID;
}
uint8_t packet::getDeviceType() {
    return safetyPacketData.deviceType;
}
int32_t packet::getLat() {
    return safetyPacketData.lat;
}
int32_t packet::getLng() {
    return safetyPacketData.lng;
}
float packet::getSpeed() {
    return safetyPacketData.speed;
}
float packet::getCourse() {
    return safetyPacketData.course;
}
uint8_t packet::getAdvertiseID() {
    return advertisePacketData.ID;
}