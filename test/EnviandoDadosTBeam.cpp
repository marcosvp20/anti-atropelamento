#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

#include "LoRaBoards.h"
#include "VehicleDevice.h"
#include "SimpleTimer.h"

VehicleDevice vehicle;
SimpleTimer safetyTimer(1000);

// I2C do OLED no T-Beam Supreme
static const int OLED_SDA = 17;
static const int OLED_SCL = 18;

// Cria o SEU display (não usa o u8g2 extern do LoRaBoards)
U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

static inline bool hasFixSimple() {
  return vehicle.hasLocation() && vehicle.getSatValid() && vehicle.getSatValue() >= 4;
}

void drawGpsScreen() {
  oled.clearBuffer();
  oled.setFont(u8g2_font_6x12_tf);

  bool satValid = vehicle.getSatValid();
  uint32_t sats = satValid ? vehicle.getSatValue() : 0;
  bool fixOk = hasFixSimple(); // loc válida + sat válido + >=4

  // Título fixo
  oled.drawStr(0, 12, "TSA GPS");

  if (!fixOk) {
    // Tela "procurando"
    oled.drawStr(0, 26, "Procurando sinal GPS...");

    char line2[32];
    if (satValid) snprintf(line2, sizeof(line2), "SAT: %lu", (unsigned long)sats);
    else          snprintf(line2, sizeof(line2), "SAT: --");
    oled.drawStr(0, 40, line2);

    char line3[32];
    snprintf(line3, sizeof(line3), "Tempo: %lus", (unsigned long)(millis() / 1000));
    oled.drawStr(0, 54, line3);

    // opcional: oled.drawStr(0, 64, "Aguardando FIX..."); // 64 pode sair da tela (128x64)
  } else {
    // Tela com dados
    char line1[32], line2[32], line3[32], line4[16];

    snprintf(line1, sizeof(line1), "Lat: %.6f", vehicle.getLatitude());
    snprintf(line2, sizeof(line2), "Lng: %.6f", vehicle.getLongitude());
    snprintf(line3, sizeof(line3), "SAT: %lu", (unsigned long)sats);
    snprintf(line4, sizeof(line4), "FIX: OK");

    oled.drawStr(0, 26, line1);
    oled.drawStr(0, 38, line2);
    oled.drawStr(0, 50, line3);
    oled.drawStr(0, 62, line4);  
  }

  oled.sendBuffer();
}


void setup() {
  setupBoards();
  delay(300);

  // Inicializa I2C do OLED (evita depender do LoRaBoards)
  Wire.begin(OLED_SDA, OLED_SCL);
  oled.begin();

  oled.clearBuffer();
  oled.setFont(u8g2_font_6x12_tf);
  oled.drawStr(0, 12, "Iniciando GPS...");
  oled.sendBuffer();

  Serial.println(F("Envio de coordenadas T-Beam"));
  vehicle.setup();
  vehicle.setID(10);
}

void loop() {
  vehicle.alimentandoGPS();

  // Atualiza display ~5 Hz
  static uint32_t lastDisp = 0;
  if (millis() - lastDisp > 200) {
    drawGpsScreen();
    lastDisp = millis();
  }

  // Envio LoRa 1 Hz
  if (safetyTimer.isReady()) {
    Serial.print(F("Transmitindo pacote de seguranca... "));

    if (vehicle.hasLocation()) {
      vehicle.setLatitude(vehicle.getLatitude());
      vehicle.setLongitude(vehicle.getLongitude());

      if (!vehicle.isChannelBusy(SAFETY_CHANNEL)) {
        vehicle.sendSafety();
      } else {
        Serial.println(F("Canal ocupado, aguardando..."));
      }
    } else {
      Serial.println(F("Aguardando sinal GPS..."));
    }

<<<<<<< HEAD
    safetyTimer.reset();
  }
}