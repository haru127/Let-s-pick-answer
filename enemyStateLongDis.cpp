#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "scene.h"
#include "result.h"
#include "gameObject.h"
#include "player.h"
#include "enemy.h"
#include "enemyStateLongDis.h"

void EnemyStateLongDis::Update(Enemy* enemy)
{
	Scene* scene = Manager::GetScene();

	//Enemy* enemy = scene->GetGameObject<Enemy>();
	Player* player = scene->GetGameObject<Player>();

	//Ú‹ß
	if (enemy->GetPosition().x < player->GetPosition().x)
	{
		enemy->Mo_MoveR();
		//enemy->At_Shot();
	}
	else 
	{
		enemy->Mo_MoveL();
		//enemy->At_Shot();
	}
}
