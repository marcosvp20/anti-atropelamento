#include <Arduino.h>
#include <TinyGPS++.h>
#include <XPowersLib.h>
#include "utilities.h"
#include "PersonalDevice.h"
#include "SimpleTimer.h"
#include <TFT_eSPI.h>
#include <SPI.h>

TinyGPSPlus gps;
XPowersAXP2101 PMU;
PersonalDevice device;
SimpleTimer safetyTimer(1000);
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite img = TFT_eSprite(&tft);

// UART do GPS
#define GPS_RX_PIN  41   // ajuste
#define GPS_TX_PIN  42   // ajuste
#define GPS_BAUD    38400

#define USING_TWATCH_S3
#define GPSSerial Serial1

// 1. Adicione isso perto da declaração do tft

// 2. No setup(), logo após tft.init(), inicialize o Sprite
// O T-Watch S3 Plus tem RAM de sobra para um Sprite de 240x160 (ou maior)

void beginPower(){
    
    PMU.setALDO1Voltage(3300); PMU.enableALDO1();
    PMU.setALDO2Voltage(3300); PMU.enableALDO2();
    //PMU.setALDO3Voltage(3300); PMU.enableALDO3();
    PMU.setALDO4Voltage(3300); PMU.enableALDO4();
    PMU.setBLDO1Voltage(3300); PMU.enableBLDO1();
    PMU.setBLDO2Voltage(3300); PMU.enableBLDO2();
    // PMU.setDLDO1Voltage(3300); PMU.enableDLDO1();
    
    PMU.disableDC2();
    PMU.disableDC4();
    PMU.disableDC5();
    PMU.disableCPUSLDO();
    PMU.disableDLDO1();
    PMU.disableDLDO2();


    PMU.setVbusVoltageLimit(XPOWERS_AXP2101_VBUS_VOL_LIM_4V36);
    PMU.setVbusCurrentLimit(XPOWERS_AXP2101_VBUS_CUR_LIM_900MA);
    PMU.setSysPowerDownVoltage(2600);
    
    PMU.setPowerKeyPressOffTime(XPOWERS_POWEROFF_4S);
    PMU.setPowerKeyPressOnTime(XPOWERS_POWERON_128MS);
    PMU.disableTSPinMeasure();
    PMU.enableBattDetection();
    PMU.enableVbusVoltageMeasure();
    PMU.enableBattVoltageMeasure();
    PMU.enableSystemVoltageMeasure();
    PMU.setChargingLedMode(XPOWERS_CHG_LED_OFF);

    PMU.disableIRQ(XPOWERS_AXP2101_ALL_IRQ);
    PMU.enableIRQ(
        XPOWERS_AXP2101_BAT_INSERT_IRQ    | XPOWERS_AXP2101_BAT_REMOVE_IRQ      |   //BATTERY
        XPOWERS_AXP2101_VBUS_INSERT_IRQ   | XPOWERS_AXP2101_VBUS_REMOVE_IRQ     |   //VBUS
        XPOWERS_AXP2101_PKEY_SHORT_IRQ    | XPOWERS_AXP2101_PKEY_LONG_IRQ       |   //POWER KEY
        XPOWERS_AXP2101_BAT_CHG_DONE_IRQ  | XPOWERS_AXP2101_BAT_CHG_START_IRQ       //CHARGE
    );
    PMU.clearIrqStatus();

    PMU.setPrechargeCurr(XPOWERS_AXP2101_PRECHARGE_50MA);
    PMU.setChargerConstantCurr(XPOWERS_AXP2101_CHG_CUR_125MA);
    PMU.setChargerTerminationCurr(XPOWERS_AXP2101_CHG_ITERM_25MA);
    PMU.setChargeTargetVoltage(XPOWERS_AXP2101_CHG_VOL_4V35);
    PMU.setButtonBatteryChargeVoltage(3300);
    PMU.enableButtonBatteryCharge();
}
void displayInfo();
void setup()
{
    Serial.begin(115200);
    delay(1000);
    Wire.begin(10,11);
    if (!PMU.begin(Wire, AXP2101_SLAVE_ADDRESS, 10, 11)) {
        Serial.println("Falha ao encontrar o PMU AXP2101!");
        
    }
    delay(5000);
    

    Serial.println();
    Serial.println(F("DeviceExample.ino"));
    Serial.println(F("TinyGPS++ sem LoRaBoards"));
    Serial.print(F("TinyGPS++ v "));
    Serial.println(TinyGPSPlus::libraryVersion());
    Serial.println();

    // Inicializa UART do GPS
    GPSSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    device.setup();
    device.setID(1); // ID do dispositivo
    tft.init();
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);
    pinMode(45, OUTPUT);
    digitalWrite(45, HIGH); // Brilho no máximo
    img.createSprite(240, 240);
    tft.setTextColor(TFT_WHITE, TFT_BLACK); // Texto branco, fundo preto para evitar rastro
    tft.setTextSize(2);
    beginPower();

    delay(1500); // alguns GPS precisam
}
void updateDisplay() {
    // Desenha no buffer (Sprite) em vez de desenhar na tela diretamente
    img.fillSprite(TFT_BLACK); 
    img.drawString("   TSA MONITOR", 10, 10, 2);
    img.drawFastHLine(0, 35, 240, TFT_DARKGREY);

    if (gps.location.isValid()) {
        img.setTextColor(TFT_GREEN, TFT_BLACK);
        img.drawString("STATUS: FIX OK", 10, 50, 2);
        
        img.setTextColor(TFT_WHITE, TFT_BLACK);
        char buf[32];
        sprintf(buf, "LAT: %.6f", gps.location.lat());
        img.drawString(buf, 10, 80, 2);
        
        sprintf(buf, "LNG: %.6f", gps.location.lng());
        img.drawString(buf, 10, 105, 2);
        
        sprintf(buf, "SATS: %d", gps.satellites.value());
        img.drawString(buf, 10, 135, 2);
    } else {
        img.setTextColor(TFT_RED, TFT_BLACK);
        img.drawString("STATUS: NO FIX", 10, 50, 2);
        img.setTextColor(TFT_WHITE, TFT_BLACK);
        
        char satBuf[32];
        sprintf(satBuf, "Sats visiveis: %d", gps.satellites.value());
        img.drawString(satBuf, 10, 80, 2);
        img.drawString("Aguardando sinal...", 10, 110, 2);
    }

    // 3. Empurra o buffer pronto para a tela física (Posição 0,0)
    // Isso é instantâneo e elimina o piscar completamente!
    img.pushSprite(0, 0); 
}


void loop() {
    // 1. PROCESSAMENTO EM TEMPO REAL (NUNCA coloque delay aqui)
    while (GPSSerial.available() > 0) {
        gps.encode(GPSSerial.read());   
    }

    static uint32_t lastDisp = 0;
    if (millis() - lastDisp > 200) {
        updateDisplay();
        lastDisp = millis();
    }

    if (safetyTimer.isReady()) {
    Serial.print(F("Transmitindo pacote de seguranca... "));

    if (gps.location.isValid()) {
        device.setLatitude(gps.location.lat());
        device.setLongitude(gps.location.lng());

      if (!device.isChannelBusy(SAFETY_CHANNEL)) {
        device.sendSafety();
      } else {
        Serial.println(F("Canal ocupado, aguardando..."));
      }
    } else {
        Serial.println(F("Aguardando sinal GPS..."));
    }
    
    safetyTimer.reset();
  }
  displayInfo();
}


void displayInfo()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}