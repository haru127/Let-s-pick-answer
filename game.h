#pragma once
#include "scene.h"

class Game : public Scene
{
private:
	static bool m_isPause;
	const char* m_UiImage[28][5];
	const char* m_CountDownImagePass[28][4];
	const char* m_ProcessImagePass[28][4];
	class Polygon2D* m_PauseImage{};
	class Polygon2D* m_CountDownImage{};
	class Polygon2D* m_ProcessImage{};
	class Polygon2D* m_CharactorImage[2];
	int m_ImageCount{};//ポーズ画面で使う
	int m_ProcessCount{};
	int m_CountDown[4];

	bool isFinishCountDown = false;
	bool isFinishProcess   = false;

	//撃墜演出
	bool m_IsKillShot[2]{};//player.enemy
	int  m_KillShotTime[2]{};
	class Polygon2D* m_KillShotImage[2];

public:
	void Init() override;
	void Uninit() override;
	void Update() override;

	static bool GetIsPause() 
	{
		return m_isPause;
	}
};