//Library versions are detailed in the README
#include "Arduino.h"
#include "src/CustomEspNow/CustomEspNow.h"
#include "src/AudioPlayer/AudioPlayer.h"
#include "src/StatueSetting/StatueSetting.h"
#include "src/Lights/Lights.h"

#include "src/StatueStateMachine.h"
#include "src/GlobalStateMachine.h"

#include "src/DeltaTime/DeltaTime.h"
#include "src/TouchSensor/TouchSensor.h"
#include "src/SensorsManager/SensorsManager.h"


#pragma region->Pines

//Sensores
#define SENSOR_10_PIN 14
#define SENSOR_11_PIN 27
#define SENSOR_12_PIN 26
//---
#define SENSOR_20_PIN 18
#define SENSOR_21_PIN 19
#define SENSOR_22_PIN 21
//---
#define SENSOR_30_PIN 34
#define SENSOR_31_PIN 36  //pin SVP (Solo input)
#define SENSOR_32_PIN 25

// Luces
#define LEDS_BOCA 33
#define LEDS_ENTORNO 32

// MP3 module
#define RXD2 16  // esp receiver (rx) --> module transciever (tx)
#define TXD2 17  // D2 = segundo puerto serial, pero es solo nombre
#define BUSY 4

#pragma endregion
//===========================================================================================================


#pragma region->Declaraciones

TouchSensor sensors[9] = {
  //id 21... "2" fila y "1" columna
  TouchSensor(SENSOR_10_PIN, 10),
  TouchSensor(SENSOR_11_PIN, 11),
  TouchSensor(SENSOR_12_PIN, 12),
  TouchSensor(SENSOR_20_PIN, 20),
  TouchSensor(SENSOR_21_PIN, 21),
  TouchSensor(SENSOR_22_PIN, 22),
  TouchSensor(SENSOR_30_PIN, 30),
  TouchSensor(SENSOR_31_PIN, 31),
  TouchSensor(SENSOR_32_PIN, 32)
};

Lights lights(LEDS_BOCA, LEDS_ENTORNO);
DeltaTime deltaTime;

StatueStateMachine statueStateMachine;
GlobalStateMachine globalStateMachine;

int contadorInteraccion;
int contadorMimitos;

#pragma endregion
//===========================================================================================================


#pragma region->Parametros modificables
//===================================================
#ifdef STATUE_HAPPY
StatueSetting statueSetting(StatueSetting::Name::SENSORS_HAPPY);  //SENSORS_HAPPY || SENSORS_SAD
const uint8_t SENSORS_COUNT = 5;                                  //Sensores activos. Evita pinouts de más
const int MIN_SENSORS_ACTIVE_TO_PET = 1;                          //Minimo de sensores activados para contar "Mimito" || INTERACION -> MIMITOS
const float pettingTriggerTime = 1.25;                             // Minimmo: 0.5 -> Tiempo de interaccion para Mimito || INTERACION -> MIMITOS
#endif

#ifdef STATUE_SAD
StatueSetting statueSetting(StatueSetting::Name::SENSORS_SAD);  //SENSORS_HAPPY || SENSORS_SAD
const uint8_t SENSORS_COUNT = 7;                                //Sensores activos. Evita pinouts de más
const int MIN_SENSORS_ACTIVE_TO_PET = 2;                        //Minimo de sensores activados para contar "Mimito" || INTERACION -> MIMITOS
const float pettingTriggerTime = 0.75;                           //Minimmo: 0.5 -> Tiempo de interaccion para Mimito || INTERACION -> MIMITOS
#endif

const float INACTIVITY_TIMEOUT = 60.0;
const float BAD_ENDING_RESET_TIMEOUT = 30.0;
SensorsManager sensorsManager(sensors, SENSORS_COUNT);
#pragma endregion
//===========================================================================================================




//En un "h" -> C++ lee se arriba para abajo
//===================================================
void OnReceiveData(const EspNowMessage& data);
void OnSendData(const EspNowMessage& data);
void OnAudioFinished();
void OnPettingStarted();
void DebugStage(GlobalStateMachine* global, GlobalStateMachine::Stages x);
//===================================================


//Métodos Principales
//===================================================
void Sensores_Setup() {
  Serial.begin(115200);
  delay(3000);
  Serial.println("Iniciando sensores");

  //Global state machine
  globalStateMachine.Init(&statueSetting, &statueStateMachine, &deltaTime);
  //DebugStage(&globalStateMachine, GlobalStateMachine::Stages::DESARROLLO);

  //State Machine
  statueStateMachine.Init(&statueSetting, &sensorsManager, &lights, &deltaTime);
  statueStateMachine.RegisterOnPettingStarted(OnPettingStarted);
  statueStateMachine.RegisterOnAudioFinished(OnAudioFinished);
  statueStateMachine.SetCanInteract(true);

  //Esp now
  EspNowInit(statueSetting.name);
  EspNowRegisterOnSend(OnSendData);
  EspNowRegisterOnReceive(OnReceiveData);

  // //Sensors init
  sensorsManager.SetShowDebug(true);
  sensorsManager.InitSensors();

  //Leds
  lights.Init();
  lights.Update(Lights::LightState::ON, &deltaTime);

  delay(1000);
  Serial.println("setup is complete");
  Serial.println("--------------------------------------------");
}

void Sensores_Loop() {
  //Sensors update
  sensorsManager.UpdateSensors(deltaTime.Get());


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

void DebugStage(GlobalStateMachine* global, GlobalStateMachine::Stages x) {
  global->stage = x;
  Serial.println("ESTOY MODIFICANDO EL STAGE EN EL SETUP POR DEBUG ====================================");
}
