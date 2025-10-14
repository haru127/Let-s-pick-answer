#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "scene.h"
#include "result.h"
#include "gameObject.h"
#include "player.h"
#include "enemy.h"
#include "enemyStateDefend.h"

void EnemyStateDefend::Update(Enemy* enemy)
{
	Scene* scene = Manager::GetScene();

	//Enemy* enemy = scene->GetGameObject<Enemy>();
	if (enemy->isAttack() == false)
	{
		enemy->De_Guard(); //ÉKÅ[Éh
	}
}
