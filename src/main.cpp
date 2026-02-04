#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

#include "LoRaBoards.h"
#include "DeviceBase.h"
#include "vehicleDevice.h"
#include "PersonalDevice.h"

#include <BLEDevice.h>
#include <BLEServer.h>
#include "BLEHandle.h"

#include "mainFunctions.h"
#include "esp_system.h"   
#include "SimpleTimer.h"

VehicleDevice vehicle;
mainFunctions MF;

SimpleTimer st(4000);

unsigned long jitterTargetTime = 0;
bool waitingToSend = false;

void setup() {
  Serial.begin(115200);
  delay(200);

  setupBoards();
  delay(300);

  vehicle.setID(12);
  vehicle.setup();

  randomSeed((uint32_t)esp_random() ^ (uint32_t)micros());
}

void loop() {
  //MF.SetVehicleConst(vehicle);

  // ===================== RX sempre =====================
  if (vehicle.receive()) {
    uint8_t srcId = vehicle.getReceivedID();
    Serial.println("Pacote recebido. Device ID: " + String(srcId));
    Serial.println();
  }

  if (st.isReady() && !waitingToSend) {
    st.reset(); 

    long jitter = random(0, 201);
    jitterTargetTime = millis() + jitter; 
    waitingToSend = true; 
  }

  // ===================== TX slot =====================
  if (waitingToSend && millis() >= jitterTargetTime) {
    vehicle.sendSafety();
    Serial.println("Pacote enviado");
    Serial.println("Device ID: " + String(vehicle.getID()));
    Serial.println();

    waitingToSend = false; 
  }
}
