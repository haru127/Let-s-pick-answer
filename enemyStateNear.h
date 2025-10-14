#pragma once

#include "enemyState.h"

class EnemyStateNear: public EnemyState 
{
public:
	int generateRandomNumber(int max);

	void Update(Enemy* enemy) override;
};

