#pragma once

#include "enemyState.h"

class EnemyStateMiddleNearDis: public EnemyState 
{

public :
	int generateRandomNumber(int max);

	void Update(Enemy* enemy) override;

};

