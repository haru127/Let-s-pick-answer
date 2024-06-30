#pragma once

#include "gameObject.h"
#include <string>

#define EnemyAttackType (13)

enum StatePatern
{
	INIT = 0,
	LONGDIS,
	MIDDLE,
	MIDDLENEAR,
	NEARDIS,
	DEFEND
};

class EnemyState;

class Enemy : public GameObject //�p��
{

private:
	class EnemyManager* m_EnemyManager = nullptr;

	class AnimationModel* m_Model{};
	class BodyCollision*  m_body{};
	class EnemyName*      m_name{};
	class Hp* m_Hp{};
	class Polygon2D* m_Stock[3];
	class RespornImage* m_RespornImage;

	//�G�l�~�[�̃X�e�[�g�}�V��
	StatePatern m_State = StatePatern::INIT;

	ID3D11VertexShader* m_VertexShader{};
	ID3D11PixelShader* m_PixelShader{};
	ID3D11InputLayout* m_VertexLayout{};

	D3DXVECTOR3 m_Velocity{};

	class Shadow* m_Shadow{};
	D3DXMATRIX m_Matrix;

	int m_Time{};
	int m_pTime{};
	float m_BlendRate{};
	std::string m_AnimationName;
	std::string m_NextAnimationName;
	int m_AnimationFrame{};
	int m_GuardFrame{};
	int punchStartFrame{};
	int punchEndFrame{};
	int m_DownTime{};
	int m_HideTime{};
	int m_ChanceTime{};

	//�R���W�����֘A
	float m_DistancePlayerCollisionX{};
	float m_DistancePlayerCollisionY{};

	bool m_FaceIsRight = false;
	bool m_IsField = true; //���n���Ă��邩�ǂ���
	bool m_isAttack[EnemyAttackType];//�e��U�������������ǂ����@0�����,�����㋭�U��

		//�W�����v
	bool m_isFieldJump = false;
	bool m_isSkyJump = false;

	bool m_IsMove = false;//�����Ă��邩�ǂ���
	bool m_IsGuard = false;
	bool m_IsCool = false;//true�͐�����ђ�
	bool m_IsChance = false;//true�͍d����
	bool m_IsHited = false;//�U���𓖂Ă�ꂽ���ǂ���
	bool m_IsGrabHit = false;//���݂�������Ă��邩
	bool m_isPushFinished = false;//�ӂ��Ƃт��I���
	bool m_isReBorn = false;

	//�T�E���h�֘A
	static int m_HitSE[2];//�U�����q�b�g��������SE,����SE

public:
	void Init();
	void Uninit();
	void Update();
	void Draw();

	////�X�e�[�g�}�V����3
	//void InitState();
	//void UninitState();
	//void UpdateState();

	//static EnemyState* GetState() { return m_State; }

	//template <typename T>
	//static void SetState()
	//{
	//	m_NextState = new T();
	//}

	void SetHp(Hp* pHp) { m_Hp = pHp; }
	void SetStock(Polygon2D* pPo, int num) { m_Stock[num] = pPo; }
	void SetRespornImage(RespornImage* pI) { m_RespornImage = pI; }
	void SetCool() { m_IsCool = true, m_Time = 0; }
	void SetDownTime() { m_DownTime = 5;}
	void SetChanceTime() { m_ChanceTime = 10; }
	void SetHitGrab() { m_IsGrabHit = true; }
	void PullHideTime() { m_HideTime--; }

	//�v���C���[������
	void FaceCorrect();

	//�U��
	void At_Punch(); //��
	void At_Down();//��
	void At_Forward();//��
    void At_Upper();//��
	void At_Grab();//����
	void At_Shot();//shot

	void Mo_MoveR(); //�ڋ�Right
	void Mo_MoveL(); //�ڋ�Left
	void Ac_Jump();//�W�����v
	void De_Guard(); //�h��K�[�h
	void AddCollid(float x, float y, float sizeX, float sizeY, int zFrame, bool isRight, float angle, float speed, float damage, float distX, float distY);

	bool isAttack();//���炩�̍U�������Ă���Œ����ǂ���
	bool isAttackCollid();
	bool isGuard() { return m_IsGuard; }//
	bool isHitGrab() { return m_IsGrabHit; }//

	void SetHited() { m_IsHited = true; }
	BodyCollision* GetBodyCollision();
	D3DXMATRIX GetMatrix() { return m_Matrix; }
	Hp* GetHpPointer() { return m_Hp; }
	bool  GetCool() { return m_IsCool; }
	int   GetAttackType();
	int   GetHideTime() { return m_HideTime; }
	bool  GetIsReborn() { return m_isReBorn; }
	RespornImage* GetRespornImage() { return m_RespornImage; }
	Enemy* GetThisPointer() { return this; }
	bool  GetIsFaceIsRight() { return m_FaceIsRight; }

	template <typename T>
	bool checkState(EnemyState* state)
	{
		if (typeid(*state) == typeid(T))
		{
			return true;
		}
		else {
			return false;
		}
	}

};