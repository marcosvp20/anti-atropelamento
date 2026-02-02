#ifndef MAINFUNCTIONS_H
#define MAINFUNCTIONS_H

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "PersonalDevice.h"
#include "VehicleDevice.h"
#include "packet.h"
#include "SimpleTimer.h"

class mainFunctions {
public:

    void SendAlertDest();
    void SetVehicleConst(VehicleDevice& vehicle);

};

#endif