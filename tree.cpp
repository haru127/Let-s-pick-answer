#include "main.h"
#include "renderer.h"
#include "manager.h"
#include "scene.h"
#include "camera.h"
#include "tree.h"

ID3D11ShaderResourceView* Tree::m_Texture{};//static再宣言
//float g_alfa = 1.0f;

void Tree::Load()
{
	//テクスチャ読み込み
	D3DX11CreateShaderResourceViewFromFile(Renderer::GetDevice(), "asset\\texture\\tree.png", NULL, NULL, &m_Texture, NULL);
	assert(m_Texture);
}

void Tree::Unload()
{
	m_Texture->Release();
}

void Tree::Init()
{
	VERTEX_3D vertex[4];

	vertex[0].Position = D3DXVECTOR3(-3.0f, 5.0f, 0.0f);
	vertex[0].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	vertex[0].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = D3DXVECTOR2(0.0f, 0.0f);

	vertex[1].Position = D3DXVECTOR3(3.0f,5.0f, 0.0f);
	vertex[1].Normal = D3DXVECTOR3(0.0f, 0.0f,-1.0f);
	vertex[1].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = D3DXVECTOR2(1.0f, 0.0f);

	vertex[2].Position = D3DXVECTOR3(-3.0f, 0.0f, 0.0f);
	vertex[2].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	vertex[2].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = D3DXVECTOR2(0.0f, 1.0f);

	vertex[3].Position = D3DXVECTOR3(3.0f, 0.0f, 0.0f);
	vertex[3].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	vertex[3].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = D3DXVECTOR2(1.0f, 1.0f);

	//頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;//頂点バッファを調整しない
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//頂点バッファを調整しない

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = vertex;

	Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");

	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");
}

void Tree::Uninit()
{
	m_VertexBuffer->Release();
	m_Texture->Release();

	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();
}

void Tree::Update()
{
	//if (1.0f == g_alfa)
	//{
	//	g_alfa = 0.0f;
	//}
	//else 
	//{
	//	g_alfa = 1.0f;
	//}

	//for (int i = 0; i < 4; i++)
	//{
	//	vertex[i].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, g_alfa);
	//}
}

void Tree::Draw()
{
	////頂点データ書き換え
	//D3D11_MAPPED_SUBRESOURCE msr;
	//Renderer::GetDeviceContext()->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	//VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	//vertex[0].Position = D3DXVECTOR3(-3.0f, 5.0f, 0.0f);
	//vertex[0].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	//vertex[0].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, g_alfa);
	//vertex[0].TexCoord = D3DXVECTOR2(0.0f, 0.0f);

	//vertex[1].Position = D3DXVECTOR3(3.0f, 5.0f, 0.0f);
	//vertex[1].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	//vertex[1].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, g_alfa);
	//vertex[1].TexCoord = D3DXVECTOR2(1.0f, 0.0f);

	//vertex[2].Position = D3DXVECTOR3(-3.0f, 0.0f, 0.0f);
	//vertex[2].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	//vertex[2].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, g_alfa);
	//vertex[2].TexCoord = D3DXVECTOR2(0.0f, 1.0f);

	//vertex[3].Position = D3DXVECTOR3(3.0f, 0.0f, 0.0f);
	//vertex[3].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	//vertex[3].Diffuse = D3DXVECTOR4(1.0f, 1.0f, 1.0f, g_alfa);
	//vertex[3].TexCoord = D3DXVECTOR2(1.0f, 1.0f);

	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	//カメラのビューマトリックス設定
	Scene* scene   = Manager::GetScene();
	Camera* camera = scene->GetGameObject<Camera>();
	D3DXMATRIX view = camera->GetViewMatrix();

	//ビューの逆行列
	D3DXMATRIX invView;
	D3DXMatrixInverse(&invView, NULL, &view);//逆行列
	invView._41 = 0.0f;
	invView._42 = 0.0f;
	invView._43 = 0.0f;

	//マトリクス設定
	D3DXMATRIX world, scale, rot, trans;
	D3DXMatrixScaling(&scale, m_Scale.x, m_Scale.y, m_Scale.z);
	//D3DXMatrixRotationYawPitchRoll(&rot, m_rotation.y, m_rotation.x, m_rotation.z);
	D3DXMatrixTranslation(&trans, m_Position.x, m_Position.y, m_Position.z);
	world = scale * invView * trans;
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


