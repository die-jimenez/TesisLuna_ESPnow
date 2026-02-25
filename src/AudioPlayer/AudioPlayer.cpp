#include "esp32-hal-gpio.h"
#include "AudioPlayer.h"

HardwareSerial dfSD(1);  // Use UART channel 1
bool playerReady = false;
int busyPin;

#ifdef AUDIO_MODULE_DFP
DFRobotDFPlayerMini player;
#endif
#ifdef AUDIO_MODULE_HW247
DFPlayer player;
#endif
#ifdef AUDIO_MODULE_TF
DFRobotDFPlayerMini player;
#endif


void Mp3ModuleInit(int RX, int TX, int BUSY) {
  dfSD.begin(9600, SERIAL_8N1, RX, TX);
  Serial.println("Trying to start comunication with mp3");
  pinMode(BUSY, INPUT);
  busyPin = BUSY;
  delay(3000);

#ifdef AUDIO_MODULE_DFP
  delay(2000);
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
  int MP3_SERIAL_TIMEOUT = 400;
  player.begin(dfSD, MP3_SERIAL_TIMEOUT, DFPLAYER_HW_247A, true);
  Serial.println("HW247A is connected");
  player.setSource(2);
  playerReady = true;
  delay(500);
  player.setVolume(20);
#endif

#ifdef AUDIO_MODULE_TF
  delay(2000);
  if (player.begin(dfSD, false, true)) {
    playerReady = true;
    player.volume(20);
    Serial.println("TF-16P is connected");
  } else {
    Serial.println("ERROR: No se pudo conectar al TF-16P");
  }
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

#ifdef AUDIO_MODULE_TF
  player.play(audioIndex);
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
#ifdef AUDIO_MODULE_TF
  player.volume(volume);
#endif

  delay(500);
  PlaySound(audioIndex);
}

void StopSound() {
  if (!playerReady) return;
  player.stop();
}

bool IsPlayingAudio() {
  return digitalRead(busyPin);
}
