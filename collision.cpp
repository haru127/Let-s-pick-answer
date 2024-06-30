#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "scene.h"
#include "gameObject.h"
#include "enemy.h"
#include "player.h"
#include "collision.h"
#include "bodyCollision.h"
#include "hp.h"
#include "explosion.h"
#include "burst.h"
#include "playerActionCollision.h"
#include "enemyActionCollision.h"
#include <DirectXMath.h>
#include "sound.h"
int Collision::m_hitStop = 0;//�ÓI�����o�ϐ��͍Đ錾���K�v

//�g�̂̉�����������
void Collision::BodyCollisionBB()
{
	Scene* scene = Manager::GetScene();

	//�v���C���[�@�G�l�~�[�̃k���`�F�b�N
	if (scene->GetGameObject<Player>() != NULL && scene->GetGameObject<Enemy>() != NULL)
	{
		//Enemy* enemy = scene->GetGameObject<Enemy>();
		std::vector<Enemy*> enemies = scene->GetGameObjects<Enemy>();//�^����auto�ł���

		Player* player = scene->GetGameObject<Player>();

		for (Enemy* enemy : enemies)//�͈�for���[�v
		{
			BodyCollision*  enemyBody = enemy->GetBodyCollision();//�^����auto�ł���
			BodyCollision*  playerBody = player->GetBodyCollision();

			//�G��
			if (enemyBody->GetPosition().x - enemyBody->GetSize().x / 2 < playerBody->GetPosition().x + playerBody->GetSize().x / 2
				&& enemyBody->GetPosition().y - enemyBody->GetSize().y / 2 <= playerBody->GetPosition().y + playerBody->GetSize().y / 2
				&& playerBody->GetPosition().y - playerBody->GetSize().y / 2 <= enemyBody->GetPosition().y + enemyBody->GetSize().y / 2
				&& playerBody->GetOldPosition().x + playerBody->GetSize().x / 2 < enemyBody->GetOldPosition().x - enemyBody->GetSize().x / 2)
			{
				//���W�␳
				player->SetPosition(player->GetOldPosition());
				playerBody->SetPosition(playerBody->GetOldPosition());

				enemy->SetPosition(enemy->GetOldPosition());
				enemyBody->SetPosition(enemyBody->GetOldPosition());
			}
			//�G�E
			else if (playerBody->GetPosition().x - playerBody->GetSize().x / 2 < enemyBody->GetPosition().x + enemyBody->GetSize().x / 2
				&& enemyBody->GetPosition().y - enemyBody->GetSize().y / 2 <= playerBody->GetPosition().y + playerBody->GetSize().y / 2
				&& playerBody->GetPosition().y - playerBody->GetSize().y / 2 <= enemyBody->GetPosition().y + enemyBody->GetSize().y / 2
				&& enemyBody->GetOldPosition().x + enemyBody->GetSize().x / 2 < playerBody->GetOldPosition().x - playerBody->GetSize().x / 2)
			{
				//���W�␳
				player->SetPosition(player->GetOldPosition());
				playerBody->SetPosition(playerBody->GetOldPosition());

				enemy->SetPosition(enemy->GetOldPosition());
				enemyBody->SetPosition(enemyBody->GetOldPosition());
			}
		}
	}
	else {
		return;
	}
}

