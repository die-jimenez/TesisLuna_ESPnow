#ifndef __GLOBALSTATEMACHINE_H__
#define __GLOBALSTATEMACHINE_H__

#include <Arduino.h>
#include "src/AudioPlayer/AudioPlayer.h"
#include "src/CustomEspNow/CustomEspNow.h"
#include "src/DeltaTime/DeltaTime.h"

class GlobalStateMachine {

public:
  enum Fase { STANDBY, DESARROLLO, FINAL };
  enum Escultura { FELIZ, TRISTE };

  struct SyncData {
    Fase faseActual;
    Escultura esculturaInteractuable;
    bool felizListoParaFinal;
    bool tristeListoParaFinal;
  };

private:
  //Objects
  //===================================================
  DeltaTime* deltaTime;

  //Data
  //===================================================
  SyncData syncData;
  int statue = 100;

  //Timers
  //===================================================
  float timerLuces = 0;
  bool pendingLucesApagar = false;

  //Phases
  //===================================================
  void PrimerosMimitos();
  void MimitosDesarrollo();
  void MimitosFinal();
  void PrepararFinal();
  void EjecutarFinal();

  //Sync
  //===================================================
  void SyncToOther();

public:
  //Init
  //===================================================
  GlobalStateMachine();
  void Init(int _statue, DeltaTime* _deltaTime);

  //Update
  //===================================================
  void UpdateTimers();

  //Events (called from StatueStateMachine)
  //===================================================
  void OnInteraccion();
  void OnMimitos();

  //Receive (called from EspNow callback)
  //===================================================
  void OnReceiveData(const EspNowMessage& msg);
};

#endif