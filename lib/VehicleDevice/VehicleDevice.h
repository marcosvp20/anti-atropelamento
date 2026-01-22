#ifndef VEHICLEDEVICE_H
#define VEHICLEDEVICE_H

#include "DeviceBase.h"

class VehicleDevice : public DeviceBase {
public:
    VehicleDevice();

    bool hasLocation() { return gps.location.isValid(); }
    int  getSatValue() { return gps.satellites.value(); }
    bool getSatValid() { return gps.satellites.isValid(); }

    void buildDynamicBubble360(
        float headingDeg,
        float baseRadius,
        float maxExtra,
        int   qntdSetores,
        float exponent
    );

    const float* getBubble360() const { return bubble360_; }

    float getBubbleAt(int deg) const { return bubble360_[(deg % 360 + 360) % 360]; }


protected:
    void buildSafetyPacket() override;
    void buildMonitoringPacket() override;
    void onReceiveDecoded() override;

private:
    float bubble360_[360];

};

#endif
