// SensorsManager.cpp
#include "SensorsManager.h"

SensorsManager::SensorsManager(TouchSensor _sensors[], int _sensorsCount) {
  sensors = _sensors;
  sensorsCount = _sensorsCount;
}


// ==================== SENSORES
void SensorsManager::InitSensors() {
  for (int i = 0; i < sensorsCount; i++) {
    sensors[i].Init();
    sensors[i].SetDebugMode(showDebug);
    delay(10);
  }
}

void SensorsManager::UpdateSensors(float deltaTime) {
  for (int i = 0; i < sensorsCount; i++) {
    sensors[i].Update(deltaTime);
  }
}

void SensorsManager::SetShowDebug(bool _showDebug) {
  showDebug = _showDebug;
}

bool SensorsManager::areAllSensorsOff() {
  for (int i = 0; i < sensorsCount; i++) {
    if (sensors[i].isTouchStable()) return false;
  }
  return true;
}

bool SensorsManager::areMultipleSensorsOn(uint8_t _cant) {
  uint8_t sensoresOn = 0;
  for (int i = 0; i < sensorsCount; i++) {
    if (sensors[i].GetActiveTime() > sensors[i].GetMinTouchDuration() * 0.5)
      sensoresOn++;
  }
  return sensoresOn >= _cant;
}

void SensorsManager::ResetAllSensors() {
  for (int i = 0; i < sensorsCount; i++) {
    sensors[i].ResetTime();
  }
}

// ==================== TIMER
void SensorsManager::RunTimeToPet(float deltaTime) {
  timeToPet += deltaTime;
}

float SensorsManager::GetTimeToPet() {
  return timeToPet;
}

void SensorsManager::ResetTimeToPet() {
  timeToPet = 0;
}

// ==================== DEBUG
void SensorsManager::DebugInteraction() {
  if (!showDebug) return;
  contadorInteraccion++;
  Serial.print("Contador total de interacciones: ");
  Serial.println(contadorInteraccion);
}

void SensorsManager::DebugPetting() {
  if (!showDebug) return;
  contadorMimitos++;
  Serial.print("Contador total de MIMITOS: ");
  Serial.println(contadorMimitos);
}

void SensorsManager::GetAllError() {
  if (!showDebug) return;
  for (int i = 0; i < sensorsCount; i++) {
    Serial.print(" ---> SENSOR ");
    Serial.print(sensors[i].GetID());
    Serial.print(": contador de falsos positivos = ");
    Serial.println(sensors[i].contadorFalsosPositivo);
  }
  Serial.print("Contador total de interacciones: ");
  Serial.println(contadorInteraccion);
  Serial.print("Contador total de MIMITOS: ");
  Serial.println(contadorMimitos);
}