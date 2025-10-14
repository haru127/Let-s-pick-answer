#pragma once
#include "scene.h"

class Result : public Scene
{
private:
	class Polygon2D* pushEnter{};
	const char* m_SelectImage[28][4];
	class Polygon2D* m_backGround{};

	static bool m_IsPlayerWin;
public:
	void Init()   override;
	void Update() override;

	static void SetResult(bool isWin) { m_IsPlayerWin = isWin; }
};
