#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "debug.h"
#include "select.h"
#include "camera.h"
#include "meshField.h"
#include "field.h"
#include "player.h"
#include "enemy.h"
#include "bullet.h"
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
#include "collision.h"
#include "sky.h"
#include "polygon2D.h"
#include "hp.h"
#include "sound.h"

bool Debug::m_isFrameMode = false;
bool Debug::m_isPause = false;

static int			  DebugBGM;//タイトル画面のBGM

void Debug::Init()
{
	//リストに追加 ポーズ画面
	m_UiImage[0][0] = "asset\\texture\\Pause0.png";
	m_UiImage[1][0] = "asset\\texture\\Pause1.png";
	m_UiImage[2][0] = "asset\\texture\\Pause2.png";
	m_UiImage[3][0] = "asset\\texture\\Pause3.png";
	m_UiImage[4][0] = "asset\\texture\\controller.png";

	//Frameモード
	m_DebugImage[0][0] = "asset\\texture\\Frame.png";

	Tree::Load();
	BodyCollision::Load();
	Name::Load();
	EnemyName::Load();
	Bullet::Load();
	EnemyBullet::Load();
	PlayerActionCollision::Load();
	EnemyActionCollision::Load();

	AddGameObject<Camera>(0);
	AddGameObject<Sky>(1);
	Field* field = AddGameObject<Field>(1);

	srand(0);

	Player* player = AddGameObject<Player>(1);
	D3DXVECTOR3 ipPlayer = D3DXVECTOR3(-1.0f, 0.0f, -4.0f);
	Camera::SetInitPlayerPos(ipPlayer);
	player->SetPosition(ipPlayer);

	Enemy* enemy = AddGameObject<Enemy>(1);
	D3DXVECTOR3 ipEnemy = D3DXVECTOR3(1.0f, 0.0f, -4.0f);
	Camera::SetInitEnemyPos(ipEnemy);
	enemy->SetPosition(ipEnemy);

	//敵のHP
	Hp* kari0 = AddGameObjectNonInit<Hp>(2);
	kari0->Init(0, SCREEN_WIDTH / 4 * 3, SCREEN_HEIGHT / 4 * 3);
	enemy->SetHp(kari0);
	//PlayeｒのHP
	Hp* kari1 = AddGameObjectNonInit<Hp>(2);
	kari1->Init(0, SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4 * 3);
	player->SetHp(kari1);

	m_backGround = AddGameObjectNonInit<Polygon2D>(2);//"asset\\texture\\field002.jpg"
	m_backGround->Init(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, m_DebugImage[0][0]);
	m_backGround->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));//最初は透明

	//Pause画面UI
	m_PauseImage = AddGameObjectNonInit<Polygon2D>(2);//"asset\\texture\\field002.jpg"
	m_PauseImage->Init(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, m_UiImage[0][0]);
	m_PauseImage->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));

	char filename[] = "asset\\BGM\\B00158_kamatamago_enemy-central.wav";
	DebugBGM = LoadSound(filename);
	PlaySound(DebugBGM, -1);//タイトル画面のBGM再生
}

void Debug::Uninit()
{
	StopSound(DebugBGM);//タイトルサウンドの再生停止

	Scene::Uninit();

	Tree::Unload();
	BodyCollision::Unload();
	Name::Unload();
	EnemyName::Unload();
	Bullet::Unload();
	EnemyBullet::Unload();
	PlayerActionCollision::Unload();
	EnemyActionCollision::Unload();
}

void Debug::Update()
{
	Scene* scene = Manager::GetScene();

	//Frameモード
	if ((GetKeyboardTrigger(DIK_H) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_BACK)) && m_isFrameMode == false && m_isPause == false)
	{
		m_isFrameMode = true;
		m_backGround->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	}
	if ((GetKeyboardTrigger(DIK_N) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_A)) && m_isFrameMode == true)
	{
		m_isFrameMode = false;
		m_backGround->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
	}

	//XINPUT_GAMEPAD_START
	if ((GetKeyboardTrigger(DIK_Y) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_START)) && m_isPause == false && m_isFrameMode == false)
	{
		m_isPause = true;
		m_PauseImage->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	}

	if (m_isFrameMode == false && m_isPause == false)
	{
		Scene::Update();
		Player* player = scene->GetGameObject<Player>();
		Enemy*  enemy = scene->GetGameObject<Enemy>();

		if (player->GetHideTime() != 0)
		{
			player->PullHideTime();
		}
		if (enemy->GetHideTime() != 0)
		{
			enemy->PullHideTime();
		}
	}
	else
	{
		if ((GetKeyboardTrigger(DIK_F) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_DPAD_RIGHT)) && m_isFrameMode == true)
		{
			Scene::Update();
		}
		//ポーズ画面
		else if (m_isPause == true)
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

				if (GetKeyboardTrigger(DIK_DOWNARROW) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_DPAD_DOWN))
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
			default:
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