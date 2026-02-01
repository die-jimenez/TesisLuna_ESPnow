// StateMachine.h
#ifndef __STATEMACHINE_H__
#define __STATEMACHINE_H__

#include <Arduino.h>
#include "src/Statue/Statue.h"
#include "src/DeltaTime/DeltaTime.h"
#include "src/SensorsManager/SensorsManager.h"

class StateMachine {
private:
  Statue* statue;
  DeltaTime* deltaTime;
  SensorsManager* sensorsManager;

public:
  enum State {
    IDLE,
    INTERACTING,
    PETTING
  };
  State state = IDLE;

  StateMachine();
  void Init(Statue* _statue, SensorsManager* _sensorsManager, DeltaTime* _deltaTime);
  void ChangeState(State newState);

  void UpdateIdle();
  void UpdateInteraction(float triggerToPetting, int minSensorsToPet);
  void UpdatePetting();
};

#endif