#ifndef __GLOBALSTATEMACHINE_H__
#define __GLOBALSTATEMACHINE_H__

#include <Arduino.h>
#include "StatueStateMachine.h"
#include "src/CustomEspNow/CustomEspNow.h"
#include "src/StatueSetting/StatueSetting.h"
#include "src/DeltaTime/DeltaTime.h"

class GlobalStateMachine {

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

  //ending
  bool happyOnGoodEnding = false;
  bool sadOnGoodEnding = false;

  bool CheckPublicPassword(int receivedPassword);
  void UpdateStage(int newStage);
  void UpdateStatueEnabled(int enabledMode);
  void SyncFinalOnRecieve(const EspNowMessage& otherData);


public:
  enum Stages {
    STANDBY,
    INTRO,
    DESARROLLO,
    FINAL
  };
  Stages stage = STANDBY;

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
  void UpdateResetTimer(const float* _INACTIVITY_TIMEOUT);
  void FullReset();
  void SendMessageToReset();

  void PlayFinal(bool goodEnding);
  void PrintInfo();
};

#endif