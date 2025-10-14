#pragma once

#include "enemyState.h"

class EnemyStateDefend: public EnemyState 
{
	//弱、下、横、上、弾　攻撃の種類
	void Update(Enemy* enemy) override;
};

