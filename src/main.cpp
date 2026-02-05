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

SimpleTimer st(3000);

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
  MF.SetVehicleConst(vehicle);
  MF.ReceivePacketDevice(vehicle, st, jitterTargetTime, waitingToSend);
  MF.SendPacketDevice(vehicle, st, jitterTargetTime, waitingToSend);
}

