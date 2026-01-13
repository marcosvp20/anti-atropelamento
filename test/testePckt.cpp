#include "packet.h"
packet pckt;
uint8_t safetyPacket[13];
void setup()
{
    Serial.begin(9600);
    pckt.safetyPacket(1, VEHICLE_DEVICE, 37774900, -1224194, safetyPacket, 25.1231, 123.45);
    pckt.decodePacket(safetyPacket);
}
void loop()
{
}