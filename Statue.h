#ifndef __STATUE_H__
#define __STATUE_H__

#include <Arduino.h>

class Statue {

private:
  //Setting and songs
  //===================================================
  String name;
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
  Statue(String _name);
  ~Statue();

  enum AudiosTrack {
    TRACK_SILENCE,        // 0
    TRACK_PURR_COMPLAIN,  // 1
    TRACK_SONG_1,         // 2
    TRACK_SONG_2,         // 3
    TRACK_GOOD_ENDING,    // 4
    TRACK_BAD_ENDING      // 5
  };

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