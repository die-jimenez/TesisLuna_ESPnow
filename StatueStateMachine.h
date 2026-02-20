#ifndef __STATUESTATEMACHINE_H__
#define __STATUESTATEMACHINE_H__

#include <Arduino.h>
#include "src/Statue/Statue.h"
#include "src/DeltaTime/DeltaTime.h"
#include "src/AudioPlayer/AudioPlayer.h"
#include "src/SensorsManager/SensorsManager.h"


class StatueStateMachine {
private:
  //Objects
  //===================================================
  Statue* statue;
  DeltaTime* deltaTime;
  SensorsManager* sensorsManager;

  //States
  //===================================================
  bool isSinging = false;
  bool interactionEnabled = false;

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

  //Init
  //===================================================
  StatueStateMachine();
  void Init(Statue* _statue, SensorsManager* _sensorsManager, DeltaTime* _deltaTime);
  void GetAudioBusyPin(int pin);

  //State Update
  //===================================================
  void ChangeState(State newState);
  void UpdateIdle();
  void UpdateInteraction(float triggerToPetting, int minSensorsToPet);
  void UpdatePetting();

  //Handle Interaction State
  //===================================================
  void EnableInteraction();
  void DisableInteraction();
  bool IsEnabledInteraction();
  bool IsPlayingAudio();

  void ResetAll();
};

#endif