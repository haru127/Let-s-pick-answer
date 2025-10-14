//-------- インクルードヘッダ
#define _CRT_SECURE_NO_WARNINGS

#define STRICT

#include "main.h"
#include "manager.h"
#include "renderer.h"

#include "result.h"
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

bool Result::m_IsPlayerWin = true;//静的メンバ変数は再宣言が必要

void Result::Init()
{
	//配列に画像を追加
	m_SelectImage[0][0] = "asset\\texture\\win1.png";
	m_SelectImage[1][0] = "asset\\texture\\lose1.png";

	m_backGround = AddGameObjectNonInit<Polygon2D>(2);

	if (m_IsPlayerWin)
	{
		m_backGround->Init(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, m_SelectImage[0][0]);
	}
	else 
	{
		m_backGround->Init(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, m_SelectImage[1][0]);
	}

	m_Fade = AddGameObjectNonInit<Fade>(2);//"asset\\texture\\field002.jpg"
	m_Fade->Init(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, "asset\\texture\\black.png");
}

void Result::Update()
{
	Scene::Update();

	if (GetKeyboardTrigger(DIK_SPACE) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_B))
	{
		m_Fade->FadeOut();
	}

	if (m_Fade->GetFadeFinish())
	{
		Manager::SetScene<Select>();
	}

}
