

#include "main.h"
#include "manager.h"
#include "collision.h"
#include <thread>
#include "game.h"
#include "debug.h"
#include "input.h"
#include "inputx.h"  //コントローラーの入力処理
#include "sound.h"

#pragma comment (lib, "d3d11.lib")		
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "d3dx11.lib")	
#pragma comment (lib, "d3dx9.lib")	
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "dxerr.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "xinput.lib")

const char* CLASS_NAME = "AppClass";
const char* WINDOW_NAME = "DX11ゲーム";

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HWND g_Window;

HWND GetWindow()
{
	return g_Window;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex;
	{
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = 0;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = nullptr;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = nullptr;
		wcex.lpszMenuName = nullptr;
		wcex.lpszClassName = CLASS_NAME;
		wcex.hIconSm = nullptr;

		RegisterClassEx(&wcex);


		RECT rc = { 0, 0, (LONG)SCREEN_WIDTH, (LONG)SCREEN_HEIGHT };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

		g_Window = CreateWindowEx(0, CLASS_NAME, WINDOW_NAME, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);
	}

	Manager::Init();
	InitInput(hInstance, GetWindow());
	InitInputX();
	InitSound(GetWindow());

	ShowWindow(g_Window, nCmdShow);
	UpdateWindow(g_Window);

	DWORD dwExecLastTime;
	DWORD dwCurrentTime;
	timeBeginPeriod(1);
	dwExecLastTime = timeGetTime();
	dwCurrentTime = 0;

	MSG msg;
	while(1)
	{
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{
				break;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
        }
		else
		{
			dwCurrentTime = timeGetTime();

			if((dwCurrentTime - dwExecLastTime) >= (1000 / 60))
			{
				dwExecLastTime = dwCurrentTime;

				Manager::Update();
				UpdateInput();
				UpdateInputX();

				Manager::Draw();
				
				//コリジョン
				if (!Game::GetIsPause() && !Debug::GetIsFrameMode() && !Debug::GetIsPause() && Collision::GetHitStop() == 0)
				{
					Collision::BodyCollisionBB();
					Collision::PlayerAttackCollisionBB();
					Collision::EnemyAttackCollisionBB();
				}
				else if (Collision::GetHitStop() != 0)
				{
					Collision::PullHitStop();
				}

				if ((GetKeyboardTrigger(DIK_F) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_DPAD_RIGHT)) && Debug::GetIsFrameMode() == true)
				{
					Collision::BodyCollisionBB();
					Collision::PlayerAttackCollisionBB();
					Collision::EnemyAttackCollisionBB();
				}
			}
		}
	}

	timeEndPeriod(1);

	UnregisterClass(CLASS_NAME, wcex.hInstance);

	Manager::Uninit();
	UninitInput();
	UninitInputX();
	//サウンドの終了
	UninitSound();

	return (int)msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	switch(uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		}
		break;

	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
