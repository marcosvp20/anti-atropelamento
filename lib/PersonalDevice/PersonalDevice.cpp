#include "PersonalDevice.h"
PersonalDevice::PersonalDevice()
    : deviceID(0), deviceLatitude(0.0), deviceLongitude(0.0), batteryLevel(100), status(0) {
    for (int i = 0; i < 5; i++) {
        last5positions[i][0] = 0.0;
        last5positions[i][1] = 0.0;
        last5events[i] = 0;
    }

    
}
void PersonalDevice::setup() {
    lora.begin();
    lora.SpreadingFactor(7);
}
uint8_t PersonalDevice::getID() const {
    return deviceID;
}
void PersonalDevice::setID(uint8_t id) {
    deviceID = id;
}
int32_t PersonalDevice::getLatitude() const {
    return deviceLatitude;
}
void PersonalDevice::setLatitude(int32_t latitude) {
    deviceLatitude = latitude;
}
int32_t PersonalDevice::getLongitude() const {
    return deviceLongitude;
}
void PersonalDevice::setLongitude(int32_t longitude) {
    deviceLongitude = longitude;
}
unsigned long PersonalDevice::getSpeed() const {
    return speed;
}
void PersonalDevice::setSpeed(unsigned long speedValue) {
    speed = speedValue;
}
float PersonalDevice::getAccelerationX() const {
    return accelerationX;
}
void PersonalDevice::setAccelerationX(float ax) {
    accelerationX = ax;
}
float PersonalDevice::getAccelerationY() const {
    return accelerationY;
}
void PersonalDevice::setAccelerationY(float ay) {
    accelerationY = ay;
}
void PersonalDevice::sendSafety() {
    // pckt.safetyPacket(deviceID, deviceType, deviceLatitude, deviceLongitude, safetyPacket);
    lora.sendData(safetyPacket, SAFETY_PACKET_SIZE);
}
void PersonalDevice::sendMonitoring() {
    // pckt.monitoringPacket(deviceID, deviceType, deviceLatitude, deviceLongitude, batteryLevel, last5positions, last5events, status, monitoringPacket);
    lora.sendData(monitoringPacket, MONITORING_PACKET_SIZE);
}
bool PersonalDevice::receive() {
    lora.SpreadingFactor(7);
    if(lora.receiveData(receivedPacket, MONITORING_PACKET_SIZE, 1000)) {
    pckt.decodePacket(receivedPacket);
    return true;
    }
    return false;

}
// Verifica se o canal está ocupado e constroi os pacotes, caso o pacote fosse construido na função que envia os dados, correria o risco do o canal estar ocupado e a verificação retornaria um falso positivo
bool PersonalDevice::isChannelBusy(int channel) {
    if(channel == SAFETY_CHANNEL) {
        // talvez seja mais apropriado fazer o pacote aqui para que o envio seja logo após a verificação do canal
        lora.SpreadingFactor(7);
        pckt.safetyPacket(deviceID, deviceType, deviceLatitude, deviceLongitude, safetyPacket);
    } else if(channel == MONITORING_CHANNEL) {
        lora.SpreadingFactor(9);
        pckt.monitoringPacket(deviceID, deviceType, deviceLatitude, deviceLongitude, batteryLevel, last5positions, last5events, status, monitoringPacket);
    }
    return lora.isChannelBusy();
}
void PersonalDevice::updateFromBluetooth(String rawData) {
    int firstSemi = rawData.indexOf(';');
    int secondSemi = rawData.indexOf(';', firstSemi + 1);

    if (firstSemi != -1 && secondSemi != -1) {
        // Extrai os valores usando as funções existentes
        uint8_t novoID = (uint8_t)rawData.substring(0, firstSemi).toInt();
        float novaLat = rawData.substring(firstSemi + 1, secondSemi).toFloat();
        float novaLng = rawData.substring(secondSemi + 1).toFloat();

        setID(novoID);
        setLatitude(novaLat);
        setLongitude(novaLng);
        
        Serial.println("\n>>> Dados atualizados via BLE.");
    }
}
void PersonalDevice::sendAlert(uint8_t alertType, uint8_t targetID) {
    // Construir o pacote de alerta
    uint8_t alertPacket[ADVERTISE_PACKET_SIZE]; // Tamanho arbitrário, ajuste conforme necessário
    pckt.advertisePacket(alertType, targetID, alertPacket);    // Enviar o pacote de alerta
    lora.sendData(alertPacket, sizeof(alertPacket));
}
// Função auxiliar continua necessária
float PersonalDevice::toRadians(float degree) {
    return degree * (PI / 180.0);
}

// Implementação Otimizada (Aproximação Equirretangular)
float PersonalDevice::calculateDistance(float targetLat, float targetLng) {
    // não faz a corversão de minutos e segundo para graus, usar a biblioteca TinyGPS++ para isso antes de chamar essa função
    const float R = 6371000.0; // Raio da Terra em metros

    // Converter para radianos (necessário para o cálculo correto)
    float lat1 = toRadians(-19.96792253900216);
    float lon1 = toRadians(-43.955467856491474);
    float lat2 = toRadians(targetLat);
    float lon2 = toRadians(targetLng);

    // Ajuste da longitude baseada na latitude média (o "achatamento" do mapa)
    float x = (lon2 - lon1) * cos((lat1 + lat2) / 2.0);
    float y = lat2 - lat1;

    // Pitágoras simples: Raiz(x² + y²) * Raio
    float distance = sqrt(x * x + y * y) * R;

    return distance;
}