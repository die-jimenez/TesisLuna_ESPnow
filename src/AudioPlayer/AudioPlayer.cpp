#include "AudioPlayer.h"

//Comunicación serial
#include "HardwareSerial.h"

//Modulo Mp3
#include "DFRobotDFPlayerMini.h"

HardwareSerial dfSD(1);  // Use UART channel 1
DFRobotDFPlayerMini player;
bool playerReady = false;


void Mp3ModuleInit(int RX, int TX) {
  dfSD.begin(9600, SERIAL_8N1, RX, TX);
  Serial.println("");
  delay(5000);

  if (player.begin(dfSD)) {
    playerReady = true;
    player.volume(20); //volume -> (0 to 30)
    Serial.println("MP3-TP-16P is connected");
  } else {
    Serial.println("Connecting to DFPlayer Mini failed!");
  }
}

void PlaySound(int audioIndex) {
  if (!playerReady) return;
  if (!AudioFileExists) return;

  player.play(audioIndex);
  Serial.print("Audio played: ");
  Serial.println(audioIndex);
}

//PlaySoound con un volumen especifico
void PlaySound(int audioIndex, float volume) {
  player.volume(volume);
  PlaySound(audioIndex);
}

void StopSound() {
  if(!playerReady) return;
  player.stop();
  delay(1000);
}

bool AudioFileExists(int audioIndex) {
  // Pedimos el conteo total de archivos en la SD
  int totalAudios = player.readFileCounts();
  if (totalAudios == -1 || totalAudios == 0) {
    Serial.println("Error: No se pudo leer la SD o está vacía.");
    return false;
  }
  if (audioIndex > totalAudios) {
    Serial.printf("¡Track %d no encontrado! Total disponible: %d\n", audioIndex, totalAudios);
    PlaySound(1);
    return false;
  }
  return true;
}
