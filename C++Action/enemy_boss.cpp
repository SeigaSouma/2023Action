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
#include "stage.h"
#include "objectX.h"
#include "debugproc.h"
#include "fade.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define WIDTH			(60.0f)							// ����
#define HEIGHT			(60.0f)							// �c��
#define MAX_LIFE		(5)								// �̗�
#define PLAYER_SERCH	(800.0f)	// �v���C���[�T���͈�
#define CHACE_DISTABCE	(50.0f)		// �ǂ��|���鎞�̊Ԋu
#define JUMP			(18.0f)					// �W�����v�͏����l
#define BASECHANGETIME	(120)	// ���_�؂�ւ�����
#define PLAYERCHASETIME	(60 * 5)	// �e�ǂ��|������
#define WAITTIME	(60 * 2)	// �ҋ@����

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CEnemyBoss::CEnemyBoss(int nPriority) : CEnemy(nPriority)
{
	// �l�̃N���A
	m_sAct.nAssultAngle = 1;	// �ːi�̌���
	m_sAct.AtkType = ATKTYPE_BULLET;	// �U���̎��
	m_BaseType = BASETYPE_ORIGIN;	// ���_�̎��
	m_BaseTypeDest = BASETYPE_ORIGIN;	// �ڕW�̋��_���
	m_nCntDamage = 0;			// �_���[�W�J�E���^�[
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

	// �ҋ@����
	m_nCntState = WAITTIME;
	m_state = STATE_WAIT;
	m_sMotionFrag.bATK = false;
	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CEnemyBoss::Uninit(void)
{
	// �I������
	CEnemy::Uninit();

	// ���[�h�ݒ�
	CManager::GetInstance()->GetFade()->SetFade(CScene::MODE_RESULT);
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
// �q�b�g����
//==========================================================================
bool CEnemyBoss::Hit(const int nValue)
{
	// �����擾
	D3DXVECTOR3 rot = GetRotation();
	D3DXVECTOR3 pos = GetPosition();

	// �̗͎擾
	int nLife = GetLife();

	if (m_nCntDamage <= 0)
	{// �Ȃɂ��Ȃ���Ԃ̎�

		// �̗͌��炷
		nLife -= nValue;

		// �̗͐ݒ�
		SetLife(nLife);

		if (nLife > 0)
		{// �̗͂��Ȃ��Ȃ��ĂȂ�������

			// �_���[�W���Đ�
			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_DMG01);

			if (m_pHPGauge == NULL)
			{
				// �̗̓Q�[�W
				m_pHPGauge = CHP_Gauge::Create(350.0f, GetLifeOrigin(), 3.0f);

				if (m_pHPGauge == NULL)
				{// NULL��������
					m_pHPGauge = NULL;
				}
			}
		}

		if (nLife <= 0)
		{// �̗͂��Ȃ��Ȃ�����

			// �q�b�g�X�g�b�v
			CManager::GetInstance()->SetEnableHitStop(5);

			// �U��
			CManager::GetInstance()->GetCamera()->SetShake(5, 10.0f, 0.0f);

			// ���S��Ԃɂ���
			m_state = STATE_DEAD;

			// �J�ڃJ�E���^�[�ݒ�
			m_nCntState = 40;

			// �m�b�N�o�b�N�̈ʒu�X�V
			m_posKnokBack = GetPosition();

			// �m�b�N�o�b�N����ɂ���
			m_sMotionFrag.bKnockback = true;

			// ���ꃂ�[�V����
			//m_pMotion->Set(MOTION_KNOCKBACK);

			// ����
			return true;
		}

		// �␳
		ValueNormalize(nLife, GetLifeOrigin(), 0);

		// �J�ڃJ�E���^�[�ݒ�
		if (nValue == mylib_const::DMG_SLASH)
		{
			m_nCntDamage = 10;

			// �U��
			CManager::GetInstance()->GetCamera()->SetShake(5, 8.0f, 0.0f);
		}
		else
		{
			m_nCntDamage = 20;

			// �q�b�g�X�g�b�v
			CManager::GetInstance()->SetEnableHitStop(5);

			// �U��
			CManager::GetInstance()->GetCamera()->SetShake(10, 15.0f, 0.0f);
		}

		// �m�b�N�o�b�N�̈ʒu�X�V
		m_posKnokBack = GetPosition();
	}

	// ����łȂ�
	return false;
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

	// �_���[�W�J�E���^�[
	m_nCntDamage--;

	if (m_nCntDamage <= 0)
	{
		m_nCntDamage = 0;
	}
	else
	{
		// �F�ݒ�
		m_mMatcol = D3DXCOLOR(1.0f, 0.0f, 0.0f, m_mMatcol.a);
	}

}

//==========================================================================
// ��ޕʍX�V����
//==========================================================================
void CEnemyBoss::UpdateByType(void)
{
	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �ړ��ʎ擾
	D3DXVECTOR3 move = GetMove();

	// X�t�@�C���Ƃ̔���
	CStage *pStage = CGame::GetStage();
	if (pStage == NULL)
	{// NULL��������
		return;
	}

	bool bNowLand = false;
	for (int nCntStage = 0; nCntStage < pStage->GetNumAll(); nCntStage++)
	{
		// �I�u�W�F�N�g�擾
		CObjectX *pObjX = pStage->GetObj(nCntStage);

		if (pObjX == NULL)
		{// NULL��������
			continue;
		}

		// �����擾
		bool bLand = false;
		float fHeight = pObjX->GetHeight(pos, bLand);

		if (bLand == true && fHeight > pos.y)
		{// �n�ʂ̕���������荂��������

			// �n�ʂ̍����ɕ␳
			pos.y = fHeight;

			if (bLand == true)
			{// ���n���Ă���

				// �W�����v�g�p�\�ɂ���
				move.y = 0.0f;
				bNowLand = true;
			}
		}
	}


	// �ʒu�ݒ�
	SetPosition(pos);

	// �ړ��ʐݒ�
	SetMove(move);

	// �f�o�b�O�\��
	CManager::GetInstance()->GetDebugProc()->Print(
		"------------------[�{�X���]------------------\n"
		"�s���̎�ށF�y%d�z\n"
		"�ڕW�̋��_�F�y%d�z\n", m_ActType, m_BaseTypeDest);
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

	case ATKTYPE_ASSULT:
		UpdateAttackAssult();
		break;

	case ATKTYPE_ENEMYSPAWN:
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
	int nType = m_pMotion->GetType();
	if (nType == MOTION_BULLETATK && m_pMotion->IsFinish() == true)
	{// �e�U�����I����Ă���

		// �ҋ@����
		m_nCntState = WAITTIME;
		m_state = STATE_WAIT;
		m_sMotionFrag.bATK = false;

		// �ʏ탂�[�V�����ݒ�
		m_pMotion->Set(MOTION_DEF);
		return;
	}
	
	if (nType != MOTION_BULLETATK)
	{// �e�U������Ȃ��ꍇ

		switch (m_sAct.AtkType)
		{
		case ATKTYPE_BULLET:
			// �e�U�����[�V�����ݒ�
			m_pMotion->Set(MOTION_BULLETATK);
			break;
		}
	}

	// �U�����ɂ���
	m_sMotionFrag.bATK = true;

}

//==========================================================================
// �ːi�U��
//==========================================================================
void CEnemyBoss::UpdateAttackAssult(void)
{
	if (m_nCntState <= 0)
	{// �J�ڃJ�E���^�[��0�ɂȂ�����

		// �ʏ탂�[�V�����ݒ�
		m_pMotion->Set(MOTION_DEF);
		return;
	}
	
	// �e�U�����[�V�����ݒ�
	m_pMotion->Set(MOTION_ASSULTATK);
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

	//if (CircleRange3D(pos, posPlayer, fRadius, pPlayer->GetRadius()) == true && m_sMotionFrag.bJump == false)
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


	// �ҋ@����
	m_nCntState--;
	if (m_nCntState <= 0)
	{// �[���ɂȂ�����

		// �s�����I
		DrawingACT();
	}
}

//==========================================================================
// �v���C���[�Ǐ]
//==========================================================================
void CEnemyBoss::PlayerChase(void)
{
	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �����擾
	D3DXVECTOR3 rot = GetRotation();

	// �ڕW�̌����擾
	float fRotDest = GetRotDest();

	// �ړ��ʎ擾
	float fMove = GetVelocity();

	// ���݂ƖڕW�̍���
	float fRotDiff = 0.0f;

	// �����̔���
	bool bLen = false;

	// �v���C���[���
	CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();

	// �I�u�W�F�N�g���
	CObject *pObj = NULL;

	// ��ԑJ�ڃJ�E���^�[���Z
	m_nCntState--;

	if (m_nCntState <= 0)
	{// �J�ڃJ�E���^�[��0�ɂȂ�����
		
		// �ҋ@����
		m_nCntState = WAITTIME;
		m_state = STATE_WAIT;

		// �ʏ탂�[�V�����ݒ�
		m_pMotion->Set(MOTION_DEF);
		return;
	}

	// �v���C���[�̕������鏈��
	//RotPlayer();

	// �U���ʏ���
	UpdateByAttack();

	// �ǂ��|���ړ�����
	ChaseMove(fMove);

	// �ڕW�̌����ݒ�
	SetRotDest(fRotDest);
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
}

//==========================================================================
// �ǂ��|���ړ�
//==========================================================================
void CEnemyBoss::ChaseMove(float fMove)
{
	// �}�b�v�}�l�[�W���̎擾
	CMapManager *pMapManager = CGame::GetMapManager();
	if (pMapManager == NULL)
	{// NULL��������
		return;
	}

	// �v���C���[���
	CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();
	if (pPlayer == NULL)
	{
		return;
	}

	// �e�̈ʒu�擾
	D3DXVECTOR3 posPlayer = pPlayer->GetPosition();
	CObject *pMyObj = GetObject();

	// �ڕW�������̂ǂ����ɂ��邩�����߂�
	ANGLE TargetAngle = pMapManager->GetTargetAngle(pPlayer, pMyObj);

	// �ړ��ʎ擾
	D3DXVECTOR3 move = GetMove();

	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �����擾
	D3DXVECTOR3 rot = GetRotation();

	// �ڕW�̌����擾
	float fRotDest = GetRotDest();

	// ���݂ƖڕW�̍���
	float fRotDiff = 0.0f;

	// �}�b�v���
	int nIdxMapPoint = GetMapIndex();
	float fPointRatio = GetMapPointRatio();
	float fMoveValue = GetMapMoveValue();

	// �ړ������ʉ��Z
	//move.x += fMove * m_sAct.nAssultAngle;
	fMoveValue += fMove * m_sAct.nAssultAngle;

	// �ʒu�X�V
	pos = pMapManager->UpdateNowPosition(nIdxMapPoint, fPointRatio, fMoveValue, pos.y);

	// �ڕW�n�_
	float fDest = (fMoveValue + (fMove + GetRadius() * 0.5f) * m_sAct.nAssultAngle);
	int nDestIdx = nIdxMapPoint;
	float fDestPointRatio = fPointRatio;
	D3DXVECTOR3 sakiPos = pMapManager->UpdateNowPosition(nDestIdx, fDestPointRatio, fDest, pos.y);

	// �ڕW�̊p�x�����߂�
	fRotDest = atan2f((pos.x - sakiPos.x), (pos.z - sakiPos.z));

	// �ڕW�Ƃ̍���
	fRotDiff = fRotDest - rot.y;

	//�p�x�̐��K��
	RotNormalize(fRotDiff);

	//�p�x�̕␳������
	rot.y += fRotDiff * 0.025f;

	// �p�x�̐��K��
	RotNormalize(rot.y);

	// �ڕW�̌����ݒ�
	SetRotDest(fRotDest);

	// �����ݒ�
	SetRotation(rot);

	// �ړ��ʐݒ�
	SetMove(move);

	// �ʒu�ݒ�
	SetPosition(pos);

	// �}�b�v�̃C���f�b�N�X�ԍ��ݒ�
	SetMapIndex(nIdxMapPoint);

	// �}�b�v�|�C���g�Ԃ̊����ݒ�
	SetMapPointRatio(fPointRatio);

	// �}�b�v�|�C���g�Ԃ̈ړ��ʐݒ�
	SetMapMoveValue(fMoveValue);
}

//==========================================================================
// �_���[�W
//==========================================================================
void CEnemyBoss::Damage(void)
{
	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �ړ��ʎ擾
	D3DXVECTOR3 move = GetMove();

	// �����擾
	D3DXVECTOR3 rot = GetRotation();

	// �ڕW�̌����擾
	float fRotDest = GetRotDest();

	// �ړ��ʎ擾
	float fMove = GetVelocity();

	// ���݂ƖڕW�̍���
	float fRotDiff = 0.0f;

	// �����̔���
	bool bLen = false;

	// �v���C���[���
	CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();

#if _DEBUG
	// �F�ݒ�
	m_mMatcol = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
#endif

	// ��ԑJ�ڃJ�E���^�[���Z
	m_nCntState--;

	if (m_nCntState <= 0)
	{// �J�ڃJ�E���^�[��0�ɂȂ�����

		// �ߋ��̏�Ԃɂ���
		m_state = STATE_WAIT;
		m_nCntState = 60;
	}

	// �ʒu�ݒ�
	SetPosition(pos);

	// �ړ��ʐݒ�
	SetMove(move);

	// �����ݒ�
	SetRotation(rot);

	// �ڕW�̌����ݒ�
	SetRotDest(fRotDest);
}

//==========================================================================
// ���_�؂�ւ�
//==========================================================================
void CEnemyBoss::ChangeBase(void)
{
	// �}�b�v�}�l�[�W���̎擾
	CMapManager *pMapManager = CGame::GetMapManager();
	if (pMapManager == NULL)
	{// NULL��������
		return;
	}

	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �}�b�v���
	int nIdxMapPoint = GetMapIndex();
	float fPointRatio = GetMapPointRatio();
	float fMoveValue = GetMapMoveValue();

	// �ڕW�̈ʒu
	D3DXVECTOR3 DestPosition = GetPosition();

	// ��ԑJ�ڃJ�E���^�[���Z
	m_nCntState--;

	// �ڕW�ʏ���
	switch (m_BaseTypeDest)
	{
	case CEnemy::BASETYPE_MAP:
		DestPosition = pMapManager->UpdateNowPosition(nIdxMapPoint, fPointRatio, fMoveValue, pos.y);
		pos.x = Lerp(0.0f, DestPosition.x, 1.0f - ((float)m_nCntState / (float)BASECHANGETIME));
		pos.z = Lerp(0.0f, DestPosition.z, 1.0f - ((float)m_nCntState / (float)BASECHANGETIME));
		break;

	case CEnemy::BASETYPE_ORIGIN:
		DestPosition = pMapManager->UpdateNowPosition(nIdxMapPoint, fPointRatio, fMoveValue, pos.y);
		pos.x = Lerp(DestPosition.x, 0.0f, 1.0f - ((float)m_nCntState / (float)BASECHANGETIME));
		pos.z = Lerp(DestPosition.z, 0.0f, 1.0f - ((float)m_nCntState / (float)BASECHANGETIME));
		break;
	}

	// �v���C���[�̕������鏈��
	RotPlayer();

	// �ʒu�ݒ�
	SetPosition(pos);

	if (m_nCntState <= 0)
	{// �J�ڃJ�E���^�[��0�ɂȂ�����

		switch (m_BaseTypeDest)
		{
		case CEnemy::BASETYPE_MAP:
			m_sAct.AtkType = ATKTYPE_ASSULT;
			m_state = STATE_PLAYERCHASE;
			m_nCntState = PLAYERCHASETIME;
			break;

		case CEnemy::BASETYPE_ORIGIN:
			m_sAct.AtkType = ATKTYPE_BULLET;
			m_state = STATE_ATTACK;
			break;
		}
		m_BaseType = m_BaseTypeDest;
		//m_state = STATE_WAIT;
	}
}

