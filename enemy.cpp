#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "inputx.h"
#include "scene.h"
#include "game.h"
#include "debug.h"
#include "result.h"
#include "enemy.h"
#include "bullet.h"
#include "burst.h"
#include "polygon2D.h"
#include "respornImage.h"
#include "player.h"
#include "enemyBullet.h"
#include "animationModel.h"
#include "meshField.h"
#include "shadow.h"
#include "bodyCollision.h"
#include "enemyName.h"
#include "collision.h"
#include "hp.h"
#include "enemyManager.h"
//#include  "enemyStateMove.h"
//#include "enemyStateAttack.h"
//#include "enemyState.h"
#include "enemyStateDefend.h"
#include "enemyStateNear.h"
#include "enemyStateMiddleNearDis.h"
#include "enemyStateMiddleDis.h"
#include "enemyStateLongDis.h"
#include "enemyActionCollision.h"
#include "sound.h"

int g_count = 39;

int Enemy::m_HitSE[2]{};//�ÓI�����o�ϐ��͍Đ錾���K�v

void Enemy::Init()
{
	m_Model = new AnimationModel();
	m_Model->Load("asset\\model\\Bot.fbx");
	m_Model->LoadAnimation("asset\\model\\Breathing Idle.fbx", "Idle");
	m_Model->LoadAnimation("asset\\model\\Punching01.fbx", "Punch");  //��U��
	m_Model->LoadAnimation("asset\\model\\Head Hit.fbx", "HitAction");//���ꃂ�[�V���� 39F
	m_Model->LoadAnimation("asset\\model\\Standing Walk Forward.fbx", "Walk");//�������[�V����
	m_Model->LoadAnimation("asset\\model\\Guard.fbx", "Guard");//�K�[�h���[�V����
	m_Model->LoadAnimation("asset\\model\\Down.fbx", "Down");//�_�E�����[�V����
	m_Model->LoadAnimation("asset\\model\\Ready Idle.fbx", "Jump");//�W�����v���̃��[�V����
	m_Model->LoadAnimation("asset\\model\\Grab.fbx", "Grab");//���݃��[�V���� 28F
	m_Model->LoadAnimation("asset\\model\\shot.fbx", "Shot");//�V���b�g
	m_Model->LoadAnimation("asset\\model\\Uppercut.fbx", "Upper");  //�㋭
	m_Model->LoadAnimation("asset\\model\\downKick.fbx", "DownTilt");  //����
	m_Model->LoadAnimation("asset\\model\\sideKick.fbx", "ForwardTilt");  //����

	m_AnimationName = "Idle";
	m_NextAnimationName = "Idle";

	m_Scale = D3DXVECTOR3(0.01f, 0.01f, 0.01f);

	m_Rotation.y = -1.5;
	m_Shadow = AddComponent<Shadow>();

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\vertexLightingVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader\\vertexLightingPS.cso");

	//�g�̂̓����蔻��
	m_body = new  BodyCollision();
	m_body->Init(0.45f, 1.75f);

	m_name = new EnemyName();
	m_name->Init(1.0f, 0.8f);

	//�G�l�~�[�̃X�e�[�g�}�V��
	m_EnemyManager = new EnemyManager();
	m_EnemyManager->Init();

	for (int i = 0; i < EnemyAttackType; i++)//������
	{
		m_isAttack[i] = false;
	}

	char filename_0[] = "asset\\SE\\hit000.wav";
	m_HitSE[0] = LoadSound(filename_0);

	char filename_1[] = "asset\\SE\\hit000.wav";
	m_HitSE[1] = LoadSound(filename_1);
}

void Enemy::Uninit()
{
	GameObject::Uninit();

	m_Model->Unload();
	m_body->Uninit();
	delete m_Model;

	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();
}

