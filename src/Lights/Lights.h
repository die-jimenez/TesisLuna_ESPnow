#ifndef __LIGHTS_H__
#define __LIGHTS_H__
#include <Arduino.h>
#include "../DeltaTime/DeltaTime.h"

class Lights {

private:
  int mouthPin;
  int enviromentPin;
  float flickerTimer = 0;

public:
  enum LightState {
    ON,
    OFF,
    FLASHING
  };

  Lights(int _mouthPin, int _enviromentPin);
  void Init();
  void Update(LightState state, DeltaTime* deltaTime);
};

#endif
