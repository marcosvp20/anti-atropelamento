#ifndef VEHICLEDEVICE_H
#define VEHICLEDEVICE_H

#include "DeviceBase.h"

class VehicleDevice : public DeviceBase {
public:
    VehicleDevice();

    void alimentandoGPS();

    bool hasLocation() { return gps.location.isValid(); }
    int  getSatValue() { return gps.satellites.value(); }
    bool getSatValid() { return gps.satellites.isValid(); }

protected:
    void buildSafetyPacket() override;
    void buildMonitoringPacket() override;
    void onReceiveDecoded() override;
};

#endif
