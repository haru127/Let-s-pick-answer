#pragma once
#include "component.h"

class Sprite :public Component
{
private:
	ID3D11Buffer* m_VertexBuffer{};
	ID3D11ShaderResourceView* m_Texture{};

	D3DXCOLOR m_Color = D3DXCOLOR(1.0f, 1.0f, 1.0f,1.0f);

	D3DXVECTOR2 m_Position{};
	D3DXVECTOR2	m_Size{};
public:
	void Init(float x, float y, float width, float height, const char* TextureName);
	void Uninit();
	void Update();
	void Draw();

	void SetImage(const char* TexturenName);

	void SetColor(D3DXCOLOR color) { m_Color = color; }
	D3DXVECTOR2 GetSize() { return m_Size; }

};