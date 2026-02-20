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

void StatueStateMachine::GetAudioBusyPin(int pin) {
  audioBusyPin = pin;
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
  lights->TurnOn(LOW);
  //Check if at any sensor has a stable touch
  if (!sensorsManager->areAllSensorsOff()) {
    ChangeState(INTERACTING);
    sensorsManager->DebugInteraction();
    //PlaySound(); ------> Puede reproducir un aduio al inicio de la interaccions
  }
}

void StatueStateMachine::UpdateInteraction(float pettingTriggerTime, int minSensorsToPet) {
  lights->TurnOn(HIGH);

  bool canStartRunningTime = sensorsManager->areMultipleSensorsOn(minSensorsToPet);
  if (canStartRunningTime) {
    timeInteracting += deltaTime->Get();
  }

  bool canChangeToPetting = timeInteracting > pettingTriggerTime;
  if (canChangeToPetting) {
    ChangeState(PETTING);
    sensorsManager->DebugPetting();
    PlaySound(statueSetting->TRACK_SONG_1);
    // SendToServerFormated("IniciarMimitos");
    return;
  }

  bool canChangeToIdle = sensorsManager->areAllSensorsOff();
  if (canChangeToIdle) {
    ChangeState(IDLE);
    ResetAll();
  }
}

void StatueStateMachine::UpdatePetting() {
  lights->TurnOn(HIGH);
  bool canChangeToIdle = sensorsManager->areAllSensorsOff();  //-----> DEBUG
  //bool canChangeToIdle = !IsPlayingAudio();
  if (canChangeToIdle) {
    ChangeState(IDLE);
    ResetAll();
    Serial.println("El cambio de estados de Pettign a Idle esta usando un cambio de estado para debug");
  }
}

// ==================== HANDLE INTERACTION STATE
void StatueStateMachine::SetCanInteract(bool _canInteract) {
  canInteract = _canInteract;
}
bool StatueStateMachine::GetCanInteract() {
  return canInteract;
}
bool StatueStateMachine::IsPlayingAudio() {
  return digitalRead(audioBusyPin);
}


void StatueStateMachine::ResetAll() {
  timeInteracting = 0;
  sensorsManager->ResetAllSensors();
}
