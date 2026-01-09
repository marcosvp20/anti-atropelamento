#include "VehicleDevice.h"
VehicleDevice::VehicleDevice()
    : deviceID(0), deviceLatitude(0.0), deviceLongitude(0.0), speed(0.0), 
      course(0.0), currentRiskMask(0), riskRadius(10), // Inicialização das novas vars
      batteryLevel(100), status(0) {
    
    for (int i = 0; i < 5; i++) {
        last5positions[i][0] = 0.0;
        last5positions[i][1] = 0.0;
        last5events[i] = 0;
    }
}

void VehicleDevice::setup() {
    lora.begin();
    lora.SpreadingFactor(7);
}
uint8_t VehicleDevice::getID() const {
    return deviceID;
}
void VehicleDevice::setID(uint8_t id) {
    deviceID = id;
}
float VehicleDevice::getLatitude() const {
    return deviceLatitude;
}
void VehicleDevice::setLatitude(float latitude) {
    deviceLatitude = latitude;
}
float VehicleDevice::getLongitude() const {
    return deviceLongitude;
}
void VehicleDevice::setLongitude(float longitude) {
    deviceLongitude = longitude;
}
float VehicleDevice::getSpeed() const {
    return speed;
}
void VehicleDevice::setSpeed(float speedValue) {
    speed = speedValue;
}
#include "VehicleDevice.h"

uint8_t VehicleDevice::calculateRiskMask(float steeringAngle) {
    uint8_t mask = 0;

    // Se estiver quase parado, ativa proteção 360° (Bolha total)
    if (abs(speed) < 1.0) return 0x0F; 

    if (speed > 1.0) {
        mask |= 0x01; // Liga Bit 0: Frente (Sempre ativo se movendo pra frente)

        // Se o volante/direção indicar curva significativa
        if (steeringAngle > 10.0) mask |= 0x02; // Liga Bit 1: Lateral Direita
        if (steeringAngle < -10.0) mask |= 0x04; // Liga Bit 2: Lateral Esquerda
    } 
    else if (speed < -1.0) {
        mask |= 0x08; // Liga Bit 3: Traseira (Marcha à ré)
    }

    return mask;
}
void VehicleDevice::updateSteeringFromCourse() {
    // 1. Calcula a diferença bruta
    float deltaCourse = course - lastCourse;

    // 2. Normaliza a diferença para o intervalo [-180, 180]
    // Isso resolve o problema da passagem pelos 360°/0°
    if (deltaCourse > 180) deltaCourse -= 360;
    if (deltaCourse < -180) deltaCourse += 360;

    // 3. Filtro de ruído: Se a variação for mínima (ex: < 1 grau), ignoramos
    // Se a variação for rápida, estimamos o steeringAngle
    // Nota: O valor 5.0 é um limiar de sensibilidade que você pode ajustar
    if (abs(deltaCourse) > 1.0) {
        this->steeringAngle = deltaCourse * 5.0; // Ganho para simular ângulo do volante
    } else {
        this->steeringAngle = 0;
    }

    // 4. Atualiza o último curso para a próxima leitura
    lastCourse = course;
}

void VehicleDevice::sendSafety() {

    updateSteeringFromCourse();
    // Definimos um raio de risco dinâmico baseado na velocidade (ex: 10m base + acréscimo)
    riskRadius = (uint8_t)(10 + (abs(speed) * 1.5)); 
    
    // Para este exemplo, assumimos que o steeringAngle vem de algum sensor 
    // ou da variação de curso. Se não tiver, passe 0.
    currentRiskMask = calculateRiskMask(0); 

    // Montando o pacote reduzido para LoRa (Exemplo de preenchimento do buffer)
    // Aqui você usaria o seu objeto 'pckt' para formatar esses novos campos
    // pckt.safetyPacket(deviceID, deviceLatitude, deviceLongitude, course, riskRadius, currentRiskMask, safetyPacket);

    lora.SpreadingFactor(7);
    lora.sendData(safetyPacket, SAFETY_PACKET_SIZE);
    
    Serial.print("Broadcast Risco - Mask: 0x");
    Serial.print(currentRiskMask, HEX);
    Serial.print(" Raio: "); Serial.println(riskRadius);
}

void VehicleDevice::sendMonitoring() {
    // lora.SpreadingFactor(9);
    // pckt.monitoringPacket(deviceID, deviceType, deviceLatitude, deviceLongitude, batteryLevel, last5positions, last5events, status, monitoringPacket);
    lora.sendData(monitoringPacket, MONITORING_PACKET_SIZE);
}
bool VehicleDevice::receive() {

    lora.SpreadingFactor(7);
    if(lora.receiveData(receivedPacket, MONITORING_PACKET_SIZE, 1000)) {
    pckt.decodePacket(receivedPacket);
    Serial.println("Received Packet:");
    Serial.print("ID: ");
    Serial.println(pckt.safetyPacketData.ID);
    Serial.print("Latitude: ");
    Serial.println(pckt.safetyPacketData.latitude, 6);
    Serial.print("Longitude: ");
    Serial.println(pckt.safetyPacketData.longitude, 6);
    return true;
    }
    return false;
}

bool VehicleDevice::isChannelBusy(int channel) {
    if(channel == SAFETY_CHANNEL) {
        lora.SpreadingFactor(7);
        pckt.safetyPacket(deviceID, deviceType, deviceLatitude, deviceLongitude, course, riskRadius, currentRiskMask, safetyPacket);
    } else if(channel == MONITORING_CHANNEL) {
        lora.SpreadingFactor(9);
        pckt.monitoringPacket(deviceID, deviceType, deviceLatitude, deviceLongitude, batteryLevel, last5positions, last5events, status, monitoringPacket);
    }
    return lora.isChannelBusy();
}

