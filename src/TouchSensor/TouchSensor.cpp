#include <stdint.h>
#include "TouchSensor.h"

TouchSensor::TouchSensor(uint8_t _pin, uint8_t _id) {
  pin = _pin;
  id = _id;
}

TouchSensor::~TouchSensor() {
}

void TouchSensor::Init() {
  pinMode(pin, INPUT);
}

void TouchSensor::Update(float _deltaTime) {
  if (isTouch()) {
    RunTouchTime(_deltaTime);
    timeToReset = 0;
  } else {
    timeToReset = timeToReset <= resetTriggerTime ? timeToReset + _deltaTime : timeToReset;
    if (timeToReset > resetTriggerTime) ResetTime();
  }
}



bool TouchSensor::isTouch() {
  if (digitalRead(pin) == HIGH) return true;
  else return false;
}

bool TouchSensor::isTouchStable() {
  if (activeTime > minTouchDuration)  //
  {
    DebugEvent();
    return true;
  }
  ResetDebugEvent();
  return false;
}

void TouchSensor::RunTouchTime(float _deltaTime) {
  activeTime = activeTime + _deltaTime;
}

float TouchSensor::GetActiveTime() {
  return activeTime;
}

float TouchSensor::GetMinTouchDuration() {
  return minTouchDuration;
}

void TouchSensor::ResetTime() {
  activeTime = 0;
}

uint8_t TouchSensor::GetID() {
  return id;
}

void TouchSensor::DebugEvent() {
  if (!debug) return;
  if (debugEventExecuted) return;
  debugEventExecuted = true;
  contadorFalsosPositivo++;
  Serial.print(millis() / 1000.0);
  Serial.print(" ---> SENSOR ");
  Serial.print(id);
  Serial.print(": contador de falsos positivos = ");
  Serial.println(contadorFalsosPositivo);
}

void TouchSensor::ResetDebugEvent() {
  debugEventExecuted = false;
}

void TouchSensor::SetDebugMode(bool _state) {
  debug = _state;
}