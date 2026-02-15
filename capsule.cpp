#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#include "main.h"
#include "renderer.h"
#include "capsule.h"
using namespace DirectX;

Capsule::Capsule(ID3D11Device* dev, ID3D11DeviceContext* ctx)
{
	device = dev;
	context = ctx;
}

void Capsule::Init()
{
	m_Model = new Model();
	m_Model->Load("asset\\model\\newCapsule.obj");

	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* gsBlob = nullptr;
	ID3DBlob* psBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr;

	// ===== Vertex Shader =====
	hr = D3DCompileFromFile(L"wireframe.hlsl", nullptr, nullptr,
		"VSMain", "vs_5_0", 0, 0, &vsBlob, &errorBlob);
	if (FAILED(hr)) { if (errorBlob) { OutputDebugStringA((char*)errorBlob->GetBufferPointer()); errorBlob->Release(); } return; }
	device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);

	// ===== Input Layout =====
	D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	device->CreateInputLayout(layoutDesc, 1, vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(), &inputLayout);
	vsBlob->Release();

	// ===== Geometry Shader =====
	hr = D3DCompileFromFile(L"wireframe.hlsl", nullptr, nullptr,
		"GSMain", "gs_5_0", 0, 0, &gsBlob, &errorBlob);
	if (FAILED(hr)) { if (errorBlob) { OutputDebugStringA((char*)errorBlob->GetBufferPointer()); errorBlob->Release(); } return; }
	device->CreateGeometryShader(gsBlob->GetBufferPointer(), gsBlob->GetBufferSize(), nullptr, &geometryShader);
	gsBlob->Release();

	// ===== Pixel Shader =====
	hr = D3DCompileFromFile(L"wireframe.hlsl", nullptr, nullptr,
		"PSMain", "ps_5_0", 0, 0, &psBlob, &errorBlob);
	if (FAILED(hr)) { if (errorBlob) { OutputDebugStringA((char*)errorBlob->GetBufferPointer()); errorBlob->Release(); } return; }
	device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);
	psBlob->Release();

	//Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\vertexLightingVS.cso");

	//Renderer::CreatePixelShader(&m_PixelShader, "shader\\vertexLightingPS.cso");
}

void Capsule::Uninit()
{
	//m_Model->Unload();
	//delete m_Model;

	//m_VertexLayout->Release();
	//m_VertexShader->Release();
	//m_PixelShader->Release();

	if (vertexShader) vertexShader->Release();
	if (geometryShader) geometryShader->Release();
	if (pixelShader) pixelShader->Release();
	if (inputLayout) inputLayout->Release();
	if (m_Model) delete m_Model;
}

void Capsule::Update()
{
}

void Capsule::Draw()
{
	// ===== Input Layout & Shader 設定 =====
	context->IASetInputLayout(inputLayout);
	context->VSSetShader(vertexShader, nullptr, 0);
	context->GSSetShader(geometryShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);

	// ===== ワールド行列作成 =====
	XMMATRIX scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
	XMMATRIX rot = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
	XMMATRIX trans = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
	XMMATRIX world = scale * rot * trans;

	XMFLOAT4X4 worldFloat;
	XMStoreFloat4x4(&worldFloat, world);

	Renderer::SetWorldMatrix(&worldFloat);

	// ===== モデル描画 =====
	m_Model->Draw();
}

