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
  int statueName = 100;

  enum StatuesEnabled {
    HAPPY_ENABLED,
    SAD_ENABLED,
    BOTH_ENABLED
  };

  //Phases
  //===================================================
  void PrimerosMimitos();
  void MimitosDesarrollo();
  void MimitosFinal();
  void PrepararFinal();
  void EjecutarFinal();


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
  void Init(int _statueName, StatueStateMachine* _statueStateMachine, DeltaTime* _deltaTime);

  //Events on CustomEspNow
  //===================================================
  void OnReciveMessage(const EspNowMessage& data);
  void OnSendMessage(const EspNowMessage& otherData);

  //Events on StatueStateMachine
  //===================================================
  void OnPettingStarted();
  void OnAudioFinished();
};

#endif