#pragma once

#include "enemyState.h"
#define ACTIBLEFRAME (15)

class EnemyStateMiddleDis: public EnemyState 
{
private:
	int m_ActibleFrame = ACTIBLEFRAME;

public :
	int generateRandomNumber(int max);
	void Init() override;
	void Update(Enemy* enemy) override;

	int  GetActibleFrame() { return m_ActibleFrame; }
	void SetActibleFrame() { m_ActibleFrame = ACTIBLEFRAME; }
	void PullActibleFrame() 
	{
		if (m_ActibleFrame != 0)
		{
			m_ActibleFrame--;
		}

		if (m_ActibleFrame < 0)
		{
			m_ActibleFrame = 0;
		}
	}

};

