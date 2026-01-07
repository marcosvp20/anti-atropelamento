#include <BLEDevice.h>
#include <BLEServer.h>
#include "PersonalDevice.h"
#include "BLEHandle.h"
PersonalDevice device;
BLEHandle ble(device); // Vincula o Bluetooth ao Crachá
int i = 0;



void setup() {
    Serial.begin(9600);
    device.setup();
    device.setID(1);
    device.setLatitude(57.7749);
    device.setLongitude(-102.4194);
    device.setAccelerationX(0.5);
    device.setAccelerationY(1.0);

    ble.begin("CRACHA_LORA_01"); // Inicializa Bluetooth
    Serial.println("Aguardando configuração...");
    while (device.getID() == 0) {
        delay(500);
        Serial.print("."); // Feedback visual no Serial
    }
    Serial.println("\nConfigurado! Iniciando loop...");
}
void loop() {
    device.receive();
    if(device.isChannelBusy(SAFETY_CHANNEL) && i < 5)
    {
        Serial.println("Channel is busy, waiting... a random time");
        delay(random(500, 1000));
    }
    else
    {
        Serial.println("Sending Safety Packet...");
        device.sendSafety();
        delay(1000);
    }
    if(i == 5) {
        Serial.println("Sending Monitoring Packet...");
        if(device.isChannelBusy(MONITORING_CHANNEL))
        {
            Serial.println("Channel is busy, waiting... a random time");
            delay(random(500, 1000));
        }
        else
        {
            device.sendMonitoring();
        }
        i = 0;
    }
    i++;
    Serial.println("--- DADOS DO DISPOSITIVO ---");
    Serial.printf("ID: %d | Lat: %.6f | Lng: %.6f\n", 
                  device.getID(), 
                  device.getLatitude(), 
                  device.getLongitude());
    Serial.println("----------------------------");

    delay(2000); 
}