#ifndef __AUDIOPLAYER_H__
#define __AUDIOPLAYER_H__

#pragma once
#include <Arduino.h>
#include "HardwareSerial.h"

// ====== ELEGIR UN SOLO MODULO ======
// DFRobot original / MP3-TP-16P
// #define AUDIO_MODULE_DFP

// HW-247A clon
#define AUDIO_MODULE_HW247
// ===================================

#ifdef AUDIO_MODULE_DFP
#include "DFRobotDFPlayerMini.h"
#endif
#ifdef AUDIO_MODULE_HW247
#include "DFPlayer.h"
#endif

void Mp3ModuleInit(int RX, int TX, int BUSY);
void PlaySound(int audioIndex);
void PlaySound(int audioIndex, float volume);
void StopSound();
bool IsPlayingAudio();

#endif