#pragma once


#include "component.h"
#include <list>

class GameObject
{
protected:
	bool		m_Destroy = false;

	D3DXVECTOR3	m_Position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 m_OldPosition = m_Position;
	D3DXVECTOR3	m_Rotation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3	m_Scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	std::list<Component*>	m_Component;

	//移動ベクトル
	float m_VecX = 0.0f;
	float m_VecY = 0.0f;

public:
	void SetDestroy() { m_Destroy = true; }

	bool Destroy()
	{
		if (m_Destroy)
		{
			Uninit();
			delete this;
			return true;
		}
		else
		{
			return false;
		}
	}

	virtual void SetPosition(D3DXVECTOR3 Position){ m_Position = Position; }
	void SetScale(D3DXVECTOR3 Scale){ m_Scale = Scale; }
	void SetOldPosition(D3DXVECTOR3 pos) { m_OldPosition = pos; }
	void SetVecX(float vecX) { m_VecX = vecX; }
	void SetVecY(float vecY) { m_VecY = vecY; }
	D3DXVECTOR3 GetPosition() { return m_Position; }
	D3DXVECTOR3 GetOldPosition(){ return m_OldPosition; }
	D3DXVECTOR3 GetScale(){ return m_Scale; }

	float GetVecX() { return m_VecX; }
	float GetVecY() { return m_VecY; }



	D3DXVECTOR3 GetForward()//前方向ベクトル取得
	{
		D3DXMATRIX rot;
		D3DXMatrixRotationYawPitchRoll(&rot, m_Rotation.y, m_Rotation.x, m_Rotation.z);

		D3DXVECTOR3 forward;
		forward.x = rot._31;
		forward.y = rot._32;
		forward.z = rot._33;

		return forward;
	}

	D3DXVECTOR3 GetRight()//右方向ベクトル取得
	{
		D3DXMATRIX rot;
		D3DXMatrixRotationYawPitchRoll(&rot, m_Rotation.y, m_Rotation.x, m_Rotation.z);

		D3DXVECTOR3 forward;
		forward.x = rot._11;
		forward.y = rot._12;
		forward.z = rot._13;

		return forward;
	}




	virtual void Init(){}

	virtual void Uninit()
	{
		for (Component* component : m_Component)
		{
			component->Uninit();
			delete component;
		}

		m_Component.clear();//リスト構造の削除
	}

	virtual void Update()
	{
		for (Component* component : m_Component)
		{
			component->Update();
		}
	}

	virtual void Draw()
	{
		for (Component* component : m_Component)
		{
			component->Draw();
		}
	}


	template <typename T> //テンプレート関数
	T* AddComponent()
	{
		Component* component = new T();
		m_Component.push_back(component);
		component->Init();

		return (T*)component;
	}

	template <typename T>
	T* GetComponent()
	{
		for (int i = 0; i < 1; i++)
		{
			for (Component* component : m_Component)//範囲forループ
			{
				if (typeid(*component) == typeid(T))//型を調べる(RTTI動的型情報)
				{
					return (T*)component;//
				}
			}
		}
		return nullptr;
	}

};