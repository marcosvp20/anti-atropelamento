#include "mainFunctions.h"

/* ##########################
DEVICE MAIN FUNCTIONS
########################## */

void mainFunctions::ReceivePacketDevice(DeviceBase& device, SimpleTimer& st, unsigned long& jitterTargetTime, bool& waitingToSend) {
  if (device.receive()) {
    uint8_t srcId = device.getReceivedID();
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