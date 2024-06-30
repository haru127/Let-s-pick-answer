#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "game.h"
#include "select.h"
#include "camera.h"
#include "meshField.h"
#include "field.h"
#include "player.h"
#include "enemy.h"
#include "bullet.h"
#include "ball.h"
#include "enemyBullet.h"
#include "input.h"
#include "inputx.h"
#include "rock.h"
#include "tree.h"
#include "bodyCollision.h"
#include "name.h"
#include "enemyName.h"
#include "playerActionCollision.h"
#include "enemyActionCollision.h"
#include "respornImage.h"
#include "collision.h"
#include "sky.h"
#include "polygon2D.h"
#include "hp.h"
#include "sound.h" 

bool Game::m_isPause = false;

static int			  GameBGM;//タイトル画面のBGM

void Game::Init()
{
	//リストに追加 ポーズ画面
	m_UiImage[0][0] = "asset\\texture\\Pause0.png";
	m_UiImage[1][0] = "asset\\texture\\Pause1.png";
	m_UiImage[2][0] = "asset\\texture\\Pause2.png";
	m_UiImage[3][0] = "asset\\texture\\Pause3.png";
	m_UiImage[4][0] = "asset\\texture\\controller.png";

	//リストに追加 カウントダウン
	m_CountDownImagePass[0][0] = "asset\\texture\\countDown3.png";
	m_CountDownImagePass[1][0] = "asset\\texture\\countDown2.png";
	m_CountDownImagePass[2][0] = "asset\\texture\\countDown1.png";
	m_CountDownImagePass[3][0] = "asset\\texture\\countDownGo.png";

	//プロセス
	m_ProcessImagePass[0][0] = "asset\\texture\\rule.png"; //ルール
	m_ProcessImagePass[1][0] = "asset\\texture\\controller.png"; //基本操作

	//キルショット
	for (int i = 0; i < 2; i++)
	{
		m_IsKillShot[i] = false;//player.enemy
		m_KillShotTime[i] = 0;
	}

	Tree::Load();
	RespornImage::Load();
	BodyCollision::Load();
	Name::Load();
	EnemyName::Load();
	Bullet::Load();
	EnemyBullet::Load();
	PlayerActionCollision::Load();
	EnemyActionCollision::Load();

	AddGameObject<Camera>(0);
	AddGameObject<Sky>(1);
	//AddGameObject<Field>(1);
	//MeshField* meshField = AddGameObject<MeshField>(1);
	//Field* field = AddGameObject<Field>(1);

	srand(0);

	Rock* rock = AddGameObject<Rock>(1);
	rock->SetPosition(D3DXVECTOR3(0.0f, -1.0f, 0.0f));

	Player* player = AddGameObject<Player>(1);
	D3DXVECTOR3 ipPlayer = D3DXVECTOR3(-3.0f, 0.0f, -4.0f);
	Camera::SetInitPlayerPos(ipPlayer);
	player->SetPosition(ipPlayer);

	Enemy* enemy = AddGameObject<Enemy>(1);
	D3DXVECTOR3 ipEnemy = D3DXVECTOR3(3.0f, 0.0f, -4.0f);
	Camera::SetInitEnemyPos(ipEnemy);
	enemy->SetPosition(ipEnemy);

	//敵のHP
	Hp* kari0 = AddGameObjectNonInit<Hp>(2);
	kari0->Init(0, SCREEN_WIDTH / 4 * 3, SCREEN_HEIGHT / 4 * 3);
	enemy->SetHp(kari0);
	//PlayerのHP
	Hp* kari1 = AddGameObjectNonInit<Hp>(2);
	kari1->Init(0, SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4 * 3);
	player->SetHp(kari1);

	//プレイヤーエネミーUI
	m_CharactorImage[0] = AddGameObjectNonInit<Polygon2D>(2);
	m_CharactorImage[0]->Init((SCREEN_WIDTH / 4 - 25.0f), SCREEN_HEIGHT / 6 * 5, 80.0f, 80.0f, "asset\\texture\\player.png");
	//エネミー
	m_CharactorImage[1] = AddGameObjectNonInit<Polygon2D>(2);
	m_CharactorImage[1]->Init((SCREEN_WIDTH / 4 * 3 - 25.0f), SCREEN_HEIGHT / 6 * 5, 80.0f, 80.0f, "asset\\texture\\enemy.png");

	//キルショット
	m_KillShotImage[0] = AddGameObjectNonInit<Polygon2D>(2);
	m_KillShotImage[0]->Init((0.0f), SCREEN_HEIGHT / 6 * 1, 200.0f, 200.0f, "asset\\texture\\player.png");//左
	m_KillShotImage[0]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));//透明

	m_KillShotImage[1] = AddGameObjectNonInit<Polygon2D>(2);
	m_KillShotImage[1]->Init((SCREEN_WIDTH - 200.0f), SCREEN_HEIGHT / 6 * 1, 200.0f, 200.0f, "asset\\texture\\enemy.png");//右
	m_KillShotImage[1]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));//透明

	//stock初期化 Player
	for (int i = 0; i < 3; i++)
	{
		Polygon2D* kari = AddGameObjectNonInit<Polygon2D>(2);
		kari->Init((SCREEN_WIDTH / 4 + 50.0f) + ((i + 1) * 30.0f), SCREEN_HEIGHT / 7 * 6, 30.0f, 30.0f, "asset\\texture\\stockImage.png");
		player->SetStock(kari, i);
	}

	//stock初期化 Enemy
	for (int i = 0; i < 3; i++)
	{
		Polygon2D* kari = AddGameObjectNonInit<Polygon2D>(2);
		kari->Init((SCREEN_WIDTH / 4 * 3 + 50.0f) + ((i + 1) * 30.0f), SCREEN_HEIGHT / 7 * 6, 30.0f, 30.0f, "asset\\texture\\stockImage.png");
		enemy->SetStock(kari, i);
	}

	//カウントダウン初期化
	for (int i = 0; i < 3; i++)
	{
		m_CountDown[i] = 60;
	}
	m_CountDown[3] = 100;

	m_CountDownImage = AddGameObjectNonInit<Polygon2D>(2);//"asset\\texture\\field002.jpg"
	m_CountDownImage->Init(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, m_CountDownImagePass[0][0]);
	m_CountDownImage->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));//透明

	//プロセス
	m_ProcessImage = AddGameObjectNonInit<Polygon2D>(2);//"asset\\texture\\field002.jpg"
	m_ProcessImage->Init(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, m_ProcessImagePass[0][0]);
	m_ProcessImage->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));//

	//Pause画面UI
	m_PauseImage = AddGameObjectNonInit<Polygon2D>(2);//"asset\\texture\\field002.jpg"
	m_PauseImage->Init(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, m_UiImage[0][0]);
	m_PauseImage->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));


	char filename[] = "asset\\BGM\\B00158_kamatamago_enemy-central.wav";
	GameBGM = LoadSound(filename);
	PlaySound(GameBGM, -1);//タイトル画面のBGM再生

	//プレイヤーリスポーン画像
	{
		RespornImage* kari = AddGameObject<RespornImage>(1);
		player->SetRespornImage(kari);
		kari->SetPosition(D3DXVECTOR3(-1.0f, 0.0f, -4.0f));
	}

	//エネミーリスポーン画像
	{
		RespornImage* kari = AddGameObject<RespornImage>(1);
		enemy->SetRespornImage(kari);
		kari->SetPosition(D3DXVECTOR3(1.0f, 0.0f, -4.0f));
	}

}

