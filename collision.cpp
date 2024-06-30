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
int Collision::m_hitStop = 0;//静的メンバ変数は再宣言が必要

//身体の押し合い判定
void Collision::BodyCollisionBB()
{
	Scene* scene = Manager::GetScene();

	//プレイヤー　エネミーのヌルチェック
	if (scene->GetGameObject<Player>() != NULL && scene->GetGameObject<Enemy>() != NULL)
	{
		//Enemy* enemy = scene->GetGameObject<Enemy>();
		std::vector<Enemy*> enemies = scene->GetGameObjects<Enemy>();//型名はautoでも可

		Player* player = scene->GetGameObject<Player>();

		for (Enemy* enemy : enemies)//範囲forループ
		{
			BodyCollision*  enemyBody = enemy->GetBodyCollision();//型名はautoでも可
			BodyCollision*  playerBody = player->GetBodyCollision();

			//敵左
			if (enemyBody->GetPosition().x - enemyBody->GetSize().x / 2 < playerBody->GetPosition().x + playerBody->GetSize().x / 2
				&& enemyBody->GetPosition().y - enemyBody->GetSize().y / 2 <= playerBody->GetPosition().y + playerBody->GetSize().y / 2
				&& playerBody->GetPosition().y - playerBody->GetSize().y / 2 <= enemyBody->GetPosition().y + enemyBody->GetSize().y / 2
				&& playerBody->GetOldPosition().x + playerBody->GetSize().x / 2 < enemyBody->GetOldPosition().x - enemyBody->GetSize().x / 2)
			{
				//座標補正
				player->SetPosition(player->GetOldPosition());
				playerBody->SetPosition(playerBody->GetOldPosition());

				enemy->SetPosition(enemy->GetOldPosition());
				enemyBody->SetPosition(enemyBody->GetOldPosition());
			}
			//敵右
			else if (playerBody->GetPosition().x - playerBody->GetSize().x / 2 < enemyBody->GetPosition().x + enemyBody->GetSize().x / 2
				&& enemyBody->GetPosition().y - enemyBody->GetSize().y / 2 <= playerBody->GetPosition().y + playerBody->GetSize().y / 2
				&& playerBody->GetPosition().y - playerBody->GetSize().y / 2 <= enemyBody->GetPosition().y + enemyBody->GetSize().y / 2
				&& enemyBody->GetOldPosition().x + enemyBody->GetSize().x / 2 < playerBody->GetOldPosition().x - playerBody->GetSize().x / 2)
			{
				//座標補正
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

	//プレイヤー　エネミーのヌルチェック
	if (scene->GetGameObject<Player>() != NULL && scene->GetGameObject<Enemy>() != NULL)
	{
		Player* player = scene->GetGameObject<Player>();
		//Enemy* enemy = scene->GetGameObject<Enemy>();

		std::vector<Enemy*> enemies = scene->GetGameObjects<Enemy>();//型名はautoでも可

		for (Enemy* enemy : enemies)//範囲forループ
		{

			BodyCollision*  enemyBody = enemy->GetBodyCollision();//型名はautoでも可

			if (enemy->GetHideTime() == 0)
			{
				//プレイヤーが攻撃しているなら
				if (scene->GetGameObject<PlayerActionCollision>() != NULL)
				{
					PlayerActionCollision* playerActionCollition = scene->GetGameObject<PlayerActionCollision>();

					//当たり判定
					//敵右
					if (enemyBody->GetPosition().x - enemyBody->GetSize().x / 2 < playerActionCollition->GetPosition().x + playerActionCollition->GetSize().x / 2
						&& enemyBody->GetPosition().y - enemyBody->GetSize().y / 2 <= playerActionCollition->GetPosition().y + playerActionCollition->GetSize().y / 2
						&& playerActionCollition->GetPosition().y - playerActionCollition->GetSize().y / 2 <= enemyBody->GetPosition().y + enemyBody->GetSize().y / 2
						&& player->GetPosition().x < enemy->GetPosition().x //座標制御
						/*&&*/ /*playerActionCollition->GetOldPosition().x < enemyBody->GetOldPosition().x*/)
					{
						//if () {}無敵orガード,生身
						//つかみ
						if (player->GetAttackType() == 4)
						{
							player->SetHitSE();

							//吹っ飛び中
							enemy->SetCool();

							//敵をダウン状態、反動をつける
							enemy->SetHitGrab();
							enemy->GetHpPointer()->AddCount(playerActionCollition->GetDamage());//ダメージ

							enemy->SetVecX(0.5f);
							enemy->SetVecY(-0.5f);
							if (IsBurst(0.5f, -0.5f, enemy))
							{
								//致命
								scene->AddGameObject<Burst>(1)->SetPosition(D3DXVECTOR3(enemy->GetPosition().x, enemy->GetPosition().y + 1.0f, enemy->GetPosition().z));
								Collision::SetHitStop();
							}
							else
							{
								scene->AddGameObject<Explosion>(1)->SetPosition(D3DXVECTOR3(enemy->GetPosition().x, enemy->GetPosition().y + 1.0f, enemy->GetPosition().z));
							}

							enemy->SetPosition(D3DXVECTOR3(player->GetPosition().x + player->GetVecX(), player->GetPosition().y + player->GetVecY(), player->GetPosition().z));

							playerActionCollition->DeleteCollision(); //持続フレームを消す.

						}
						//つかみ以外の攻撃
						else
						{
							//ガードをしていない
							if (!(enemy->isGuard()) || (enemy->GetIsFaceIsRight() == true && enemy->isGuard() == true))
							{
								player->SetHitSE();


								//攻撃のオルドポスを更新
								//playerActionCollition->SetOldPosition(playerActionCollition->GetPosition());

								//当たり仮処理
								//enemy->SetPosition(D3DXVECTOR3(enemy->GetPosition().x + 2.0f, enemy->GetPosition().y, enemy->GetPosition().z));

								//ふっとばし
								float speed = playerActionCollition->GetSpeed(); // 速度
								float Angle = playerActionCollition->GetAngle();//角度
								enemy->GetHpPointer()->AddCount(playerActionCollition->GetDamage());//ダメージ

								//パーセント係数
								speed += (enemy->GetHpPointer()->GetCount() / 2) * 0.1;

								// 曲線的な反動を与える関数（例：sin）
								float curveFactor = sin(DirectX::XMConvertToRadians(playerActionCollition->GetAngle()));

								// 曲線的な速度の計算
								float curvedSpeed = speed * curveFactor;

								//enemy->SetVecX(speed * cos(radianAngle));
								//enemy->SetVecY(speed * sin(radianAngle));
								enemy->SetVecX(curvedSpeed * cos(DirectX::XMConvertToRadians(Angle)));
								enemy->SetVecY(curvedSpeed * sin(DirectX::XMConvertToRadians(Angle)));
								//致命
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

								//吹っ飛び中
								enemy->SetCool();
							}
							else //ガード中
							{
								scene->AddGameObject<Explosion>(1)->SetPosition(D3DXVECTOR3(enemy->GetPosition().x, enemy->GetPosition().y + 1.0f, enemy->GetPosition().z));
							}
						}
					}

					//敵左
					if (playerActionCollition->GetPosition().x - playerActionCollition->GetSize().x / 2 < enemyBody->GetPosition().x + enemyBody->GetSize().x / 2
						&& enemyBody->GetPosition().y - enemyBody->GetSize().y / 2 <= playerActionCollition->GetPosition().y + playerActionCollition->GetSize().y / 2
						&& playerActionCollition->GetPosition().y - playerActionCollition->GetSize().y / 2 <= enemyBody->GetPosition().y + enemyBody->GetSize().y / 2
						&& enemy->GetPosition().x < player->GetPosition().x //座標制御
						)
					{
						//つかみ
						if (player->GetAttackType() == 4)
						{
							player->SetHitSE();


							//吹っ飛び中
							enemy->SetCool();

							//敵をダウン状態、反動をつける
							enemy->SetHitGrab();
							enemy->GetHpPointer()->AddCount(playerActionCollition->GetDamage());//ダメージ

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

							playerActionCollition->DeleteCollision(); //持続フレームを消す.
						}
						//つかみ以外の攻撃
						else
						{
							//ガードをしていない
							if (!(enemy->isGuard()) || (enemy->GetIsFaceIsRight() == false && enemy->isGuard() == true))
							{
								player->SetHitSE();

								//攻撃のオルドポスを更新
								//playerActionCollition->SetOldPosition(playerActionCollition->GetPosition());

								//当たり仮処理
								//enemy->SetPosition(D3DXVECTOR3(enemy->GetPosition().x + 2.0f, enemy->GetPosition().y, enemy->GetPosition().z));

								//ふっとばし
								float speed = playerActionCollition->GetSpeed(); // 速度
								float Angle = playerActionCollition->GetAngle();//角度
								enemy->GetHpPointer()->AddCount(playerActionCollition->GetDamage());//ダメージ

								//パーセント係数
								if (speed < 0)
								{
									speed -= (enemy->GetHpPointer()->GetCount() / 2) * 0.1;
								}
								else {
									speed += (enemy->GetHpPointer()->GetCount() / 2) * 0.1;
								}

								// 曲線的な反動を与える関数（例：sin）
								float curveFactor = sin(DirectX::XMConvertToRadians(playerActionCollition->GetAngle()));

								// 曲線的な速度の計算
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

								//吹っ飛び中
								enemy->SetCool();
							}
							else
							{
								scene->AddGameObject<Explosion>(1)->SetPosition(D3DXVECTOR3(enemy->GetPosition().x, enemy->GetPosition().y + 1.0f, enemy->GetPosition().z));
							}
						}
					}

					//持続を減らす
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

	//プレイヤー　エネミーのヌルチェック
	if (scene->GetGameObject<Player>() != NULL && scene->GetGameObject<Enemy>() != NULL)
	{
		Player* player = scene->GetGameObject<Player>();
		Enemy* enemy = scene->GetGameObject<Enemy>();

		BodyCollision*  playerBody = player->GetBodyCollision();//型名はautoでも可

		//無敵時間でないのなら
		if (player->GetHideTime() == 0)
		{
			//エネミーが攻撃しているなら
			if (scene->GetGameObject<EnemyActionCollision>() != NULL)
			{
				EnemyActionCollision* enemyActionCollision = scene->GetGameObject<EnemyActionCollision>();

				//ガードをしていないなら
				//当たり判定
				//敵右
				if (playerBody->GetPosition().x - playerBody->GetSize().x / 2 < enemyActionCollision->GetPosition().x + enemyActionCollision->GetSize().x / 2
					&& playerBody->GetPosition().y - playerBody->GetSize().y / 2 <= enemyActionCollision->GetPosition().y + enemyActionCollision->GetSize().y / 2
					&& enemyActionCollision->GetPosition().y - enemyActionCollision->GetSize().y / 2 <= playerBody->GetPosition().y + playerBody->GetSize().y / 2
					&& enemy->GetPosition().x < player->GetPosition().x //座標制御
					/*&&*/ /*enemyActionCollision->GetOldPosition().x < playerBody->GetOldPosition().x*/)
				{
					//つかみ
					if (enemy->GetAttackType() == 4)
					{
						player->SetHitSE();

						//吹っ飛び中
						player->SetCool();

						//敵をダウン状態、反動をつける
						player->SetHitGrab();
						player->GetHpPointer()->AddCount(enemyActionCollision->GetDamage());//ダメージ

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

						enemyActionCollision->DeleteCollision(); //持続フレームを消す.
					}
					//つかみ以外の攻撃
					else
					{
						if (!(player->isGuard()) || (player->GetIsFaceIsRight() == true && enemy->isGuard() == true))
						{
							player->SetHitSE();

							//攻撃のオルドポスを更新
							//enemyActionCollision->SetOldPosition(enemyActionCollision->GetPosition());

							//当たり仮処理
							//enemy->SetPosition(D3DXVECTOR3(enemy->GetPosition().x + 2.0f, enemy->GetPosition().y, enemy->GetPosition().z));

							//ふっとばし
							float speed = enemyActionCollision->GetSpeed(); // 速度
							float Angle = enemyActionCollision->GetAngle();//角度
							player->GetHpPointer()->AddCount(enemyActionCollision->GetDamage());//ダメージ

							//パーセント係数
							speed += (player->GetHpPointer()->GetCount() / 2) * 0.1;

							// 曲線的な反動を与える関数（例：sin）
							float curveFactor = sin(DirectX::XMConvertToRadians(enemyActionCollision->GetAngle()));

							// 曲線的な速度の計算
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

							//吹っ飛び中
							player->SetCool();
						}
						//ガード中
						else
						{
							scene->AddGameObject<Explosion>(1)->SetPosition(D3DXVECTOR3(player->GetPosition().x, player->GetPosition().y + 1.0f, player->GetPosition().z));
						}
					}
				}

				//敵左
				if (enemyActionCollision->GetPosition().x - enemyActionCollision->GetSize().x / 2 < playerBody->GetPosition().x + playerBody->GetSize().x / 2
					&& playerBody->GetPosition().y - playerBody->GetSize().y / 2 <= enemyActionCollision->GetPosition().y + enemyActionCollision->GetSize().y / 2
					&& enemyActionCollision->GetPosition().y - enemyActionCollision->GetSize().y / 2 <= playerBody->GetPosition().y + playerBody->GetSize().y / 2
					&& player->GetPosition().x < enemy->GetPosition().x //座標制御
					)
				{
					//つかみ
					if (enemy->GetAttackType() == 4)
					{
						player->SetHitSE();

						//吹っ飛び中
						player->SetCool();

						//敵をダウン状態、反動をつける
						player->SetHitGrab();
						player->GetHpPointer()->AddCount(enemyActionCollision->GetDamage());//ダメージ

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

						enemyActionCollision->DeleteCollision(); //持続フレームを消す.
					}
					//つかみ以外の攻撃
					else
					{
						if (!(player->isGuard()) || (player->GetIsFaceIsRight() == false && enemy->isGuard() == true))
						{
							player->SetHitSE();

							//攻撃のオルドポスを更新
							//enemyActionCollision->SetOldPosition(enemyActionCollision->GetPosition());

							//当たり仮処理
							//enemy->SetPosition(D3DXVECTOR3(enemy->GetPosition().x + 2.0f, enemy->GetPosition().y, enemy->GetPosition().z));

							//ふっとばし
							float speed = enemyActionCollision->GetSpeed(); // 速度
							float Angle = enemyActionCollision->GetAngle();//角度
							player->GetHpPointer()->AddCount(enemyActionCollision->GetDamage());//ダメージ

							//パーセント係数
							if (speed < 0)
							{
								speed -= (player->GetHpPointer()->GetCount() / 2) * 0.1;
							}
							else {
								speed += (player->GetHpPointer()->GetCount() / 2) * 0.1;
							}

							// 曲線的な反動を与える関数（例：sin）
							float curveFactor = sin(DirectX::XMConvertToRadians(enemyActionCollision->GetAngle()));

							// 曲線的な速度の計算
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

							//吹っ飛び中
							player->SetCool();
						}
						//ガード中
						else
						{
							scene->AddGameObject<Explosion>(1)->SetPosition(D3DXVECTOR3(player->GetPosition().x, player->GetPosition().y + 1.0f, player->GetPosition().z));
						}
					}
				}

				//持続を減らす
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
	//ポジションを変数に保存
	D3DXVECTOR3 reservePos = gameobject->GetPosition();
	//衝突後
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

	//whileに抜けて、限界値を超えたらreturn trueを返してヒットストップ　撃墜演出を付ける感じ
}

