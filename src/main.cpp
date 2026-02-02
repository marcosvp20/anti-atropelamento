#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

#include "LoRaBoards.h"
#include "DeviceBase.h"
#include "SimpleTimer.h"
#include "vehicleDevice.h"

#include <BLEDevice.h>
#include <BLEServer.h>
#include "BLEHandle.h"

#include "mainFunctions.h"

unsigned long lastMillis = 0;

SimpleTimer safetyTimer(1000);

VehicleDevice vehicle;

mainFunctions MF;

void setup() {
  Serial.begin(115200);
  delay(200);

  setupBoards();
  delay(300);

  vehicle.setID(12);
  vehicle.setup();
  
  randomSeed(digitalRead(0));
  lastMillis = millis();
}


void loop() {

  //MF.SetVehicleConst(vehicle);

  //if (safetyTimer.isReady()) {
  //  if (!vehicle.isChannelBusy(SAFETY_CHANNEL)) {
  //    vehicle.sendSafety();
  //    Serial.println("Pacote enviado.");
  //    Serial.println("Device ID: " + String(vehicle.getID()));
  //    Serial.println("\n");
  //  }
  //  safetyTimer.reset();
  //}

  /*
  ###########################
  PACOTE RECEBIDO DE OUTRO DISPOSITIVO
  ###########################
  */
  static uint8_t deviceID = 0;
  if (vehicle.receive()) {
      deviceID = vehicle.getReceivedID();
      Serial.println("Pacote recebido. Device ID: " + String(deviceID));
      Serial.println("\n");
  }
 
}

