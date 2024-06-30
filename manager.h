#pragma once

//class Scene;//‘O•ûéŒ¾
#include "scene.h"

class Manager
{
private:
	static Scene* m_Scene;
	static Scene* m_NextScene;

public:
	static void Init();
	static void Uninit();
	static void Update();
	static void Draw();

	static Scene* GetScene() { return m_Scene; }

	template <typename T>
	static void SetScene()
	{
		m_NextScene = new T();
	}

};