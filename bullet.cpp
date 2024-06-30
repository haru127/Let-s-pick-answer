#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "scene.h"
#include "player.h"
#include "enemy.h"
#include "collision.h"
#include "hp.h"
#include "bullet.h"
#include "explosion.h"
#include "burst.h"
#include <DirectXMath.h>

Model* Bullet::m_Model{};//static�Đ錾

void Bullet::Load()
{
	m_Model = new Model();
	m_Model->Load("asset\\model\\sphere.obj");
}

void Bullet::Unload()
{
	m_Model->Unload();
	delete m_Model;
}

void Bullet::Init()
{
	m_Scale = D3DXVECTOR3(0.5f, 0.5f, 0.5f);
	Scene* scene = Manager::GetScene();
	Player* player = scene->GetGameObject<Player>();

	m_Velocity = player->GetForward();
	if (0.0f < m_Velocity.x)
	{
		m_Velocity = D3DXVECTOR3(m_Velocity.x - 0.5f, 0.0f, 0.0f);
	}
	else
	{
		m_Velocity = D3DXVECTOR3(m_Velocity.x + 0.5f, 0.0f, 0.0f);
	}

	m_Rotation = player->GetForward();

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\vertexLightingVS.cso");

	Renderer::CreatePixelShader(&m_PixelShader, "shader\\vertexLightingPS.cso");
}

void Bullet::Uninit()
{
	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();
}

void Bullet::Update()
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
	std::vector<Enemy*> enemies = scene->GetGameObjects<Enemy>();//�^����auto�ł���

	for (Enemy* enemy : enemies)//�͈�for���[�v
	{
		D3DXVECTOR3 enemyPos  = enemy->GetPosition();
		D3DXVECTOR3 direction = enemyPos - m_Position;
		float length = D3DXVec3Length(&direction);//�G�ƒe�̃x�N�g���̒���

		if (length < 1.3f)
		{   //�������Ă���
			m_Destroy = true;

			if (!enemy->isGuard())
			{
				player->SetHitSE();
			
				//�ӂ��Ƃ΂�
				float speed = 0.1f; // ���x
				float Angle = 0.0f;//�p�x
				if (player->GetPosition().x < enemy->GetPosition().x)
				{
					Angle = 21.0f;
				}
				else
				{
					Angle = 180.0f - 21.0f;
				}

				enemy->GetHpPointer()->AddCount(7.0f);//�_���[�W���Z

				//�p�[�Z���g�W��
				speed += (enemy->GetHpPointer()->GetCount() / 2) * 0.1;

				// �Ȑ��I�Ȕ�����^����֐��i��Fsin�j
				float curveFactor = sin(DirectX::XMConvertToRadians(Angle));

				// �Ȑ��I�ȑ��x�̌v�Z
				float curvedSpeed = speed * curveFactor;

				enemy->SetVecX(curvedSpeed * cos(DirectX::XMConvertToRadians(Angle)));
				enemy->SetVecY(curvedSpeed * sin(DirectX::XMConvertToRadians(Angle)));

				if(Collision::IsBurst(curvedSpeed * cos(DirectX::XMConvertToRadians(Angle)), curvedSpeed * sin(DirectX::XMConvertToRadians(Angle)), enemy))
				{
					scene->AddGameObject<Burst>(1)->SetPosition(D3DXVECTOR3(enemyPos.x, enemyPos.y + 1.0f, enemyPos.z));
					Collision::SetHitStop();
				}
				else
				{
					scene->AddGameObject<Explosion>(1)->SetPosition(D3DXVECTOR3(enemyPos.x, enemyPos.y + 1.0f, enemyPos.z));
				}

				enemy->SetPosition(D3DXVECTOR3(enemy->GetPosition().x + enemy->GetVecX(), enemy->GetPosition().y + enemy->GetVecY(), enemy->GetPosition().z));

				//������ђ�
				enemy->SetCool();
			}
			//�K�[�h��
			else 
			{
				scene->AddGameObject<Explosion>(1)->SetPosition(D3DXVECTOR3(enemyPos.x, enemyPos.y + 1.0f, enemyPos.z));
			}
			return;
		}
	}

}

void Bullet::Draw()
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

