// SensorsManager.h
#ifndef __SENSORSMANAGER_H__
#define __SENSORSMANAGER_H__

#include <Arduino.h>
#include "../TouchSensor/TouchSensor.h"

class SensorsManager {
private:
  TouchSensor* sensors;
  int sensorsCount;

  float timeToPet = 0;
  int contadorInteraccion = 0;
  int contadorMimitos = 0;

  bool showDebug = false;


public:
  SensorsManager(TouchSensor _sensors[], int _sensorsCount);

  //sensors
  void InitSensors();
  void UpdateSensors(float deltaTime);

  bool areMultipleSensorsOn(uint8_t _cant);
  bool areAllSensorsOff();
  void ResetAllSensors();

  //Trigger to petting
  void RunTimeToPet(float deltaTime);
  float GetTimeToPet();

  //Debugs
  void ResetTimeToPet();
  void SetShowDebug(bool _showDebug);
  void DebugInteraction();
  void DebugPetting();
  void GetAllError();
};

#endif