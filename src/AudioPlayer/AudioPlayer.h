#ifndef __AUDIOPLAYER_H__
#define __AUDIOPLAYER_H__

#pragma once
#include <Arduino.h>


void Mp3ModuleInit(int RX, int TX); //16,17 or 26,27

void PlaySound(int audioIndex);
void PlaySound(int audioIndex, float volume);
void StopSound();

bool AudioFileExists(int filePath);

#endif