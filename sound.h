
#pragma once

#include <windows.h>
#include "xaudio2.h"

#define VOLUME_DEFAULT (0.1f)


bool InitSound(HWND hWnd); //サウンドの初期化
void UninitSound(void); //サウンドの終了

int LoadSound(char* pFilename);//サウンドのロード
void PlaySound(int index, int loopCount); //サウンドの再生
										  //loopCount = 0で一回再生
										  //loopCount < 0でループ
										  //loopCount >= 1でループ回数2回～
void PlaySound(int index, int loopCount, float volume);

void StopSound(int index);//サウンドの停止

void StopSoundAll(void);
void SetVolume(int index, float vol);
