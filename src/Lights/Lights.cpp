#include "Lights.h"

Lights::Lights(int _mouthPin, int _enviromentPin) {
  mouthPin = _mouthPin;
  enviromentPin = _enviromentPin;
}

void Lights::Init() {
  pinMode(mouthPin, OUTPUT);
  pinMode(enviromentPin, OUTPUT);
}

void Lights::Update(LightState state, DeltaTime* deltaTime) {
  if (state == ON) {
    digitalWrite(mouthPin, HIGH);
    digitalWrite(enviromentPin, HIGH);
    flickerTimer = 0;
  }
  else if (state == OFF) {
    digitalWrite(mouthPin, LOW);
    digitalWrite(enviromentPin, LOW);
    flickerTimer = 0;
  }
  else if (state == FLASHING) {
    digitalWrite(mouthPin, HIGH);
    if (flickerTimer >= 0 && flickerTimer <= 0.6) {
      digitalWrite(enviromentPin, HIGH);
    }
    if (flickerTimer >= 0.6 && flickerTimer <= 1.2) {
      digitalWrite(enviromentPin, LOW);
    }
    if (flickerTimer >= 1.2) {
      flickerTimer = 0;
    }
    flickerTimer += deltaTime->Get();
  }
}
