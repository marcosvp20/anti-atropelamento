#include "VehicleDevice.h"

TinyGPSPlus gps;
VehicleDevice::VehicleDevice()
    : deviceID(0), deviceLatitude(0.0), deviceLongitude(0.0), speed(0.0), 
      course(0.0), lastCourse(0.0), currentRiskMask(0), riskRadius(10), // Inicialização das novas vars
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
void VehicleDevice::setLatitude() {
    deviceLatitude = gps.location.lat();
}
float VehicleDevice::getLongitude() const {
    return deviceLongitude;
}
void VehicleDevice::setLongitude() {
    deviceLongitude = gps.location.lng();
}
float VehicleDevice::getSpeed() const {
    return speed;
}
void VehicleDevice::setSpeed(float speedValue) {
    speed = speedValue;
    //speed = gps.speed.kmph();
}
float VehicleDevice::getCourse() const {
    return this->course;
}
void VehicleDevice::setCourse() {
    if (gps.course.isValid()) {
        this->course = gps.course.deg();
    }
}

uint8_t VehicleDevice::calculateRiskMask(float steeringAngle) {
    uint8_t mask = 0;
    float currentSpeed = gps.speed.kmph();

    // Se estiver quase parado, proteção 360°
    if (currentSpeed < 1.0) return 0x0F; 

    // Movimento para frente
    if (currentSpeed >= 1.0) {
        mask |= 0x01; // Bit 0: Frente

        // Usa o steeringAngle calculado na função anterior
        if (steeringAngle > 10.0)      mask |= 0x02; // Bit 1: Direita
        else if (steeringAngle < -10.0) mask |= 0x04; // Bit 2: Esquerda
    } 

    this->currentRiskMask = mask; // Armazena o resultado na classe
    return mask;
}

void VehicleDevice::updateSteeringFromCourse() {
    // 1. Primeiro atualizamos o valor interno com o dado mais recente do GPS
    float currentCourse = gps.course.deg();

    // 2. Se a velocidade for muito baixa, o curso do GPS oscila (ruído)
    // Então só calculamos o steering se houver movimento real
    if (gps.speed.kmph() < 1.0) {
        this->steeringAngle = 0;
        return;
    }

    // 3. Calcula a diferença usando o 'course' (atualizado agora) e o 'lastCourse'
    float deltaCourse = currentCourse - this->lastCourse;

    // 4. Normalização (Correção do pulo 360/0)
    if (deltaCourse > 180) deltaCourse -= 360;
    if (deltaCourse < -180) deltaCourse += 360;

    // 5. Filtro de sensibilidade e atualização do steeringAngle
    if (abs(deltaCourse) > 1.0) {
        this->steeringAngle = deltaCourse * 5.0; 
    } else {
        this->steeringAngle = 0;
    }

    // 6. Sincroniza o 'course' público e prepara o 'lastCourse' para a próxima rodada
    this->course = currentCourse;
    this->lastCourse = currentCourse;
}

void VehicleDevice::sendSafety() {

    updateSteeringFromCourse();
    // Definimos um raio de risco dinâmico baseado na velocidade (ex: 10m base + acréscimo)
    riskRadius = (uint8_t)(10 + (abs(speed) * 1.5)); 
    
    // Para este exemplo, assumimos que o steeringAngle vem de algum sensor 
    // ou da variação de curso. Se não tiver, passe 0.
    currentRiskMask = calculateRiskMask(this->steeringAngle); 

    // Montando o pacote reduzido para LoRa (Exemplo de preenchimento do buffer)
    // Aqui você usaria o seu objeto 'pckt' para formatar esses novos campos
    // pckt.safetyPacket(deviceID, deviceLatitude, deviceLongitude, course, riskRadius, currentRiskMask, safetyPacket);

    lora.SpreadingFactor(7);
    lora.sendData(safetyPacket, SAFETY_PACKET_SIZE);
    
    Serial.println("Broadcast Risco - Mask: 0x");
    Serial.println(currentRiskMask, HEX);
    Serial.println(" Raio: "); 
    Serial.println(riskRadius);
    Serial.println(" Angulo: "); 
    Serial.println(steeringAngle);
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

