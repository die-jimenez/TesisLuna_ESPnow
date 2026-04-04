#define STATUE_SAD // <--- único lugar donde cambiás
#define ROL_SENSORS   // <--- ROL_AUDIO o ROL_SENSORS

#ifdef ROL_SENSORS
#include "AppSensores.h"
#endif
#ifdef ROL_AUDIO
#include "AppAudio.h"
#endif

void setup() {
#ifdef ROL_SENSORS
  Sensores_Setup();
#endif
#ifdef ROL_AUDIO
  Audio_Setup();
#endif
}

void loop() {
#ifdef ROL_SENSORS
  Sensores_Loop();
#endif
#ifdef ROL_AUDIO
  Audio_Loop();
#endif
}