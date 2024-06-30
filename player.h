#pragma once

#include "gameObject.h"
#include <string>

#define PlayerAttackType (13)

class Player : public GameObject //継承
{

private:
	class AnimationModel* m_Model{};
	class BodyCollision*  m_body{};
	class Name*           m_name{};
	class Hp* m_Hp{};
	class Polygon2D* m_Stock[3];
	class RespornImage* m_RespornImage;

	ID3D11VertexShader* m_VertexShader{};
	ID3D11PixelShader* m_PixelShader{};
	ID3D11InputLayout* m_VertexLayout{};

	D3DXVECTOR3 m_Velocity{};
	class Shadow* m_Shadow{};
	D3DXMATRIX m_Matrix;

	int m_Time{};
	float m_BlendRate{};
	std::string m_AnimationName;
	std::string m_NextAnimationName;
	int m_AnimationFrame{};//各種攻撃の全体Frame
	int m_GuardFrame{};
	int punchStartFrame{};
	int m_DownTime{};
	int m_HideTime{};
	int m_ChanceTime{};
	
	//コリジョン関連
	float m_DistancePlayerCollisionX{};
	float m_DistancePlayerCollisionY{};

	bool m_FaceIsRight = true;
	bool m_IsField	   = true; //着地しているかどうか
	bool m_Move = false;
	bool m_isAttack[PlayerAttackType];//各種攻撃が発生中かどうか　0から弱,下横上、つかみ,shot

	//ジャンプ
	bool m_isFieldJump = false;
	bool m_isSkyJump = false;
	bool m_isRightTriggerOK = false;

	bool m_IsGuard = false;
	bool m_IsCool = false;//trueは吹っ飛び中
	bool m_IsChance = false;//trueは硬直中
	bool m_IsHited = false;//攻撃を当てられたかどうか
	bool m_IsGrabHit = false;//つかみをくらっているか
	bool m_isPushFinished = false;//ふっとびが終わり
	bool m_isReBorn = false;

	D3DXVECTOR3 offsetCollision = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	//サウンド関連
	static int m_HitSE[2];//攻撃がヒットした時のSE,撃墜SE
public:
	void Init();
	void Uninit();
	void Update();
	void Draw();

	void SetHp(Hp* pHp) { m_Hp = pHp; }
	void SetStock(Polygon2D* pPo, int num) { m_Stock[num] = pPo; }
	void SetRespornImage(RespornImage* pI) { m_RespornImage = pI; }
	void SetDistancePlayerCollisionX(float disX) { m_DistancePlayerCollisionX = disX; }
	void SetDistancePlayerCollisionY(float disY) { m_DistancePlayerCollisionY = disY; }
	void PullHideTime() { m_HideTime--; }

	void SetCool() { m_IsCool = true,m_Time = 0; }
	void SetDownTime() { m_DownTime = 5; }
	void SetChanceTime() { m_ChanceTime = 10; }
	void SetHited() { m_IsHited = true; }
	void SetHitGrab() { m_IsGrabHit = true; }

	//angle,speed,damage,プレイヤーからのxの距離、プレイヤーからのyの距離
    void AddCollid(float x, float y, float sizeX, float sizeY, int zFrame, bool isRight);
	void AddCollid(float x, float y, float sizeX, float sizeY, int zFrame, bool isRight, float angle, float speed, float damage, float distX, float distY);

	bool isAttack();//何らかの攻撃をしている最中かどうか
	bool isAttackCollid();
	bool isGuard() { return m_IsGuard; }//何らかの攻撃をしている最中かどうか
	bool isHitGrab() { return m_IsGrabHit; }//

	BodyCollision* GetBodyCollision();
	D3DXMATRIX GetMatrix() { return m_Matrix; }
	Hp* GetHpPointer() { return m_Hp; }
	float GetDistancePlayerCollisionX() { return m_DistancePlayerCollisionX; }
	float GetDistancePlayerCollisionY() { return m_DistancePlayerCollisionY; }
	bool  GetCool() { return m_IsCool; }
	int   GetAttackType();
	int   GetHideTime() { return m_HideTime; }
	bool  GetIsReborn() { return m_isReBorn; }
	RespornImage* GetRespornImage() { return m_RespornImage; }
	bool  GetIsFaceIsRight() { return m_FaceIsRight; }

    void SetHitSE();

	template <typename T>
	bool isSceneDebug(Scene* scene)
	{
		if (typeid(*scene) == typeid(T))
		{
			return true;
		}
		else {
			return false;
		}
	}
};