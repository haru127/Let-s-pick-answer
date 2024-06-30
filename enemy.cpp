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

int Enemy::m_HitSE[2]{};//静的メンバ変数は再宣言が必要

void Enemy::Init()
{
	m_Model = new AnimationModel();
	m_Model->Load("asset\\model\\Bot.fbx");
	m_Model->LoadAnimation("asset\\model\\Breathing Idle.fbx", "Idle");
	m_Model->LoadAnimation("asset\\model\\Punching01.fbx", "Punch");  //弱攻撃
	m_Model->LoadAnimation("asset\\model\\Head Hit.fbx", "HitAction");//やられモーション 39F
	m_Model->LoadAnimation("asset\\model\\Standing Walk Forward.fbx", "Walk");//歩きモーション
	m_Model->LoadAnimation("asset\\model\\Guard.fbx", "Guard");//ガードモーション
	m_Model->LoadAnimation("asset\\model\\Down.fbx", "Down");//ダウンモーション
	m_Model->LoadAnimation("asset\\model\\Ready Idle.fbx", "Jump");//ジャンプ中のモーション
	m_Model->LoadAnimation("asset\\model\\Grab.fbx", "Grab");//つかみモーション 28F
	m_Model->LoadAnimation("asset\\model\\shot.fbx", "Shot");//ショット
	m_Model->LoadAnimation("asset\\model\\Uppercut.fbx", "Upper");  //上強
	m_Model->LoadAnimation("asset\\model\\downKick.fbx", "DownTilt");  //下強
	m_Model->LoadAnimation("asset\\model\\sideKick.fbx", "ForwardTilt");  //横強

	m_AnimationName = "Idle";
	m_NextAnimationName = "Idle";

	m_Scale = D3DXVECTOR3(0.01f, 0.01f, 0.01f);

	m_Rotation.y = -1.5;
	m_Shadow = AddComponent<Shadow>();

	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\vertexLightingVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader\\vertexLightingPS.cso");

	//身体の当たり判定
	m_body = new  BodyCollision();
	m_body->Init(0.45f, 1.75f);

	m_name = new EnemyName();
	m_name->Init(1.0f, 0.8f);

	//エネミーのステートマシン
	m_EnemyManager = new EnemyManager();
	m_EnemyManager->Init();

	for (int i = 0; i < EnemyAttackType; i++)//初期化
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

		//前フレームの座標を保存しておく
		m_OldPosition = m_Position;
		m_body->SetOldPosition(m_body->GetPosition());
		m_name->SetOldPosition(m_body->GetPosition());

		if (m_IsCool == false && m_IsChance == false)
		{
			//ステート処理
			{
				FaceCorrect();//相手の方向を向く

				float calculate = 0.0f;
				if (player->GetPosition().x >= 0 && m_Position.x >= 0) {
					// 両方が正の場合
					calculate = abs(player->GetPosition().x - m_Position.x);
				}
				else if (player->GetPosition().x < 0 && m_Position.x < 0) {
					// 両方が負の場合
					calculate = abs(player->GetPosition().x - m_Position.x);
				}
				else
				{
					// 片方が正で片方が負の場合
					calculate = abs(player->GetPosition().x) + abs(m_Position.x);
				}

				if (calculate < 0.7f)
				{
					//現在のステートと更新するステートが違うなら
					if (m_State != StatePatern::NEARDIS)
 					{
						m_EnemyManager->SetState<EnemyStateNear>();
						m_State = StatePatern::NEARDIS;
					}
					// ここに差が1未満かつ範囲内の場合に実行したいコードを書く
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

				//弾ガード処理
				std::vector<Bullet*> bulletes = scene->GetGameObjects<Bullet>();//型名はautoでも可

				for (Bullet* bullet : bulletes)//範囲forループ
				{
					D3DXVECTOR3 bulletPos = bullet->GetPosition();
					D3DXVECTOR3 direction = m_Position - bulletPos;
					float length = D3DXVec3Length(&direction);//敵と弾のベクトルの長さ

					if (length < 2.0f)
					{
						//ガード
						m_EnemyManager->SetState<EnemyStateDefend>();
						m_State = StatePatern::DEFEND;
					}
				}

				m_EnemyManager->Update(GetThisPointer());
			}

			//Attack　push後
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

			//ガードの強制展開フレーム 3F処理
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

			//空中モーション
			if (!m_IsField)
			{
				if (m_NextAnimationName != "Jump")
				{
					m_AnimationName = m_NextAnimationName;
					m_NextAnimationName = "Jump";
					m_BlendRate = 0.0f;
				}
			}

			//アイドル
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
		else if(m_IsCool == true) //クールタイム中 
		{
			for (int i = 0; i < EnemyAttackType; i++)//初期化
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

			if (m_ChanceTime != 0)
			{
				m_ChanceTime--;
			}

			if (m_ChanceTime == 0)
			{
				m_IsChance = false;
			}
		}


		//重力
		m_Velocity.y -= 0.015f;//オイラー法
		//m_Velocity.y -= 0.005f;//オイラー法

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

		//障害物との衝突判定
		float groundHeight = 0.0f;

		MeshField* meshfield = scene->GetGameObject<MeshField>();

		//接地
		if (m_Position.y < groundHeight && m_Velocity.y < 0.0f)
		{
			m_Position.y = groundHeight;
			m_Velocity.y = 0.0f;
			m_IsField = true;
			m_IsMove = false;

			//ジャンプ回数初期化
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

		//影の移動
		D3DXVECTOR3 shadowPosition = m_Position;
		shadowPosition.y = groundHeight + 0.01f;
		m_Shadow->SetPosition(shadowPosition);

		//限界地チェック
		if (50.0f < m_Position.x || m_Position.x < -50.0f)
		{
			PlaySound(m_HitSE[1], 0);//;//撃墜SE
			m_VecX = 0.0f;
			m_VecY = 0.0f;
			m_IsCool = false;
			m_IsGrabHit = false;
			m_isPushFinished = false;
			m_EnemyManager->Init();

			m_HideTime = 120;
			m_isReBorn = true;

			//初期化
			m_Hp->SetHp(0);
			m_Position = D3DXVECTOR3(1.0f, 0.0f, -4.0f);
			m_OldPosition = m_Position;
			m_body->SetPosition(D3DXVECTOR3(m_Position.x, m_Position.y + 0.9f, m_Position.z));//身体のコリジョンを更新
			m_body->SetOldPosition(m_body->GetPosition());
			m_name->SetPosition(D3DXVECTOR3(m_Position.x, m_Position.y + 0.9f, m_Position.z));//身体のコリジョンを更新
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
						Result::SetResult(true);//勝ち
						Manager::SetScene<Result>();//リザルトへ
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
				//AddCollid(m_Position.x, m_Position.y, 0.7f, 0.2f, 2, m_FaceIsRight, 21.0f, 3.0f, 9.0f, 0.7f, 1.2f);
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
		//右
		if (m_IsField == true && (isAttack() == false))
		{
			m_Rotation.y = 1.5f;//着地していたら反転出来る
			m_FaceIsRight = true;
		}
	}
	else 
	{
		//左
		if (m_IsField == true && (isAttack() == false))
		{
			m_Rotation.y = -1.5f;//着地していたら反転出来る
			m_FaceIsRight = false;
		}

	}
}

void Enemy::At_Punch()
{
	//弱
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
	//下
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
	//横
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
	//上
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
	//ガード
	if (isAttack() == false)
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
}

void Enemy::AddCollid(float x, float y, float sizeX, float sizeY, int zFrame, bool isRight, float angle, float speed, float damage, float distX, float distY)
{
	Scene* scene = Manager::GetScene();

	if (!isAttackCollid())
	{
		EnemyActionCollision* kari = scene->AddGameObjectNonInit<EnemyActionCollision>(1);

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
	return -1;//攻撃していない

}

