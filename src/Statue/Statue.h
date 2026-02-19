#ifndef __STATUE_H__
#define __STATUE_H__
#include <Arduino.h>

class Statue {

private:
  //Setting and songs
  //===================================================
  int LEDs;
  float timeLightFlickering;

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
};


#endif