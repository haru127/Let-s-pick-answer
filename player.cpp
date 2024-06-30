#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "inputx.h"
#include "scene.h"
#include "game.h"
#include "debug.h"
#include "result.h"
#include "gameObject.h"
#include "player.h"
#include "enemy.h"
#include "bullet.h"
#include "burst.h"
#include "polygon2D.h"
#include "respornImage.h"
#include "hp.h"
#include "animationModel.h"
#include "meshField.h"
#include "shadow.h"
#include "bodyCollision.h"
#include "name.h"
#include "collision.h"
#include "playerActionCollision.h"
#include <math.h>
#include "sound.h"

int Player::m_HitSE[2]{};//�ÓI�����o�ϐ��͍Đ錾���K�v

void Player::Init()
{
	m_Model = new AnimationModel();
	m_Model->Load("asset\\model\\Bot.fbx");

	m_Model->LoadAnimation("asset\\model\\Breathing Idle.fbx", "Idle");
	m_Model->LoadAnimation("asset\\model\\Punching01.fbx", "Punch");  //��U��
	m_Model->LoadAnimation("asset\\model\\Uppercut.fbx", "Upper");  //�㋭
	m_Model->LoadAnimation("asset\\model\\downKick.fbx", "DownTilt");  //����
	m_Model->LoadAnimation("asset\\model\\sideKick.fbx", "ForwardTilt");  //����
	m_Model->LoadAnimation("asset\\model\\Standing Walk Forward.fbx", "Walk");//�������[�V����
	m_Model->LoadAnimation("asset\\model\\Ready Idle.fbx", "Jump");//�W�����v���̃��[�V����
	m_Model->LoadAnimation("asset\\model\\Guard.fbx", "Guard");//�K�[�h���[�V����
	m_Model->LoadAnimation("asset\\model\\Down.fbx", "Down");//�_�E�����[�V����
	m_Model->LoadAnimation("asset\\model\\Head Hit.fbx", "HitAction");//���ꃂ�[�V���� 39F
	m_Model->LoadAnimation("asset\\model\\Grab.fbx", "Grab");//���݃��[�V���� 28F
	m_Model->LoadAnimation("asset\\model\\shot.fbx", "Shot");//
	
	m_AnimationName = "Idle";
	m_NextAnimationName = "Idle";

	m_Scale = D3DXVECTOR3(0.01f, 0.01f, 0.01f);

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\vertexLightingVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader\\vertexLightingPS.cso");

	m_Rotation.y = 1.5f;
	m_Shadow = AddComponent<Shadow>();

	//�g�̂̓����蔻��
	m_body = new  BodyCollision();
	m_body->Init(0.45f,1.75f);

	m_name = new Name();
	m_name->Init(1.45f, 1.0f);

	for (int i = 0; i < PlayerAttackType; i++)//������
	{
		m_isAttack[i] = false;
	}

	//m_backGround = AddGameObjectNonInit<Polygon2D>(2);//"asset\\texture\\field002.jpg"
	//m_backGround->Init(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, m_SelectImage[0][0]);

	char filename_1[] = "asset\\SE\\hit000.wav";
	m_HitSE[0] = LoadSound(filename_1);

	char filename_2[] = "asset\\SE\\hit000.wav";
	m_HitSE[1] = LoadSound(filename_2);
}

void Player::Uninit()
{
	GameObject::Uninit();

	m_Model->Unload();
	m_body->Uninit();
	m_name->Uninit();
	delete m_Model;

	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();
}


