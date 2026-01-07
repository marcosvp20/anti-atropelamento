#include <BLEDevice.h>
#include <BLEServer.h>
#include "PersonalDevice.h"
#include "BLEHandle.h"
PersonalDevice device;
BLEHandle ble(device); // Vincula o Bluetooth ao Crachá
int SafetyCount = 0;
int MonitoringCount = 0;
int TxSafetyPeriod = 5; // Periodicidade de envio dos pacotes 
int TxMonitoringPeriod = 180; // Periodicidade de envio dos pacotes (3 minutos)
void setup() {
    Serial.begin(9600);
    device.setup();
    device.setID(1);
    device.setLatitude(57.7749);
    device.setLongitude(-102.4194);
    device.setAccelerationX(0.5);
    device.setAccelerationY(1.0);
    device.setSpeed(0.0);

    ble.begin("CRACHA_LORA_01"); // Inicializa Bluetooth
    Serial.println("Aguardando configuração...");
    while (device.getID() == 0) {
        delay(500);
        Serial.print("."); // Feedback visual no Serial
    }
    Serial.println("\nConfigurado! Iniciando loop...");
}
void loop() {
    
    device.receive(); // essa instrução tem um timeout de 1 segundo e será usada como base para o restante das temporizações
    SafetyCount++;
    MonitoringCount++;
    Serial.println("SafetyCount: " + String(SafetyCount) + " | MonitoringCount: " + String(MonitoringCount) + " | TxSafetyPeriod: " + String(TxSafetyPeriod));
    if(device.getSpeed() > 1.0)
    {
        Serial.println(">>> Device is moving.");
        //O dispositivo esta se movendo, periodo de envio é 1 segundo
        TxSafetyPeriod = 1;
    }
    else
    {
        //O dispositivo esta parado, periodo de envio é 5 segundos
        TxSafetyPeriod = 5;
    }
    if(device.isChannelBusy(SAFETY_CHANNEL) && SafetyCount == TxSafetyPeriod)
    {
        Serial.println("Channel is busy, waiting... a random time");
        delay(random(500, 1000));
        SafetyCount--; // mantem o contador para tentar enviar no próximo ciclo
    }
    else if(!device.isChannelBusy(SAFETY_CHANNEL) && SafetyCount == TxSafetyPeriod)
    {
        Serial.println("Sending Safety Packet...");
        device.sendSafety();
        SafetyCount = 0;
    }
    if(MonitoringCount == 60) {
        Serial.println("Sending Monitoring Packet...");
        if(device.isChannelBusy(MONITORING_CHANNEL))
        {
            Serial.println("Channel is busy, waiting... a random time");
            delay(random(500, 1000));
            MonitoringCount--; // mantem o contador para tentar enviar no próximo ciclo
        }
        else
        {
            Serial.println("Sending Monitoring Packet...");
            device.sendMonitoring();
            device.setSpeed(0.0);
            MonitoringCount = 0;
        }
        
    }
    if(MonitoringCount == 50)
    {
        device.setSpeed(3.0);
        SafetyCount = 0;
        Serial.println(">>> Speed set to 3.0 m/s");
    }
    // Serial.println("--- DADOS DO DISPOSITIVO ---");
    // Serial.printf("ID: %d | Lat: %.6f | Lng: %.6f\n", 
    //               device.getID(), 
    //               device.getLatitude(), 
    //               device.getLongitude());
    // Serial.println("----------------------------");
}