#include <BLEDevice.h>
#include <BLEServer.h>
#include "PersonalDevice.h"

PersonalDevice meuCracha;

// Callback para recepção via Bluetooth
class MyBLECallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string rxValue = pCharacteristic->getValue();
        if (rxValue.length() > 0) {
            // A classe processa a string "ID;LAT;LNG" e usa os setters internos
            meuCracha.updateFromBluetooth(String(rxValue.c_str()));
        }
    }
};

void setup() {
    Serial.begin(115200);
    meuCracha.setup(); // Inicializa hardware

    // Inicialização do Bluetooth (mesmo código anterior)
    BLEDevice::init("CRACHA_LORA_DEVICE");
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(CONFIG_SERVICE_UUID);
    BLECharacteristic *pChar = pService->createCharacteristic(
                                    CONFIG_CHARACTERISTIC_UUID,
                                    BLECharacteristic::PROPERTY_WRITE
                                );
    pChar->setCallbacks(new MyBLECallbacks());
    pService->start();
    BLEDevice::startAdvertising();

    // --- LOOP DE BLOQUEIO ---
    Serial.println("AGUARDANDO CONFIGURAÇÃO VIA BLUETOOTH...");
    
    // Enquanto o ID for 0, o código não avança para o loop()
    while (meuCracha.getID() == 0) {
        delay(500);
        Serial.print("."); // Feedback visual no Serial
    }

    Serial.println("\nConfiguração recebida! Saindo do setup...");
}

void loop() {
    // Feedback constante dos dados da biblioteca
    Serial.println("--- DADOS DO DISPOSITIVO ---");
    Serial.print("ID do Funcionário: "); Serial.println(meuCracha.getID());
    Serial.print("Latitude (A-GPS): ");  Serial.println(meuCracha.getLatitude(), 6);
    Serial.print("Longitude (A-GPS): "); Serial.println(meuCracha.getLongitude(), 6);
    Serial.println("----------------------------");

    delay(2000); 
}