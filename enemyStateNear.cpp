#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "scene.h"
#include "result.h"
#include "gameObject.h"
#include "player.h"
#include "enemy.h"
#include "enemyStateNear.h"
#include <iostream>
#include <random>

void EnemyStateNear::Update(Enemy* enemy)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());

	Scene* scene = Manager::GetScene();
	Player* player = scene->GetGameObject<Player>();

	if (player->isGuard() == true)
	{
		if (enemy->isAttack() == false)
		{
			enemy->At_Grab(); //Žã
		}
	}

	switch (generateRandomNumber(4))
	{
	case 0:
		enemy->At_Down();
		break;
	case 1:
		enemy->At_Forward();
		break;
	case 2:
		enemy->At_Upper();
		break;
	case 3:
		enemy->At_Punch();
		break;
	case 4:
		enemy->At_Shot();
		break;
	default:
		break;
	}
}

int EnemyStateNear::generateRandomNumber(int max)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, max);
	return dis(gen);
}