void Game::Uninit()
{
	StopSound(GameBGM);//タイトルサウンドの再生停止

	Scene::Uninit();

	Tree::Unload();
	RespornImage::Unload();
	BodyCollision::Unload();
	Name::Unload(); 
	EnemyName::Unload();
	Bullet::Unload();
	EnemyBullet::Unload();
	PlayerActionCollision::Unload();
	EnemyActionCollision::Unload();
}

void Game::Update()
{
	Scene* scene = Manager::GetScene();
	Player* player = scene->GetGameObject<Player>();
	Enemy*  enemy =  scene->GetGameObject<Enemy>();

	//for (int i = 0; i < 2; i++)
	//{
	//	if (m_KillShotTime[i] != 0)
	//	{
	//		m_KillShotTime[i]--;
	//	}
	//	else 
	//	{
	//		m_KillShotImage[i]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));//透明
	//	}
	//}

	if (isFinishProcess == false)
	{
		//進む　戻る
		switch (m_ProcessCount)
		{
		case 0:
			//進む
			if (GetKeyboardTrigger(DIK_W) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_B))
			{
				m_ProcessCount++;
				m_ProcessImage->SetImage(m_ProcessImagePass[1][0]);//コントローラー
			}
			//戻る
			if (GetKeyboardTrigger(DIK_S) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_A))
			{
				Manager::SetScene<Select>();
			}
			break;
		case 1:
			//進む
			if (GetKeyboardTrigger(DIK_W) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_B))
			{
				isFinishProcess = true;
				m_ProcessImage->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));//透明に
				m_CountDownImage->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));//カウントダウン開始
			}
			//戻る
			if (GetKeyboardTrigger(DIK_S) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_A))
			{
				m_ProcessCount--;
				m_ProcessImage->SetImage(m_ProcessImagePass[0][0]);//コントローラー
			}

			break;
		default:
			break;
		}

	}

	{ //カウントダウン
		//if() プロセスが終わったら
		if (isFinishProcess == true)
		{
			for (int i = 0; i < 3; i++)
			{
				if (m_CountDown[i] != 0)
				{
					m_CountDown[i]--;

					if (m_CountDown[i] == 0)
					{
						//画像
						switch (i)
						{
						case 0:
							m_CountDownImage->SetImage(m_CountDownImagePass[1][0]);//デバッグ
							break;
						case 1:
							m_CountDownImage->SetImage(m_CountDownImagePass[2][0]);//デバッグ
							break;
						case 2:
							m_CountDownImage->SetImage(m_CountDownImagePass[3][0]);//デバッグ
							isFinishCountDown = true;
							break;
							//case 3:
							//	m_CountDownImage->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
							//	break;
						}
					}

					return;
				}
			}
		}
	}

	if (isFinishCountDown == true && m_CountDown[3] != 0)
	{
		m_CountDown[3]--;
		if (m_CountDown[3] == 0)
		{
			m_CountDownImage->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
		}
	}

	//ゲーム処理
	if (isFinishCountDown == true)
	{
		//XINPUT_GAMEPAD_START
		if ((GetKeyboardTrigger(DIK_H) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_START)) && m_isPause == false)
		{
			m_isPause = true;
			m_PauseImage->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
		}

		//キルショット処理
		//playerやられ
		if (player->GetIsReborn())
		{
			//キルショット描画
			m_KillShotImage[0]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
			//カウント追加
			m_KillShotTime[0] = 60;
			//updateでカウント下げていく
		}

		//enemyやられ
		if (enemy->GetIsReborn())
		{
			//キルショット描画
			m_KillShotImage[1]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
			//カウント追加
			m_KillShotTime[1] = 60;
			//updateでカウント下げていく
		}

		//キルショットタイム処理
		if (player->GetHideTime() != 0)
		{
			player->PullHideTime();
			if (player->GetHideTime() <= 60)
			{
				m_KillShotImage[0]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));//透明
				player->GetRespornImage()->SetResporn(true);
			}
		}
		else
		{
			player->GetRespornImage()->SetResporn(false);
		}

		if (enemy->GetHideTime() != 0)
		{
			enemy->PullHideTime();
			if (enemy->GetHideTime() <= 60)
			{
				m_KillShotImage[1]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));//透明
				enemy->GetRespornImage()->SetResporn(true);
			}
		}
		else
		{
			enemy->GetRespornImage()->SetResporn(false);
		}



		if (!m_isPause && Collision::GetHitStop() == 0)
		{
			Scene::Update();
		}
		else
		{
			switch (m_ImageCount)
			{
			case 0:
				if (GetKeyboardTrigger(DIK_SPACE) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_B))
				{
					////ゲーム再開
					m_isPause = false;
					m_PauseImage->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
				}

				if (GetKeyboardTrigger(DIK_DOWNARROW) || IsButtonTriggeredX(0,XINPUT_GAMEPAD_DPAD_DOWN))
				{
					m_PauseImage->SetImage(m_UiImage[1][0]);
					m_ImageCount++;
				}

				break;
			case 1:
				if (GetKeyboardTrigger(DIK_SPACE) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_B))
				{
					////基本操作画像描画
					m_PauseImage->SetImage(m_UiImage[4][0]);
					m_ImageCount += 3;
				}

				if (GetKeyboardTrigger(DIK_DOWNARROW) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_DPAD_DOWN))
				{
					m_PauseImage->SetImage(m_UiImage[2][0]);
					m_ImageCount++;
				}

				if (GetKeyboardTrigger(DIK_UPARROW) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_DPAD_UP))
				{
					m_PauseImage->SetImage(m_UiImage[0][0]);
					m_ImageCount--;
				}

				break;
			case 2:
				if (GetKeyboardTrigger(DIK_SPACE) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_B))
				{
					////セレクト画面に遷移
					Manager::SetScene<Select>();
					m_isPause = false;
				}

				if (GetKeyboardTrigger(DIK_DOWNARROW) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_DPAD_DOWN))
				{
					m_PauseImage->SetImage(m_UiImage[3][0]);
					m_ImageCount++;
				}

				if (GetKeyboardTrigger(DIK_UPARROW) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_DPAD_UP))
				{
					m_PauseImage->SetImage(m_UiImage[1][0]);
					m_ImageCount--;
				}

				break;
			case 3:
				if (GetKeyboardTrigger(DIK_SPACE) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_B))
				{
					////ゲーム終了
					exit(988);
				}

				if (GetKeyboardTrigger(DIK_UPARROW) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_DPAD_UP))
				{
					m_PauseImage->SetImage(m_UiImage[2][0]);
					m_ImageCount--;
				}

				break;
			default :
				//戻る
				if (GetKeyboardTrigger(DIK_S) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_A))
				{
					m_PauseImage->SetImage(m_UiImage[1][0]);
					m_ImageCount = 1;
				}
				break;
			}

		}
	}
}