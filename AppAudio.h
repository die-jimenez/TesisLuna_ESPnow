#include "Arduino.h"
#include "src/DeltaTime/DeltaTime.h"
#include "src/AudioPlayer/AudioPlayer.h"
#include "src/CustomEspNow/CustomEspNow.h"
#include "src/StatueSetting/StatueSetting.h"

#define RXD2 16  // 20
#define TXD2 17  // 21
#define BUSY 4   //10

#ifdef STATUE_HAPPY
StatueSetting statueSetting(StatueSetting::Name::AUDIO_HAPPY);
#endif
#ifdef STATUE_SAD
StatueSetting statueSetting(StatueSetting::Name::AUDIO_SAD);
#endif

bool wasPlaying = false;
void OnReceiveData(const EspNowMessage& data);



void Audio_Setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.print("Iniciando ESP-Audio: ");
  Serial.print(statueSetting.name);
  delay(12000);

  EspNowInit(statueSetting.name);
  EspNowRegisterOnReceive(OnReceiveData);

  Mp3ModuleInit(RXD2, TXD2, BUSY);
  delay(2000);

  PlaySound(1);

  Serial.println("Setup completo");
  Serial.println("--------------------------------------------");
}

bool isCurrentlyPlaying() {
  // Invertimos aquí para que la función devuelva TRUE cuando REALMENTE suena
  // Antes se hacia de otra fomra y esto es un maquillaje
  return IsPlayingAudio() == 0;
}

void Audio_Loop() {
  bool nowPlaying = isCurrentlyPlaying();

  if (wasPlaying && !nowPlaying) {

    //El pin busy del df-player no es consistente, asi que se sumula cuando termino el audio
    // Serial.println("Audio terminado, avisando al ESP-Sensores");
    // EspNowSetAndSendMessage(
    //   (int)statueSetting.name,  // AUDIO_HAPPY o AUDIO_SAD
    //   0,
    //   3,  //NONE -> esta en el GlobalStateMachine
    //   false,
    //   false  // toAudio = false, va al ESP-Sensores
    // );
    
    wasPlaying = false;
  }
  DelayForBusyUpdate();
}

void OnReceiveData(const EspNowMessage& data) {

  bool isForMe =
    (statueSetting.name == (int)StatueSetting::Name::AUDIO_HAPPY && data.name == (int)StatueSetting::Name::SENSORS_HAPPY)
    || (statueSetting.name == (int)StatueSetting::Name::AUDIO_SAD && data.name == (int)StatueSetting::Name::SENSORS_SAD);

  if (!isForMe || !data.toAudio) {
    Serial.println("LLego un mensaje que no era para mi");
    return;
  }

  Serial.print("Reproduciendo track para stage: ");
  Serial.println(data.stage);

  switch (data.stage) {
    case 0:
      PlaySound(StatueSetting::AudiosTrack::TRACK_PURR_COMPLAIN);
      break;
    case 1:
      PlaySound(StatueSetting::AudiosTrack::TRACK_SONG_1);
      break;
    case 2:
      PlaySound(StatueSetting::AudiosTrack::TRACK_SONG_2);
      break;
    case 3:
      if (data.isReadyToHappyEnding) {
        PlaySound(StatueSetting::AudiosTrack::TRACK_GOOD_ENDING);
      } else PlaySound(StatueSetting::AudiosTrack::TRACK_BAD_ENDING);
      break;
  }
  wasPlaying = true;
  DelayForBusyUpdate();
}
