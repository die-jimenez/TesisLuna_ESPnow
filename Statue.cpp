#include "Statue.h"

Statue::Statue(String _name, const char* _songsPath[4]) {
  name = _name;
  for (int i = 0; i < 4; i++) {
    songsPath[i] = _songsPath[i];
  }
}

Statue::~Statue() {
}
