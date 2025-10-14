#pragma once
#include "scene.h"
#include <list>

enum SELECT
{
	SelectGame = 0,
	SelectDebug,
	SelectMission,
	SelectEtc
};

class Select : public Scene
{
private:
	SELECT m_select = SelectGame;
	const char* m_SelectImage[28][4];
	class Polygon2D* m_pushEnter{};
	class Polygon2D* m_backGround{};
public:
	void Init()   override;
	void Update() override;
};
