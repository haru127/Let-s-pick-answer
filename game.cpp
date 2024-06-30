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

static int			  GameBGM;//�^�C�g����ʂ�BGM

void Game::Init()
{
	//���X�g�ɒǉ� �|�[�Y���
	m_UiImage[0][0] = "asset\\texture\\Pause0.png";
	m_UiImage[1][0] = "asset\\texture\\Pause1.png";
	m_UiImage[2][0] = "asset\\texture\\Pause2.png";
	m_UiImage[3][0] = "asset\\texture\\Pause3.png";
	m_UiImage[4][0] = "asset\\texture\\controller.png";

	//���X�g�ɒǉ� �J�E���g�_�E��
	m_CountDownImagePass[0][0] = "asset\\texture\\countDown3.png";
	m_CountDownImagePass[1][0] = "asset\\texture\\countDown2.png";
	m_CountDownImagePass[2][0] = "asset\\texture\\countDown1.png";
	m_CountDownImagePass[3][0] = "asset\\texture\\countDownGo.png";

	//�v���Z�X
	m_ProcessImagePass[0][0] = "asset\\texture\\rule.png"; //���[��
	m_ProcessImagePass[1][0] = "asset\\texture\\controller.png"; //��{����

	//�L���V���b�g
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

	//�G��HP
	Hp* kari0 = AddGameObjectNonInit<Hp>(2);
	kari0->Init(0, SCREEN_WIDTH / 4 * 3, SCREEN_HEIGHT / 4 * 3);
	enemy->SetHp(kari0);
	//Player��HP
	Hp* kari1 = AddGameObjectNonInit<Hp>(2);
	kari1->Init(0, SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4 * 3);
	player->SetHp(kari1);

	//�v���C���[�G�l�~�[UI
	m_CharactorImage[0] = AddGameObjectNonInit<Polygon2D>(2);
	m_CharactorImage[0]->Init((SCREEN_WIDTH / 4 - 25.0f), SCREEN_HEIGHT / 6 * 5, 80.0f, 80.0f, "asset\\texture\\player.png");
	//�G�l�~�[
	m_CharactorImage[1] = AddGameObjectNonInit<Polygon2D>(2);
	m_CharactorImage[1]->Init((SCREEN_WIDTH / 4 * 3 - 25.0f), SCREEN_HEIGHT / 6 * 5, 80.0f, 80.0f, "asset\\texture\\enemy.png");

	//�L���V���b�g
	m_KillShotImage[0] = AddGameObjectNonInit<Polygon2D>(2);
	m_KillShotImage[0]->Init((0.0f), SCREEN_HEIGHT / 6 * 1, 200.0f, 200.0f, "asset\\texture\\player.png");//��
	m_KillShotImage[0]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));//����

	m_KillShotImage[1] = AddGameObjectNonInit<Polygon2D>(2);
	m_KillShotImage[1]->Init((SCREEN_WIDTH - 200.0f), SCREEN_HEIGHT / 6 * 1, 200.0f, 200.0f, "asset\\texture\\enemy.png");//�E
	m_KillShotImage[1]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));//����

	//stock������ Player
	for (int i = 0; i < 3; i++)
	{
		Polygon2D* kari = AddGameObjectNonInit<Polygon2D>(2);
		kari->Init((SCREEN_WIDTH / 4 + 50.0f) + ((i + 1) * 30.0f), SCREEN_HEIGHT / 7 * 6, 30.0f, 30.0f, "asset\\texture\\stockImage.png");
		player->SetStock(kari, i);
	}

	//stock������ Enemy
	for (int i = 0; i < 3; i++)
	{
		Polygon2D* kari = AddGameObjectNonInit<Polygon2D>(2);
		kari->Init((SCREEN_WIDTH / 4 * 3 + 50.0f) + ((i + 1) * 30.0f), SCREEN_HEIGHT / 7 * 6, 30.0f, 30.0f, "asset\\texture\\stockImage.png");
		enemy->SetStock(kari, i);
	}

	//�J�E���g�_�E��������
	for (int i = 0; i < 3; i++)
	{
		m_CountDown[i] = 60;
	}
	m_CountDown[3] = 100;

	m_CountDownImage = AddGameObjectNonInit<Polygon2D>(2);//"asset\\texture\\field002.jpg"
	m_CountDownImage->Init(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, m_CountDownImagePass[0][0]);
	m_CountDownImage->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));//����

	//�v���Z�X
	m_ProcessImage = AddGameObjectNonInit<Polygon2D>(2);//"asset\\texture\\field002.jpg"
	m_ProcessImage->Init(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, m_ProcessImagePass[0][0]);
	m_ProcessImage->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));//

	//Pause���UI
	m_PauseImage = AddGameObjectNonInit<Polygon2D>(2);//"asset\\texture\\field002.jpg"
	m_PauseImage->Init(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, m_UiImage[0][0]);
	m_PauseImage->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));


	char filename[] = "asset\\BGM\\B00158_kamatamago_enemy-central.wav";
	GameBGM = LoadSound(filename);
	PlaySound(GameBGM, -1);//�^�C�g����ʂ�BGM�Đ�

	//�v���C���[���X�|�[���摜
	{
		RespornImage* kari = AddGameObject<RespornImage>(1);
		player->SetRespornImage(kari);
		kari->SetPosition(D3DXVECTOR3(-1.0f, 0.0f, -4.0f));
	}

	//�G�l�~�[���X�|�[���摜
	{
		RespornImage* kari = AddGameObject<RespornImage>(1);
		enemy->SetRespornImage(kari);
		kari->SetPosition(D3DXVECTOR3(1.0f, 0.0f, -4.0f));
	}

}

