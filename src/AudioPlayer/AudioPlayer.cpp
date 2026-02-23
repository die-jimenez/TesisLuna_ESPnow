#include "AudioPlayer.h"

HardwareSerial dfSD(1);  // Use UART channel 1
bool playerReady = false;

#ifdef AUDIO_MODULE_DFP
DFRobotDFPlayerMini player;
#endif
#ifdef AUDIO_MODULE_HW247
DFPlayer player;
#endif


void Mp3ModuleInit(int RX, int TX) {
  dfSD.begin(9600, SERIAL_8N1, RX, TX);
  Serial.println("Trying to start comunication with mp3");
  delay(5000);

#ifdef AUDIO_MODULE_DFP
  int intentos = 0;
  const int MAX_INTENTOS = 5;
  while (!playerReady && intentos < MAX_INTENTOS) {
    if (player.begin(dfSD)) {
      playerReady = true;
      player.volume(20);
      Serial.println("DFP is connected");
    } else {
      intentos++;
      Serial.print("Intento fallido: ");
      Serial.println(intentos);
      delay(2000);
    }
  }
  if (!playerReady) Serial.println("ERROR: No se pudo conectar al DFPlayer");
#endif

#ifdef AUDIO_MODULE_HW247
  int intentos = 0;
  const int MAX_INTENTOS = 5;
  while (!playerReady && intentos < MAX_INTENTOS) {
    player.begin(dfSD, DFPLAYER_HW_247A);  //No devuelve bool
    player.setVolume(20);

    if (player.getStatus() != 0) {
      playerReady = true;
      Serial.println("HW247A is connected");
    } else {
      intentos++;
      Serial.print("Intento fallido: ");
      Serial.println(intentos);
      delay(2000);
    }
  }
  if (!playerReady) Serial.println("ERROR: No se pudo conectar al DFPlayer");
#endif
}


void PlaySound(int audioIndex) {
  if (!playerReady) return;

#ifdef AUDIO_MODULE_DFP
  player.play(audioIndex);
#endif
#ifdef AUDIO_MODULE_HW247
  player.playTrack(audioIndex);
#endif

  Serial.print("Audio played: ");
  Serial.println(audioIndex);
}


void PlaySound(int audioIndex, float volume) {
#ifdef AUDIO_MODULE_DFP
  player.volume(volume);
#endif
#ifdef AUDIO_MODULE_HW247
  player.setVolume(volume);
#endif

  PlaySound(audioIndex);
}

void StopSound() {
  if (!playerReady) return;
  player.stop();
}

bool IsPlayingAudio(int busyPin) {
  return digitalRead(busyPin) == LOW;
}
