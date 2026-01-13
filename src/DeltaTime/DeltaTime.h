#ifndef __DELTATIME_H__
#define __DELTATIME_H__

#include <Arduino.h>

class DeltaTime {
private:
  unsigned long lastTime;
  unsigned long currentTime;
  float deltaTime;

public:
  //Constructor por defecto: Para caundo creas un arreglo y no los definis en el momento
  DeltaTime();
  ~DeltaTime();

  void Run();
  float Get();
};





#endif