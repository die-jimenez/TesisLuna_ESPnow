#ifndef __AUDIOPLAYER_H__
#define __AUDIOPLAYER_H__


#pragma once
#include <Arduino.h>

void SDcardInit(int SCK, int MISO, int MOSI, int SS);
void AudioInit(int BCLK, int LRC, int DOUT);
void AudioUpdate();

void PlaySound(const char *filePath);
void StopSound();

bool AudioFileExists(const char *filePath);
void CleanMemory();

#endif