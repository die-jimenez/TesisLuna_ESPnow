#ifndef __STATUESTATEMACHINE_H__
#define __STATUESTATEMACHINE_H__

#include <Arduino.h>
#include "Lights/Lights.h"
#include "DeltaTime/DeltaTime.h"
#include "AudioPlayer/AudioPlayer.h"
#include "StatueSetting/StatueSetting.h"
#include "SensorsManager/SensorsManager.h"


class StatueStateMachine {
private:
  //Objects
  //===================================================
  StatueSetting* statueSetting;
  SensorsManager* sensorsManager;
  DeltaTime* deltaTime;
  Lights* lights;

  //States
  //===================================================
  bool canInteract = true;

  //Cronometros
  //===================================================
  float timeInteracting = 0;

  //Eventos
  //===================================================
  typedef void (*OnPettingStartedCallback)();
  typedef void (*OnAudioFinishedCallback)();
  OnPettingStartedCallback onPettingStartedCallback = nullptr;
  OnAudioFinishedCallback onAudioFinishedCallback = nullptr;


public:
  //Main States
  //===================================================
  enum State {
    IDLE,
    INTERACTING,
    PETTING
  };
  State state = IDLE;

  //Init
  //===================================================
  StatueStateMachine();
  void Init(StatueSetting* _statueSetting, SensorsManager* _sensorsManager, Lights* _lights, DeltaTime* _deltaTime);

  //State Update
  //===================================================
  void ChangeState(State newState);
  void UpdateIdle();
  void UpdateInteraction(float triggerToPetting, int minSensorsToPet);
  void UpdatePetting();

  //Handle Interaction State
  //===================================================
  void SetCanInteract(bool _canInteract);
  bool GetCanInteract();
  void ResetStatue();
  void ResetInteractionState();



  //Events to send message through EspNow
  //===================================================
  void RegisterOnPettingStarted(OnPettingStartedCallback fn);
  void RegisterOnAudioFinished(OnAudioFinishedCallback fn);
};

#endif