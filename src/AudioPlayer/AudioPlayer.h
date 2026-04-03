#ifndef __AUDIOPLAYER_H__
#define __AUDIOPLAYER_H__

#pragma once
#include <Arduino.h>
#include "HardwareSerial.h"

// ====== ELEGIR UN SOLO MODULO ======
// DFRobot original 
//#define AUDIO_MODULE_DFP

// HW-247A clon
//#define AUDIO_MODULE_HW247

// MP3-TF-16P clon
#define AUDIO_MODULE_TF
// ===================================

#ifdef AUDIO_MODULE_DFP
#include "DFRobotDFPlayerMini.h"
#endif
#ifdef AUDIO_MODULE_HW247
#include "DFPlayer.h"
#endif
#ifdef AUDIO_MODULE_TF
#include "DFRobotDFPlayerMini.h"
#endif

void Mp3ModuleInit(int RX, int TX, int BUSY);
void PlaySound(int audioIndex);
void PlaySound(int audioIndex, float volume);
void StopSound();
bool IsPlayingAudio();
void DelayForBusyUpdate();

#endif