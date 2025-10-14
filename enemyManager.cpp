#include "main.h"
#include "renderer.h"
#include "scene.h"
#include "input.h"
#include "game.h"
#include "enemy.h"
#include "enemyManager.h"
#include "enemyState.h"
#include "enemyStateLongDis.h"

void EnemyManager::Init()
{
	SetState<EnemyState>(); 
}

void EnemyManager::Uninit()
{
	delete m_State;
}

void EnemyManager::Update(Enemy* enemy)
{
	if(m_NextState)
	{
		if (m_State)
		{
			m_State->Uninit();
			delete m_State;
		}

		m_State = m_NextState;
		m_State->Init();

		m_NextState = nullptr;
	}

	m_State->Update(enemy);
}
