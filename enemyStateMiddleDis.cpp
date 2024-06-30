#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "scene.h"
#include "result.h"
#include "gameObject.h"
#include "player.h"
#include "enemy.h"
#include "enemyStateMiddleDis.h"
#include <iostream>
#include <random>

void EnemyStateMiddleDis::Update(Enemy* enemy)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());

	Scene* scene = Manager::GetScene();

	//Enemy* enemy = scene->GetGameObject<Enemy>();
	Player* player = scene->GetGameObject<Player>();

	//ステート
	if(m_ActibleFrame == 0)
	{
		//接近
		if (enemy->GetPosition().x < player->GetPosition().x)
		{
			switch (generateRandomNumber(3))
			{
			case 0:
				enemy->At_Punch();
				break;
			case 1:
				enemy->At_Shot();
				//enemy->Mo_MoveR();
				break;
			case 2:
				enemy->At_Grab();
				break;
			case 3:
				enemy->Ac_Jump();
				break;
			default:
				break;
			}

		}
		else
		{
			switch (generateRandomNumber(3))
			{
			case 0:
				enemy->At_Punch();
				break;
			case 1:
				enemy->At_Shot();
				break;
			case 2:
				enemy->At_Grab();
				break;
			case 3:
				enemy->Ac_Jump();
				break;
			default:
				break;
			}

		}
		//初期化
		m_ActibleFrame = ACTIBLEFRAME;
	}
	else 
	{
		if (enemy->GetPosition().x < player->GetPosition().x)
		{
			enemy->Mo_MoveR();
		}
		else
		{
			enemy->Mo_MoveL();
		}
		PullActibleFrame();
	}
}

int EnemyStateMiddleDis::generateRandomNumber(int max) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, max);
	return dis(gen);
}

void EnemyStateMiddleDis::Init()
{
	m_ActibleFrame = ACTIBLEFRAME;
}
