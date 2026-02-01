#include "StateMachine.h"

StateMachine::StateMachine() {
  statue = nullptr;
  sensorsManager = nullptr;
  deltaTime = nullptr;
}

void StateMachine::Init(Statue* _statue, SensorsManager* _sensorsManager, DeltaTime* _deltaTime) {
  statue = _statue;
  sensorsManager = _sensorsManager;
  deltaTime = _deltaTime;
}

void StateMachine::ChangeState(State newState) {
  Serial.print("Nuevo estado: ");
  
  if (newState == IDLE) Serial.println("IDLE");
  else if (newState == INTERACTING) Serial.println("INTERACTING");
  else if (newState == PETTING) Serial.println("PETTING");
  else Serial.println("UNKNOWN");
  state = newState;
}


// ==================== ESTADOS
void StateMachine::UpdateIdle() {
  statue->TurnOnLEDs(LOW);
  if (!sensorsManager->areAllSensorsOff()) {
    ChangeState(INTERACTING);
    sensorsManager->DebugInteraction();
  }
}

void StateMachine::UpdateInteraction(float triggerToPetting, int minSensorsToPet) {
  statue->TurnOnLEDs(HIGH);

  bool canStartRunningTime = sensorsManager->areMultipleSensorsOn(minSensorsToPet);
  if (canStartRunningTime) {
    sensorsManager->RunTimeToPet(deltaTime->Get());
  }

  bool canChangeToPetting = sensorsManager->GetTimeToPet() > triggerToPetting;
  if (canChangeToPetting) {
    ChangeState(PETTING);
    sensorsManager->ResetTimeToPet();
    sensorsManager->ResetAllSensors();
    sensorsManager->DebugPetting();
    // SendToServerFormated("IniciarMimitos");
    return;
  }

  bool canChangeToIdle = sensorsManager->areAllSensorsOff();
  if (canChangeToIdle) {
    ChangeState(IDLE);
    sensorsManager->ResetTimeToPet();
    sensorsManager->ResetAllSensors();
  }
}

void StateMachine::UpdatePetting() {
  statue->TurnOnLEDs(HIGH);
  bool canChangeToIdle = sensorsManager->areAllSensorsOff();
  if (canChangeToIdle) {
    ChangeState(IDLE);
    sensorsManager->ResetAllSensors();
  }
}