void Collision::PlayerAttackCollisionBB()
{
	Scene* scene = Manager::GetScene();

	//�v���C���[�@�G�l�~�[�̃k���`�F�b�N
	if (scene->GetGameObject<Player>() != NULL && scene->GetGameObject<Enemy>() != NULL)
	{
		Player* player = scene->GetGameObject<Player>();
		//Enemy* enemy = scene->GetGameObject<Enemy>();

		std::vector<Enemy*> enemies = scene->GetGameObjects<Enemy>();//�^����auto�ł���

		for (Enemy* enemy : enemies)//�͈�for���[�v
		{

			BodyCollision*  enemyBody = enemy->GetBodyCollision();//�^����auto�ł���

			if (enemy->GetHideTime() == 0)
			{
				//�v���C���[���U�����Ă���Ȃ�
				if (scene->GetGameObject<PlayerActionCollision>() != NULL)
				{
					PlayerActionCollision* playerActionCollition = scene->GetGameObject<PlayerActionCollision>();

					//�����蔻��
					//�G�E
					if (enemyBody->GetPosition().x - enemyBody->GetSize().x / 2 < playerActionCollition->GetPosition().x + playerActionCollition->GetSize().x / 2
						&& enemyBody->GetPosition().y - enemyBody->GetSize().y / 2 <= playerActionCollition->GetPosition().y + playerActionCollition->GetSize().y / 2
						&& playerActionCollition->GetPosition().y - playerActionCollition->GetSize().y / 2 <= enemyBody->GetPosition().y + enemyBody->GetSize().y / 2
						&& player->GetPosition().x < enemy->GetPosition().x //���W����
						/*&&*/ /*playerActionCollition->GetOldPosition().x < enemyBody->GetOldPosition().x*/)
					{
						//if () {}���Gor�K�[�h,���g
						//����
						if (player->GetAttackType() == 4)
						{
							player->SetHitSE();

							//������ђ�
							enemy->SetCool();

							//�G���_�E����ԁA����������
							enemy->SetHitGrab();
							enemy->GetHpPointer()->AddCount(playerActionCollition->GetDamage());//�_���[�W

							enemy->SetVecX(0.5f);
							enemy->SetVecY(-0.5f);
							if (IsBurst(0.5f, -0.5f, enemy))
							{
								//�v��
								scene->AddGameObject<Burst>(1)->SetPosition(D3DXVECTOR3(enemy->GetPosition().x, enemy->GetPosition().y + 1.0f, enemy->GetPosition().z));
								Collision::SetHitStop();
							}
							else
							{
								scene->AddGameObject<Explosion>(1)->SetPosition(D3DXVECTOR3(enemy->GetPosition().x, enemy->GetPosition().y + 1.0f, enemy->GetPosition().z));
							}

							enemy->SetPosition(D3DXVECTOR3(player->GetPosition().x + player->GetVecX(), player->GetPosition().y + player->GetVecY(), player->GetPosition().z));

							playerActionCollition->DeleteCollision(); //�����t���[��������.

						}
						//���݈ȊO�̍U��
						else
						{
							//�K�[�h�����Ă��Ȃ�
							if (!(enemy->isGuard()) || (enemy->GetIsFaceIsRight() == true && enemy->isGuard() == true))
							{
								player->SetHitSE();


								//�U���̃I���h�|�X���X�V
								//playerActionCollition->SetOldPosition(playerActionCollition->GetPosition());

								//�����艼����
								//enemy->SetPosition(D3DXVECTOR3(enemy->GetPosition().x + 2.0f, enemy->GetPosition().y, enemy->GetPosition().z));

								//�ӂ��Ƃ΂�
								float speed = playerActionCollition->GetSpeed(); // ���x
								float Angle = playerActionCollition->GetAngle();//�p�x
								enemy->GetHpPointer()->AddCount(playerActionCollition->GetDamage());//�_���[�W

								//�p�[�Z���g�W��
								speed += (enemy->GetHpPointer()->GetCount() / 2) * 0.1;

								// �Ȑ��I�Ȕ�����^����֐��i��Fsin�j
								float curveFactor = sin(DirectX::XMConvertToRadians(playerActionCollition->GetAngle()));

								// �Ȑ��I�ȑ��x�̌v�Z
								float curvedSpeed = speed * curveFactor;

								//enemy->SetVecX(speed * cos(radianAngle));
								//enemy->SetVecY(speed * sin(radianAngle));
								enemy->SetVecX(curvedSpeed * cos(DirectX::XMConvertToRadians(Angle)));
								enemy->SetVecY(curvedSpeed * sin(DirectX::XMConvertToRadians(Angle)));
								//�v��
								if (IsBurst(curvedSpeed * cos(DirectX::XMConvertToRadians(Angle)), curvedSpeed * sin(DirectX::XMConvertToRadians(Angle)), enemy))
								{
									scene->AddGameObject<Burst>(1)->SetPosition(D3DXVECTOR3(enemy->GetPosition().x, enemy->GetPosition().y + 1.0f, enemy->GetPosition().z));
									Collision::SetHitStop();
								}
								else
								{
									scene->AddGameObject<Explosion>(1)->SetPosition(D3DXVECTOR3(enemy->GetPosition().x, enemy->GetPosition().y + 1.0f, enemy->GetPosition().z));
								}
								//float velocityX = speed * cos(radianAngle);
								//float velocityY = speed * sin(radianAngle);
								//enemy->SetPosition(D3DXVECTOR3(enemy->GetPosition().x + enemy->GetVecX(), enemy->GetPosition().y + enemy->GetVecY(), enemy->GetPosition().z));

								playerActionCollition->DeleteCollision();

								//������ђ�
								enemy->SetCool();
							}
							else //�K�[�h��
							{
								scene->AddGameObject<Explosion>(1)->SetPosition(D3DXVECTOR3(enemy->GetPosition().x, enemy->GetPosition().y + 1.0f, enemy->GetPosition().z));
							}
						}
					}

					//�G��
					if (playerActionCollition->GetPosition().x - playerActionCollition->GetSize().x / 2 < enemyBody->GetPosition().x + enemyBody->GetSize().x / 2
						&& enemyBody->GetPosition().y - enemyBody->GetSize().y / 2 <= playerActionCollition->GetPosition().y + playerActionCollition->GetSize().y / 2
						&& playerActionCollition->GetPosition().y - playerActionCollition->GetSize().y / 2 <= enemyBody->GetPosition().y + enemyBody->GetSize().y / 2
						&& enemy->GetPosition().x < player->GetPosition().x //���W����
						)
					{
						//����
						if (player->GetAttackType() == 4)
						{
							player->SetHitSE();


							//������ђ�
							enemy->SetCool();

							//�G���_�E����ԁA����������
							enemy->SetHitGrab();
							enemy->GetHpPointer()->AddCount(playerActionCollition->GetDamage());//�_���[�W

							enemy->SetVecX(-0.5f);
							enemy->SetVecY(-0.5f);
							if (IsBurst(-0.5f, -0.5f, enemy))
							{
								scene->AddGameObject<Burst>(1)->SetPosition(D3DXVECTOR3(enemy->GetPosition().x, enemy->GetPosition().y + 1.0f, enemy->GetPosition().z));
								Collision::SetHitStop();
							}
							else
							{
								scene->AddGameObject<Explosion>(1)->SetPosition(D3DXVECTOR3(enemy->GetPosition().x, enemy->GetPosition().y + 1.0f, enemy->GetPosition().z));
							}


							enemy->SetPosition(D3DXVECTOR3(player->GetPosition().x + player->GetVecX(), player->GetPosition().y + player->GetVecY(), player->GetPosition().z));

							playerActionCollition->DeleteCollision(); //�����t���[��������.
						}
						//���݈ȊO�̍U��
						else
						{
							//�K�[�h�����Ă��Ȃ�
							if (!(enemy->isGuard()) || (enemy->GetIsFaceIsRight() == false && enemy->isGuard() == true))
							{
								player->SetHitSE();

								//�U���̃I���h�|�X���X�V
								//playerActionCollition->SetOldPosition(playerActionCollition->GetPosition());

								//�����艼����
								//enemy->SetPosition(D3DXVECTOR3(enemy->GetPosition().x + 2.0f, enemy->GetPosition().y, enemy->GetPosition().z));

								//�ӂ��Ƃ΂�
								float speed = playerActionCollition->GetSpeed(); // ���x
								float Angle = playerActionCollition->GetAngle();//�p�x
								enemy->GetHpPointer()->AddCount(playerActionCollition->GetDamage());//�_���[�W

								//�p�[�Z���g�W��
								if (speed < 0)
								{
									speed -= (enemy->GetHpPointer()->GetCount() / 2) * 0.1;
								}
								else {
									speed += (enemy->GetHpPointer()->GetCount() / 2) * 0.1;
								}

								// �Ȑ��I�Ȕ�����^����֐��i��Fsin�j
								float curveFactor = sin(DirectX::XMConvertToRadians(playerActionCollition->GetAngle()));

								// �Ȑ��I�ȑ��x�̌v�Z
								float curvedSpeed = speed * curveFactor;

								//enemy->SetVecX(speed * cos(radianAngle));
								//enemy->SetVecY(speed * sin(radianAngle));
								enemy->SetVecX(curvedSpeed * cos(DirectX::XMConvertToRadians(Angle)));
								enemy->SetVecY(curvedSpeed * sin(DirectX::XMConvertToRadians(Angle)));
								if (IsBurst(curvedSpeed * cos(DirectX::XMConvertToRadians(Angle)), curvedSpeed * sin(DirectX::XMConvertToRadians(Angle)), enemy))
								{
									scene->AddGameObject<Burst>(1)->SetPosition(D3DXVECTOR3(enemy->GetPosition().x, enemy->GetPosition().y + 1.0f, enemy->GetPosition().z));
									Collision::SetHitStop();
								}
								else
								{
									scene->AddGameObject<Explosion>(1)->SetPosition(D3DXVECTOR3(enemy->GetPosition().x, enemy->GetPosition().y + 1.0f, enemy->GetPosition().z));
								}
								//float velocityX = speed * cos(radianAngle);
								//float velocityY = speed * sin(radianAngle);
								enemy->SetPosition(D3DXVECTOR3(enemy->GetPosition().x + enemy->GetVecX(), enemy->GetPosition().y + enemy->GetVecY(), enemy->GetPosition().z));

								playerActionCollition->DeleteCollision();

								//������ђ�
								enemy->SetCool();
							}
							else
							{
								scene->AddGameObject<Explosion>(1)->SetPosition(D3DXVECTOR3(enemy->GetPosition().x, enemy->GetPosition().y + 1.0f, enemy->GetPosition().z));
							}
						}
					}

					//���������炷
					playerActionCollition->PullZizokuFrame();
				}
				else {
					return;
				}
			}
			else
			{
				return;
			}
		}
	}
 else 
	{
	return;
	}
}

