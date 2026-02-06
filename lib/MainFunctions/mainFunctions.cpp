#include "mainFunctions.h"

/* ##########################
DEVICE MAIN FUNCTIONS
########################## */

void mainFunctions::ReceivePacketDevice(DeviceBase& device, SimpleTimer& st, unsigned long& jitterTargetTime, bool& waitingToSend, bool& hasTarget) {
  if (device.receive()) {
    uint8_t srcId = device.getReceivedID();
    hasTarget = true;
  }else {
    hasTarget = false;
  }

  if (st.isReady() && !waitingToSend) {
    st.reset(); 

    long jitter = random(0, 201);
    jitterTargetTime = millis() + jitter; 
    waitingToSend = true;
  }
}

void mainFunctions::SendPacketDevice(DeviceBase& device, SimpleTimer& st, unsigned long& jitterTargetTime, bool& waitingToSend) {
  if (waitingToSend && millis() >= jitterTargetTime) {
    if (!device.isChannelBusy(SAFETY_CHANNEL)) {
    device.sendSafety();
    Serial.println("\n");
    Serial.println("###########################");
    Serial.println("Pacote enviado");
    Serial.println("Device ID: " + String(device.getID()));
    Serial.println("Device Latitude: " + String(device.getLatitude()));
    Serial.println("Device Longitude: " + String(device.getLongitude()));
    Serial.println("Satellites: " + String(device.getSatValue()));
    Serial.println("Hdop: " + String(device.getHdop()));
    Serial.println("###########################");

    waitingToSend = false; 
    }
  }
}

/* ##########################
VEHICLE MAIN FUNCTIONS
########################## */

static inline bool hasFixSimpleVehicle(VehicleDevice& vehicle) {
  return vehicle.hasLocation() && vehicle.getSatValid() && vehicle.getHdop() <= 2.0;
}

void mainFunctions::SetVehicleConst(VehicleDevice& vehicle) {

  vehicle.alimentandoGPS();
  bool fixOk = hasFixSimpleVehicle(vehicle);
  bool satValid = vehicle.getSatValid();
  uint32_t sats = satValid ? vehicle.getSatValue() : 0;
  vehicle.setHdop();
  
  if (fixOk) {
    vehicle.setLatitude();
    vehicle.setLongitude();
    vehicle.setSpeed();
    vehicle.setCourse();
    vehicle.setRadius(vehicle.getHdop());
  }
}

/* ##########################
PERSONAL MAIN FUNCTIONS
########################## */

static inline bool hasFixSimplePersonal(PersonalDevice& personal) {
  return personal.hasLocation() && personal.getSatValid() && personal.getHdop() <= 2.0;
}

void mainFunctions::SetPersonalConst(PersonalDevice& personal) {
  personal.alimentandoGPS();
  bool fixOk = hasFixSimplePersonal(personal);
  bool satValid = personal.getSatValid();
  uint32_t sats = satValid ? personal.getSatValue() : 0;
  personal.setHdop();
  
  if (fixOk) {
    personal.setLatitude();
    personal.setLongitude();
    personal.setRadius(personal.getHdop());
  }
}

void mainFunctions::SendTime(PersonalDevice& personal, SimpleTimer& st, bool& hasTarget, int& level, int& lastLevel) {
  if (hasTarget) {
    level = personal.isValidSend(personal.getReceivedLat(), personal.getReceivedLng()); 
  }

  
  if (level != lastLevel) {
    if (level == 1) st.setInterval(3000);
    else if (level == 2) st.setInterval(10000);
    else if (level == 3) st.setInterval(20000);
    st.reset();
    lastLevel = level;
    Serial.println("Level: " + String(lastLevel));
    Serial.println("Nível de alerta atualizado: " + String(level));
  }
  
}
