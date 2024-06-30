
#pragma once

#include <windows.h>
#include "xaudio2.h"

#define VOLUME_DEFAULT (0.1f)


bool InitSound(HWND hWnd); //�T�E���h�̏�����
void UninitSound(void); //�T�E���h�̏I��

int LoadSound(char* pFilename);//�T�E���h�̃��[�h
void PlaySound(int index, int loopCount); //�T�E���h�̍Đ�
										  //loopCount = 0�ň��Đ�
										  //loopCount < 0�Ń��[�v
										  //loopCount >= 1�Ń��[�v��2��`
void PlaySound(int index, int loopCount, float volume);

void StopSound(int index);//�T�E���h�̒�~

void StopSoundAll(void);
void SetVolume(int index, float vol);
