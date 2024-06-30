#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "camera.h"
#include "player.h"
#include "enemy.h"
#include "collision.h"
#include "scene.h"
#include <math.h>

D3DXVECTOR3 Camera::m_InitPlayerPos{};//静的メンバ変数は再宣言が必要
D3DXVECTOR3 Camera::m_InitEnemyPos{};//静的メンバ変数は再宣言が必要

void Camera::Init()
{
	m_Position = D3DXVECTOR3(0.0f, 5.0f, -10.0f);
	m_Target = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

void Camera::Update()
{
	Scene* scene = Manager::GetScene();
	Player* player = scene->GetGameObject<Player>();
	Enemy*  enemy = scene->GetGameObject<Enemy>();

	m_OldPosition = m_Position;//前フレームの座標を取得
	//カメラの初期値をセット
	if (!m_IsInitCameraPos)
	{
		SetInitPos();
		m_IsInitCameraPos = true;
	}

	
	//カメラをプレイヤーエネミー間の距離に応じて動的に変えていく
	float initDistanceX = fabsf(m_InitPlayerPos.x - m_InitEnemyPos.x);
	float initDistanceY = fabsf(m_InitPlayerPos.y - m_InitEnemyPos.y);

	float distanceX = fabsf(player->GetPosition().x - enemy->GetPosition().x);
	float distanceY = fabsf(player->GetPosition().y - enemy->GetPosition().y);

	float centerPosX = 0;
	if (player->GetPosition().x < enemy->GetPosition().x)
	{
		centerPosX = enemy->GetPosition().x - distanceX / 2;
	}
	else 
	{
		centerPosX = player->GetPosition().x - distanceX / 2;
	}

	float centerPosY = 0;
	if (player->GetPosition().y < enemy->GetPosition().y)
	{
		centerPosY = enemy->GetPosition().y - distanceY / 2;
	}
	else
	{
		centerPosY = player->GetPosition().y - distanceY / 2;
	}

	m_Target = D3DXVECTOR3(centerPosX, centerPosY, player->GetPosition().z - enemy->GetPosition().z) + D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	//m_Position = m_Target
	//	+ D3DXVECTOR3(0.0f, 0.0f, -5.0f);

	//float calculate = fabsf(distance - initDistance);
	if (initDistanceX < distanceX)
	{
		float cal = fabsf(initDistanceX - distanceX) * 0.6f;
		m_Position = m_Target + D3DXVECTOR3(0.0f, 0.0f, -12.0f  - cal);
	}
	else 
	{
		m_Position = m_Target + D3DXVECTOR3(0.0f, 0.0f, -12.0f);
	}

	if (Collision::GetHitStop() != 0)
	{
		//ヒットストップ中はカメラが寄る
		m_Position = m_Target + D3DXVECTOR3(0.0f, 0.0f, -5.0f);
	}
}




void Camera::Draw()
{
	//ビューマトリクス設定
	D3DXVECTOR3 up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&m_ViewMatrix, &m_Position, &m_Target, &up);

	Renderer::SetViewMatrix(&m_ViewMatrix);

	//プロジェクションマトリクス設定
	D3DXMATRIX projectionMatrix;
	D3DXMatrixPerspectiveFovLH(&projectionMatrix, 1.0f,
		(float)SCREEN_WIDTH / SCREEN_HEIGHT, 1.0f, 1000.0f);

	Renderer::SetProjectionMatrix(&projectionMatrix);
}

void Camera::SetInitPos()
{
	Scene* scene = Manager::GetScene();
	Player* player = scene->GetGameObject<Player>();
	Enemy*  enemy = scene->GetGameObject<Enemy>();

	m_Target = player->GetPosition()
		+ D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_Position = m_Target
		+ D3DXVECTOR3(0.0f, 0.0f, -5.0f);

	m_InitCameraPos = m_Position;
}

