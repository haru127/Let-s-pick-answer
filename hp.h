#pragma once
#include "gameObject.h"

class Hp :public GameObject
{
private:
	ID3D11Buffer* m_VertexBuffer{};
	ID3D11ShaderResourceView* m_Texture{};

	ID3D11VertexShader* m_VertexShader{};
	ID3D11PixelShader*  m_PixelShader{};
	ID3D11InputLayout*  m_VertexLayout{};

	//Œ…”•ª‚¾‚¯
	int m_Length = 1;

	int m_HP{};
	float m_x{};
	float m_y{};
public:
	void Init(int hp, float h_x, float h_y);
	void Uninit();
	void Update();
	void Draw();
	void AddCount(int count) { m_HP += count; }
	int GetCount() { return m_HP; }
	void SetHp(int hp) { m_HP = hp; }
};
