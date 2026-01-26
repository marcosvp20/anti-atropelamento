#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

#include "LoRaBoards.h"
#include "VehicleDevice.h"
#include "PersonalDevice.h"
#include "SimpleTimer.h"

#include "Display.h"
#include "ScreenNavigator.h"

// ===== T-BEAM S3 CORE =====
static const int PIN_BOOT  = 0;

// ===== I2C do OLED =====
static const int OLED_SDA = 17;
static const int OLED_SCL = 18;

VehicleDevice vehicle;
PersonalDevice personal;

SimpleTimer safetyTimer(5000);

// SH1106 via HW I2C
U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE);

Display display(oled);
ScreenNavigator nav(display, PIN_BOOT, true);

static inline bool hasFixSimple() {
  return vehicle.hasLocation() && vehicle.getSatValid() && vehicle.getSatValue() >= 4;
}

void setup() {
  Serial.begin(115200);
  delay(200);

  setupBoards();
  delay(300);

  Wire.begin(OLED_SDA, OLED_SCL);

  oled.begin();
  oled.setFont(u8g2_font_6x12_tf);

  display.begin();
  display.setRefreshMs(200);

  nav.begin();

  display.setGpsStatus(false, 0, false, 0, 0, 0, 0);
}

void loop() {
  // ===== GPS =====
  vehicle.alimentandoGPS();

  bool fixOk = hasFixSimple();
  bool satValid = vehicle.getSatValid();
  uint32_t sats = satValid ? vehicle.getSatValue() : 0;

  display.setGpsStatus(
    satValid, sats, fixOk,
    vehicle.getLatitude(), vehicle.getLongitude(),
    vehicle.getHdop(), vehicle.getSpeed()
  );

  // alvo (ex.: ultima posicao recebida do outro dispositivo)
  float targetLat = device.getReceivedLat();
  float targetLng = device.getReceivedLng();

  // Aqui eu chamei como se ela existisse em vehicle:
  int level = personal.isValidSend(targetLat, targetLng);
  // level: 1 -> 1s, 2 -> 5s, 3 -> nao envia

  // ajusta timer quando muda o level
  static int lastLevel = -1;
  if (level != lastLevel) {
    if (level == 1) safetyTimer.setInterval(1000);
    else if (level == 2) safetyTimer.setInterval(5000);
    safetyTimer.reset();     
    lastLevel = level;
  }

  // ===== ENVIO LoRa COM safetyTimer =====
  if ((level == 1 || level == 2) && safetyTimer.isReady()) {
    if (vehicle.hasLocation() && !vehicle.isChannelBusy(SAFETY_CHANNEL)) {
      vehicle.sendSafety();
    }
    safetyTimer.reset();
  }

  // ===== UI =====
  nav.tick();
  display.tick();
}
