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

int Player::m_HitSE[2]{};//静的メンバ変数は再宣言が必要

void Player::Init()
{
	m_Model = new AnimationModel();
	m_Model->Load("asset\\model\\Bot.fbx");

	m_Model->LoadAnimation("asset\\model\\Breathing Idle.fbx", "Idle");
	m_Model->LoadAnimation("asset\\model\\Punching01.fbx", "Punch");  //弱攻撃
	m_Model->LoadAnimation("asset\\model\\Uppercut.fbx", "Upper");  //上強
	m_Model->LoadAnimation("asset\\model\\downKick.fbx", "DownTilt");  //下強
	m_Model->LoadAnimation("asset\\model\\sideKick.fbx", "ForwardTilt");  //横強
	m_Model->LoadAnimation("asset\\model\\Standing Walk Forward.fbx", "Walk");//歩きモーション
	m_Model->LoadAnimation("asset\\model\\Ready Idle.fbx", "Jump");//ジャンプ中のモーション
	m_Model->LoadAnimation("asset\\model\\Guard.fbx", "Guard");//ガードモーション
	m_Model->LoadAnimation("asset\\model\\Down.fbx", "Down");//ダウンモーション
	m_Model->LoadAnimation("asset\\model\\Head Hit.fbx", "HitAction");//やられモーション 39F
	m_Model->LoadAnimation("asset\\model\\Grab.fbx", "Grab");//つかみモーション 28F
	m_Model->LoadAnimation("asset\\model\\shot.fbx", "Shot");//
	
	m_AnimationName = "Idle";
	m_NextAnimationName = "Idle";

	m_Scale = D3DXVECTOR3(0.01f, 0.01f, 0.01f);

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\vertexLightingVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader\\vertexLightingPS.cso");

	m_Rotation.y = 1.5f;
	m_Shadow = AddComponent<Shadow>();

	//身体の当たり判定
	m_body = new  BodyCollision();
	m_body->Init(0.45f,1.75f);

	m_name = new Name();
	m_name->Init(1.45f, 1.0f);

	for (int i = 0; i < PlayerAttackType; i++)//初期化
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
			//サードパーソンビュー
			if ((GetKeyboardPress(DIK_A) || GetThumbLeftX(0) < 0.0f) && (isAttack() == false) && (isGuard() == false))//移動
			{
				if (m_IsField)
				{
					m_Rotation.y = -1.5f;//着地していたら反転出来る
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
			if ((GetKeyboardPress(DIK_D) || GetThumbLeftX(0) > 0.0f) && (isAttack() == false) && (isGuard() == false))//移動
			{
				if (m_IsField)
				{
					m_Rotation.y = 1.5f;//着地していたら反転出来る
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

			//弱
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

			//下
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

			//横
			if (((GetKeyboardTrigger(DIK_K) || 0.0f < GetThumbRightX(0))) && (isAttack() == false))
			{
				if (m_IsField)
				{
					//右向く
					m_Rotation.y = 1.5f;//着地していたら反転出来る
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
					//左向く
					m_Rotation.y = -1.5f;//着地していたら反転出来る
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

			//上
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

			//つかみ
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

			//ショット
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

			//Attack　push後
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

			//ジャンプ
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

			//ZRがもどったかどうか
			if (0 == GetRightTrigger(0))
			{
				m_isRightTriggerOK = false;
			}

			//ガード
			if (((GetKeyboardPress(DIK_G) || 0 < GetLeftTrigger(0))) && (isAttack() == false))
			{
				if (m_IsField)
				{
					if (m_IsGuard == false)//押し始め
					{
						m_GuardFrame = 0;//初期化
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

			//ガードの強制展開フレーム 3F処理
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

			////弾発射
			//if (Input::GetKeyTrigger(VK_SPACE))
			//{
			//	scene->AddGameObject<Bullet>(1)->SetPosition(D3DXVECTOR3(m_Position.x, m_Position.y + 0.9f, m_Position.z));
			//}

		}
		else if(m_IsCool == true) //クールタイム中 
		{
			//初期化
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
			//ふっとびおわり、ダウン時間終わりに呼ぶ
			if (m_isPushFinished && m_DownTime == 0)
			{
				//初期化
				m_IsCool = false;
				m_IsGrabHit = false;
				m_isPushFinished = false;
			}

			if (78 < m_Time)
			{
				//やられモーションの全体フレーム*2?を超えたら初期化
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

		//重力
		m_Velocity.y -= 0.015f;//オイラー法

		//移動
		m_Position += m_Velocity;//オイラー法

		//衝突後
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
			m_body->SetPosition(D3DXVECTOR3(m_Position.x, m_Position.y + 0.9f, m_Position.z));//身体のコリジョンを更新
		}
		else
		{
			if (m_FaceIsRight)
			{
				m_body->SetPosition(D3DXVECTOR3(m_Position.x + 0.3f, m_Position.y + 0.9f, m_Position.z));//身体のコリジョンを更新
			}
			else
			{
				m_body->SetPosition(D3DXVECTOR3(m_Position.x - 0.3f, m_Position.y + 0.9f, m_Position.z));//身体のコリジョンを更新
			}
		}

		//攻撃判定の更新
		PlayerActionCollision* kari = scene->GetGameObject<PlayerActionCollision>();

		////コリジョンがあってかつ、Distanceがあるなら移動するをUpdateで
		//if ((kari != nullptr) && (m_DistancePlayerCollisionX != 0.0) || m_DistancePlayerCollisionY != 0.0)
		//{
		//	//左右対応
		//	if (m_FaceIsRight)
		//	{
		//		kari->SetPosition(D3DXVECTOR3(m_Position.x + m_DistancePlayerCollisionX, m_Position.y + m_DistancePlayerCollisionY, m_Position.z));
		//	}
		//	else
		//	{
		//		kari->SetPosition(D3DXVECTOR3(m_Position.x - m_DistancePlayerCollisionX, m_Position.y + m_DistancePlayerCollisionY, m_Position.z));
		//	}
		//}

		//障害物との衝突判定
		//float groundHeight = 0.0f;

		MeshField* meshfield = scene->GetGameObject<MeshField>();

		////box
		//Enemy* enemy = scene->GetGameObject<Enemy>();//型名はautoでも可
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

		//接地
		if (m_Position.y < groundHeight && m_Velocity.y < 0.0f)
		{
			m_Position.y = groundHeight;
			m_Velocity.y = 0.0f;
			m_IsField = true;
			m_Move = false;
			//ジャンプ回数　初期化
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

		//影の移動
		D3DXVECTOR3 shadowPosition = m_Position;
		shadowPosition.y = groundHeight + 0.01f;
		m_Shadow->SetPosition(shadowPosition);

		//限界値チェック
		if (50.0f < m_Position.x || m_Position.x < -50.0f)
		{
			PlaySound(m_HitSE[1], 0);//;//撃墜SE
			m_VecX = 0.0f;
			m_VecY = 0.0f;
			m_IsCool = false;
			m_IsGrabHit = false;
			m_isPushFinished = false;

			m_HideTime = 120;
			m_isReBorn = true;

			//初期化
			m_Hp->SetHp(0);
			m_Position = D3DXVECTOR3(-1.0f, 0.0f, -4.0f);
			m_OldPosition = m_Position;
			m_body->SetPosition(D3DXVECTOR3(m_Position.x, m_Position.y + 0.9f, m_Position.z));//身体のコリジョンを更新
			m_name->SetPosition(D3DXVECTOR3(m_Position.x, m_Position.y + 0.9f, m_Position.z));//身体のコリジョンを更新
			m_body->SetOldPosition(m_body->GetPosition());
			m_name->SetOldPosition(m_body->GetPosition());


			//ストック処理
			for (int i = 2; 0 <= i; i--)
			{
				//実体がある
				if (m_Stock[i] != nullptr)
				{
					m_Stock[i]->SetDestroy();
					m_Stock[i] = nullptr;

					if (i == 0)
					{
						//リザルト処理
						Result::SetResult(false);//負け
						Manager::SetScene<Result>();//リザルトへ
					}
					return;
				}
			}

		}

		//Enemy* enemy = scene->GetGameObject<Enemy>();

		//if (50.0f < enemy->GetPosition().x || enemy->GetPosition().x < -50.0f)
		//{
		//	Result::SetResult(true);//勝ち
		//	Manager::SetScene<Result>();//リザルトへ
		//}
	}
}

//元々Draw持続2で描画 Updateで1 コリジョンUpdate()　描画持続1で描画Updateで0で　描画されない
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
		// 入力レイアウト設定
		Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

		// シェーダ設定
		Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
		Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

		// マトリクス設定
		D3DXMATRIX matrix, scale, rot, trans;
		D3DXMatrixScaling(&scale, m_Scale.x, m_Scale.y, m_Scale.z);
		D3DXMatrixRotationYawPitchRoll(&rot, m_Rotation.y, m_Rotation.x, m_Rotation.z);
		D3DXMatrixTranslation(&trans, m_Position.x, m_Position.y, m_Position.z);
		matrix = scale * rot * trans;
		m_Matrix = matrix;

		Renderer::SetWorldMatrix(&matrix);

		m_Model->Update(m_AnimationName.c_str(), m_Time, m_NextAnimationName.c_str(), m_Time, m_BlendRate);
		m_Model->Draw();

		//弱
		if (m_AnimationName == "Punch")
		{
			//コリジョン
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

		//下
		if (m_AnimationName == "DownTilt")
		{
			//コリジョン
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

		//横
		if (m_AnimationName == "ForwardTilt")
		{
			//コリジョン
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

		//上
		if (m_AnimationName == "Upper")
		{
			//コリジョン
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

		//つかみ
		if (m_AnimationName == "Grab")
		{
			//コリジョン
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
			//コリジョン
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

		//更新処理
		if (Game::GetIsPause() == false && Debug::GetIsFrameMode() == false && Debug::GetIsPause() == false && Collision::GetHitStop() == 0)
		{
			m_Time++;
			if (isAttack())
			{
				m_AnimationFrame++;
			}

			if (m_AnimationName == "Guard")
			{
				//ガードの強制展開フレーム 3F処理
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
					//ガードの強制展開フレーム 3F処理
					m_GuardFrame++;
				}

				m_BlendRate += 0.05f;
				if (m_BlendRate > 1.0f)
					m_BlendRate = 1.0f;
			}
		}

		//ガード硬直処理
		if (m_IsGuard == false && 3 <= m_GuardFrame)
		{
			m_AnimationName = "Idle";
			m_NextAnimationName = "Idle";
			m_BlendRate = 0.0f;

			m_IsChance = true;
			SetChanceTime();//ガード解除の硬直フレーム11F
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

	//左右対応
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

//angle,speed,damage,プレイヤーからのxの距離、プレイヤーからのyの距離
void Player::AddCollid(float x, float y, float sizeX, float sizeY, int zFrame, bool isRight, float angle, float speed, float damage, float distX, float distY)
{
	Scene* scene = Manager::GetScene();

	if (!isAttackCollid())
	{
		PlayerActionCollision* kari = scene->AddGameObjectNonInit<PlayerActionCollision>(1);

		//コリジョンディスタンス
		m_DistancePlayerCollisionX = distX;
		m_DistancePlayerCollisionY = distY;

		//左右対応
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
	return -1;//攻撃していない
}

void Player::SetHitSE()
{
	PlaySound(m_HitSE[0], 0);//
}


