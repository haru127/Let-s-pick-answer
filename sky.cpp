#include "main.h"
#include "renderer.h"
#include "manager.h"
#include "scene.h"
#include "camera.h"
#include "sky.h"

void Sky::Init()
{
	m_Scale = D3DXVECTOR3(150.0f, 150.0f, 150.0f);
	m_Model = new Model();
	m_Model->Load("asset\\model\\skydome.obj");

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");
}

void Sky::Uninit()
{
	m_Model->Unload();
	delete m_Model;

	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();
}

void Sky::Update()
{
	Scene* scene = Manager::GetScene();
	Camera* camera = scene->GetGameObject<Camera>();

	m_Position = camera->GetPosition();
	m_Rotation.y = 230.0f;

}

void Sky::Draw()
{
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	//マトリクス設定
	D3DXMATRIX world, scale, rot, trans;
	D3DXMatrixScaling(&scale, m_Scale.x, m_Scale.y, m_Scale.z);
	D3DXMatrixRotationYawPitchRoll(&rot, m_Rotation.y, m_Rotation.x, m_Rotation.z);
	D3DXMatrixTranslation(&trans, m_Position.x, m_Position.y, m_Position.z);
	world = scale * rot * trans;
	Renderer::SetWorldMatrix(&world);

	m_Model->Draw();

}

