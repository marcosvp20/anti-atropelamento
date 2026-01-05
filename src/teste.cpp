#include "packet.h"

uint8_t safetyPacket[SAFETY_PACKET_SIZE];
uint8_t monitoringPacket[MONITORING_PACKET_SIZE];
void setup() {
    Serial.begin(9600);

    // Example data for safety packet
    uint8_t packetID = SAFETY_PACKET;
    uint8_t ID = 1;
    uint8_t deviceType = PERSONAL_DEVICE;
    float latitude = 37.7749;
    float longitude = -122.4194;
    Serial.println("Creating Safety Packet...");

    // Create safety packet
    packet pkt;
    pkt.safetyPacket(packetID, ID, deviceType, latitude, longitude, safetyPacket);

    // Decode the created safety packet
    pkt.decodePacket(safetyPacket);

    // Print decoded safety packet data
    Serial.println("Decoded Safety Packet:");
    Serial.print("Packet ID: "); Serial.println(pkt.safetyPacketData.packetID);
    Serial.print("ID: "); Serial.println(pkt.safetyPacketData.ID);
    Serial.print("Device Type: "); Serial.println(pkt.safetyPacketData.deviceType);
    Serial.print("Latitude: "); Serial.println(pkt.safetyPacketData.latitude, 6);
    Serial.print("Longitude: "); Serial.println(pkt.safetyPacketData.longitude, 6);

    Serial.println("\nCreating Monitoring Packet...");
    // Example data for monitoring packet
    packetID = MONITORING_PACKET;
    ID = 2;
    deviceType = VEHICLE_DEVICE;
    latitude = 34.0522;
    longitude = -118.2437;
    uint8_t batteryLevel = 85;
    float last5positions[5][2] = {
        {34.0522, -118.2437},
        {34.0523, -118.2438},
        {34.0524, -118.2439},
        {34.0525, -118.2440},
        {34.0526, -118.2441}
    };
    uint8_t last5events[5] = {1, 0, 1, 0, 1};
    uint8_t status = 0;
    // Create monitoring packet
    pkt.monitoringPacket(packetID, ID, deviceType, latitude, longitude, batteryLevel, last5positions, last5events, status, monitoringPacket);
    // Decode the created monitoring packet
    pkt.decodePacket(monitoringPacket);
    // Print decoded monitoring packet data
    Serial.println("Decoded Monitoring Packet:");
    Serial.print("Packet ID: "); Serial.println(pkt.monitoringPacketData.packetID);
    Serial.print("ID: "); Serial.println(pkt.monitoringPacketData.ID);
    Serial.print("Device Type: "); Serial.println(pkt.monitoringPacketData.deviceType);
    Serial.print("Latitude: "); Serial.println(pkt.monitoringPacketData.latitude, 6);
    Serial.print("Longitude: "); Serial.println(pkt.monitoringPacketData.longitude, 6);
    Serial.print("Battery Level: "); Serial.println(pkt.monitoringPacketData.batteryLevel);
    Serial.println("Last 5 Positions:");
    for (int i = 0; i < 5; i++) {
        Serial.print("Position "); Serial.print(i + 1); Serial.print(": (");
        Serial.print(pkt.monitoringPacketData.last5positions[i][0], 6); Serial.print(", ");
        Serial.print(pkt.monitoringPacketData.last5positions[i][1], 6); Serial.println(")");
    }
    Serial.println("Last 5 Events:");
    for (int i = 0; i < 5; i++) {
        Serial.print("Event "); Serial.print(i + 1); Serial.print(": ");
        Serial.println(pkt.monitoringPacketData.last5events[i]);
    }
    Serial.print("Status: "); Serial.println(pkt.monitoringPacketData.status);

}
void loop() {

}