void Enemy::Update()
{
	if (m_HideTime == 0)
	{
		GameObject::Update();

		m_IsGuard = false;

		Scene* scene = Manager::GetScene();
		Player* player = scene->GetGameObject<Player>();
		EnemyState* nowState = m_EnemyManager->GetState();

		//�O�t���[���̍��W��ۑ����Ă���
		m_OldPosition = m_Position;
		m_body->SetOldPosition(m_body->GetPosition());
		m_name->SetOldPosition(m_body->GetPosition());

		if (m_IsCool == false && m_IsChance == false)
		{
			//�X�e�[�g����
			{
				FaceCorrect();//����̕���������

				float calculate = 0.0f;
				if (player->GetPosition().x >= 0 && m_Position.x >= 0) {
					// ���������̏ꍇ
					calculate = abs(player->GetPosition().x - m_Position.x);
				}
				else if (player->GetPosition().x < 0 && m_Position.x < 0) {
					// ���������̏ꍇ
					calculate = abs(player->GetPosition().x - m_Position.x);
				}
				else
				{
					// �Е������ŕЕ������̏ꍇ
					calculate = abs(player->GetPosition().x) + abs(m_Position.x);
				}

				if (calculate < 0.7f)
				{
					//���݂̃X�e�[�g�ƍX�V����X�e�[�g���Ⴄ�Ȃ�
					if (m_State != StatePatern::NEARDIS)
 					{
						m_EnemyManager->SetState<EnemyStateNear>();
						m_State = StatePatern::NEARDIS;
					}
					// �����ɍ���1�������͈͓��̏ꍇ�Ɏ��s�������R�[�h������
				}
				else if (0.7f <= calculate  && calculate < 3.0f)
				{
					if (m_State != StatePatern::MIDDLENEAR)
					{
						m_EnemyManager->SetState<EnemyStateMiddleNearDis>();
						m_State = StatePatern::MIDDLENEAR;
					}
				}
				else if (3.0 <= calculate  && calculate < 15.0f)
				{
					if (m_State != StatePatern::MIDDLE)
					{
						m_EnemyManager->SetState<EnemyStateMiddleDis>();
						m_State = StatePatern::MIDDLE;
					}
				}
				else
				{
					if (m_State != StatePatern::LONGDIS)
					{
						m_EnemyManager->SetState<EnemyStateLongDis>();
						m_State != StatePatern::LONGDIS;
					}
				}

				//�e�K�[�h����
				std::vector<Bullet*> bulletes = scene->GetGameObjects<Bullet>();//�^����auto�ł���

				for (Bullet* bullet : bulletes)//�͈�for���[�v
				{
					D3DXVECTOR3 bulletPos = bullet->GetPosition();
					D3DXVECTOR3 direction = m_Position - bulletPos;
					float length = D3DXVec3Length(&direction);//�G�ƒe�̃x�N�g���̒���

					if (length < 2.0f)
					{
						//�K�[�h
						m_EnemyManager->SetState<EnemyStateDefend>();
						m_State = StatePatern::DEFEND;
					}
				}

				m_EnemyManager->Update(GetThisPointer());
			}

			//Attack�@push��
			for (int i = 0; i < EnemyAttackType; i++)
			{
				if (m_isAttack[i] == true)
				{
					switch (i)
					{
					case 0:
						if (m_NextAnimationName != "Punch")
						{
							m_AnimationName = m_NextAnimationName;
							m_NextAnimationName = "Punch";
							m_BlendRate = 0.0f;
						}
						break;
					case 1:
						if (m_NextAnimationName != "DownTilt")
						{
							m_AnimationName = m_NextAnimationName;
							m_NextAnimationName = "DownTilt";
							m_BlendRate = 0.0f;
						}
						break;
					case 2:
						if (m_NextAnimationName != "ForwardTilt")
						{
							m_AnimationName = m_NextAnimationName;
							m_NextAnimationName = "ForwardTilt";
							m_BlendRate = 0.0f;
						}
						break;
					case 3:
						if (m_NextAnimationName != "Upper")
						{
							m_AnimationName = m_NextAnimationName;
							m_NextAnimationName = "Upper";
							m_BlendRate = 0.0f;
						}
						break;
					case 4:
						if (m_NextAnimationName != "Grab")
						{
							m_AnimationName = m_NextAnimationName;
							m_NextAnimationName = "Grab";
							m_BlendRate = 0.0f;
						}
						break;
					case 5:
						if (m_NextAnimationName != "Shot")
						{
							m_AnimationName = m_NextAnimationName;
							m_NextAnimationName = "Shot";
							m_BlendRate = 0.0f;
						}
						break;

					}
				}
			}

			//�K�[�h�̋����W�J�t���[�� 3F����
			if (1 <= m_GuardFrame && m_GuardFrame < 3)
			{
				if (m_NextAnimationName != "Guard")
				{
					m_AnimationName = "Guard";
					m_NextAnimationName = "Guard";
					m_BlendRate = 0.0f;
					m_IsGuard = true;
				}
			}

			//�󒆃��[�V����
			if (!m_IsField)
			{
				if (m_NextAnimationName != "Jump")
				{
					m_AnimationName = m_NextAnimationName;
					m_NextAnimationName = "Jump";
					m_BlendRate = 0.0f;
				}
			}

			//�A�C�h��
			if (m_IsMove == false)
			{
				if (m_NextAnimationName != "Idle")
				{
					m_AnimationName = m_NextAnimationName;
					m_NextAnimationName = "Idle";
					m_BlendRate = 0.0f;
				}
			}
		}
		else if(m_IsCool == true) //�N�[���^�C���� 
		{
			for (int i = 0; i < EnemyAttackType; i++)//������
			{
				m_isAttack[i] = false;
			}

			if (m_IsGrabHit)
			{
				m_AnimationName = "Down";
				m_NextAnimationName = "Down";
				m_BlendRate = 0.0f;
			}
			else
			{
				m_AnimationName = "HitAction";
				m_NextAnimationName = "HitAction";
				m_BlendRate = 0.0f;
			}

			if (m_DownTime != 0)
			{
				m_DownTime--;
			}

			//�ӂ��Ƃт����A�_�E�����ԏI���ɌĂ�
			if (m_isPushFinished && m_DownTime == 0)
			{
				//������
				m_IsCool = false;
				m_IsGrabHit = false;
				m_isPushFinished = false;
			}

			if (78 < m_Time)
			{
				//���ꃂ�[�V�����̑S�̃t���[��*2?�𒴂����珉����
				m_AnimationName = "Idle";
				m_NextAnimationName = "Idle";
			}
		} 
		else if (m_IsChance == true)
		{
			m_AnimationName = "Idle";
			m_NextAnimationName = "Idle";
			m_BlendRate = 0.0f;

			if (m_ChanceTime != 0)
			{
				m_ChanceTime--;
			}

			if (m_ChanceTime == 0)
			{
				m_IsChance = false;
			}
		}


		//�d��
		m_Velocity.y -= 0.015f;//�I�C���[�@
		//m_Velocity.y -= 0.005f;//�I�C���[�@

		//�ړ�
		m_Position += m_Velocity;//�I�C���[�@

		//�Փˌ�
		if (m_VecX != 0.0f)
		{
			m_Position.x += m_VecX;
			m_VecX *= 0.9f;

			if ((fabsf(m_VecX) - 0.1f) < 0.0f)
			{
				m_VecX = 0.0f;

				if (m_VecY == 0.0f)
				{
					m_IsCool = false;

					if (m_IsGrabHit)
					{
						m_IsCool = true;
						SetDownTime();
						m_isPushFinished = true;
						//m_IsGrabHit = false;
					}
				}
			}
		}

		if (m_VecY != 0.0f)
		{
			m_Position.y += m_VecY;
			m_VecY *= 0.9f;

			if ((fabsf(m_VecY) - 0.1f) < 0.0f)
			{
				m_VecY = 0.0f;

				if (m_VecX == 0.0f)
				{
					m_IsCool = false;

					if (m_IsGrabHit)
					{
						m_IsCool = true;
						SetDownTime();
						m_isPushFinished = true;
						//m_IsGrabHit = false;
					}
				}
			}

		}

		m_name->SetPosition(D3DXVECTOR3(m_Position.x, m_Position.y + 2.3f, m_Position.z));

		if (!m_IsGuard)
		{
			m_body->SetPosition(D3DXVECTOR3(m_Position.x, m_Position.y + 0.9f, m_Position.z));//�g�̂̃R���W�������X�V
		}
		else
		{
			if (m_FaceIsRight)
			{
				m_body->SetPosition(D3DXVECTOR3(m_Position.x + 0.3f, m_Position.y + 0.9f, m_Position.z));//�g�̂̃R���W�������X�V
			}
			else
			{
				m_body->SetPosition(D3DXVECTOR3(m_Position.x - 0.3f, m_Position.y + 0.9f, m_Position.z));//�g�̂̃R���W�������X�V
			}
		}

		//��Q���Ƃ̏Փ˔���
		float groundHeight = 0.0f;

		MeshField* meshfield = scene->GetGameObject<MeshField>();

		//�ڒn
		if (m_Position.y < groundHeight && m_Velocity.y < 0.0f)
		{
			m_Position.y = groundHeight;
			m_Velocity.y = 0.0f;
			m_IsField = true;
			m_IsMove = false;

			//�W�����v�񐔏�����
			m_isFieldJump = false;
			m_isSkyJump = false;

			if (m_VecY != 0.0f)
			{
				m_VecY *= -1;
			}
		}
		else
		{
			m_IsField = false;
			m_IsMove = true;
		}

		//�e�̈ړ�
		D3DXVECTOR3 shadowPosition = m_Position;
		shadowPosition.y = groundHeight + 0.01f;
		m_Shadow->SetPosition(shadowPosition);

		//���E�n�`�F�b�N
		if (50.0f < m_Position.x || m_Position.x < -50.0f)
		{
			PlaySound(m_HitSE[1], 0);//;//����SE
			m_VecX = 0.0f;
			m_VecY = 0.0f;
			m_IsCool = false;
			m_IsGrabHit = false;
			m_isPushFinished = false;
			m_EnemyManager->Init();

			m_HideTime = 120;
			m_isReBorn = true;

			//������
			m_Hp->SetHp(0);
			m_Position = D3DXVECTOR3(1.0f, 0.0f, -4.0f);
			m_OldPosition = m_Position;
			m_body->SetPosition(D3DXVECTOR3(m_Position.x, m_Position.y + 0.9f, m_Position.z));//�g�̂̃R���W�������X�V
			m_body->SetOldPosition(m_body->GetPosition());
			m_name->SetPosition(D3DXVECTOR3(m_Position.x, m_Position.y + 0.9f, m_Position.z));//�g�̂̃R���W�������X�V
			m_name->SetOldPosition(m_body->GetPosition());

			//�X�g�b�N����
			for (int i = 2; 0 <= i; i--)
			{
				//���̂�����
				if (m_Stock[i] != nullptr)
				{
					m_Stock[i]->SetDestroy();
					m_Stock[i] = nullptr;

					if (i == 0)
					{
						//���U���g����
						Result::SetResult(true);//����
						Manager::SetScene<Result>();//���U���g��
					}
					return;
				}
			}

		}
	}

}

