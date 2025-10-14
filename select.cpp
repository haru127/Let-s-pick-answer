//-------- インクルードヘッダ
#define _CRT_SECURE_NO_WARNINGS

#define STRICT

#include "main.h"
#include "manager.h"
#include "renderer.h"

#include "select.h"
#include "game.h"
#include "debug.h"
#include "mission.h"
#include "fade.h"

#include "camera.h"
#include "field.h"
#include "player.h"
#include "input.h"
#include "inputx.h"
#include "enemy.h"
#include "polygon2D.h"
#include "bullet.h"
#include "explosion.h"

void Select::Init()
{
	//リストに追加
	m_SelectImage[0][0] = "asset\\texture\\game.png";
	m_SelectImage[1][0] = "asset\\texture\\debug.png";
	m_SelectImage[2][0] = "asset\\texture\\mission.png";
	m_SelectImage[3][0] = "asset\\texture\\etc.png";

	m_Fade = AddGameObjectNonInit<Fade>(2);//"asset\\texture\\field002.jpg"
	m_Fade->Init(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, "asset\\texture\\black.png");

	m_backGround = AddGameObjectNonInit<Polygon2D>(2);//"asset\\texture\\field002.jpg"
	m_backGround->Init(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, m_SelectImage[0][0]);
}

void Select::Update()
{
	Scene::Update();

	//シーン選択
	{
		if (GetKeyboardTrigger(DIK_D) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_DPAD_RIGHT))
		{
			switch (m_select)
			{
			case SelectGame:
				m_backGround->SetImage(m_SelectImage[1][0]);//デバッグ
				m_select = SelectDebug;
				break;
			case SelectMission:
				m_backGround->SetImage(m_SelectImage[3][0]);//etc
				m_select = SelectEtc;
			default:
				break;
			}
		}

		if (GetKeyboardTrigger(DIK_A) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_DPAD_LEFT))
		{
			switch (m_select)
			{
			case SelectDebug:
				m_backGround->SetImage(m_SelectImage[0][0]);//対戦
				m_select = SelectGame;
				break;
			case SelectEtc:
				m_backGround->SetImage(m_SelectImage[2][0]);//mission
				m_select = SelectMission;
			default:
				break;
			}
		}

		if (GetKeyboardTrigger(DIK_S) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_DPAD_DOWN))
		{
			switch (m_select)
			{
			case SelectGame:
				m_backGround->SetImage(m_SelectImage[2][0]);//mission
				m_select = SelectMission;
				break;
			case SelectDebug:
				m_backGround->SetImage(m_SelectImage[3][0]);//etc
				m_select = SelectEtc;
			default:
				break;
			}
		}

		if (GetKeyboardTrigger(DIK_W) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_DPAD_UP))
		{
			switch (m_select)
			{
			case SelectMission:
				m_backGround->SetImage(m_SelectImage[0][0]);//対戦
				m_select = SelectGame;
				break;
			case SelectEtc:
				m_backGround->SetImage(m_SelectImage[1][0]);//デバッグ
				m_select = SelectDebug;
			default:
				break;
			}
		}

	}


	//シーン決定
	if ((GetKeyboardTrigger(DIK_SPACE) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_B)))
	{
		m_Fade->FadeOut();
	}

	if (m_Fade->GetFadeFinish())
	{
		switch (m_select)
		{
		case SelectGame:
			Manager::SetScene<Game>();
			break;
		case SelectDebug:
			Manager::SetScene<Debug>();
			break;
		case SelectMission:
			Manager::SetScene<Mission>();
			break;

		}
	}

}


