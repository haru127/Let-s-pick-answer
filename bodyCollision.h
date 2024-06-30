#pragma once
#include "gameObject.h"

class BodyCollision :public GameObject
{
private:

	ID3D11Buffer* m_VertexBuffer{};
	static ID3D11ShaderResourceView* m_Texture;

	ID3D11VertexShader* m_VertexShader{};
	ID3D11PixelShader*  m_PixelShader{};
	ID3D11InputLayout*  m_VertexLayout{};

	D3DXVECTOR3 m_size{};

	int m_Count{};
public:
	static void Load();
	static void Unload();

	void Init(float x, float y);
	void Uninit();
	void Update();
	void Draw();

	template <typename T>
	bool isSceneGame(Scene* scene)
	{
		if (typeid(*scene) == typeid(T))
		{
			return true;
		}
		else {
			return false;
		}
	}


	D3DXVECTOR3 GetSize();
};