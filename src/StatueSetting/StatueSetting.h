#ifndef __STATUESETTING_H__
#define __STATUESETTING_H__
#include <Arduino.h>

class StatueSetting {

public:
  enum AudiosTrack {
    TRACK_SILENCE,        // 0
    TRACK_PURR_COMPLAIN,  // 1
    TRACK_SONG_1,         // 2
    TRACK_SONG_2,         // 3
    TRACK_GOOD_ENDING,    // 4
    TRACK_BAD_ENDING      // 5
  };
  enum Name {
    SENSORS_HAPPY,
    SENSORS_SAD,
    AUDIO_HAPPY,
    AUDIO_SAD
  };
  enum Destination {
    ToOtherMaster,  //false
    ToAudio         //true
  };
  Name name;

  StatueSetting(Name _name);
};

#endif
