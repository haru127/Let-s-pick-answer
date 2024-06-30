#include "main.h"
#include "renderer.h"
#include "polygon2D.h"
#include "sprite.h"

void Polygon2D::Init(float x, float y, float sizeX, float sizeY, const char* d)
{
	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");

	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");

	m_Sprite = AddComponent<Sprite>();
	m_Sprite->Init(x, y, sizeX, sizeY, d);

	m_Position = D3DXVECTOR3(x, y, 0.0f);
	m_Size = D3DXVECTOR2(sizeX,sizeY);
}

void Polygon2D::Uninit()
{
	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();

	GameObject::Uninit();//GameObjectのを呼ぶ　基底クラスの関数を呼ぶ
}

void Polygon2D::Update()
{
	GameObject::Update();//GameObjectのを呼ぶ　基底クラスの関数を呼ぶ
}

void Polygon2D::Draw()
{
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	Renderer::SetWorldViewProjection2D();

	GameObject::Draw();//GameObjectのDrawを呼ぶ　基底クラスの関数を呼ぶ
}

void Polygon2D::SetImage(const char * d)
{
	m_Sprite->SetImage(d);
}

void Polygon2D::SetColor(D3DXCOLOR color)
{
	GetComponent<Sprite>()->SetColor(color);
}


