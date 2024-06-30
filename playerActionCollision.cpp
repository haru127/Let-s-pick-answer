#include "main.h"
#include "renderer.h"
#include "manager.h"
#include "scene.h"
#include "game.h"
#include "debug.h"
#include "camera.h"
#include "playerActionCollision.h"

ID3D11ShaderResourceView* PlayerActionCollision::m_Texture{};//static再宣言

void PlayerActionCollision::Load()
{
	//テクスチャ読み込み
	D3DX11CreateShaderResourceViewFromFile(Renderer::GetDevice(), "asset\\texture\\blue.png", NULL, NULL, &m_Texture, NULL);
	assert(m_Texture);
}

void PlayerActionCollision::Unload()
{
	m_Texture->Release();
}

void PlayerActionCollision::Init(float x, float y, float sizeX, float sizeY, int zFrame, float angle, float speed, float damage)
{
	Scene* scene = Manager::GetScene();

	VERTEX_3D vertex[4];

	if (!(isSceneGame<Game>(scene)))//デバッグシーンの場合
	{
		vertex[0].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.55f);
		vertex[1].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.55f);
		vertex[2].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.55f);
		vertex[3].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.55f);
	}
	else if (isSceneGame<Game>(scene))//ゲーム
	{
		vertex[0].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.0f);
		vertex[1].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.0f);
		vertex[2].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.0f);
		vertex[3].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.0f);
	}

	vertex[0].Position = D3DXVECTOR3(-sizeX /2, sizeY/2, 0.0f);
	vertex[0].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	//vertex[0].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.55f);
	//vertex[0].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.0f);
	vertex[0].TexCoord = D3DXVECTOR2(0.0f, 0.0f);

	vertex[1].Position = D3DXVECTOR3(sizeX /2, sizeY/2, 0.0f);
	vertex[1].Normal = D3DXVECTOR3(0.0f, 0.0f,-1.0f);
	//vertex[1].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.55f);
	//vertex[1].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.0f);
	vertex[1].TexCoord = D3DXVECTOR2(1.0f, 0.0f);

	vertex[2].Position = D3DXVECTOR3(-sizeX /2, -sizeY/2, 0.0f);
	vertex[2].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	//vertex[2].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.55f);
	//vertex[2].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.0f);
	vertex[2].TexCoord = D3DXVECTOR2(0.0f, 1.0f);

	vertex[3].Position = D3DXVECTOR3(sizeX /2, -sizeY /2, 0.0f);
	vertex[3].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	//vertex[3].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.55f);
	//vertex[3].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.0f);
	vertex[3].TexCoord = D3DXVECTOR2(1.0f, 1.0f);

	//頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;//頂点バッファを調整しない
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;//頂点バッファを調整しない

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = vertex;

	Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");

	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");

	m_size.x = sizeX;
	m_size.y = sizeY;

	m_ZizokuFrame = zFrame;
	m_Angle = angle;
	m_Speed = speed;
	m_Damage = damage;
}

void PlayerActionCollision::Uninit()
{
	GameObject::Uninit();

	m_VertexBuffer->Release();

	//m_Texture->Release(); //Unloadでテクスチャは解放する

	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();
}

void PlayerActionCollision::Update()
{
	GameObject::Update();//GameObjectのを呼ぶ　基底クラスの関数を呼ぶ

	////座標変更があればoldを更新
	//if (m_Position != D3DXVECTOR3(0.0f, 0.0f, 0.0f))
	//{
	//	m_OldPosition = m_Position;
	//}

}

void PlayerActionCollision::Draw()
{
	if (0 < m_ZizokuFrame)//持続
	{
		Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

		Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
		Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

		////カメラのビューマトリックス設定
		//Scene* scene   = Manager::GetScene();
		//Camera* camera = scene->GetGameObject<Camera>();
		//D3DXMATRIX view = camera->GetViewMatrix();

		////ビューの逆行列
		//D3DXMATRIX invView;
		//D3DXMatrixInverse(&invView, NULL, &view);//逆行列
		//invView._41 = 0.0f;
		//invView._42 = 0.0f;
		//invView._43 = 0.0f;

		//マトリクス設定
		D3DXMATRIX world, scale, rot, trans;
		D3DXMatrixScaling(&scale, m_Scale.x, m_Scale.y, m_Scale.z);
		//D3DXMatrixRotationYawPitchRoll(&rot, m_rotation.y, m_rotation.x, m_rotation.z);
		D3DXMatrixTranslation(&trans, m_Position.x, m_Position.y, m_Position.z);
		world = scale * trans;
		Renderer::SetWorldMatrix(&world);

		//頂点バッファ設定
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
		//マテリアル設定
		MATERIAL material;
		ZeroMemory(&material, sizeof(material));
		material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		material.TextureEnable = true;
		Renderer::SetMaterial(material);
		//テクスチャ設定
		Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);
		//プリミティブトポロジー設定
		Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		Renderer::SetATCEnable(true);
		//ポリゴン描画
		Renderer::GetDeviceContext()->Draw(4, 0);

		Renderer::SetATCEnable(false);
	}
}

void PlayerActionCollision::PullZizokuFrame()
{
	if (m_ZizokuFrame != 0)
	{
		m_ZizokuFrame--;
	}

	if (m_ZizokuFrame <= 0)
	{
		m_Destroy = true; //リストから削除
		return;
	}
}

void PlayerActionCollision::DeleteCollision()
{
	m_ZizokuFrame = 0;

	if (m_ZizokuFrame <= 0)
	{
		m_Destroy = true; //リストから削除
		return;
	}

}

D3DXVECTOR3 PlayerActionCollision::GetSize()
{
	return m_size;
}




