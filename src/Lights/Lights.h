#ifndef __LIGHTS_H__
#define __LIGHTS_H__
#include <Arduino.h>

class Lights {

private:
  int mouthLightPin;
  int enviromentLightPin;
  float timeLightFlickering;

public:
  Lights(int _mouthLightPin, int _enviromentLightPin);

  void Init();
  void TurnOn(bool val);
};

#endif
