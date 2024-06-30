#pragma once
#include "gameObject.h"

class EnemyActionCollision :public GameObject
{
private:

	ID3D11Buffer* m_VertexBuffer{};
	static ID3D11ShaderResourceView* m_Texture;

	ID3D11VertexShader* m_VertexShader{};
	ID3D11PixelShader*  m_PixelShader{};
	ID3D11InputLayout*  m_VertexLayout{};

	D3DXVECTOR3 m_size{};

	int m_Count{};
	int m_ZizokuFrame{};

	float m_Angle{}; // �p�x
	float m_Speed{};
	float m_Damage{};
public:
	static void Load();
	static void Unload();

	void Init(float x, float y, float sizeX, float sizeY, int zFrame, float angle, float speed,float damage);
	void Uninit();
	void Update();
	void Draw();

	void PullZizokuFrame();
	void DeleteCollision();//�U�����肪����������2�񓖂���Ȃ��悤�ɂɎ����������B

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
	float GetAngle() { return m_Angle; }
	float GetSpeed() { return m_Speed; }
	float GetDamage() { return m_Damage; }
};