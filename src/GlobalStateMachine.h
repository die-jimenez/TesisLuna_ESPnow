#ifndef __GLOBALSTATEMACHINE_H__
#define __GLOBALSTATEMACHINE_H__

#include <Arduino.h>
#include "StatueStateMachine.h"
#include "CustomEspNow/CustomEspNow.h"
#include "StatueSetting/StatueSetting.h"
#include "DeltaTime/DeltaTime.h"

class GlobalStateMachine {

public:
  enum Stages {
    STANDBY,
    INTRO,
    DESARROLLO,
    FINAL
  };
  Stages stage = STANDBY;
  Stages lastStage = STANDBY;

private:
  DeltaTime* deltaTime;
  StatueStateMachine* statueStateMachine;

  enum StatuesEnabled {
    HAPPY_ENABLED,
    SAD_ENABLED,
    BOTH_ENABLED,
    NONE
  };
  StatuesEnabled statueEnabled = BOTH_ENABLED;
  StatueSetting* statueSetting;

  //timer to reset
  float resetTimer = 0;
  float finalEndingTimer = 0;
  float randomSoundTimer = 0;
  bool badEndingTimerActive = false;

  //Audio finished simulated
  float timePlayingAudio;
  float audioDuration;

  //ending
  bool happyOnGoodEnding = false;
  bool sadOnGoodEnding = false;

  bool CheckPublicPassword(int receivedPassword);
  void UpdateStage(int newStage);
  void UpdateStatueEnabled(int enabledMode);
  void SyncFinalOnRecieve(const EspNowMessage& otherData);

  void NextStageOrPassTurn(GlobalStateMachine::Stages nextStage);
  void PrintEndingInfo();

  void MessageToAudio(int _stage, bool _canPlayGoodEnding);



public:
  //Init
  //===================================================
  GlobalStateMachine();
  void Init(StatueSetting* _statueSetting, StatueStateMachine* _statueStateMachine, DeltaTime* _deltaTime);

  //Events on CustomEspNow
  //===================================================
  void OnReciveMessage(const EspNowMessage& data);
  void OnSendMessage(const EspNowMessage& otherData);

  //Events on StatueStateMachine
  //===================================================
  void OnPettingStarted();
  void OnAudioFinished();

  //Reset event
  //===================================================
  void UpdateResetTimer(const float* _INACTIVITY_TIMEOUT, const float* _BAD_ENDING_RESET_TIMEOUT);
  void FullReset();
  void SendMessageToReset();

  void PlayFinal(bool goodEnding);
  void PrintInfo();
  void AudioFinishedSimulated();

  void UpdateTimerToPlayRandomSound(float interlapse);
};

#endif