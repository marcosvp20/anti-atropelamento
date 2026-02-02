#include "mainFunctions.h"

static inline bool hasFixSimple(VehicleDevice& vehicle) {
  return vehicle.hasLocation() && vehicle.getSatValid() && vehicle.getHdop() <= 2.0;
}

void mainFunctions::SetVehicleConst(VehicleDevice& vehicle) {

  vehicle.alimentandoGPS();
  bool fixOk = hasFixSimple(vehicle);
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