void Game::Uninit()
{
	StopSound(GameBGM);//�^�C�g���T�E���h�̍Đ���~

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
	//		m_KillShotImage[i]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));//����
	//	}
	//}

	if (isFinishProcess == false)
	{
		//�i�ށ@�߂�
		switch (m_ProcessCount)
		{
		case 0:
			//�i��
			if (GetKeyboardTrigger(DIK_W) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_B))
			{
				m_ProcessCount++;
				m_ProcessImage->SetImage(m_ProcessImagePass[1][0]);//�R���g���[���[
			}
			//�߂�
			if (GetKeyboardTrigger(DIK_S) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_A))
			{
				Manager::SetScene<Select>();
			}
			break;
		case 1:
			//�i��
			if (GetKeyboardTrigger(DIK_W) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_B))
			{
				isFinishProcess = true;
				m_ProcessImage->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));//������
				m_CountDownImage->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));//�J�E���g�_�E���J�n
			}
			//�߂�
			if (GetKeyboardTrigger(DIK_S) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_A))
			{
				m_ProcessCount--;
				m_ProcessImage->SetImage(m_ProcessImagePass[0][0]);//�R���g���[���[
			}

			break;
		default:
			break;
		}

	}

	{ //�J�E���g�_�E��
		//if() �v���Z�X���I�������
		if (isFinishProcess == true)
		{
			for (int i = 0; i < 3; i++)
			{
				if (m_CountDown[i] != 0)
				{
					m_CountDown[i]--;

					if (m_CountDown[i] == 0)
					{
						//�摜
						switch (i)
						{
						case 0:
							m_CountDownImage->SetImage(m_CountDownImagePass[1][0]);//�f�o�b�O
							break;
						case 1:
							m_CountDownImage->SetImage(m_CountDownImagePass[2][0]);//�f�o�b�O
							break;
						case 2:
							m_CountDownImage->SetImage(m_CountDownImagePass[3][0]);//�f�o�b�O
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

	//�Q�[������
	if (isFinishCountDown == true)
	{
		//XINPUT_GAMEPAD_START
		if ((GetKeyboardTrigger(DIK_H) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_START)) && m_isPause == false)
		{
			m_isPause = true;
			m_PauseImage->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
		}

		//�L���V���b�g����
		//player����
		if (player->GetIsReborn())
		{
			//�L���V���b�g�`��
			m_KillShotImage[0]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
			//�J�E���g�ǉ�
			m_KillShotTime[0] = 60;
			//update�ŃJ�E���g�����Ă���
		}

		//enemy����
		if (enemy->GetIsReborn())
		{
			//�L���V���b�g�`��
			m_KillShotImage[1]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
			//�J�E���g�ǉ�
			m_KillShotTime[1] = 60;
			//update�ŃJ�E���g�����Ă���
		}

		//�L���V���b�g�^�C������
		if (player->GetHideTime() != 0)
		{
			player->PullHideTime();
			if (player->GetHideTime() <= 60)
			{
				m_KillShotImage[0]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));//����
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
				m_KillShotImage[1]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));//����
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
					////�Q�[���ĊJ
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
					////��{����摜�`��
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
					////�Z���N�g��ʂɑJ��
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
					////�Q�[���I��
					exit(988);
				}

				if (GetKeyboardTrigger(DIK_UPARROW) || IsButtonTriggeredX(0, XINPUT_GAMEPAD_DPAD_UP))
				{
					m_PauseImage->SetImage(m_UiImage[2][0]);
					m_ImageCount--;
				}

				break;
			default :
				//�߂�
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