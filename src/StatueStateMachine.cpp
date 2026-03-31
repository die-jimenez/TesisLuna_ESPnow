#include "StatueStateMachine.h"

// ==================== INIT
StatueStateMachine::StatueStateMachine() {
  statueSetting = nullptr;
  sensorsManager = nullptr;
  deltaTime = nullptr;
}

void StatueStateMachine::Init(StatueSetting* _statueSetting, SensorsManager* _sensorsManager, Lights* _lights, DeltaTime* _deltaTime) {
  statueSetting = _statueSetting;
  sensorsManager = _sensorsManager;
  deltaTime = _deltaTime;
  lights = _lights;
}


// ==================== STATE UPDATE
void StatueStateMachine::ChangeState(State newState) {
  Serial.print("Nuevo estado: ");

  if (newState == IDLE) Serial.println("IDLE");
  else if (newState == INTERACTING) Serial.println("INTERACTING");
  else if (newState == PETTING) Serial.println("PETTING");
  else Serial.println("UNKNOWN");
  state = newState;
}

void StatueStateMachine::UpdateIdle() {
  lights->Update(Lights::LightState::FLASHING, deltaTime);
  //Check if at any sensor has a stable touch
  if (!sensorsManager->areAllSensorsOff()) {
    ChangeState(INTERACTING);
    sensorsManager->DebugInteraction();
    //PlaySound(); ------> Puede reproducir un aduio al inicio de la interaccions
  }
}

void StatueStateMachine::UpdateInteraction(float pettingTriggerTime, int minSensorsToPet) {
  lights->Update(Lights::LightState::FLASHING, deltaTime);

  bool canStartRunningTime = sensorsManager->areMultipleSensorsOn(minSensorsToPet);
  if (canStartRunningTime) {
    timeInteracting += deltaTime->Get();
  }

  bool canChangeToPetting = timeInteracting > pettingTriggerTime;
  //To Petting
  if (canChangeToPetting) {
    ChangeState(PETTING);
    sensorsManager->DebugPetting();
    if (onPettingStartedCallback != nullptr) onPettingStartedCallback();
    return;
  }

  bool canChangeToIdle = sensorsManager->areAllSensorsOff();
  //To idle
  if (canChangeToIdle) {
    ResetInteractionState();
  }
}

void StatueStateMachine::UpdatePetting() {
  lights->Update(Lights::LightState::ON, deltaTime);

  bool canChangeToIdle = IsPlayingAudio();
  if (canChangeToIdle) {
    //Enviar mensaje a la otra escultura
    if (onAudioFinishedCallback != nullptr) {
      onAudioFinishedCallback();
    }
    ResetInteractionState();
  }
}

// ==================== HANDLE INTERACTION STATE
void StatueStateMachine::SetCanInteract(bool _canInteract) {
  canInteract = _canInteract;
}

bool StatueStateMachine::GetCanInteract() {
  return canInteract;
}

void StatueStateMachine::ResetInteractionState() {
  timeInteracting = 0;
  sensorsManager->ResetAllSensors();
  ChangeState(State::IDLE);
}


void StatueStateMachine::ResetStatue() {
  timeInteracting = 0;
  sensorsManager->ResetAllSensors();
  ChangeState(State::IDLE);
  SetCanInteract(true);
}


// ==================== Events to send message through EspNow
void StatueStateMachine::RegisterOnPettingStarted(StatueStateMachine::OnPettingStartedCallback fn) {
  onPettingStartedCallback = fn;
}
void StatueStateMachine::RegisterOnAudioFinished(StatueStateMachine::OnAudioFinishedCallback fn) {
  onAudioFinishedCallback = fn;
}
