#pragma once
#include "scene.h"

class Title : public Scene
{
private:
	class Polygon2D* pushEnter{};
public:
	void Init()   override;
	void Update() override;
};
