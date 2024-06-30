#pragma once
#include "scene.h"

class Debug : public Scene
{
private:
	static bool m_isPause;
	static bool m_isFrameMode;

	const char* m_UiImage[28][5];
	const char* m_DebugImage[28][4];
	class Polygon2D* m_backGround{};
	class Polygon2D* m_PauseImage{};
	int m_ImageCount{};//ポーズ画面で使う


public:
	void Init() override;
	void Uninit() override;
	void Update() override;

	static bool GetIsFrameMode()
	{
		return m_isFrameMode;
	}

	static bool GetIsPause()
	{
		return m_isPause;
	}

};