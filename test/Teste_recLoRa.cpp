#include "packet.h"
#include "cmslora.h"
uint8_t safetyPacket[SAFETY_PACKET_SIZE];
uint8_t monitoringPacket[MONITORING_PACKET_SIZE];

packet pckt;
CMSLoRa lora;

void setup()
{
    Serial.begin(9600);
    lora.begin();
}
void loop()
{
    lora.receiveData(safetyPacket, SAFETY_PACKET_SIZE);
    Serial.println("Receiving Safety Packet...");
    pckt.decodePacket(safetyPacket);
    Serial.print("Packet ID: "); Serial.println(pckt.safetyPacketData.packetID);
    Serial.print("ID: "); Serial.println(pckt.safetyPacketData.ID);
    Serial.print("Device Type: "); Serial.println(pckt.safetyPacketData.deviceType);
    Serial.print("Latitude: "); Serial.println(pckt.safetyPacketData.latitude, 6);
    Serial.print("Longitude: "); Serial.println(pckt.safetyPacketData.longitude, 6);
    delay(10000);
}