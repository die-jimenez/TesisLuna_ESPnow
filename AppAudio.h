//Library versions are detailed in the README
#include "Arduino.h"
#include "src/StatueStateMachine.h"
#include "src/DeltaTime/DeltaTime.h"

#include "src/AudioPlayer/AudioPlayer.h"
#include "src/CustomEspNow/CustomEspNow.h"
#include "src/StatueSetting/StatueSetting.h"


#define RXD2 16  // esp receiver (rx) --> module transciever (tx)
#define TXD2 17  // D2 = segundo puerto serial, pero es solo nombre
#define BUSY 4

DeltaTime deltaTime;
StatueStateMachine statueStateMachine;
GlobalStateMachine globalStateMachine;


#pragma region->Parametros modificables
//===================================================
#define STATUE_SAD //--------------------------------> CAMBIAR ESTA VARIABLE PARA CAMBIAR LAS ESTATUAS =>

#ifdef STATUE_HAPPY

#endif

#ifdef STATUE_SAD

#endif

#pragma endregion
//===========================================================================================================




//En un "h" -> C++ lee se arriba para abajo
//===================================================
void OnReceiveData(const EspNowMessage& data);
void OnSendData(const EspNowMessage& data);
void OnAudioFinished();
void OnPettingStarted();
//===================================================


//Métodos Principales
//===================================================
void Audio_Setup() {
  Serial.begin(115200);
  Serial.println("Iniciando audio");
  delay(10000);

  //Global state machine
  globalStateMachine.Init(&statueSetting, &statueStateMachine, &deltaTime);

  //State Machine
  statueStateMachine.Init(&statueSetting, &sensorsManager, &lights, &deltaTime);
  statueStateMachine.RegisterOnPettingStarted(OnPettingStarted);
  statueStateMachine.RegisterOnAudioFinished(OnAudioFinished);
  statueStateMachine.SetCanInteract(true);

  //Esp now
  EspNowInit();
  EspNowRegisterOnSend(OnSendData);
  EspNowRegisterOnReceive(OnReceiveData);

  //Audio module
  Mp3ModuleInit(RXD2, TXD2, BUSY);
  delay(500);
  PlaySound(1);

  delay(1000);
  Serial.println("setup is complete");
  Serial.println("--------------------------------------------");
}

void Audio_Loop() {



  //Maquina de estados de la estatua
  //===================================================
  if (statueStateMachine.GetCanInteract()) {
    if (statueStateMachine.state == StatueStateMachine::IDLE) {
      statueStateMachine.UpdateIdle();
    }  //
    else if (statueStateMachine.state == StatueStateMachine::INTERACTING) {
      statueStateMachine.UpdateInteraction(pettingTriggerTime, MIN_SENSORS_ACTIVE_TO_PET);
    }  //
    else if (statueStateMachine.state == StatueStateMachine::PETTING) {
      statueStateMachine.UpdatePetting();
    }
  }

  if (!statueStateMachine.GetCanInteract()) {
    lights.Update(Lights::LightState::OFF, &deltaTime);
  }
  globalStateMachine.UpdateResetTimer(&INACTIVITY_TIMEOUT, &BAD_ENDING_RESET_TIMEOUT);

  deltaTime.Run();
  delay(5);
}
//===========================================================================================================



//Estos son los metodos custom para utilizar CustomEspNow
void OnReceiveData(const EspNowMessage& data) {
  globalStateMachine.OnReciveMessage(data);
}

void OnSendData(const EspNowMessage& data) {
  globalStateMachine.OnSendMessage(data);
}

void OnAudioFinished() {
  globalStateMachine.OnAudioFinished();
}

void OnPettingStarted() {
  globalStateMachine.OnPettingStarted();
}

