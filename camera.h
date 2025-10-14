#pragma once

#include "gameObject.h"

class Camera : public GameObject
{

private:

	D3DXVECTOR3	m_Target{};
	D3DXMATRIX m_ViewMatrix{};

	bool m_IsInitCameraPos = false;

	D3DXVECTOR3		   m_InitCameraPos{};
	static D3DXVECTOR3 m_InitPlayerPos;
	static D3DXVECTOR3 m_InitEnemyPos;

public:
	void Init() override;
	void Update() override;
	void Draw() override;

	static void SetInitPlayerPos(D3DXVECTOR3 pos) { m_InitPlayerPos = pos; }
	static void SetInitEnemyPos(D3DXVECTOR3 pos) { m_InitEnemyPos = pos; }
	void SetInitPos();

	D3DXMATRIX GetViewMatrix() { return m_ViewMatrix; }


};