#pragma once
#define HITSTOP (40)

class Collision
{
private:
	static int m_hitStop;
	public:
		static void BodyCollisionBB();
		static void PlayerAttackCollisionBB();
		static void EnemyAttackCollisionBB();

		static bool IsBurst(float vecX,float vecY, GameObject* gameobject);
		static void SetHitStop() { m_hitStop = HITSTOP; }
		static void PullHitStop() 
		{
			if (m_hitStop != 0)
			{
				m_hitStop--;
			}

			if (m_hitStop < 0)
			{
				m_hitStop = 0;
			}
		}

		static int GetHitStop() { return m_hitStop; }
};