void Enemy::Draw()
{
	if (m_HideTime == 0)
	{
		if (m_isReBorn)
		{
			m_isReBorn = false;
			m_AnimationName = "Idle";
			m_NextAnimationName = "Idle";
		}

		GameObject::Draw();
		m_body->Draw();
		m_name->Draw();
		Scene* scene = Manager::GetScene();

		// ���̓��C�A�E�g�ݒ�
		Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

		// �V�F�[�_�ݒ�
		Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
		Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

		// �}�g���N�X�ݒ�
		D3DXMATRIX matrix, scale, rot, trans;
		D3DXMatrixScaling(&scale, m_Scale.x, m_Scale.y, m_Scale.z);
		D3DXMatrixRotationYawPitchRoll(&rot, m_Rotation.y, m_Rotation.x, m_Rotation.z);
		D3DXMatrixTranslation(&trans, m_Position.x, m_Position.y, m_Position.z);
		matrix = scale * rot * trans;
		m_Matrix = matrix;

		Renderer::SetWorldMatrix(&matrix);

		m_Model->Update(m_AnimationName.c_str(), m_Time, m_NextAnimationName.c_str(), m_Time, m_BlendRate);
		m_Model->Draw();

		//��
		if (m_AnimationName == "Punch")
		{
			//�R���W����
			if (m_AnimationFrame == 13 && m_isAttack[0] == true)
			{
				//float angle, float speed, float damage, float distX, float distY
				//AddCollid(m_Position.x, m_Position.y, 0.7f, 0.2f, 2, m_FaceIsRight, 21.0f, 3.0f, 9.0f, 0.7f, 1.2f);
				AddCollid(m_Position.x, m_Position.y, 0.7f, 0.2f, 2, m_FaceIsRight, 21.0f, 0.2f, 9.0f, 0.7f, 1.2f);
			}

			if (m_AnimationFrame == 25)
			{
				m_isAttack[0] = false;
				m_AnimationFrame = 0;
			}
		}

		//��
		if (m_AnimationName == "DownTilt")
		{
			//�R���W����
			if (m_AnimationFrame == 20 && m_isAttack[1] == true)
			{
				AddCollid(m_Position.x, m_Position.y, 0.7f, 0.5f, 3, m_FaceIsRight, 45.0f, 0.1f, 7.0f, 0.8f, 0.95f);
			}

			if (m_AnimationFrame == 26)
			{
				m_isAttack[1] = false;
				m_AnimationFrame = 0;
			}
		}

		//��
		if (m_AnimationName == "ForwardTilt")
		{
			//�R���W����
			if (m_AnimationFrame == 20 && m_isAttack[2] == true)
			{
				AddCollid(m_Position.x, m_Position.y, 0.4f, 1.0f, 2, m_FaceIsRight, 25.0f, 0.1f, 7.0f, 1.5f, 1.25f);
			}

			if (m_AnimationFrame == 35)
			{
				m_isAttack[2] = false;
				m_AnimationFrame = 0;
			}
		}

		//��
		if (m_AnimationName == "Upper")
		{
			//�R���W����
			if (m_AnimationFrame == 15 && m_isAttack[3] == true)
			{
				AddCollid(m_Position.x, m_Position.y, 0.4f, 1.5f, 2, m_FaceIsRight, 60.0f, 0.005f, 7.0f, 0.5f, 0.95f);
			}

			if (m_AnimationFrame == 28)
			{
				m_isAttack[3] = false;
				m_AnimationFrame = 0;
			}
		}

		//����
		if (m_AnimationName == "Grab")
		{
			//�R���W����
			if (m_AnimationFrame == 5 && m_isAttack[4] == true)
			{
				AddCollid(m_Position.x, m_Position.y, 0.4f, 1.5f, 2, m_FaceIsRight, 0.0f, 0.0f, 7.0f, 0.5f, 0.95f);
			}

			if (m_AnimationFrame == 50)
			{
				m_isAttack[4] = false;
				m_AnimationFrame = 0;
			}
		}

		//Shot
		if (m_AnimationName == "Shot")
		{
			//�R���W����
			if (m_AnimationFrame == 5 && m_isAttack[5] == true)
			{
				if (m_FaceIsRight)
				{
					scene->AddGameObject<EnemyBullet>(1)->SetPosition(D3DXVECTOR3(m_Position.x + 0.3f, m_Position.y + 1.2f, m_Position.z));
				}
				else
				{
					scene->AddGameObject<EnemyBullet>(1)->SetPosition(D3DXVECTOR3(m_Position.x - 0.3f, m_Position.y + 1.2f, m_Position.z));
				}
			}

			if (m_AnimationFrame == 15)
			{
				m_isAttack[5] = false;
				m_AnimationFrame = 0;
			}
		}

		//�X�V����
		if (Game::GetIsPause() == false && Debug::GetIsFrameMode() == false && Debug::GetIsPause() == false && Collision::GetHitStop() == 0)
		{
			m_Time++;
			if (isAttack())
			{
				m_AnimationFrame++;
			}

			if (m_AnimationName == "Guard")
			{
				//�K�[�h�̋����W�J�t���[�� 3F����
				m_GuardFrame++;
			}

			m_BlendRate += 0.05f;
			if (m_BlendRate > 1.0f)
				m_BlendRate = 1.0f;
		}

		if (Debug::GetIsFrameMode() == true)
		{
			if (GetKeyboardTrigger(DIK_F) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_DPAD_RIGHT))
			{
				m_Time++;
				if (isAttack())
				{
					m_AnimationFrame++;
				}

				if (m_AnimationName == "Guard")
				{
					//�K�[�h�̋����W�J�t���[�� 3F����
					m_GuardFrame++;
				}

				m_BlendRate += 0.05f;
				if (m_BlendRate > 1.0f)
					m_BlendRate = 1.0f;
			}
		}

		//�K�[�h�d������
		if (m_IsGuard == false && 3 <= m_GuardFrame)
		{
			m_AnimationName = "Idle";
			m_NextAnimationName = "Idle";
			m_BlendRate = 0.0f;

			m_IsChance = true;
			SetChanceTime();//�K�[�h�����̍d���t���[��11F
			m_GuardFrame = 0;
		}

	}
	else
	{
		//m_HideTime--;
	}

}

