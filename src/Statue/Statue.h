#ifndef __STATUE_H__
#define __STATUE_H__
#include <Arduino.h>

class Statue {

private:
  //Setting and songs
  //===================================================
  int LEDs;

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
  enum AudiosTrack {
    TRACK_SILENCE,        // 0
    TRACK_PURR_COMPLAIN,  // 1
    TRACK_SONG_1,         // 2
    TRACK_SONG_2,         // 3
    TRACK_GOOD_ENDING,    // 4
    TRACK_BAD_ENDING      // 5
  };
  enum StatueName {
    happy,
    sad
  };
  
  StatueName name;
  Statue(StatueName _name, int ledsPin);
  ~Statue();

  //LEDs
  void InitLeds();
  void TurnOnLEDs(bool val);

  void Enable();
  void Disable();
  bool IsEnabled();
};


#endif