void Collision::EnemyAttackCollisionBB()
{
	Scene* scene = Manager::GetScene();

	//�v���C���[�@�G�l�~�[�̃k���`�F�b�N
	if (scene->GetGameObject<Player>() != NULL && scene->GetGameObject<Enemy>() != NULL)
	{
		Player* player = scene->GetGameObject<Player>();
		Enemy* enemy = scene->GetGameObject<Enemy>();

		BodyCollision*  playerBody = player->GetBodyCollision();//�^����auto�ł���

		//���G���ԂłȂ��̂Ȃ�
		if (player->GetHideTime() == 0)
		{
			//�G�l�~�[���U�����Ă���Ȃ�
			if (scene->GetGameObject<EnemyActionCollision>() != NULL)
			{
				EnemyActionCollision* enemyActionCollision = scene->GetGameObject<EnemyActionCollision>();

				//�K�[�h�����Ă��Ȃ��Ȃ�
				//�����蔻��
				//�G�E
				if (playerBody->GetPosition().x - playerBody->GetSize().x / 2 < enemyActionCollision->GetPosition().x + enemyActionCollision->GetSize().x / 2
					&& playerBody->GetPosition().y - playerBody->GetSize().y / 2 <= enemyActionCollision->GetPosition().y + enemyActionCollision->GetSize().y / 2
					&& enemyActionCollision->GetPosition().y - enemyActionCollision->GetSize().y / 2 <= playerBody->GetPosition().y + playerBody->GetSize().y / 2
					&& enemy->GetPosition().x < player->GetPosition().x //���W����
					/*&&*/ /*enemyActionCollision->GetOldPosition().x < playerBody->GetOldPosition().x*/)
				{
					//����
					if (enemy->GetAttackType() == 4)
					{
						player->SetHitSE();

						//������ђ�
						player->SetCool();

						//�G���_�E����ԁA����������
						player->SetHitGrab();
						player->GetHpPointer()->AddCount(enemyActionCollision->GetDamage());//�_���[�W

						player->SetVecX(0.5f);
						player->SetVecY(-0.5f);
						if (IsBurst(0.5f, -0.5f, player))
						{
							scene->AddGameObject<Burst>(1)->SetPosition(D3DXVECTOR3(player->GetPosition().x, player->GetPosition().y + 1.0f, player->GetPosition().z));
							Collision::SetHitStop();
						}
						else
						{
							scene->AddGameObject<Explosion>(1)->SetPosition(D3DXVECTOR3(player->GetPosition().x, player->GetPosition().y + 1.0f, player->GetPosition().z));
						}


						player->SetPosition(D3DXVECTOR3(player->GetPosition().x + player->GetVecX(), player->GetPosition().y + player->GetVecY(), player->GetPosition().z));

						enemyActionCollision->DeleteCollision(); //�����t���[��������.
					}
					//���݈ȊO�̍U��
					else
					{
						if (!(player->isGuard()) || (player->GetIsFaceIsRight() == true && enemy->isGuard() == true))
						{
							player->SetHitSE();

							//�U���̃I���h�|�X���X�V
							//enemyActionCollision->SetOldPosition(enemyActionCollision->GetPosition());

							//�����艼����
							//enemy->SetPosition(D3DXVECTOR3(enemy->GetPosition().x + 2.0f, enemy->GetPosition().y, enemy->GetPosition().z));

							//�ӂ��Ƃ΂�
							float speed = enemyActionCollision->GetSpeed(); // ���x
							float Angle = enemyActionCollision->GetAngle();//�p�x
							player->GetHpPointer()->AddCount(enemyActionCollision->GetDamage());//�_���[�W

							//�p�[�Z���g�W��
							speed += (player->GetHpPointer()->GetCount() / 2) * 0.1;

							// �Ȑ��I�Ȕ�����^����֐��i��Fsin�j
							float curveFactor = sin(DirectX::XMConvertToRadians(enemyActionCollision->GetAngle()));

							// �Ȑ��I�ȑ��x�̌v�Z
							float curvedSpeed = speed * curveFactor;

							player->SetVecX(curvedSpeed * cos(DirectX::XMConvertToRadians(Angle)));
							player->SetVecY(curvedSpeed * sin(DirectX::XMConvertToRadians(Angle)));
							if (IsBurst(curvedSpeed * cos(DirectX::XMConvertToRadians(Angle)), curvedSpeed * sin(DirectX::XMConvertToRadians(Angle)),player))
							{
								scene->AddGameObject<Burst>(1)->SetPosition(D3DXVECTOR3(player->GetPosition().x, player->GetPosition().y + 1.0f, player->GetPosition().z));
								Collision::SetHitStop();
							}
							else
							{
								scene->AddGameObject<Explosion>(1)->SetPosition(D3DXVECTOR3(player->GetPosition().x, player->GetPosition().y + 1.0f, player->GetPosition().z));
							}
							//player->SetVecX(0.5f);
							//player->SetVecY(-0.5f);

							player->SetPosition(D3DXVECTOR3(player->GetPosition().x + player->GetVecX(), player->GetPosition().y + player->GetVecY(), player->GetPosition().z));
							//player->SetHited();

							enemyActionCollision->DeleteCollision();

							//������ђ�
							player->SetCool();
						}
						//�K�[�h��
						else
						{
							scene->AddGameObject<Explosion>(1)->SetPosition(D3DXVECTOR3(player->GetPosition().x, player->GetPosition().y + 1.0f, player->GetPosition().z));
						}
					}
				}

				//�G��
				if (enemyActionCollision->GetPosition().x - enemyActionCollision->GetSize().x / 2 < playerBody->GetPosition().x + playerBody->GetSize().x / 2
					&& playerBody->GetPosition().y - playerBody->GetSize().y / 2 <= enemyActionCollision->GetPosition().y + enemyActionCollision->GetSize().y / 2
					&& enemyActionCollision->GetPosition().y - enemyActionCollision->GetSize().y / 2 <= playerBody->GetPosition().y + playerBody->GetSize().y / 2
					&& player->GetPosition().x < enemy->GetPosition().x //���W����
					)
				{
					//����
					if (enemy->GetAttackType() == 4)
					{
						player->SetHitSE();

						//������ђ�
						player->SetCool();

						//�G���_�E����ԁA����������
						player->SetHitGrab();
						player->GetHpPointer()->AddCount(enemyActionCollision->GetDamage());//�_���[�W

						player->SetVecX(-0.5f);
						player->SetVecY(-0.5f);
						if (IsBurst(-0.5f, -0.5f,player))
						{
							scene->AddGameObject<Burst>(1)->SetPosition(D3DXVECTOR3(player->GetPosition().x, player->GetPosition().y + 1.0f, player->GetPosition().z));
							Collision::SetHitStop();
						}
						else
						{
							scene->AddGameObject<Explosion>(1)->SetPosition(D3DXVECTOR3(player->GetPosition().x, player->GetPosition().y + 1.0f, player->GetPosition().z));
						}

						player->SetPosition(D3DXVECTOR3(player->GetPosition().x + player->GetVecX(), player->GetPosition().y + player->GetVecY(), player->GetPosition().z));

						enemyActionCollision->DeleteCollision(); //�����t���[��������.
					}
					//���݈ȊO�̍U��
					else
					{
						if (!(player->isGuard()) || (player->GetIsFaceIsRight() == false && enemy->isGuard() == true))
						{
							player->SetHitSE();

							//�U���̃I���h�|�X���X�V
							//enemyActionCollision->SetOldPosition(enemyActionCollision->GetPosition());

							//�����艼����
							//enemy->SetPosition(D3DXVECTOR3(enemy->GetPosition().x + 2.0f, enemy->GetPosition().y, enemy->GetPosition().z));

							//�ӂ��Ƃ΂�
							float speed = enemyActionCollision->GetSpeed(); // ���x
							float Angle = enemyActionCollision->GetAngle();//�p�x
							player->GetHpPointer()->AddCount(enemyActionCollision->GetDamage());//�_���[�W

							//�p�[�Z���g�W��
							if (speed < 0)
							{
								speed -= (player->GetHpPointer()->GetCount() / 2) * 0.1;
							}
							else {
								speed += (player->GetHpPointer()->GetCount() / 2) * 0.1;
							}

							// �Ȑ��I�Ȕ�����^����֐��i��Fsin�j
							float curveFactor = sin(DirectX::XMConvertToRadians(enemyActionCollision->GetAngle()));

							// �Ȑ��I�ȑ��x�̌v�Z
							float curvedSpeed = speed * curveFactor;

							player->SetVecX(curvedSpeed * cos(DirectX::XMConvertToRadians(Angle)));
							player->SetVecY(curvedSpeed * sin(DirectX::XMConvertToRadians(Angle)));
							if (IsBurst(curvedSpeed * cos(DirectX::XMConvertToRadians(Angle)), curvedSpeed * sin(DirectX::XMConvertToRadians(Angle)), player))
							{
								scene->AddGameObject<Burst>(1)->SetPosition(D3DXVECTOR3(player->GetPosition().x, player->GetPosition().y + 1.0f, player->GetPosition().z));
								Collision::SetHitStop();
							}
							else
							{
								scene->AddGameObject<Explosion>(1)->SetPosition(D3DXVECTOR3(player->GetPosition().x, player->GetPosition().y + 1.0f, player->GetPosition().z));
							}
							//player->SetVecX(-0.1f);
							//player->SetVecY(-0.5f);

							player->SetPosition(D3DXVECTOR3(player->GetPosition().x + player->GetVecX(), player->GetPosition().y + player->GetVecY(), player->GetPosition().z));
							//player->SetHited();

							enemyActionCollision->DeleteCollision();

							//������ђ�
							player->SetCool();
						}
						//�K�[�h��
						else
						{
							scene->AddGameObject<Explosion>(1)->SetPosition(D3DXVECTOR3(player->GetPosition().x, player->GetPosition().y + 1.0f, player->GetPosition().z));
						}
					}
				}

				//���������炷
				enemyActionCollision->PullZizokuFrame();
			}
			else {
				return;
			}
		} 
		else
		{
			return;
		 }
	}
	else
	{
		return;
	}

}

bool Collision::IsBurst(float vecX, float vecY, GameObject* gameobject)
{
	//�|�W�V������ϐ��ɕۑ�
	D3DXVECTOR3 reservePos = gameobject->GetPosition();
	//�Փˌ�
	while (vecX != 0.0f && vecY != 0.0f)
	{
		if (vecX != 0.0f)
		{
			reservePos.x += vecX;
			vecX *= 0.9f;
		}


		if ((fabsf(vecX) - 0.1f) < 0.0f)
		{
			vecX = 0.0f;

			if (vecY == 0.0f)
			{
				break;
			}
		}

		if (vecY != 0.0f)
		{
			reservePos.y += vecY;
			vecY *= 0.9f;

		}

		if ((fabsf(vecY) - 0.1f) < 0.0f)
		{
			vecY = 0.0f;

			if (vecX == 0.0f)
			{
				break;
			}

		}


	}

	if (50.0f < reservePos.x || reservePos.x < -50.0f)
	{
		return true;
	}
	else
	{
		return false;
	}

	//while�ɔ����āA���E�l�𒴂�����return true��Ԃ��ăq�b�g�X�g�b�v�@���ĉ��o��t���銴��
}

