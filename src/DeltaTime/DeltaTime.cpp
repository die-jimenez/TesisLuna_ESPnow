#include "DeltaTime.h"

DeltaTime::DeltaTime() {
}

DeltaTime::~DeltaTime() {
}

void DeltaTime::Run() {
  currentTime = millis();
  //".0" mantiene el resultado en float
  deltaTime = (currentTime - lastTime)/1000.0;
  lastTime = currentTime;
}

float DeltaTime::Get() {
  return deltaTime;
}