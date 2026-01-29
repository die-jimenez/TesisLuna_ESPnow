#include "AudioPlayer.h"

//Memoria SD
#include <SPI.h>
#include <SD.h>

//Audio
#include "AudioFileSourceSD.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"

static AudioGeneratorMP3 *mp3 = nullptr;
static AudioFileSourceSD *file = nullptr;
static AudioOutputI2S *out = nullptr;

void SDcardInit(int SCK, int MISO, int MOSI, int SS) {
  SPI.begin(SCK, MISO, MOSI, SS);

  if (!SD.begin(SS, SPI)) {
    Serial.println("¡Error al inicializar tarjeta SD!");
    while (1)
      ;  // Detener ejecución si falla
  }
}

void AudioInit(int BLCK, int LRC, int DOUT) {
  //Iniciarlizar DAC
  out = new AudioOutputI2S();
  out->SetPinout(BLCK, LRC, DOUT);
  out->SetOutputModeMono(true);
  out->SetGain(1);  //0.0 – 1.0

  mp3 = new AudioGeneratorMP3();
  file = new AudioFileSourceSD("/1-L1.mp3");
  mp3->begin(file, out);
}

void AudioUpdate() {
  if (mp3 && mp3->isRunning()) {
    mp3->loop();
  } else if (mp3) {
    mp3->stop();
    CleanMemory();
  }
}

void PlaySound(const char *filePath) {
  if (!AudioFileExists) return;
  CleanMemory();

  // Cargar nuevo archivo
  file = new AudioFileSourceSD(filePath);
  mp3 = new AudioGeneratorMP3();

  // Iniciar reproducción
  if (!mp3->begin(file, out)) {
    Serial.println("Error al iniciar MP3");
    CleanMemory();
    return;
  }
  Serial.printf("Reproduciendo: %s\n", filePath);
}

//PlaySoound con un volumen especifico
void PlaySound(const char *filePath, float volume) {
  if (out) out->SetGain(volume);
  PlaySound(filePath);
}

void StopSound() {
  if (mp3 && mp3->isRunning()) {
    mp3->stop();
    CleanMemory();
  }
}

bool AudioFileExists(const char *filePath) {
  if (!SD.exists(filePath)) {
    Serial.println();
    Serial.printf("¡Archivo %s no encontrado!\n", filePath);
    return false;
  }
  return true;
}

void CleanMemory() {
  // Limpiar generador MP3
  if (mp3) {
    if (mp3->isRunning()) {
      mp3->stop();
    }
    delete mp3;
    mp3 = nullptr;
  }

  // Limpiar fuente de archivo
  if (file) {
    delete file;
    file = nullptr;
  }
}