#ifndef PACKET_H
#define PACKET_H

#include <Arduino.h>

// tipos de pacotes
#define SAFETY_PACKET       0x01
#define MONITORING_PACKET   0x02
#define ADVERTISE_PACKET    0x03 
// tipos de dispositivos
#define VEHICLE_DEVICE      0x01
#define PERSONAL_DEVICE     0x02 
// tipos de avisos
#define ALERT_ADVERTISE    0x01
#define ALERT_INTERLOCK   0x02

#define SAFETY_PACKET_SIZE      sizeof(SafetyPayload)
#define MONITORING_PACKET_SIZE  sizeof(MonitoringPayload)
#define ADVERTISE_PACKET_SIZE   sizeof(AdvertisePayload)

// Estruturas auxiliares para armazenar dados decodificados (opcional, para acesso fácil)
extern struct SafetyData {
    uint8_t packetID;
    uint8_t ID;
    uint8_t deviceType;
    int32_t lat;
    int32_t lng;
    float speed;
    float course;
} safetyPacketData;

extern struct MonitoringData {
    uint8_t packetID;
    uint8_t ID;
    uint8_t deviceType;
    int32_t lat;
    int32_t lng;
    uint8_t batteryLevel;
    int32_t last5positions[5][2];
    uint8_t last5events[5];
    uint8_t status;
} monitoringPacketData;

extern struct AdvertiseData {
    uint8_t deviceID;
    uint8_t ID;
} advertisePacketData;


// --- ESTRUTURAS COMPACTADAS (PACKED) ---
// Garantem que os dados fiquem alinhados sem "lixo" de memória

struct __attribute__((packed)) SafetyPayload {
    uint8_t packetType;
    uint8_t id;
    uint8_t deviceType;
    int32_t lat;
    int32_t lng;
    uint8_t speed;
    uint8_t course;
};

struct __attribute__((packed)) MonitoringPayload {
    uint8_t packetType;
    uint8_t id;
    uint8_t deviceType;
    int32_t lat;
    int32_t lng;
    uint8_t batteryLevel;
    int32_t last5positions[5][2];
    uint8_t last5events[5];
    uint8_t status;
};
// __attribute__((packed)) obriga o compilador a não deixar nenhum espaço vazio entre as variáveis da sua estrutura.
struct __attribute__((packed)) AdvertisePayload {
    uint8_t packetType;
    uint8_t id;
    uint8_t deviceID;
};

// --- CLASSE ---
class packet {
public:
    // Mantive as assinaturas idênticas ao seu código original para compatibilidade
    void safetyPacket(uint8_t ID, uint8_t deviceType, int32_t latitude, int32_t longitude, uint8_t *returnPacket, unsigned long speed = 0, unsigned long course = 0);
    
    void monitoringPacket(uint8_t ID, uint8_t deviceType, int32_t latitude, int32_t longitude, uint8_t batteryLevel, int32_t last5positions[5][2], uint8_t last5events[5], uint8_t status, uint8_t *returnPacket);
    
    void advertisePacket(uint8_t ID, uint8_t deviceID, uint8_t *returnPacket);
    
    void decodePacket(uint8_t *receivedPacket);

    // Helpers (Mantive se você quiser usar fora, mas internamente o código resolve)
    uint8_t mapULToUint8(unsigned long value);
    float mapUint8ToFloat(uint8_t value);

    SafetyData safetyData;         
    MonitoringData monitoringData;
};


#endif