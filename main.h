#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#define NOMINMAX
#include <windows.h>
#include <assert.h>
#include <functional>



#pragma warning(push)
#pragma warning(disable:4005)

#include <d3dx9.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>

#define DIRECTINPUT_VERSION 0x0800		// åxçêëŒèà
#include "dinput.h"
#include "mmsystem.h"

#pragma warning(pop)



#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "d3dx11.lib")

#define SCREEN_WIDTH	(1280)
#define SCREEN_HEIGHT	(720)

HWND GetWindow();


