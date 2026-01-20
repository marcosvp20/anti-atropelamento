#include <Wire.h>
#include <U8g2lib.h>

#include "VehicleDevice.h"
#include "SimpleTimer.h"
#include "display.h"

VehicleDevice vehicle;
SimpleTimer safetyTimer(1000);

// I2C do OLED no T-Beam Supreme
static const int OLED_SDA = 17;
static const int OLED_SCL = 18;

U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE);

Display display(oled);

static inline bool hasFixSimple() {
  return vehicle.hasLocation() && vehicle.getSatValid() && vehicle.getSatValue() >= 4;
}


void setup() {
  setupBoards();
  delay(300);

  // Inicializa I2C do OLED (evita depender do LoRaBoards)
  Wire.begin(OLED_SDA, OLED_SCL);
  oled.begin();
  oled.setFont(u8g2_font_6x12_tf);

  display.begin();
  display.setRefreshMs(200); 
  display.setScreen(Display::SCREEN_GPS);

  Serial.println(F("Envio de coordenadas T-Beam"));
  vehicle.setup();
  vehicle.setID(10);
}

void loop() {
  vehicle.alimentandoGPS();

  display.tick();

  if (safetyTimer.isReady()) {
    Serial.print(F("Transmitindo pacote de seguranca... "));

    if (vehicle.hasLocation()) {
      vehicle.setLatitude();
      vehicle.setLongitude();
      vehicle.setSpeed();
      if (!vehicle.isChannelBusy(SAFETY_CHANNEL)) {
        vehicle.sendSafety();
      } else {
        Serial.println(F("Canal ocupado, aguardando..."));
      }
    } else {
      Serial.println(F("Aguardando sinal GPS..."));
    }

    safetyTimer.reset();
  }
}
