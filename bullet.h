#pragma once
#include "model.h"
#include "gameObject.h"

class Bullet : public GameObject//åpè≥
{
private:
	D3DXVECTOR3 m_Velocity{};

	static Model* m_Model;

	ID3D11VertexShader* m_VertexShader{};
	ID3D11PixelShader*  m_PixelShader{};
	ID3D11InputLayout*  m_VertexLayout{};

	D3DXVECTOR3	m_InitPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
public:
	static void Load();
	static void Unload();

	void Init();
	void Uninit();
	void Update();
	void Draw();

	void SetPosition(D3DXVECTOR3 Position) { m_Position = Position, m_InitPosition = Position; }
};