void Player::Update()
{
	if (m_HideTime == 0)
	{
		GameObject::Update();

		float groundHeight = 0.0f;

		Scene* scene = Manager::GetScene();

		//Old
		m_OldPosition = m_Position;
		m_body->SetOldPosition(m_body->GetPosition());
		m_name->SetOldPosition(m_body->GetPosition());

		//bool m_Move = false;

		if (m_IsCool == false && m_IsChance == false)
		{
			//�T�[�h�p�[�\���r���[
			if ((GetKeyboardPress(DIK_A) || GetThumbLeftX(0) < 0.0f) && (isAttack() == false) && (isGuard() == false))//�ړ�
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

				m_Move = true;

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
			if ((GetKeyboardPress(DIK_D) || GetThumbLeftX(0) > 0.0f) && (isAttack() == false) && (isGuard() == false))//�ړ�
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

				m_Move = true;

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

			//��
			if (((GetKeyboardTrigger(DIK_P)) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_B)) && (isAttack() == false))
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

			//��
			if (((GetKeyboardTrigger(DIK_M)) || GetThumbRightY(0) < 0.0f) && (isAttack() == false))
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

			//��
			if (((GetKeyboardTrigger(DIK_K) || 0.0f < GetThumbRightX(0))) && (isAttack() == false))
			{
				if (m_IsField)
				{
					//�E����
					m_Rotation.y = 1.5f;//���n���Ă����甽�]�o����
					m_FaceIsRight = true;

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
			else if (((GetKeyboardTrigger(DIK_K) || GetThumbRightX(0) < 0.0f)) && (isAttack() == false))
			{
				if (m_IsField)
				{
					//������
					m_Rotation.y = -1.5f;//���n���Ă����甽�]�o����
					m_FaceIsRight = false;

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

			//��
			if (((GetKeyboardTrigger(DIK_L) || 0.0f < GetThumbRightY(0))) && (isAttack() == false))
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

			//����
			if (((GetKeyboardTrigger(DIK_O) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_LEFT_SHOULDER) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_RIGHT_SHOULDER))) && (isAttack() == false))
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

			//�V���b�g
			if (((GetKeyboardTrigger(DIK_I)) || (IsButtonTriggeredX(0, XINPUT_GAMEPAD_A))) && (isAttack() == false))
			{
				//if (m_IsField)
				if(m_IsGuard == false)
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

			//Attack�@push��
			for (int i = 0; i < PlayerAttackType; i++)
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

			//�W�����v
			if ((GetKeyboardTrigger(DIK_J) || (0 < GetRightTrigger(0) && m_isRightTriggerOK == false) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_X) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_Y)) && (isAttack() == false) && (m_isFieldJump == false || m_isSkyJump == false))
			{
				if (m_IsField)
				{
					m_isFieldJump = true;
					m_isRightTriggerOK = true;
				}
				else
				{
					m_isSkyJump = true;
				}
				m_Move = true;
				m_IsField = false;

				m_Velocity.y = 0.25f;
			}

			//ZR�����ǂ������ǂ���
			if (0 == GetRightTrigger(0))
			{
				m_isRightTriggerOK = false;
			}

			//�K�[�h
			if (((GetKeyboardPress(DIK_G) || 0 < GetLeftTrigger(0))) && (isAttack() == false))
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
			else
			{
				m_IsGuard = false;
			}

			//�K�[�h�̋����W�J�t���[�� 3F����
			if(1 <= m_GuardFrame && m_GuardFrame < 3)
			{
				if (m_NextAnimationName != "Guard")
				{
					m_AnimationName = "Guard";
					m_NextAnimationName = "Guard";
					m_BlendRate = 0.0f;
					m_IsGuard = true;
				}
			}

			if (!m_IsField)
			{
				if (m_NextAnimationName != "Jump")
				{
					m_AnimationName = m_NextAnimationName;
					m_NextAnimationName = "Jump";
					m_BlendRate = 0.0f;
				}
			}

			if (m_Move == false)
			{
				if (m_NextAnimationName != "Idle")
				{
					m_AnimationName = m_NextAnimationName;
					m_NextAnimationName = "Idle";
					m_BlendRate = 0.0f;
				}
			}

			//if (Input::GetKeyTrigger('R'))
			//{
			//	Enemy* enemy = scene->GetGameObject<Enemy>();
			//	enemy->GetHpPointer()->SetHp(0);
			//}

			////�e����
			//if (Input::GetKeyTrigger(VK_SPACE))
			//{
			//	scene->AddGameObject<Bullet>(1)->SetPosition(D3DXVECTOR3(m_Position.x, m_Position.y + 0.9f, m_Position.z));
			//}

		}
		else if(m_IsCool == true) //�N�[���^�C���� 
		{
			//������
			for (int i = 0; i < PlayerAttackType; i++)
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

			if(m_ChanceTime != 0)
			{
				m_ChanceTime--;
			}

			if (m_ChanceTime == 0)
			{
				m_IsChance = false;
			}
		}
		//if ((Input::GetKeyPress('G')) && ((Input::GetKeyPress('A')) || (Input::GetKeyPress('D'))))
		//{
		//	if (m_NextAnimationName != "Guard")
		//	{
		//		m_AnimationName = m_NextAnimationName;
		//		m_NextAnimationName = "Guard";
		//		m_BlendRate = 0.0f;
		//		m_IsGuard = true;
		//	}
		//}

		//�d��
		m_Velocity.y -= 0.015f;//�I�C���[�@

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

		//�U������̍X�V
		PlayerActionCollision* kari = scene->GetGameObject<PlayerActionCollision>();

		////�R���W�����������Ă��ADistance������Ȃ�ړ������Update��
		//if ((kari != nullptr) && (m_DistancePlayerCollisionX != 0.0) || m_DistancePlayerCollisionY != 0.0)
		//{
		//	//���E�Ή�
		//	if (m_FaceIsRight)
		//	{
		//		kari->SetPosition(D3DXVECTOR3(m_Position.x + m_DistancePlayerCollisionX, m_Position.y + m_DistancePlayerCollisionY, m_Position.z));
		//	}
		//	else
		//	{
		//		kari->SetPosition(D3DXVECTOR3(m_Position.x - m_DistancePlayerCollisionX, m_Position.y + m_DistancePlayerCollisionY, m_Position.z));
		//	}
		//}

		//��Q���Ƃ̏Փ˔���
		//float groundHeight = 0.0f;

		MeshField* meshfield = scene->GetGameObject<MeshField>();

		////box
		//Enemy* enemy = scene->GetGameObject<Enemy>();//�^����auto�ł���
		//{
		//	D3DXVECTOR3 position = enemy->GetPosition();
		//	D3DXVECTOR3 scale = enemy->GetScale();

		//	if (position.x - scale.x - 0.3f < m_Position.x &&
		//		m_Position.x < position.x + scale.x + 0.3f &&
		//		position.z - scale.z - 0.3f < m_Position.z &&
		//		m_Position.z < position.z + scale.z + 0.3f)
		//	{
		//		if (m_Position.y < position.y + scale.y * 2.0f - 0.01f)
		//		{
		//			m_Position.x = m_OldPosition.x;
		//			m_Position.z = m_OldPosition.z;
		//		}
		//		else {
		//			groundHeight = position.y + scale.y * 2.0f;
		//		}
		//	}
		//}

		//�ڒn
		if (m_Position.y < groundHeight && m_Velocity.y < 0.0f)
		{
			m_Position.y = groundHeight;
			m_Velocity.y = 0.0f;
			m_IsField = true;
			m_Move = false;
			//�W�����v�񐔁@������
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
			m_Move = true;
		}

		//�e�̈ړ�
		D3DXVECTOR3 shadowPosition = m_Position;
		shadowPosition.y = groundHeight + 0.01f;
		m_Shadow->SetPosition(shadowPosition);

		//���E�l�`�F�b�N
		if (50.0f < m_Position.x || m_Position.x < -50.0f)
		{
			PlaySound(m_HitSE[1], 0);//;//����SE
			m_VecX = 0.0f;
			m_VecY = 0.0f;
			m_IsCool = false;
			m_IsGrabHit = false;
			m_isPushFinished = false;

			m_HideTime = 120;
			m_isReBorn = true;

			//������
			m_Hp->SetHp(0);
			m_Position = D3DXVECTOR3(-1.0f, 0.0f, -4.0f);
			m_OldPosition = m_Position;
			m_body->SetPosition(D3DXVECTOR3(m_Position.x, m_Position.y + 0.9f, m_Position.z));//�g�̂̃R���W�������X�V
			m_name->SetPosition(D3DXVECTOR3(m_Position.x, m_Position.y + 0.9f, m_Position.z));//�g�̂̃R���W�������X�V
			m_body->SetOldPosition(m_body->GetPosition());
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
						Result::SetResult(false);//����
						Manager::SetScene<Result>();//���U���g��
					}
					return;
				}
			}

		}

		//Enemy* enemy = scene->GetGameObject<Enemy>();

		//if (50.0f < enemy->GetPosition().x || enemy->GetPosition().x < -50.0f)
		//{
		//	Result::SetResult(true);//����
		//	Manager::SetScene<Result>();//���U���g��
		//}
	}
}

