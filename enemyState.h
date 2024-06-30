#pragma once

class EnemyState
{
protected:
	EnemyState* m_Child{};
	
public :
	virtual void Init();
	virtual void Update(Enemy* enemy);
	virtual void Uninit();
	virtual void Draw();
};


