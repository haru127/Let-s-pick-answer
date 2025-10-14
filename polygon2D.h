#pragma once
#include "gameObject.h"

class Polygon2D :public GameObject
{
private:
	class Sprite* m_Sprite{};
	ID3D11VertexShader* m_VertexShader{};
	ID3D11PixelShader*  m_PixelShader{};
	ID3D11InputLayout*  m_VertexLayout{};

	D3DXVECTOR2	m_Size;
public:
	void Init(float x, float y, float sizeX, float sizeY, const char* d);
	void Uninit();
	void Update();
	void Draw();

	void GetOutImage(){}
	void SetImage(const char* d);
	D3DXVECTOR2 GetSize() { return m_Size; }

	void SetColor(D3DXCOLOR color);
};