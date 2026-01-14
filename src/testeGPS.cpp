#include <Arduino.h>
#include <Wire.h>
#include <TinyGPS++.h>
#include <XPowersLib.h> // Certifique-se que esta lib está no platformio.ini

// --- DEFINIÇÕES DE PINOS (T-Beam Supreme S3) ---
#define GPS_RX_PIN      42
#define GPS_TX_PIN      41
#define GPS_BAUD_RATE   9600

#define PMU_SDA         17
#define PMU_SCL         18

// --- OBJETOS ---
TinyGPSPlus gps;

// CORREÇÃO: Usar a classe específica do chip AXP2101 em vez de XPowersPMU
XPowersAXP2101 PMU; 

void setup() {
    // Configurações USB para ESP32-S3 (Necessário para ver o Serial)
    Serial.begin(115200);
    delay(3000); // Espera um pouco para dar tempo de abrir o monitor
    Serial.println("\n--- Iniciando T-Beam Supreme GPS Test ---");

    // 1. INICIALIZAR O GERENCIADOR DE ENERGIA
    // O endereço do AXP2101 é padrão, mas passamos Wire e pinos
    if (!PMU.begin(Wire, AXP2101_SLAVE_ADDRESS, PMU_SDA, PMU_SCL)) {
        Serial.println("ERRO CRÍTICO: Falha ao comunicar com o AXP2101!");
        Serial.println("Verifique se a bateria está conectada.");
        while (1) delay(1000);
    }

    // 2. LIGAR ENERGIA DO GPS
    // No T-Beam Supreme, o GPS é alimentado pelo ALDO4
    Serial.println("Ligando energia do GPS (ALDO4)...");
    
    PMU.setALDO4Voltage(3300); // Define 3.3V
    PMU.enableALDO4();         // Liga o canal

    // Opcional: ALDO3 costuma ser LED ou periférico do GPS
    PMU.setALDO3Voltage(3300);
    PMU.enableALDO3();

    // Importante: Limpar buffer de IRQ para evitar problemas
    PMU.clearIrqStatus();

    Serial.println("Energia OK.");

    // 3. INICIALIZAR COMUNICAÇÃO SERIAL COM GPS
    Serial1.begin(GPS_BAUD_RATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    Serial.println("Aguardando dados de satélites (Vá para céu aberto)...");
}

void loop() {
    // Leitura dos dados do GPS
    while (Serial1.available() > 0) {
        char c = Serial1.read();
        gps.encode(c);
        
        // Debug: Se quiser ver se CHEGA algo (mesmo lixo), descomente:
        // Serial.write(c); 
    }

    // Se tivermos dados válidos decodificados
    if (gps.location.isUpdated()) {
        Serial.print("FIX OK! | Sats: ");
        Serial.print(gps.satellites.value());
        Serial.print(" | Lat: ");
        Serial.print(gps.location.lat(), 6);
        Serial.print(" | Lng: ");
        Serial.print(gps.location.lng(), 6);
        Serial.print(" | Alt: ");
        Serial.println(gps.altitude.meters());
    }

    // Diagnóstico simples
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 2000) {
        if (gps.charsProcessed() < 10) {
             Serial.println("Sem dados brutos do GPS. Verifique pinos/energia.");
        } else if (!gps.location.isValid()) {
             Serial.print("Recebendo dados, mas sem FIX (Sats: ");
             Serial.print(gps.satellites.value());
             Serial.println(")");
        }
        lastPrint = millis();
    }
}