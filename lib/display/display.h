#pragma once
#include <Arduino.h>
#include <U8g2lib.h>
#include "packet.h"  

class Display {
public:
  explicit Display(U8G2 &d);

  void begin();

  // Atualização
  void setRefreshMs(uint32_t ms);
  void tick(); 

  // escolhe qual tela mostrar
  enum Screen : uint8_t { SCREEN_GPS, SCREEN_SAFETY, SCREEN_MONITORING, SCREEN_ADVERTISE, SCREEN_MESSAGE };

  void setScreen(Screen s);

  // Alimenta dados
  void setSafety(const SafetyData &d);
  void setMonitoring(const MonitoringData &d);
  void setAdvertise(const AdvertiseData &d);


  // passa os valores 
  void setGpsStatus(bool satValid, uint32_t sats, bool fixOk, double lat, double lng, float hdop, float speed);

  // Mensagem genérica
  void setMessage(const String &title, const String &l1, const String &l2 = "", const String &l3 = "");

private:
  U8G2 &oled;

  // timing do tick()
  uint32_t refreshMs = 200;
  uint32_t lastDisp = 0;

  Screen current = SCREEN_GPS;

  // caches
  SafetyData sData{};
  MonitoringData mData{};
  AdvertiseData aData{};

  struct {
    bool satValid = false;
    uint32_t sats = 0;
    bool fixOk = false;
    double lat = 0;
    double lng = 0;
    float hdop = 0;
    float speed = 0;
  } gps;

  String msgTitle, msg1, msg2, msg3;

  // draw functions (equivalentes às “funcionalidades”)
  void drawGpsScreen();
  void drawSafetyScreen();
  void drawMonitoringScreen();
  void drawAdvertiseScreen();
  void drawMessageScreen();
};
