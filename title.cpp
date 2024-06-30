//-------- インクルードヘッダ
#define _CRT_SECURE_NO_WARNINGS

#define STRICT

#include "main.h"
#include "manager.h"
#include "renderer.h"

#include "title.h"
#include "game.h"
#include "select.h"
#include "fade.h"

#include "camera.h"
#include "field.h"
#include "player.h"
#include "input.h"
#include "inputx.h"
#include "enemy.h"
#include "bullet.h"
#include "explosion.h"
#include "polygon2D.h"
#include "sound.h"

static int			  TitleBGM;//タイトル画面のBGM

//static float g_alpha = 0.1f;
void Title::Init()
{
	pushEnter = AddGameObjectNonInit<Polygon2D>(2);//"asset\\texture\\field002.jpg"
	pushEnter->Init(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, "asset\\texture\\title.png");

	m_Fade = AddGameObjectNonInit<Fade>(2);//"asset\\texture\\field002.jpg"
	m_Fade->Init(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, "asset\\texture\\black.png");

	char filename[] = "asset\\BGM\\DEAF KEV - Invincible [NCS Release] (online-audio-converter.com).wav";
	TitleBGM = LoadSound(filename);
	PlaySound(TitleBGM, -1);//タイトル画面のBGM再生
}

void Title::Update()
{
	Scene::Update();

	//g_alpha += 0.015f;
	//if (g_alpha > 1.0f)
	//{
	//	g_alpha = 0.0f;
	//}
	pushEnter->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

	if (IsButtonTriggeredX(0, XINPUT_GAMEPAD_B) || (GetKeyboardTrigger(DIK_SPACE)))
	{
		m_Fade->FadeOut();
	}

	if (IsButtonTriggeredX(0, XINPUT_GAMEPAD_A))
	{
		////ゲーム終了
		exit(988);
	}



	if (m_Fade->GetFadeFinish())
	{
		Manager::SetScene<Select>();
		StopSound(TitleBGM);//タイトルサウンドの再生停止
	}

}
