#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "scene.h"
#include "result.h"
#include "gameObject.h"
#include "player.h"
#include "enemy.h"
#include "enemyStateMiddleNearDis.h"
#include <iostream>
#include <random>

void EnemyStateMiddleNearDis::Update(Enemy* enemy)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());

	Scene* scene = Manager::GetScene();

	//Enemy* enemy = scene->GetGameObject<Enemy>();
	Player* player = scene->GetGameObject<Player>();

	//Ú‹ß
	if (enemy->GetPosition().x < player->GetPosition().x)
	{
		switch (generateRandomNumber(3))
		{
		case 0:
			enemy->Mo_MoveR();
			break;
		case 1:
			enemy->Mo_MoveR();
			break;
		case 2:
			enemy->Mo_MoveR();
			break;
		case 3:
			enemy->Mo_MoveR();
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
			enemy->Mo_MoveL();
			break;
		case 1:
			enemy->Mo_MoveL();
			break;
		case 2:
			enemy->Mo_MoveL();
			break;
		case 3:
			enemy->Mo_MoveL();
			break;
		default:
			break;
		}

	}
}

int EnemyStateMiddleNearDis::generateRandomNumber(int max) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, max);
	return dis(gen);
}