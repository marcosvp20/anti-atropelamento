#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include "PersonalDevice.h"
#include "BLEHandle.h"
#include "SimpleTimer.h" 
PersonalDevice device;
BLEHandle ble(device); // Vincula o Bluetooth ao Crachá

#define MOVING_SAFETY_TIMER 1000
#define STOPPED_SAFETY_TIMER 5000
#define MONITORING_TIMER 60000

// Local 2 (O outro ponto que você mandou)
#define LOCAL_1_LAT  -19.96792253900216
#define LOCAL_1_LNG  -43.955467856491474

#define LOCAL_2_LAT  -19.96747866279618
#define LOCAL_2_LNG  -43.955390748973954

// #define LOCAL_2_LAT  -19.968256996158512
// #define LOCAL_2_LNG  -43.95160025506842
// --- INSTANCIAÇÃO DOS TIMERS ---
// Define os intervalos iniciais
SimpleTimer safetyTimer(STOPPED_SAFETY_TIMER);      
SimpleTimer monitoringTimer(MONITORING_TIMER); 
SimpleTimer simTimer(50000);        

bool speedSimulated = false;
unsigned long lastMillis = 0;

void setup() {
    Serial.begin(9600);
    device.setup();
    device.setID(1);
    device.setLatitude(LOCAL_1_LAT);
    device.setLongitude(LOCAL_1_LNG);
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
    
    // Inicializa semente para números aleatórios
    randomSeed(digitalRead(0));
    lastMillis = millis();
}

void loop() {
    
    device.receive();
    // IMPORTANTE: Só alteramos o intervalo se ele for >= 1000.
    // Se for menor (ex: 750ms), significa que estamos num "random backoff" esperando o canal liberar,
    // então não podemos forçar de volta para 1s ou 5s agora.
    if (safetyTimer.getInterval() >= 1000) {
        if(device.getSpeed() > 1.0) {
            // O dispositivo esta se movendo, periodo de envio é 1 segundo
            // Só seta se for diferente para evitar chamadas desnecessárias
            if(safetyTimer.getInterval() != MOVING_SAFETY_TIMER) {
                 Serial.println(">>> Device is moving.");
                 safetyTimer.setInterval(MOVING_SAFETY_TIMER); 
            }
        } else {
            // O dispositivo esta parado, periodo de envio é 5 segundos
            if(safetyTimer.getInterval() != STOPPED_SAFETY_TIMER) {
                 safetyTimer.setInterval(STOPPED_SAFETY_TIMER); 
            }
        }
    }

    // --- SAFETY PACKET LOGIC ---
    if (safetyTimer.isReady()) {
        
        if (device.isChannelBusy(SAFETY_CHANNEL)) {
            Serial.println("Channel is busy, waiting... a random time");
            
            // Backoff: Configura timer para tentar dnv entre 500ms e 1000ms
            safetyTimer.setInterval(random(500, 1000));
            safetyTimer.reset(); 
        } 
        else {
            Serial.println("Sending Safety Packet...");
            //Serial.println("Intervalo atual do Safety Timer: " + String(millis() - lastMillis) + " ms");
            device.sendSafety();
            //Serial.println("Distance calculed between devices: "+ String(device.calculateDistance(LOCAL_2_LAT, LOCAL_2_LNG)) + "m");
            
            safetyTimer.setInterval(STOPPED_SAFETY_TIMER);
            safetyTimer.reset(); 
        }
    }

    // --- MONITORING PACKET LOGIC ---
    if (monitoringTimer.isReady()) {
        Serial.println("Sending Monitoring Packet..."); 

        if (device.isChannelBusy(MONITORING_CHANNEL)) {
            Serial.println("Channel is busy, waiting... a random time");
            
            // Backoff: Tenta novamente em breve
            monitoringTimer.setInterval(random(500, 1000));
            monitoringTimer.reset();
        } 
        else {
            Serial.println("Sending Monitoring Packet...");
            device.sendMonitoring();
            device.setSpeed(0.0);
            
            // Retorna para o padrão de 60 segundos
            monitoringTimer.setInterval(MONITORING_TIMER);
            monitoringTimer.reset();
        }
    }


    // --- SIMULATION LOGIC ---
    if (!speedSimulated && simTimer.isReady()) {
        device.setSpeed(3.0);
        Serial.println(">>> Speed set to 3.0 m/s");
        speedSimulated = true;
    }
}