//void Enemy::InitState()
//{
//	SetState<EnemyStateAttack>();
//}
//
//void Enemy::UninitState()
//{
//	delete m_State;
//}
//
//void Enemy::UpdateState()
//{
//	if (m_NextState)
//	{
//		if (m_State)
//		{
//			m_State->Uninit();
//			delete m_State;
//		}
//
//		m_State = m_NextState;
//		m_State->Init();
//
//		m_NextState = nullptr;
//	}
//
//	m_State->Update();
//
//}

void Enemy::FaceCorrect()
{
	Scene* scene = Manager::GetScene();

	Enemy* enemy = scene->GetGameObject<Enemy>();
	Player* player = scene->GetGameObject<Player>();

	if (enemy->GetPosition().x < player->GetPosition().x)
	{
		//�E
		if (m_IsField == true && (isAttack() == false))
		{
			m_Rotation.y = 1.5f;//���n���Ă����甽�]�o����
			m_FaceIsRight = true;
		}
	}
	else 
	{
		//��
		if (m_IsField == true && (isAttack() == false))
		{
			m_Rotation.y = -1.5f;//���n���Ă����甽�]�o����
			m_FaceIsRight = false;
		}

	}
}

void Enemy::At_Punch()
{
	//��
	if ((isAttack() == false))
	{
		if (m_IsField)
		{
			if (m_NextAnimationName != "Punch")
			{
				m_isAttack[0] = true;
				m_AnimationName = m_NextAnimationName;
				m_NextAnimationName = "Punch";
				m_BlendRate = 0.0f;
				m_AnimationFrame = 0;
				m_Time = 0;
			}
		}
	}

}

