//=============================================================================
// 
//  �{�X���� [enemy_boss.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "enemy_boss.h"
#include "manager.h"
#include "renderer.h"
#include "game.h"
#include "texture.h"
#include "input.h"
#include "calculation.h"
#include "bullet.h"
#include "explosion.h"
#include "particle.h"
#include "motion.h"
#include "model.h"
#include "elevation.h"
#include "player.h"
#include "enemymanager.h"
#include "camera.h"
#include "ballast.h"
#include "3D_Effect.h"
#include "hp_gauge.h"
#include "shadow.h"
#include "sound.h"
#include "mapmanager.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define WIDTH			(60.0f)							// ����
#define HEIGHT			(60.0f)							// �c��
#define MAX_LIFE		(5)								// �̗�
#define PLAYER_SERCH	(800.0f)	// �v���C���[�T���͈�
#define CHACE_DISTABCE	(50.0f)		// �ǂ��|���鎞�̊Ԋu
#define JUMP			(18.0f)					// �W�����v�͏����l

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CEnemyBoss::CEnemyBoss(int nPriority) : CEnemy(nPriority)
{
	// �l�̃N���A
	m_sAct.nCntWait = 0;				// �ҋ@����
	m_sAct.AtkType = ATKTYPE_BULLET;	// �U���̎��
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CEnemyBoss::~CEnemyBoss()
{

}


//==========================================================================
// ����������
//==========================================================================
HRESULT CEnemyBoss::Init(void)
{
	// ����������
	CEnemy::Init();

	m_state = STATE_WAIT;	// �e�ǂ��|�����
	m_Oldstate = STATE_NONE;
	m_ActType = ACTTYPE_BOSS;
	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CEnemyBoss::Uninit(void)
{
	// �I������
	CEnemy::Uninit();
}

//==========================================================================
// ���S����
//==========================================================================
void CEnemyBoss::Kill(void)
{
	// ���S����
	CEnemy::Kill();
}

//==========================================================================
// �X�V����
//==========================================================================
void CEnemyBoss::Update(void)
{
	// ���S�̔���
	if (IsDeath() == true)
	{// ���S�t���O�������Ă�����
		return;
	}

	// �X�V����
	CEnemy::Update();

	if (IsDeath() == true)
	{// ���S�t���O�������Ă�����
		return;
	}

}

//==========================================================================
// ��ޕʍX�V����
//==========================================================================
void CEnemyBoss::UpdateByType(void)
{
	//// �ҋ@����
	//m_sAct.nCntWait--;
	//if (m_sAct.nCntWait <= 0)
	//{// �[���ɂȂ�����

	//	m_sAct.nCntWait = 0;
	//	m_sAct.AtkType = ATKTYPE_BULLET;
	//}
}

//==========================================================================
// �U���ʏ���
//==========================================================================
void CEnemyBoss::UpdateByAttack(void)
{
	switch (m_sAct.AtkType)
	{
	case ATKTYPE_BULLET:
		UpdateAttackBullet();
		break;

	default:
		break;
	}
}

//==========================================================================
// �e�U��
//==========================================================================
void CEnemyBoss::UpdateAttackBullet(void)
{
	if (m_pMotion->GetType() == MOTION_BULLETATK && m_pMotion->IsFinish() == true)
	{// �e�U�����I����Ă���

		// �ҋ@����
		m_sAct.nCntWait = 120;
		m_state = STATE_WAIT;
		m_sMotionFrag.bATK = false;


		// �ʏ탂�[�V�����ݒ�
		m_pMotion->Set(MOTION_DEF);
		return;
	}
	
	if (m_pMotion->GetType() != MOTION_BULLETATK)
	{// �e�U������Ȃ��ꍇ

		// �e�U�����[�V�����ݒ�
		m_pMotion->Set(MOTION_BULLETATK);
	}

	// �U�����ɂ���
	m_sMotionFrag.bATK = true;

}

//==========================================================================
// ���n���̏���
//==========================================================================
void CEnemyBoss::ProcessLanding(void)
{
	// ���n���̏���
	CEnemy::ProcessLanding();
}

//==========================================================================
// �U����Ԉڍs����
//==========================================================================
void CEnemyBoss::ChangeToAttackState(void)
{
	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �v���C���[���
	CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();

	if (pPlayer == NULL)
	{
		return;
	}

	// �e�̈ʒu�擾
	D3DXVECTOR3 posPlayer = pPlayer->GetPosition();

	float fRadius = 500.0f;

	if (CircleRange(pos, posPlayer, fRadius, pPlayer->GetRadius()) == true && m_sMotionFrag.bJump == false)
	{// ��苗���ԂɃv���C���[����������

		// �U����Ԃɂ���
		m_state = STATE_ATTACK;
		m_sMotionFrag.bATK = true;
	}
}

//==========================================================================
// �o��
//==========================================================================
void CEnemyBoss::Spawn(void)
{
	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �ړ��ʎ擾
	D3DXVECTOR3 move = GetMove();

	// ��ԑJ�ڃJ�E���^�[���Z
	m_nCntState++;

	// �ʒu�X�V
	pos = (D3DXVECTOR3(0.0f, SPAWN_GRAVITY, 0.0f) * (float)(m_nCntState * m_nCntState) + move * (float)m_nCntState) + m_posOrigin;

	// �N���Ƃ̔���
	if (CManager::GetInstance()->GetScene()->GetElevation()->IsHit(pos))
	{// �n�ʂƓ������Ă�����
		m_state = m_Oldstate;	// �ߋ��̏��ۑ�
		m_nCntState = 0;	// �J�ڃJ�E���^�[�ݒ�
		move.y = 0.0f;			// �ړ��ʃ[��
	}

	// �ʒu�ݒ�
	SetPosition(pos);

	// �ړ��ʐݒ�
	SetMove(move);
}

//==========================================================================
// �ҋ@����
//==========================================================================
void CEnemyBoss::StateWait(void)
{
	// �ҋ@����
	m_sAct.nCntWait--;
	if (m_sAct.nCntWait <= 0)
	{// �[���ɂȂ�����
		m_sAct.nCntWait = 0;
		m_sAct.AtkType = ATKTYPE_BULLET;
		m_state = STATE_ATTACK;
	}
}

//==========================================================================
// �U������
//==========================================================================
void CEnemyBoss::StateAttack(void)
{
	
	// �U���ʏ���
	UpdateByAttack();

	// ���[�V�����̏��擾
	CMotion::Info aInfo = m_pMotion->GetInfo(m_pMotion->GetType());

	// �U�����̑����擾
	int nNumAttackInfo = aInfo.nNumAttackInfo;

	bool bAtkWait = true;	// �U���ҋ@��
	for (int nCntAttack = 0; nCntAttack < nNumAttackInfo; nCntAttack++)
	{
		if (aInfo.AttackInfo[nCntAttack] == NULL)
		{// NULL��������
			continue;
		}

		// ���[�V�����J�E���^�[�擾
		if (m_pMotion->GetAllCount() > aInfo.AttackInfo[nCntAttack]->nMinCnt)
		{// �U�����蒆

			// �U�����蒆�ɂ���
			bAtkWait = false;
		}
	}

	if (bAtkWait == false)
	{// ���蒆�̎�
		return;
	}


	// �v���C���[���
	CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();
	if (pPlayer == NULL)
	{// NULL��������
		return;
	}

	// �e�̈ʒu�擾
	D3DXVECTOR3 posPlayer = pPlayer->GetPosition();

	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �����擾
	D3DXVECTOR3 rot = GetRotation();

	// �ڕW�̌����擾
	float fRotDest = GetRotDest();

	// �ڕW�̊p�x�����߂�
	fRotDest = atan2f((pos.x - posPlayer.x), (pos.z - posPlayer.z));

	// �ڕW�Ƃ̍���
	float fRotDiff = fRotDest - rot.y;

	//�p�x�̐��K��
	RotNormalize(fRotDiff);

	//�p�x�̕␳������
	rot.y += fRotDiff * 0.025f;

	// �p�x�̐��K��
	RotNormalize(rot.y);

	// �����ݒ�
	SetRotation(rot);
}

//==========================================================================
// �ǂ��|���ړ�
//==========================================================================
void CEnemyBoss::ChaseMove(float fMove)
{
	// �����擾
	D3DXVECTOR3 rot = GetRotation();

	// �ړ��ʎ擾
	D3DXVECTOR3 move = GetMove();

	// �ړ��ʉ��Z
	move.x += sinf(D3DX_PI + rot.y) * fMove;
	move.z += cosf(D3DX_PI + rot.y) * fMove;

	// �ړ��ʐݒ�
	SetMove(move);
}

//==========================================================================
// ���[�V�����̐ݒ�
//==========================================================================
void CEnemyBoss::MotionSet(void)
{
	if (m_pMotion->IsFinish() == true)
	{// �I�����Ă�����

		// ���݂̎�ގ擾
		int nType = m_pMotion->GetType();

		if (m_sMotionFrag.bMove == true && m_sMotionFrag.bKnockback == false)
		{// �ړ����Ă�����

			m_sMotionFrag.bMove = false;	// �ړ�����OFF

			// �ړ����[�V����
			m_pMotion->Set(MOTION_WALK);
		}
		else if (m_sMotionFrag.bKnockback == true)
		{// ���ꒆ��������

			// ���ꃂ�[�V����
			m_pMotion->Set(MOTION_KNOCKBACK);
		}
	}
}

//==========================================================================
// �U��������
//==========================================================================
void CEnemyBoss::AttackAction(int nModelNum, CMotion::AttackInfo ATKInfo)
{
	D3DXMATRIX mtxTrans;	// �v�Z�p�}�g���b�N�X�錾

	// �U��������
	CEnemy::AttackAction(nModelNum, ATKInfo);

	if (GetObjectChara()->GetModel()[nModelNum] == NULL)
	{// NULL��������
		return;
	}

	// ���肷��p�[�c�̃}�g���b�N�X�擾
	D3DXMATRIX mtxWepon = GetObjectChara()->GetModel()[nModelNum]->GetWorldMtx();

	// �ʒu�𔽉f����
	D3DXMatrixTranslation(&mtxTrans, ATKInfo.Offset.x, ATKInfo.Offset.y, ATKInfo.Offset.z);
	D3DXMatrixMultiply(&mtxWepon, &mtxTrans, &mtxWepon);

	// ����̈ʒu
	D3DXVECTOR3 weponpos = D3DXVECTOR3(mtxWepon._41, mtxWepon._42, mtxWepon._43);

	CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();

	if (pPlayer == NULL)
	{// NULL��������
		return;
	}

	// �v���C���[�̈ʒu
	D3DXVECTOR3 posPlayer = pPlayer->GetPosition();


	// �ڕW�̊p�x�����߂�
	float fRotDest = atan2f((weponpos.x - posPlayer.x), (weponpos.z - posPlayer.z));

	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �ʒu�擾
	D3DXVECTOR3 rot = GetRotation();

	// �e�̐���
	CObject *pBullet = CBullet::Create(
		CBullet::TYPE_ENEMY,
		CBullet::MOVETYPE_NORMAL,
		D3DXVECTOR3(pos.x, 150.0f, pos.z),
		rot,
		D3DXVECTOR3(13.0f, 0.0f, 0.0f),
		40.0f);

	// �}�b�v�}�l�[�W���̎擾
	CMapManager *pMapManager = CManager::GetInstance()->GetScene()->GetMapManager();
	if (pMapManager == NULL)
	{// NULL��������
		return;
	}
	ANGLE setAngle = pMapManager->GetTargetAngle(GetMapIndex(), pPlayer->GetMapIndex(), GetMapMoveValue(), pPlayer->GetMapMoveValue());

	pBullet->SetMapIndex(GetMapIndex());
	pBullet->SetMapMoveValue(GetMapMoveValue());
	pBullet->SetMapPointRatio(GetMapPointRatio());
	pBullet->SetMoveAngle(setAngle);


	/*pBullet = CBullet::Create(
		CBullet::TYPE_ENEMY,
		CBullet::MOVETYPE_NORMAL,
		D3DXVECTOR3(pos.x, 150.0f, pos.z),
		rot,
		D3DXVECTOR3(13.0f, 0.0f, 0.0f),
		40.0f);
	pBullet->SetMapIndex(GetMapIndex());
	pBullet->SetMapMoveValue(GetMapMoveValue());
	pBullet->SetMapPointRatio(GetMapPointRatio());
	pBullet->SetMoveAngle(setAngle);


	pBullet = CBullet::Create(
		CBullet::TYPE_ENEMY,
		CBullet::MOVETYPE_NORMAL,
		D3DXVECTOR3(pos.x, 250.0f, pos.z),
		rot,
		D3DXVECTOR3(13.0f, 0.0f, 0.0f),
		40.0f);
	pBullet->SetMapIndex(GetMapIndex());
	pBullet->SetMapMoveValue(GetMapMoveValue());
	pBullet->SetMapPointRatio(GetMapPointRatio());
	pBullet->SetMoveAngle(setAngle);*/
}

//==========================================================================
// �`�揈��
//==========================================================================
void CEnemyBoss::Draw(void)
{
	// �`�揈��
	CEnemy::Draw();
}

//==========================================================================
// �G�̏��擾
//==========================================================================
CEnemyBoss *CEnemyBoss::GetEnemy(void)
{
	// �������g�̃|�C���^���擾
	return this;
}