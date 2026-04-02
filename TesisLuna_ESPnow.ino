#define STATUE_SAD    // <--- único lugar donde cambiás
#define ROL_AUDIO   // <--- ROL_AUDIO o ROL_SENSORES

#ifdef ROL_SENSORES
#include "AppSensores.h"
#endif
#ifdef ROL_AUDIO
#include "AppAudio.h"
#endif

void setup() {
#ifdef ROL_SENSORES
  Sensores_Setup();
#endif
#ifdef ROL_AUDIO
  Audio_Setup();
#endif
}

void loop() {
#ifdef ROL_SENSORES
  Sensores_Loop();
#endif
#ifdef ROL_AUDIO
  Audio_Loop();
#endif
}