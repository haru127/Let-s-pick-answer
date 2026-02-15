#pragma once
#include "model.h"
#include "gameObject.h"

class Capsule : public GameObject
{
private:

	Model* m_Model{};

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;

	//ID3D11VertexShader* m_VertexShader{};
	//ID3D11PixelShader*  m_PixelShader{};
	//ID3D11InputLayout*  m_VertexLayout{};

	ID3D11InputLayout* inputLayout = nullptr;
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11GeometryShader* geometryShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;

	XMFLOAT3 m_Scale{ 1.0f, 1.0f, 1.0f };
	XMFLOAT3 m_Rotation{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 m_Position{ 0.0f, 0.0f, 0.0f };


public:
	Capsule(ID3D11Device * dev, ID3D11DeviceContext * ctx);
	void Init();
	void Uninit();
	void Update();
	void Draw();
};