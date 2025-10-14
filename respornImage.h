#pragma once
#include "gameObject.h"

class RespornImage :public GameObject
{
private:

	ID3D11Buffer* m_VertexBuffer{};
	static ID3D11ShaderResourceView* m_Texture;

	ID3D11VertexShader* m_VertexShader{};
	ID3D11PixelShader*  m_PixelShader{};
	ID3D11InputLayout*  m_VertexLayout{};

	bool m_Resporn{};

	int m_Count{};
	float m_alfa = 0.0f;
public:
	static void Load();
	static void Unload();

	void Init();
	void Uninit();
	void Update();
	void Draw();

	void SetResporn(bool is) { m_Resporn = is; }

	void Hide();
};