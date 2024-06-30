#pragma once

//class Scene;//‘O•ûéŒ¾
#include "EnemyState.h"

class EnemyManager
{
private:
	EnemyState* m_State;
	EnemyState* m_NextState;
public:
	void Init();
	void Uninit();
	void Update(Enemy* enemy);

	EnemyState* GetState() { return m_State; }

	template <typename T>
	void SetState()
	{
		m_NextState = new T();
	}

	template <typename T>
	bool checkState(EnemyState* state)
	{
		if (typeid(*state) == typeid(T))
		{
			return true;
		}
		else {
			return false;
		}
	}

};