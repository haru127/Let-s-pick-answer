#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "scene.h"
#include "player.h"
#include "enemy.h"
#include "collision.h"
#include "hp.h"
#include "enemyBullet.h"
#include "explosion.h"
#include "burst.h"
#include <DirectXMath.h>

Model* EnemyBullet::m_Model{};//static�Đ錾

void EnemyBullet::Load()
{
	m_Model = new Model();
	m_Model->Load("asset\\model\\sphere.obj");
}

void EnemyBullet::Unload()
{
	m_Model->Unload();
	delete m_Model;
}

void EnemyBullet::Init()
{
	m_Scale = D3DXVECTOR3(0.5f, 0.5f, 0.5f);
	Scene* scene = Manager::GetScene();
	Enemy* enemy = scene->GetGameObject<Enemy>();

	m_Velocity = enemy->GetForward();
	if (0.0f < m_Velocity.x)
	{
		m_Velocity = D3DXVECTOR3(m_Velocity.x - 0.5f, 0.0f, 0.0f);
	}
	else
	{
		m_Velocity = D3DXVECTOR3(m_Velocity.x + 0.5f, 0.0f, 0.0f);
	}

	m_Rotation = enemy->GetForward();

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\vertexLightingVS.cso");

	Renderer::CreatePixelShader(&m_PixelShader, "shader\\vertexLightingPS.cso");
}

void EnemyBullet::Uninit()
{
	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();
}

void EnemyBullet::Update()
{
	Scene* scene = Manager::GetScene();

	m_Rotation += D3DXVECTOR3(0.0f, 1.0f, 1.0f);
	m_Position += m_Velocity;

	if (m_InitPosition.x + 20.0f < m_Position.x)
	{
		m_Destroy = true;
	}
	else if (m_Position.x < m_InitPosition.x - 20.0f)
	{
		m_Destroy = true;
	}
	//if (m_Position.z > 20.0f)
	//{
	//	//�e�������\�������@�Q�[���I�u�W�F�N�g�̃��X�g�\����������̂�scene.h
	//	m_Destroy = true;
	//	return;
	//}
	//if (m_Position.z < -20.0f)
	//{
	//	m_Destroy = true;
	//	return;
	//}

	//�G�Ƃ̓����蔻��
	Player* player = scene->GetGameObject<Player>();
	Enemy*  enemy = scene->GetGameObject<Enemy>();
	//std::vector<Enemy*> enemies = scene->GetGameObjects<Enemy>();//�^����auto�ł���

	//for (Enemy* enemy : enemies)//�͈�for���[�v
	{
		D3DXVECTOR3 playerPos  = player->GetPosition();
		D3DXVECTOR3 direction = playerPos - m_Position;
		float length = D3DXVec3Length(&direction);//�G�ƒe�̃x�N�g���̒���

		if (length < 1.3f)
		{   //�������Ă���

			m_Destroy = true;

			if (!player->isGuard())
			{
				player->SetHitSE();

				//�ӂ��Ƃ΂�
				float speed = 0.1f; // ���x
				float Angle = 0.0f;//�p�x
				//�G���E
				if (player->GetPosition().x < enemy->GetPosition().x)
				{
					Angle = 180.0f - 21.0f;
				}
				else
				{
					Angle = 21.0f;
				}

				player->GetHpPointer()->AddCount(7.0f);//�_���[�W���Z

				//�p�[�Z���g�W��
				speed += (player->GetHpPointer()->GetCount() / 2) * 0.1;

				// �Ȑ��I�Ȕ�����^����֐��i��Fsin�j
				float curveFactor = sin(DirectX::XMConvertToRadians(Angle));

				// �Ȑ��I�ȑ��x�̌v�Z
				float curvedSpeed = speed * curveFactor;

				player->SetVecX(curvedSpeed * cos(DirectX::XMConvertToRadians(Angle)));
				player->SetVecY(curvedSpeed * sin(DirectX::XMConvertToRadians(Angle)));
				if (Collision::IsBurst(curvedSpeed * cos(DirectX::XMConvertToRadians(Angle)), curvedSpeed * sin(DirectX::XMConvertToRadians(Angle)),player))
				{
					scene->AddGameObject<Burst>(1)->SetPosition(D3DXVECTOR3(playerPos.x, playerPos.y + 1.0f, playerPos.z));
					Collision::SetHitStop();
				}
				else
				{
					scene->AddGameObject<Explosion>(1)->SetPosition(D3DXVECTOR3(playerPos.x, playerPos.y + 1.0f, playerPos.z));
				}

				player->SetPosition(D3DXVECTOR3(player->GetPosition().x + player->GetVecX(), player->GetPosition().y + player->GetVecY(), player->GetPosition().z));

				//������ђ�
				player->SetCool();
			}
			//�K�[�h��
			else 
			{
				scene->AddGameObject<Explosion>(1)->SetPosition(D3DXVECTOR3(playerPos.x, playerPos.y + 1.0f, playerPos.z));
			}
			return;
		}
	}

}

void EnemyBullet::Draw()
{
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	//�}�g���N�X�ݒ�
	D3DXMATRIX world, scale, rot, trans;
	D3DXMatrixScaling(&scale, m_Scale.x, m_Scale.y, m_Scale.z);
	D3DXMatrixRotationYawPitchRoll(&rot, m_Rotation.y, m_Rotation.x, m_Rotation.z);
	D3DXMatrixTranslation(&trans, m_Position.x, m_Position.y, m_Position.z);
	world = scale * rot * trans;
	Renderer::SetWorldMatrix(&world);

	m_Model->Draw();

}

