#include "Display.h"

Display::Display(U8G2 &d) : oled(d) {}

void Display::begin() {
  // NÃO faz oled.begin() aqui, porque a main já tem a configuração toda.
  oled.setFont(u8g2_font_6x12_tf);
}

void Display::setRefreshMs(uint32_t ms) { refreshMs = ms; }
void Display::setScreen(Screen s) { current = s; }
void Display::setSafety(const SafetyData &d) { sData = d; }
void Display::setMonitoring(const MonitoringData &d) { mData = d; }
void Display::setAdvertise(const AdvertiseData &d) { aData = d; }

void Display::setGpsStatus(bool satValid, uint32_t sats, bool fixOk,
                                  double lat, double lng, float hdop, float speed) {
  gps.satValid = satValid;
  gps.sats = sats;
  gps.fixOk = fixOk;
  gps.lat = lat;
  gps.lng = lng;
  gps.hdop = hdop;
  gps.speed = speed;
}

void Display::setMessage(const String &title, const String &l1, const String &l2, const String &l3) {
  msgTitle = title; msg1 = l1; msg2 = l2; msg3 = l3;
  current = SCREEN_MESSAGE;
}

void Display::tick() {
  if (millis() - lastDisp < refreshMs) return;
  lastDisp = millis();

  switch (current) {
    case SCREEN_GPS:        drawGpsScreen(); break;
    case SCREEN_SAFETY:     drawSafetyScreen(); break;
    case SCREEN_MONITORING: drawMonitoringScreen(); break;
    case SCREEN_ADVERTISE:  drawAdvertiseScreen(); break;
    case SCREEN_MESSAGE:    drawMessageScreen(); break;
  }
}

// -------------------- TELAS --------------------

void Display::drawGpsScreen() {
  oled.clearBuffer();
  oled.setFont(u8g2_font_6x12_tf);

  if (!gps.fixOk) {
    oled.drawStr(0, 12, "TSA GPS");
    oled.drawStr(0, 28, "Procurando sinal GPS...");

    char line2[32];
    if (gps.satValid) snprintf(line2, sizeof(line2), "SAT: %lu", (unsigned long)gps.sats);
    else             snprintf(line2, sizeof(line2), "SAT: --");
    oled.drawStr(0, 44, line2);

    char line3[32];
    snprintf(line3, sizeof(line3), "Tempo: %lus", (unsigned long)(millis() / 1000));
    oled.drawStr(0, 60, line3);

    oled.sendBuffer();
    return;
  }

  // Tela com fix OK
  oled.drawStr(0, 12, "GPS OK");

  // Lat/Lng
  char l1[32], l2[32];
  snprintf(l1, sizeof(l1), "Lat: %.6f", gps.lat);
  snprintf(l2, sizeof(l2), "Lng: %.6f", gps.lng);
  oled.drawStr(0, 28, l1);
  oled.drawStr(0, 40, l2);

  // HDOP / Speed / SAT
  char l3[32], l4[32];
  snprintf(l3, sizeof(l3), "HDOP: %.2f  SAT:%lu", gps.hdop, (unsigned long)(gps.satValid ? gps.sats : 0));
  snprintf(l4, sizeof(l4), "Speed: %.2f", gps.speed);
  oled.drawStr(0, 52, l3);
  oled.drawStr(0, 64, l4);

  oled.sendBuffer();
}

void Display::drawSafetyScreen() {
  oled.clearBuffer();
  oled.setFont(u8g2_font_6x12_tf);

  oled.drawStr(0, 12, "SAFETY");

  // sData.lat/lng geralmente em micrograus no seu pacote
  float lat = sData.lat / 1000000.0f;
  float lng = sData.lng / 1000000.0f;

  char l1[32], l2[32], l3[32], l4[32];
  snprintf(l1, sizeof(l1), "ID:%u  Type:%u", sData.ID, sData.deviceType);
  snprintf(l2, sizeof(l2), "Lat: %.6f", lat);
  snprintf(l3, sizeof(l3), "Lng: %.6f", lng);
  snprintf(l4, sizeof(l4), "Spd: %.1f  Crs: %.0f", sData.speed, sData.course);

  oled.drawStr(0, 28, l1);
  oled.drawStr(0, 40, l2);
  oled.drawStr(0, 52, l3);
  oled.drawStr(0, 64, l4);

  oled.sendBuffer();
}

void Display::drawMonitoringScreen() {
  oled.clearBuffer();
  oled.setFont(u8g2_font_6x12_tf);

  oled.drawStr(0, 12, "MONITORING");

  char l1[32], l2[32];
  snprintf(l1, sizeof(l1), "ID:%u  Bat:%u%%", mData.ID, mData.batteryLevel);
  snprintf(l2, sizeof(l2), "Status:%u", mData.status);

  oled.drawStr(0, 28, l1);
  oled.drawStr(0, 40, l2);

  // mostra 1 posicao (ex.: a mais recente = [0])
  float lat = mData.last5positions[0][0] / 1000000.0f;
  float lng = mData.last5positions[0][1] / 1000000.0f;

  char l3[32], l4[32];
  snprintf(l3, sizeof(l3), "P1: %.4f", lat);
  snprintf(l4, sizeof(l4), "    %.4f", lng);

  oled.drawStr(0, 52, l3);
  oled.drawStr(0, 64, l4);

  oled.sendBuffer();
}

void Display::drawAdvertiseScreen() {
  oled.clearBuffer();
  oled.setFont(u8g2_font_6x12_tf);

  oled.drawStr(0, 12, "ADVERTISE");

  char l1[32], l2[32], l3[32];
  snprintf(l1, sizeof(l1), "Target ID:%u", aData.deviceID);
  snprintf(l2, sizeof(l2), "Alert ID:%u", aData.ID);

  const char *tipo = "DESCONHEC.";
  if (aData.ID == ALERT_ADVERTISE) tipo = "ADVERTISE";
  else if (aData.ID == ALERT_INTERLOCK) tipo = "INTERLOCK";

  snprintf(l3, sizeof(l3), "Tipo: %s", tipo);

  oled.drawStr(0, 28, l1);
  oled.drawStr(0, 44, l2);
  oled.drawStr(0, 60, l3);

  oled.sendBuffer();
}

void Display::drawMessageScreen() {
  oled.clearBuffer();
  oled.setFont(u8g2_font_6x12_tf);

  oled.drawStr(0, 12, msgTitle.c_str());
  oled.drawStr(0, 28, msg1.c_str());
  if (msg2.length()) oled.drawStr(0, 44, msg2.c_str());
  if (msg3.length()) oled.drawStr(0, 60, msg3.c_str());

  oled.sendBuffer();
}
