#include "Lights.h"

Lights::Lights(int _mouthLightPin, int _enviromentLightPin) {
  mouthLightPin = _mouthLightPin;
  enviromentLightPin = _enviromentLightPin;
}

void Lights::Init() {
  pinMode(mouthLightPin, OUTPUT);
  pinMode(enviromentLightPin, OUTPUT);

}

void Lights::TurnOn(bool val) {
  digitalWrite(mouthLightPin, val);
  digitalWrite(enviromentLightPin, val);

}
