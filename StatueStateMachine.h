#ifndef __STATUESTATEMACHINE_H__
#define __STATUESTATEMACHINE_H__

#include <Arduino.h>
#include "src/Lights/Lights.h"
#include "src/DeltaTime/DeltaTime.h"
#include "src/AudioPlayer/AudioPlayer.h"
#include "src/StatueSetting/StatueSetting.h"
#include "src/SensorsManager/SensorsManager.h"


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
  bool isSinging = false;
  bool canInteract = false;

  //Audio
  //===================================================
  float audioBusyPin;

  //Cronometros
  //===================================================
  float timeInteracting = 0;


public:
  //Main States
  //===================================================
  enum State {
    IDLE,
    INTERACTING,
    PETTING
  };
  State state = IDLE;
  bool onHappyEnding = false;

  //Init
  //===================================================
  StatueStateMachine();
  void Init(StatueSetting* _statueSetting, SensorsManager* _sensorsManager, Lights* _lights, DeltaTime* _deltaTime);
  void GetAudioBusyPin(int pin);

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
  bool IsPlayingAudio();
  void ResetAll();
};

#endif