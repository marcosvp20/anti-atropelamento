#include <Arduino.h>
#include <TinyGPS++.h>
#include "LoRaBoards.h"     
#include "VehicleDevice.h"
#include "SimpleTimer.h"

VehicleDevice vehicle;
TinyGPSPlus gps;
SimpleTimer safetyTimer(1000); 

void setup() {
    setupBoards();
    delay(1500);

    Serial.println(F("--- VEÍCULO: V2V Simplificado ---"));
    vehicle.setup();
    vehicle.setID(10); 
}

void loop() {
    // 1. Alimenta o GPS
    while (SerialGPS.available() > 0) {
        gps.encode(SerialGPS.read());
    }

    // 2. Transmissão do Veículo
    if (safetyTimer.isReady()) {
        if (gps.location.isValid()) {
            // Atualiza os atributos da classe usando o GPS real
            vehicle.setLatitude(vehicle.getLatitude()); 
            vehicle.setLongitude(vehicle.getLongitude());
            vehicle.setSpeed(gps.speed.kmph());
            
            Serial.print(F("Veículo em: "));
            Serial.print(gps.location.lat(), 6);
            Serial.print(F(", "));
            Serial.println(gps.location.lng(), 6);

            vehicle.sendSafety();
        } else {
            Serial.println(F("Aguardando sinal GPS..."));
        }
        safetyTimer.reset();
    }

    // 3. Recepção e Cálculo de Distância Direto
    if (vehicle.receive()) {
        // Acessamos os dados decodificados diretamente do objeto 'pckt' da classe
        float latRecebida = vehicle.pckt.safetyData.lat;
        float lngRecebida = vehicle.pckt.safetyData.lng;
        uint8_t idRecebido = vehicle.pckt.safetyData.ID;

        if (gps.location.isValid()) {
            double distancia = TinyGPSPlus::distanceBetween(
                gps.location.lat(),
                gps.location.lng(),
                latRecebida,
                lngRecebida
            );

            Serial.print(F("!!! DISPOSITIVO DETECTADO !!! ID: "));
            Serial.print(idRecebido);
            Serial.print(F(" | Distância: "));
            Serial.print(distancia);
            Serial.println(F(" metros"));
        }
    }
}