//���XDraw����2�ŕ`�� Update��1 �R���W����Update()�@�`�掝��1�ŕ`��Update��0�Ł@�`�悳��Ȃ�
void Player::Draw()
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
					scene->AddGameObject<Bullet>(1)->SetPosition(D3DXVECTOR3(m_Position.x + 0.3f, m_Position.y + 1.2f, m_Position.z));
				}
				else
				{
					scene->AddGameObject<Bullet>(1)->SetPosition(D3DXVECTOR3(m_Position.x - 0.3f, m_Position.y + 1.2f, m_Position.z));
				}
			}

			if (m_AnimationFrame == 40)
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

void Player::AddCollid(float x, float y, float sizeX, float sizeY, int zFrame,bool isRight)
{
	Scene* scene = Manager::GetScene();

	PlayerActionCollision* kari = scene->AddGameObjectNonInit<PlayerActionCollision>(1);

	//���E�Ή�
	if (isRight)
	{
		kari->Init(x, y, sizeX, sizeY, zFrame, 21.0f, 3.0f, 9.0f);
		kari->SetPosition(D3DXVECTOR3(m_Position.x + 0.7f, m_Position.y + 1.2f, m_Position.z));
	}
	else
	{
		kari->Init(x, y, sizeX, sizeY, zFrame, 159.0f, 3.0f, 9.0f);
		kari->SetPosition(D3DXVECTOR3(m_Position.x - 0.7f, m_Position.y + 1.2f, m_Position.z));
	}
}

//angle,speed,damage,�v���C���[�����x�̋����A�v���C���[�����y�̋���
void Player::AddCollid(float x, float y, float sizeX, float sizeY, int zFrame, bool isRight, float angle, float speed, float damage, float distX, float distY)
{
	Scene* scene = Manager::GetScene();

	if (!isAttackCollid())
	{
		PlayerActionCollision* kari = scene->AddGameObjectNonInit<PlayerActionCollision>(1);

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


bool Player::isAttack()
{
	for (int i = 0; i < PlayerAttackType; i++)
	{
		if (m_isAttack[i] == true)
		{
			return true;
		}
	} 
	return false;
}

bool Player::isAttackCollid()
{
	Scene* scene = Manager::GetScene();
	if (scene->GetGameObject<PlayerActionCollision>() != NULL)
	{
		return true;
	}

	return false;
}

BodyCollision * Player::GetBodyCollision()
{
	return m_body;
}

int Player::GetAttackType()
{
	for (int i = 0; i < PlayerAttackType; i++)
	{
		if (m_isAttack[i] == true)
		{
			return i;
		}
	}
	return -1;//�U�����Ă��Ȃ�
}

void Player::SetHitSE()
{
	PlaySound(m_HitSE[0], 0);//
}


