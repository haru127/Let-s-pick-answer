#pragma once
#include "gameObject.h"

class Fade :public GameObject
{
private:
	ID3D11VertexShader* m_VertexShader{};
	ID3D11PixelShader*  m_PixelShader{};
	ID3D11InputLayout*  m_VertexLayout{};

	class Sprite* m_Sprite{};
	float m_Alpha = 1.0f;
	bool m_FadeOut = false;
	bool m_FadeFinish = false;

public:
	void Init(float x, float y, float sizeX, float sizeY, const char* d);
	void Uninit();
	void Update();
	void Draw();

	void FadeOut() { m_FadeOut = true; }
	bool GetFadeFinish() { return m_FadeFinish; }
};