void Enemy::At_Down()
{
	//��
	if ((isAttack() == false))
	{
		if (m_IsField)
		{
			if (m_NextAnimationName != "DownTilt")
			{
				m_isAttack[1] = true;
				m_AnimationName = m_NextAnimationName;
				m_NextAnimationName = "DownTilt";
				m_BlendRate = 0.0f;
				m_AnimationFrame = 0;
				m_Time = 0;
			}
		}
	}
}

void Enemy::At_Forward()
{
	//��
	if ((isAttack() == false))
	{
		if (m_IsField)
		{
			if (m_NextAnimationName != "ForwardTilt")
			{
				m_isAttack[2] = true;
				m_AnimationName = m_NextAnimationName;
				m_NextAnimationName = "ForwardTilt";
				m_BlendRate = 0.0f;
				m_AnimationFrame = 0;
				m_Time = 0;
			}
		}
	}
}

void Enemy::At_Upper()
{
	//��
	if ((isAttack() == false))
	{
		if (m_IsField)
		{
			if (m_NextAnimationName != "Upper")
			{
				m_isAttack[3] = true;
				m_AnimationName = m_NextAnimationName;
				m_NextAnimationName = "Upper";
				m_BlendRate = 0.0f;
				m_AnimationFrame = 0;
				m_Time = 0;
			}
		}
	}
}

