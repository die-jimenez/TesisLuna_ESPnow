#include <stdint.h>
#ifndef __TOUCHSENSOR_H__
#define __TOUCHSENSOR_H__

#include <Arduino.h>

class TouchSensor {
private:
  uint8_t id;
  uint8_t pin;

  float timeToReset;
  const float resetTriggerTime = 1;

  //Reconocer interaccion
  float activeTime;
  const float minTouchDuration = 1.5;

  //Debug
  bool debugEventExecuted;
  bool debug = false;


public:
  //Constructor por defecto: Para caundo creas un arreglo y no los definis en el momento
  TouchSensor(uint8_t _pin, uint8_t id);
  ~TouchSensor();

  void Init();
  void Update(float _deltaTime);

  bool isTouch();
  bool isTouchStable();

  void RunTouchTime(float _deltaTime);
  void ResetTime();
  float GetActiveTime();
  float GetMinTouchDuration();

  uint8_t GetID();
  void DebugEvent();
  void ResetDebugEvent();
  void SetDebugMode(bool _state);
  int contadorFalsosPositivo;
};


#endif