//==========================================================================
// �s�����I
//==========================================================================
void CEnemyBoss::DrawingACT(void)
{
	m_sAct.AtkType = (ATKTYPE)(rand() % ATKTYPE_MAX);

	// ���̍s����
	switch (m_sAct.AtkType)
	{
	case ATKTYPE_BULLET:
		if (m_BaseType == BASETYPE_MAP)
		{// �}�b�v�ňړ����Ă�ꍇ
			m_state = STATE_BASECHANGE;
			m_nCntState = BASECHANGETIME;
			m_BaseTypeDest = BASETYPE_ORIGIN;
		}
		else
		{// ���S�ɂ���ꍇ
			m_state = STATE_ATTACK;
		}
		break;

	case ATKTYPE_ASSULT:	// �ːi

		if (m_BaseType == BASETYPE_MAP)
		{// �}�b�v�ňړ����Ă�ꍇ
			m_state = STATE_PLAYERCHASE;
			m_nCntState = PLAYERCHASETIME;
		}
		else
		{// ���S�ɂ���ꍇ
			m_state = STATE_BASECHANGE;
			m_nCntState = BASECHANGETIME;
			m_BaseTypeDest = BASETYPE_MAP;
		}
		m_sAct.nAssultAngle = 1;	// �ːi�̌���

		if (Random(0, 1) == 0)
		{
			m_sAct.nAssultAngle = -1;	// �ːi�̌���
		}

		break;

	case ATKTYPE_ENEMYSPAWN:
		break;

	default:
		break;
	}
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

	int nType = m_pMotion->GetType();

	// �}�b�v�}�l�[�W���̎擾
	CMapManager *pMapManager = CGame::GetMapManager();
	if (pMapManager == NULL)
	{// NULL��������
		return;
	}

	CObject *pBullet = NULL;
	switch (nType)
	{
	case MOTION_BULLETATK:
		for (int i = 0; i < 17; i++)
		{
			float fRot = ((D3DX_PI * 2.0f) / 16.0f) * i;

			// �e�̐���
			pBullet = CBullet::Create(
				CBullet::TYPE_ENEMY,
				CBullet::MOVETYPE_NORMAL,
				D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z),
				rot,
				D3DXVECTOR3(sinf(D3DX_PI + fRot) * 10.0f, 0.0f, cosf(D3DX_PI + fRot) * 10.0f),
				80.0f);

			pBullet->SetMapIndex(GetMapIndex());
			pBullet->SetMapMoveValue(GetMapMoveValue());
			pBullet->SetMapPointRatio(GetMapPointRatio());
			pBullet->SetMoveAngle(ANGLE_UP);
		}
		break;
	}
	
}

//==========================================================================
// �v���C���[�̕�������
//==========================================================================
void CEnemyBoss::RotPlayer(void)
{
	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �����擾
	D3DXVECTOR3 rot = GetRotation();

	// �ڕW�̌����擾
	float fRotDest = GetRotDest();

	// ���݂ƖڕW�̍���
	float fRotDiff = 0.0f;

	// �v���C���[���
	CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();

	if (pPlayer == NULL)
	{
		return;
	}

	// �e�̈ʒu�擾
	D3DXVECTOR3 posPlayer = pPlayer->GetPosition();

	// �ڕW�̊p�x�����߂�
	fRotDest = atan2f((pos.x - posPlayer.x), (pos.z - posPlayer.z));

	// �ڕW�Ƃ̍���
	fRotDiff = fRotDest - rot.y;

	//�p�x�̐��K��
	RotNormalize(fRotDiff);

	//�p�x�̕␳������
	rot.y += fRotDiff * 0.025f;

	// �p�x�̐��K��
	RotNormalize(rot.y);

	// �ڕW�̌����ݒ�
	SetRotDest(fRotDest);

	// �����ݒ�
	SetRotation(rot);
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