void Enemy::At_Grab()
{
	if ((isAttack() == false))
	{
		if (m_IsField)
		{
			if (m_NextAnimationName != "Grab")
			{
				m_isAttack[4] = true;
				m_AnimationName = m_NextAnimationName;
				m_NextAnimationName = "Grab";
				m_BlendRate = 0.0f;
				m_AnimationFrame = 0;
				m_Time = 0;
			}
		}
	}
}

void Enemy::At_Shot()
{
	if ((isAttack() == false) && m_IsGuard == false)
	{
		if (m_NextAnimationName != "Shot")
		{
			m_isAttack[5] = true;
			m_AnimationName = m_NextAnimationName;
			m_NextAnimationName = "Shot";
			m_BlendRate = 0.0f;
			m_AnimationFrame = 0;
			m_Time = 0;
		}
	}

}

void Enemy::Mo_MoveR()
{
	if ((isAttack() == false))
	{
		if (m_IsField)
		{
			m_Rotation.y = 1.5f;//���n���Ă����甽�]�o����
			m_FaceIsRight = true;
		}

		if (m_FaceIsRight)
		{
			m_Position.x += GetForward().x * 0.2f;
		}
		else {
			m_Position.x -= GetForward().x * 0.2f;
		}

		m_IsMove = true;

		if (m_IsField)
		{
			if (m_NextAnimationName != "Walk")
			{
				m_AnimationName = m_NextAnimationName;
				m_NextAnimationName = "Walk";
				m_BlendRate = 0.0f;
			}
		}
	}

}

