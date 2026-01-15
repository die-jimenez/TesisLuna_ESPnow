#ifndef __STATUE_H__
#define __STATUE_H__

#include <Arduino.h>

class Statue {

private:
  //Setting and songs
  //===================================================
  String name;
  const char* songsPath[4];
  int currentSong = 0;
  int nextSong = 1;

  //Main States
  //===================================================
  bool interactionEnabled = true;
  bool isSinging = false;

  //Intercation
  //===================================================
  enum InteractionState {
    NONE,
    MIMITOS_STARTED,
    MIMITOS_COMPLETED
  };
  InteractionState interactionState = InteractionState::NONE;






public:
  Statue(String _name, const char* _songsPath[4]);
  ~Statue();

  void Enable() {
    interactionEnabled = true;
  }
  void Disable() {
    interactionEnabled = false;
  }
  bool IsEnabled() {
    return interactionEnabled;
  }
};


#endif