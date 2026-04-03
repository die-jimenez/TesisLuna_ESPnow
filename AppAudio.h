#include "Arduino.h"
#include "src/DeltaTime/DeltaTime.h"
#include "src/AudioPlayer/AudioPlayer.h"
#include "src/CustomEspNow/CustomEspNow.h"
#include "src/StatueSetting/StatueSetting.h"

#define RXD2 16   // 20
#define TXD2 17   // 21
#define BUSY 4  //10

#ifdef STATUE_HAPPY
StatueSetting statueSetting(StatueSetting::Name::AUDIO_HAPPY);
#endif
#ifdef STATUE_SAD
StatueSetting statueSetting(StatueSetting::Name::AUDIO_SAD);
#endif

bool wasPlaying = true;
void OnReceiveData(const EspNowMessage& data);



void Audio_Setup() {
  Serial.begin(115200);

  Serial.println("Iniciando ESP-Audio");
  delay(12000);

  EspNowInit();
  EspNowRegisterOnReceive(OnReceiveData);

  Mp3ModuleInit(RXD2, TXD2, BUSY);
  delay(2000);

  PlaySound(1);

  Serial.println("Setup completo");
  Serial.println("--------------------------------------------");
}

void Audio_Loop() {
  bool playing = IsPlayingAudio();

  if (wasPlaying && !playing) {
    Serial.println("Audio terminado, avisando al ESP-Sensores");
    EspNowSetAndSendMessage(
      (int)statueSetting.name,  // AUDIO_HAPPY o AUDIO_SAD
      0,
      3,  //NONE -> esta en el GlobalStateMachine
      false,
      false  // toAudio = false, va al ESP-Sensores
    );
  }
  wasPlaying = playing;
  delay(5);
}

void OnReceiveData(const EspNowMessage& data) {
  Serial.println("LLego un mensaje");
  if (!data.toAudio) return;
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
  DelayForBusyUpdate();
}