void Enemy::Mo_MoveL()
{
	if ((isAttack() == false))
	{
		if (m_IsField)
		{
			m_Rotation.y = -1.5f;//���n���Ă����甽�]�o����
			m_FaceIsRight = false;
		}


		if (m_FaceIsRight)
		{
			m_Position.x -= GetForward().x * 0.2f;
		}
		else {
			m_Position.x += GetForward().x * 0.2f;
		}

		m_IsMove = true;

		if (m_IsField)
		{
			if (m_NextAnimationName != "Walk")
			{
				m_AnimationName = m_NextAnimationName;
				m_NextAnimationName = "Walk";
				m_BlendRate = 0.0f;
			}
		}
	}
}

void Enemy::Ac_Jump()
{
	if ((isAttack() == false) && (m_isFieldJump == false || m_isSkyJump == false))
	{
		if (m_IsField)
		{
			m_isFieldJump = true;
		}
		else
		{
			m_isSkyJump = true;
		}
		m_IsMove = true;
		m_IsField = false;

		m_Velocity.y = 0.25f;
	}

}

void Enemy::De_Guard()
{
	//�K�[�h
	if (isAttack() == false)
	{
		if (m_IsField)
		{
			if (m_IsGuard == false)//�����n��
			{
				m_GuardFrame = 0;//������
			}

			if (m_NextAnimationName != "Guard")
			{
				m_AnimationName = "Guard";
				m_NextAnimationName = "Guard";
				m_BlendRate = 0.0f;
				m_IsGuard = true;
			}
		}
	}
}

void Enemy::AddCollid(float x, float y, float sizeX, float sizeY, int zFrame, bool isRight, float angle, float speed, float damage, float distX, float distY)
{
	Scene* scene = Manager::GetScene();

	if (!isAttackCollid())
	{
		EnemyActionCollision* kari = scene->AddGameObjectNonInit<EnemyActionCollision>(1);

		//�R���W�����f�B�X�^���X
		m_DistancePlayerCollisionX = distX;
		m_DistancePlayerCollisionY = distY;

		//���E�Ή�
		if (isRight)
		{
			kari->Init(x, y, sizeX, sizeY, zFrame, angle, speed, damage);
			kari->SetPosition(D3DXVECTOR3(m_Position.x + m_DistancePlayerCollisionX, m_Position.y + m_DistancePlayerCollisionY, m_Position.z));
		}
		else
		{
			kari->Init(x, y, sizeX, sizeY, zFrame, 180.0f - angle, speed, damage);
			kari->SetPosition(D3DXVECTOR3(m_Position.x - m_DistancePlayerCollisionX, m_Position.y + m_DistancePlayerCollisionY, m_Position.z));
		}

	}
	else
	{
		return;
	}

}

bool Enemy::isAttack()
{
	for (int i = 0; i < EnemyAttackType; i++)
	{
		if (m_isAttack[i] == true)
		{
			return true;
		}
	}
	return false;
}

bool Enemy::isAttackCollid()
{
	Scene* scene = Manager::GetScene();
	if (scene->GetGameObject<EnemyActionCollision>() != NULL)
	{
		return true;
	}

	return false;
}

BodyCollision * Enemy::GetBodyCollision()
{
	return m_body;
}

int Enemy::GetAttackType()
{
	for (int i = 0; i < EnemyAttackType; i++)
	{
		if (m_isAttack[i] == true)
		{
			return i;
		}
	}
	return -1;//�U�����Ă��Ȃ�

}

