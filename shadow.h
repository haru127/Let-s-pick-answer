#pragma once
#include "component.h"

class Shadow :public Component
{
private:
	D3DXVECTOR3 m_position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 m_rotation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 m_Scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	ID3D11Buffer* m_VertexBuffer{};
	ID3D11ShaderResourceView* m_Texture{};

	ID3D11VertexShader* m_VertexShader{};
	ID3D11PixelShader*  m_PixelShader{};
	ID3D11InputLayout*  m_VertexLayout{};
public:
	void SetPosition(D3DXVECTOR3 pos) 
	{
		m_position = pos;
	}

	void SetScale(D3DXVECTOR3 sca) { m_Scale = sca; }

	void Init();
	void Uninit();
	void Update() {}
	void Draw();
};