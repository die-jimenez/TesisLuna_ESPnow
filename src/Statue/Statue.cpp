#include "Statue.h"

Statue::Statue(StatueName _name, int ledsPin) {
  name = _name;
  LEDs = ledsPin;
}

Statue::~Statue() {
}

void Statue::InitLeds() {
  pinMode(LEDs, OUTPUT);
}

void Statue::TurnOnLEDs(bool val) {
  digitalWrite(LEDs, val);
}


void Statue::Enable() {
  interactionEnabled = true;
}
void Statue::Disable() {
  interactionEnabled = false;
}
bool Statue::IsEnabled() {
  return interactionEnabled;
}
