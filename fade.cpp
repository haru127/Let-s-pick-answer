#include "main.h"
#include "renderer.h"
#include "fade.h"
#include "sprite.h"

void Fade::Init(float x, float y, float sizeX, float sizeY, const char* d)
{
	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");

	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");

	m_Sprite = AddComponent<Sprite>();
	m_Sprite->Init(x, y, sizeX, sizeY, d);
}

void Fade::Uninit()
{
	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();

	GameObject::Uninit();//GameObjectのを呼ぶ　基底クラスの関数を呼ぶ
}

void Fade::Update()
{
	GameObject::Update();//GameObjectのを呼ぶ　基底クラスの関数を呼ぶ

	if (!m_FadeOut)
	{
		//フェードイン
		m_Alpha -= 0.05f;
		if (m_Alpha < 0.0f)
		{
			m_Alpha = 0.0f;
		}

	}
	else
	{
		//フェードアウト

		m_Alpha += 0.05f;
		if (m_Alpha > 1.0f)
		{
			m_Alpha = 1.0f;
			m_FadeFinish = true;
		}

	}
	m_Sprite->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, m_Alpha));
}

void Fade::Draw()
{
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	Renderer::SetWorldViewProjection2D();

	GameObject::Draw();//GameObjectのDrawを呼ぶ　基底クラスの関数を呼ぶ
}


