#include "StatueStateMachine.h"

// ==================== INIT
StatueStateMachine::StatueStateMachine() {
  statue = nullptr;
  sensorsManager = nullptr;
  deltaTime = nullptr;
}

void StatueStateMachine::Init(Statue* _statue, SensorsManager* _sensorsManager, DeltaTime* _deltaTime) {
  statue = _statue;
  sensorsManager = _sensorsManager;
  deltaTime = _deltaTime;
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
  statue->TurnOnLEDs(LOW);
  //Check if at any sensor has a stable touch
  if (!sensorsManager->areAllSensorsOff()) {
    ChangeState(INTERACTING);
    sensorsManager->DebugInteraction();
    //PlaySound(); ------> Puede reproducir un aduio al inicio de la interaccions
  }
}

void StatueStateMachine::UpdateInteraction(float pettingTriggerTime, int minSensorsToPet) {
  statue->TurnOnLEDs(HIGH);

  bool canStartRunningTime = sensorsManager->areMultipleSensorsOn(minSensorsToPet);
  if (canStartRunningTime) {
    timeInteracting += deltaTime->Get();
  }

  bool canChangeToPetting = timeInteracting > pettingTriggerTime;
  if (canChangeToPetting) {
    ChangeState(PETTING);
    sensorsManager->DebugPetting();
    PlaySound(statue->TRACK_SONG_1);
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
  statue->TurnOnLEDs(HIGH);
  bool canChangeToIdle = sensorsManager->areAllSensorsOff();  //-----> DEBUG
  //bool canChangeToIdle = !IsPlayingAudio();
  if (canChangeToIdle) {
    ChangeState(IDLE);
    ResetAll();
    Serial.println("El cambio de estados de Pettign a Idle esta usando un cambio de estado para debug");
  }
}

// ==================== HANDLE INTERACTION STATE
void StatueStateMachine::EnableInteraction() {
  interactionEnabled = true;
}
void StatueStateMachine::DisableInteraction() {
  interactionEnabled = false;
}
bool StatueStateMachine::IsEnabledInteraction() {
  return interactionEnabled;
}
bool StatueStateMachine::IsPlayingAudio() {
  return digitalRead(audioBusyPin);
}



void StatueStateMachine::ResetAll() {
  timeInteracting = 0;
  sensorsManager->ResetAllSensors();
}
