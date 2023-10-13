//=============================================================================
// 
//  �v���C���[���� [enemy.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "enemy.h"
#include "manager.h"
#include "renderer.h"
#include "game.h"
#include "texture.h"
#include "input.h"
#include "calculation.h"
#include "explosion.h"
#include "particle.h"
#include "motion.h"
#include "model.h"
#include "elevation.h"
#include "player.h"
#include "camera.h"
#include "ballast.h"
#include "3D_Effect.h"
#include "hp_gauge.h"
#include "shadow.h"
#include "sound.h"
#include "impactwave.h"
#include "score.h"
#include "bullet.h"
#include "mapmanager.h"

// �q�N���X
#include "enemy_power.h"
#include "enemy_crowd.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define WIDTH			(60.0f)		// ����
#define HEIGHT			(60.0f)		// �c��
#define MAX_LIFE		(5)			// �̗�
#define PLAYER_SERCH	(800.0f)	// �v���C���[�T���͈�
#define CHACE_DISTABCE	(50.0f)		// �ǂ��|���鎞�̊Ԋu
#define JUMP			(12.0f)		// �W�����v�͏����l
#define SURVIVALLIFE	(60 * 20)	// ��������
#define FADEOUT_TIME	(40)		// �t�F�[�h�A�E�g����

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================


//==========================================================================
// �R���X�g���N�^
//==========================================================================
CEnemy::CEnemy(int nPriority) : CObjectChara(nPriority)
{
	// �l�̃N���A
	m_posOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// �ŏ��̈ʒu
	memset(&m_sFormationInfo, NULL, sizeof(m_sFormationInfo));	// ����̏��
	m_posKnokBack = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// �m�b�N�o�b�N�̈ʒu
	m_rotOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// �ŏ��̌���
	m_type = TYPE_POWER;	// ���
	m_state = STATE_NONE;	// ���
	m_Oldstate = m_state;	// �O��̏��
	m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// �}�e���A���̐F
	m_nCntState = 0;		// ��ԑJ�ڃJ�E���^�[
	m_nTexIdx = 0;			// �e�N�X�`���̃C���f�b�N�X�ԍ�
	m_nNumChild = 0;		// ���̐�
	m_nIdxManager = 0;		// �}�l�[�W���̃C���f�b�N�X�ԍ�
	m_nSurvivalLife = 0;	// ��������
	m_nSurvivalLifeOrigin = 0;	// ��������
	m_bAddScore = false;	// �X�R�A���Z���邩�̔���
	m_AddType = CResultManager::ADDTYPE_NONE;	// �X�R�A�̉��Z��
	m_nBallastEmission = 0;	// ���I�̔����J�E���^�[
	m_sMotionFrag.bJump = false;		// �W�����v�����ǂ���
	m_sMotionFrag.bKnockback = false;	// �m�b�N�o�b�N�����ǂ���
	m_sMotionFrag.bMove = false;		// �ړ������ǂ���
	m_sMotionFrag.bATK = false;			// �U�������ǂ���
	m_pParent = NULL;					// �e�̃|�C���^
	m_pHPGauge = NULL;					// HP�Q�[�W�̏��
	m_pMotion = NULL;					// ���[�V�����̏��
	m_colorType = COLORTYPE_NORMAL;		// �F���Ƃ̎��

	memset(&m_pChild[0], NULL, sizeof(m_pChild));	// �q�̃|�C���^
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CEnemy::~CEnemy()
{

}

//==========================================================================
// ��������
//==========================================================================
CEnemy *CEnemy::Create(int nIdx, const char *pFileName, D3DXVECTOR3 pos, TYPE type)
{
	// �����p�̃I�u�W�F�N�g
	CEnemy *pEnemy = NULL;

	if (pEnemy == NULL)
	{// NULL��������

		// �������̊m��
		switch (type)
		{
		case TYPE_POWER:
			pEnemy = DEBUG_NEW CEnemyPower;
			break;

		case TYPE_CROUWD:
			pEnemy = DEBUG_NEW CEnemyCrowd;
			break;

		default:
			return NULL;
			break;
		}

		if (pEnemy != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// �C���f�b�N�X�ԍ�
			pEnemy->m_nIdxManager = nIdx;

			// ���
			pEnemy->m_type = type;

			// �ŏ��̈ʒu�ݒ�
			pEnemy->m_posOrigin = pos;

			// �ʒu�ݒ�
			pEnemy->SetPosition(pos);

			// �e�L�X�g�ǂݍ���
			HRESULT hr = pEnemy->RoadText(pFileName);
			if (FAILED(hr))
			{// ���s���Ă�����
				return NULL;
			}

			// ����������
			pEnemy->Init();
		}

		return pEnemy;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CEnemy::Init(void)
{
	// �e��ϐ��̏�����
	m_state = STATE_SPAWN;	// ���
	m_Oldstate = STATE_PLAYERCHASE;
	m_nCntState = 0;		// ��ԑJ�ڃJ�E���^�[
	m_nSurvivalLife = SURVIVALLIFE;	// ��������
	m_posKnokBack = m_posOrigin;	// �m�b�N�o�b�N�̈ʒu
	m_bAddScore = true;	// �X�R�A���Z���邩�̔���

	// ��ނ̐ݒ�
	SetType(TYPE_ENEMY);

	// �̗͎擾
	int nLife = GetLife();

	switch (m_type)
	{
	case TYPE_POWER:
		break;
	}

	//if (m_pHPGauge == NULL)
	//{// NULL��������
	//	m_pHPGauge = NULL;
	//}

	// �����ݒ�
	SetRotation(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	// �ړ��ʐݒ�
	SetMove(D3DXVECTOR3(0.0f, JUMP, 0.0f));

	// �e�̐���
	m_pShadow = CShadow::Create(GetPosition(), GetRadius() * 0.5f);

	// �����擾
	bool bLand = false;
	m_posOrigin.y = CGame::GetElevation()->GetHeight(m_posOrigin, bLand);

	// �ʒu�ݒ�
	SetPosition(m_posOrigin);

	// �|�[�Y�̃��Z�b�g
	m_pMotion->ResetPose(MOTION_DEF);

	// ��������
	m_nSurvivalLifeOrigin = m_nSurvivalLife;


	return S_OK;
}

//==========================================================================
// �e�L�X�g�ǂݍ���
//==========================================================================
HRESULT CEnemy::RoadText(const char *pFileName)
{
	// �L�����쐬
	HRESULT hr = SetCharacter(pFileName);

	if (FAILED(hr))
	{// ���s���Ă�����
		return E_FAIL;
	}

	// ���[�V�����̐�������
	if (m_pMotion == NULL)
	{
		m_pMotion = CMotion::Create(pFileName);
	}

	if (m_pMotion == NULL)
	{
		return E_FAIL;
	}

	// ���[�V�����̃t�@�C���ǂݍ���
	//m_pMotion->ReadText(pFileName);

	// �I�u�W�F�N�g�L�����N�^�[�̏��擾
	CObjectChara *pObjChar = GetObjectChara();

	// ���[�V�����̐ݒ�
	m_pMotion->SetModel(pObjChar->GetModel(), pObjChar->GetNumModel(), pObjChar);

	return S_OK;
}

//==========================================================================
// �q�o�^
//==========================================================================
void CEnemy::RegistrChild(CEnemy *pChild)
{
	if (m_nNumChild >= mylib_const::MAX_ENEMY)
	{
		return;
	}

	if (m_pChild[m_nNumChild] == NULL)
	{
		m_pChild[m_nNumChild] = pChild;
		m_nNumChild++;	// �q�̐����Z
	}
	else
	{
		int n = 0;
	}
}

//==========================================================================
// �q�̃��Z�b�g
//==========================================================================
void CEnemy::ResetChild(CEnemy *pChild)
{
	int nMax = m_nNumChild;

	for (int nCntChild = 0; nCntChild < nMax; nCntChild++)
	{
		if (m_pChild[nCntChild] == NULL)
		{
			continue;
		}

		if (m_pChild[nCntChild] == pChild)
		{
			m_pChild[nCntChild] = NULL;
		}
	}
}


//==========================================================================
// �e�̃|�C���^�ݒ�
//==========================================================================
void CEnemy::SetParent(CEnemy *pParent)
{
	if (this != nullptr)
	{
		// �q�̊��蓖��
		if (pParent != NULL)
		{
			pParent->RegistrChild(this);
		}

		// �e�̃|�C���^�n��
		m_pParent = pParent;
		
	}
	else
	{
		int n = 0;
	}
}

//==========================================================================
// �I������
//==========================================================================
void CEnemy::Uninit(void)
{
	int nIdx = m_nIdxManager;

	// �I������
	if (m_pMotion != NULL)
	{
		m_pMotion->Uninit();
		delete m_pMotion;
		m_pMotion = NULL;
	}

	// �e������
	if (m_pShadow != NULL)
	{
		m_pShadow = NULL;
	}

	// �I������
	CObjectChara::Uninit();

	// �}�l�[�W���̏I��
	CEnemyManager::Release(nIdx);
}

//==========================================================================
// ���S����
//==========================================================================
void CEnemy::Kill(void)
{

	// �X�R�A���Z
	if (m_AddType != CResultManager::ADDTYPE_NONE && CManager::GetMode() == CScene::MODE_GAME)
	{
		CGame::GetScore()->Add(m_AddType);

		if (m_bAddScore == true)
		{// �X�R�A���Z���锻�肾������
			CGame::GetScore()->Add(GetAddScoreValue());
		}
	}

	for (int nCntEnemy = 0; nCntEnemy < mylib_const::MAX_ENEMY; nCntEnemy++)
	{// �q�̐�����

		if (m_pChild[nCntEnemy] != NULL)
		{// �q������ꍇ

			// �����̎q�̐e��NULL�ɂ���
			m_pChild[nCntEnemy]->m_pParent = NULL;
		}
	}

	if (m_pParent != NULL)
	{// �����ɐe������ꍇ

		// �e�̎q��NULL�ɂ���
		m_pParent->ResetChild(this);
	}

	// HP�Q�[�W������
	if (m_pHPGauge != NULL)
	{
		m_pHPGauge->Uninit();
		m_pHPGauge = NULL;
	}

	// �e������
	if (m_pShadow != NULL)
	{
		m_pShadow->Uninit();
		m_pShadow = NULL;
	}
}

//==========================================================================
// �X�V����
//==========================================================================
void CEnemy::Update(void)
{
	// ���S�̔���
	if (IsDeath() == true)
	{// ���S�t���O�������Ă�����
		return;
	}

	// �G�f�B�b�g���͔�����
	if (CGame::GetElevation()->IsEdit())
	{
		return;
	}

	// �R���{���o���͔�����
	if (CGame::GetEnemyManager()->GetState() == CEnemyManager::STATE_COMBOANIM)
	{
		return;
	}

	// �ߋ��̈ʒu�ݒ�
	SetOldPosition(GetPosition());

	// �����蔻��
	Collision();

	// ���S�̔���
	if (IsDeath() == true)
	{// ���S�t���O�������Ă�����
		return;
	}

	

	// ���[�V�����̐ݒ菈��
	MotionSet();

	// ��ԍX�V
	UpdateState();

	// ���S�̔���
	if (IsDeath() == true)
	{// ���S�t���O�������Ă�����
		return;
	}

	// �U������
	Atack();

	// ���[�V�����̍X�V
	m_pMotion->Update();

	// ��ޕʏ���
	UpdateByType();

	// HP�Q�[�W�̈ʒu�X�V
	if (m_pHPGauge != NULL)
	{
		// �ʒu�擾
		D3DXVECTOR3 pos = GetPosition();

		// �̗͎擾
		int nLife = GetLife();

		m_pHPGauge->UpdatePosition(pos, nLife);
	}

	// �e�̈ʒu�X�V
	if (m_pShadow != NULL)
	{
		// �ʒu�擾
		D3DXVECTOR3 pos = GetPosition();
		m_pShadow->SetPosition(D3DXVECTOR3(pos.x, m_pShadow->GetPosition().y, pos.z));
	}

	if (GetPosition().y <= mylib_const::KILL_Y)
	{
		// �G�̏I������
		Kill();
		Uninit();
		return;
	}
}

//==========================================================================
// �����蔻��
//==========================================================================
void CEnemy::Collision(void)
{
	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �ړ��ʎ擾
	D3DXVECTOR3 move = GetMove();

	// �����擾
	D3DXVECTOR3 rot = GetRotation();

	// �d�͏���
	move.y -= mylib_const::GRAVITY;

	// �ʒu�X�V
	pos += move;

	// �}�b�v�}�l�[�W���̎擾
	CMapManager *pMapManager = CManager::GetScene()->GetMapManager();
	if (pMapManager == NULL)
	{// NULL��������
		return;
	}

	// �ʒu�X�V
	int nIdxMapPoint = GetMapIndex();
	float fPointRatio = GetMapPointRatio();
	float fMoveValue = GetMapMoveValue();


	// �ړ��ʎ擾
	float fMove = GetVelocity();

	// �ړ������ʉ��Z
	fMoveValue += fMove;

	pos = pMapManager->UpdateNowPosition(nIdxMapPoint, fPointRatio, fMoveValue, pos.y);

	// �}�b�v�̃C���f�b�N�X�ԍ��ݒ�
	SetMapIndex(nIdxMapPoint);

	// �}�b�v�|�C���g�Ԃ̊����ݒ�
	SetMapPointRatio(fPointRatio);

	// �}�b�v�|�C���g�Ԃ̈ړ��ʐݒ�
	SetMapMoveValue(fMoveValue);

	// �����␳
	if (m_state != STATE_KNOCKBACK && m_state != STATE_SPAWN && m_state != STATE_DMG && m_state != STATE_DEAD)
	{
		move.x += (0.0f - move.x) * 0.25f;
		move.z += (0.0f - move.z) * 0.25f;
	}

	if (move.x >= 0.1f || move.x <= -0.1f ||
		move.z >= 0.1f || move.z <= -0.1f)
	{// �ړ���
		m_sMotionFrag.bMove = true;
	}
	else
	{
		m_sMotionFrag.bMove = false;
	}

	// �����擾
	bool bLand = false;
	float fHeight = CGame::GetElevation()->GetHeight(pos, bLand);

	if (fHeight > pos.y)
	{// �n�ʂ̕���������荂��������

		// �n�ʂ̍����ɕ␳
		if (m_state != STATE_DEAD && m_state != STATE_FADEOUT)
		{
			pos.y = fHeight;
		}

		if (bLand == true)
		{// ���n���Ă���

			// ���n������
			ProcessLanding();

			// �W�����v�g�p�\�ɂ���
			if (m_state != STATE_KNOCKBACK && m_state != STATE_DMG && m_state != STATE_DEAD)
			{
				move.y = 0.0f;
			}
			m_sMotionFrag.bJump = false;
		}
	}

	if (m_state != STATE_FADEOUT)
	{
		// ��
		float fLen = CGame::GetElevation()->GetWidthLen();
		int nBlock = CGame::GetElevation()->GetWidthBlock();
		nBlock /= 2;
		bool bHit = false;
		if (pos.x + GetRadius() > fLen * nBlock) { pos.x = fLen * nBlock - GetRadius(); bHit = true; }
		if (pos.x - GetRadius() < -fLen * nBlock) { pos.x = -fLen * nBlock + GetRadius(); bHit = true; }
		if (pos.z + GetRadius() > fLen * nBlock) { pos.z = fLen * nBlock - GetRadius(); bHit = true; }
		if (pos.z - GetRadius() < -fLen * nBlock) { pos.z = -fLen * nBlock + GetRadius(); bHit = true; }

		if (bHit == true && m_pParent == NULL)
		{// ���E�܂ōs���ĂĐe��������
			float fRotDest = GetRotDest();
			fRotDest += D3DX_PI * 0.5f;
			SetRotDest(fRotDest);
		}

		if (CManager::GetMode() == CScene::MODE_GAME && 
			bHit == true && m_state != STATE_DMG && m_state != STATE_DEAD && m_state != STATE_KNOCKBACK && m_state != STATE_FADEOUT)
		{
			// �t�F�[�h�A�E�g��Ԃɂ���
			m_state = STATE_FADEOUT;
			m_nCntState = 0;
		}
	}

	if (pos.y <= -100.0f)
	{
		pos.y = -100.0f;
	}

	if (pos.y <= -800.0f)
	{
		pos.y = -800.0f;
	}



	// �ʒu�ݒ�
	SetPosition(pos);

	// �ړ��ʐݒ�
	SetMove(move);
}

//==========================================================================
// ���n���̏���
//==========================================================================
void CEnemy::ProcessLanding(void)
{
	// �ړ��ʎ擾
	D3DXVECTOR3 move = GetMove();

	// �W�����v�g�p�\�ɂ���
	if (m_state != STATE_KNOCKBACK && m_state != STATE_DMG && m_state != STATE_DEAD)
	{
		move.y = 0.0f;
	}
	m_sMotionFrag.bJump = false;

	// �ړ��ʐݒ�
	SetMove(move);
}

//==========================================================================
// �q�b�g����
//==========================================================================
bool CEnemy::Hit(const int nValue)
{
	// �����擾
	D3DXVECTOR3 rot = GetRotation();
	D3DXVECTOR3 pos = GetPosition();

	// �̗͎擾
	int nLife = GetLife();

	if (nValue == 0 && m_state != STATE_KNOCKBACK)
	{// �m�[�_���[�W�̏ꍇ

		// �ߋ��̏�ԕۑ�
		m_Oldstate = m_state;

		// �m�b�N�o�b�N���
		m_state = STATE_KNOCKBACK;

		// �J�ڃJ�E���^�[�ݒ�
		m_nCntState = 30;

		D3DXVECTOR3 move = GetMove();
		move.y = 0.0f;
		move.y += 10.5f;
		SetMove(move);
	}
	else if (nValue == 0 && m_state == STATE_KNOCKBACK)
	{
		return true;
	}
	else
	{
		if (m_state != STATE_DMG && m_state != STATE_KNOCKBACK && m_state != STATE_DEAD && m_state != STATE_SPAWN)
		{// �Ȃɂ��Ȃ���Ԃ̎�

			CManager::SetEnableHitStop(5);

			// �U��
			CManager::GetCamera()->SetShake(5, 10.0f, 0.0f);

			// �̗͌��炷
			nLife -= nValue;

			// �̗͐ݒ�
			SetLife(nLife);

			if (nLife > 0)
			{// �̗͂��Ȃ��Ȃ��ĂȂ�������

				// �_���[�W���Đ�
				CManager::GetSound()->PlaySound(CSound::LABEL_SE_DMG01);
			}

			if (nLife <= 0)
			{// �̗͂��Ȃ��Ȃ�����

				// ���S��Ԃɂ���
				m_state = STATE_DEAD;

				// �J�ڃJ�E���^�[�ݒ�
				m_nCntState = 0;

				// �m�b�N�o�b�N�̈ʒu�X�V
				m_posKnokBack = GetPosition();

				// �m�b�N�o�b�N����ɂ���
				m_sMotionFrag.bKnockback = true;

				// ���ꃂ�[�V����
				m_pMotion->Set(MOTION_KNOCKBACK);

				// ����
				return true;
			}

			// �␳
			ValueNormalize(nLife, GetLifeOrigin(), 0);

			// �ߋ��̏�ԕۑ�
			m_Oldstate = m_state;

			// �_���[�W��Ԃɂ���
			m_state = STATE_DMG;

			// �J�ڃJ�E���^�[�ݒ�
			m_nCntState = 0;

			// �m�b�N�o�b�N�̈ʒu�X�V
			m_posKnokBack = GetPosition();

			// �m�b�N�o�b�N����ɂ���
			m_sMotionFrag.bKnockback = true;

			// ���ꃂ�[�V����
			m_pMotion->Set(MOTION_KNOCKBACK);

		}
	}

	// ����łȂ�
	return false;
}

//==========================================================================
// ��ޕʍX�V����
//==========================================================================
void CEnemy::UpdateByType(void)
{
	// ���݂̃��[�V������ގ擾
	int nType = m_pMotion->GetType();

	switch (m_type)
	{
	case TYPE_POWER:
		break;

	default:
		break;
	}
}

//==========================================================================
// ��ԍX�V����
//==========================================================================
void CEnemy::UpdateState(void)
{
	//return;
#if TOPCUR
	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR3 pos11 = D3DXVECTOR3(GetObjectChara()->GetModel()[0]->GetWorldMtx()._41, GetObjectChara()->GetModel()[0]->GetWorldMtx()._42, GetObjectChara()->GetModel()[0]->GetWorldMtx()._43);

	//CEffect3D::Create(pos11, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), GetRadius(), 10, CEffect3D::MOVEEFFECT_NONE, CEffect3D::TYPE_NORMAL);

	// �ړ��ʎ擾
	D3DXVECTOR3 move = GetMove();

	// �F�ݒ�
	m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_mMatcol.a);

	switch (m_state)
	{
	case STATE_NONE:

		// ��ԑJ�ڃJ�E���^�[���Z
		m_nCntState--;

		if (m_nCntState <= 0)
		{// �J�ڃJ�E���^�[��0�ɂȂ�����

			// �v���C���[�T����Ԃɂ���
			m_state = STATE_PLAYERSEARCH;
			m_nCntState = 60 * 8;
		}

		// �F�ݒ�
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		break;

	case STATE_SPAWN:
		Spawn();
		break;

	case STATE_DMG:
		Damage();
		break;

	case STATE_DEAD:
		Dead();
		break;

	case STATE_FADEOUT:
		FadeOut();
		break;

	case STATE_KNOCKBACK:
		KnockBack();
		break;

	case STATE_PLAYERCHASE:
		PlayerChase();
		break;

	case STATE_RETURNBASE:
		ReturnBase();
		break;

	case STATE_PLAYERSEARCH:
		PlayerSearch();
		break;

	case STATE_PARENTCHASE:
		ParentChase();
		break;

	case STATE_ATTACK:
		StateAttack();
		break;

	case STATE_WAIT:
		StateWait();
		break;

	}
#endif
}

//==========================================================================
// ��ޕʏ�ԍX�V����
//==========================================================================
void CEnemy::UpdateStateByType(void)
{
	return;
}

//==========================================================================
// �o��
//==========================================================================
void CEnemy::Spawn(void)
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
	if (CGame::GetElevation()->IsHit(pos))
	{// �n�ʂƓ������Ă�����
		m_state = m_Oldstate;	// �ߋ��̏��ۑ�
		m_nCntState = 0;		// �J�ڃJ�E���^�[�ݒ�
		move.y = 0.0f;			// �ړ��ʃ[��
	}

	// �ʒu�ݒ�
	SetPosition(pos);

	// �ړ��ʐݒ�
	SetMove(move);
}

//==========================================================================
// �_���[�W
//==========================================================================
void CEnemy::Damage(void)
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
	CPlayer *pPlayer = CManager::GetScene()->GetPlayer();

#if _DEBUG
	// �F�ݒ�
	m_mMatcol = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
#endif

	// ��ԑJ�ڃJ�E���^�[���Z
	m_nCntState++;

	// �ʒu�X�V
#if 0
	pos = (D3DXVECTOR3(0.0f, -0.4f, 0.0f) * (float)(m_nCntState * m_nCntState) + move * (float)m_nCntState) + m_posKnokBack;
#else
	pos.y = (-0.4f * (float)(m_nCntState * m_nCntState) + move.y * (float)m_nCntState) + m_posKnokBack.y;
	pos.x += move.x;
	pos.z += move.z;
#endif

	// �N���Ƃ̔���
	if (CGame::GetElevation()->IsHit(pos) && m_nCntState >= 20)
	{// �n�ʂƓ������Ă�����
		m_state = m_Oldstate;
		m_nCntState = 0;
		move.y = 0.0f;			// �ړ��ʃ[��
		
		// �m�b�N�o�b�N�������
		m_sMotionFrag.bKnockback = false;
		m_pMotion->ToggleFinish(true);
	}

	if (m_nCntState >= 30)
	{// �J�ڃJ�E���^�[��20�ɂȂ�����

		// �m�b�N�o�b�N��Ԃɂ���
		m_state = STATE_KNOCKBACK;
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
// ���S
//==========================================================================
void CEnemy::Dead(void)
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
	CPlayer *pPlayer = CManager::GetScene()->GetPlayer();

	// ��ԑJ�ڃJ�E���^�[���Z
	m_nCntState++;

	// �F�ݒ�
	m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_mMatcol.a);
	m_mMatcol.a -= 1.0f / 80.0f;

	// �ʒu�X�V
	pos = (D3DXVECTOR3(0.0f, -0.15f, 0.0f) * (float)(m_nCntState * m_nCntState) + move * (float)m_nCntState) + m_posKnokBack;

	if (m_nCntState >= 40 || (CGame::GetElevation()->IsHit(pos) && m_nCntState >= 20))
	{// �J�ڃJ�E���^�[��0�ɂȂ����� or �n�ʂɐڐG

		// �p�[�e�B�N������
		my_particle::Create(pos, my_particle::TYPE_ENEMY_FADE);

		// �G�̏I������
		Kill();
		Uninit();
		return;
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
// �t�F�[�h�A�E�g
//==========================================================================
void CEnemy::FadeOut(void)
{
	// �ړ��ʎ擾
	float fMove = GetVelocity();

	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �t�F�[�h�A�E�g���[�V�����ݒ�
	m_pMotion->Set(MOTION_FADEOUT);

	// �t�F�[�h�A�E�g�̃t���[����
	int nAllFrame = m_pMotion->GetMaxAllCount(MOTION_FADEOUT);

	// ���[�V�����̏��擾
	CMotion::Info aInfo = m_pMotion->GetInfo(m_pMotion->GetType());

	// �U�����̑����擾
	int nNumAttackInfo = aInfo.nNumAttackInfo;

	// �����擾
	bool bLand = false;
	float fHeight = CGame::GetElevation()->GetHeight(pos, bLand);

	m_sMotionFrag.bMove = false;	// �ړ�����OFF
	m_sMotionFrag.bATK = false;		// �U������OFF

	// �J�ڃJ�E���^�[���Z
	m_nCntState++;

	if (m_nCntState >= nAllFrame)
	{// �J�ڃJ�E���^�[�����[�V�����𒴂�����

		// �X�R�A���Z�̔���I�t
		m_bAddScore = false;

		// �G�̏I������
		Kill();
		Uninit();
		return;
	}
}

//==========================================================================
// �m�b�N�o�b�N
//==========================================================================
void CEnemy::KnockBack(void)
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
	CPlayer *pPlayer = CManager::GetScene()->GetPlayer();


	// ��ԑJ�ڃJ�E���^�[���Z
	m_nCntState++;

	// �ʒu�X�V
#if 0
	pos = (D3DXVECTOR3(0.0f, -0.4f, 0.0f) * (float)(m_nCntState * m_nCntState) + move * (float)m_nCntState) + m_posKnokBack;
#else
	pos.y = (-0.4f * (float)(m_nCntState * m_nCntState) + move.y * (float)m_nCntState) + m_posKnokBack.y;
	pos.x += move.x;
	pos.z += move.z;
#endif

	// �N���Ƃ̔���
	if (CGame::GetElevation()->IsHit(pos) && m_nCntState >= 20)
	{// �n�ʂƓ������Ă�����
		m_state = m_Oldstate;
		m_nCntState = 0;
		move.y = 0.0f;	// �ړ��ʃ[��
		
		// �m�b�N�o�b�N�������
		m_sMotionFrag.bKnockback = false;
		m_pMotion->ToggleFinish(true);
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
// �v���C���[�T��
//==========================================================================
void CEnemy::PlayerSearch(void)
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
	CPlayer *pPlayer = CManager::GetScene()->GetPlayer();


	// ��ԑJ�ڃJ�E���^�[���Z
	m_nCntState--;

	if (m_nCntState <= 0)
	{// �J�ڃJ�E���^�[��0�ɂȂ�����
		m_nCntState = 0;
		m_state = STATE_RETURNBASE;
	}

	if (m_pParent != NULL)
	{// �e������ꍇ

		// �e�̈ړ��ʎ擾
		D3DXVECTOR3 moveParent = m_pParent->GetMove();

		// �e�̈ʒu�擾
		D3DXVECTOR3 posParent = m_pParent->GetPosition();

		// �ڕW�̊p�x�����߂�
		fRotDest = atan2f((pos.x - posParent.x), (pos.z - posParent.z));

		// �ڕW�Ƃ̍���
		fRotDiff = fRotDest - rot.y;

		//�p�x�̐��K��
		RotNormalize(fRotDiff);

		//�p�x�̕␳������
		rot.y += fRotDiff * 0.025f;

		// �p�x�̐��K��
		RotNormalize(rot.y);

		if (CircleRange(pos, posParent, CHACE_DISTABCE, CHACE_DISTABCE) == true)
		{// ��苗���Ԃɐe����������
			bLen = true;	// ��������
		}

		// �����Ă�����Ƀ_�b�V��
		if (bLen == false)
		{// �������ۂ���Ă�����

			// �ǂ��|���ړ�����
			ChaseMove(fMove * 1.5f);
		}
	}
	else
	{// �������g���e�̎�

		static int nCnt = 0;

		float fRotDiff = 0.0f;	// ���݂ƖڕW�̍���

		nCnt = (nCnt + 1) % 120;

		// �ڕW�̊p�x�����߂�
		if (nCnt == 0)
		{
			fRotDest = Random(-31, 31) * 0.1f;
		}

		// �ڕW�Ƃ̍���
		fRotDiff = fRotDest - rot.y;

		//�p�x�̐��K��
		RotNormalize(fRotDiff);

		//�p�x�̕␳������
		rot.y += fRotDiff * 0.025f;

		// �p�x�̐��K��
		RotNormalize(rot.y);

		// �ǂ��|���ړ�����
		ChaseMove(fMove);

#if _DEBUG
		// �F�ݒ�
		m_mMatcol = D3DXCOLOR(1.0f, 0.5f, 1.0f, 1.0f);
#endif
	}

	// �v���C���[�Ǐ]�̔���
	TriggerChasePlayer();


	// �ʒu�ݒ�
	SetPosition(pos);

	// �����ݒ�
	SetRotation(rot);

	// �ڕW�̌����ݒ�
	SetRotDest(fRotDest);
}

//==========================================================================
// �v���C���[�Ǐ]
//==========================================================================
void CEnemy::PlayerChase(void)
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
	CPlayer *pPlayer = CManager::GetScene()->GetPlayer();

	// �I�u�W�F�N�g���
	CObject *pObj = NULL;


	// ��ԑJ�ڃJ�E���^�[���Z
	m_nCntState--;

	if (m_nCntState <= 0)
	{// �J�ڃJ�E���^�[��0�ɂȂ�����
		m_nCntState = 0;
	}

	if (pPlayer != NULL)
	{// NULL����Ȃ��Ƃ�

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

		// �����ݒ�
		SetRotation(rot);

		// �U����Ԉڍs����
		ChangeToAttackState();

		// �����Ă�����Ƀ_�b�V��
		if (bLen == false)
		{// �������ۂ���Ă�����

			// �ǂ��|���ړ�����
			ChaseMove(fMove);
		}

		float fRadius = PLAYER_SERCH;
		if (CircleRange(pos, pPlayer->GetPosition(), 200.0f, fRadius) == false)
		{// �v���C���[�����E�����������

			m_state = STATE_PLAYERSEARCH;
			m_nCntState = 60 * 10;
		}
	}


	// �ʒu�ݒ�
	SetPosition(pos);

	// �ڕW�̌����ݒ�
	SetRotDest(fRotDest);
}

//==========================================================================
// �e�Ǐ]
//==========================================================================
void CEnemy::ParentChase(void)
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

	// ���Ȃ藣��Ă邩�̔���
	bool bLongDistance = true;

	// �v���C���[���
	CPlayer *pPlayer = CManager::GetScene()->GetPlayer();

	// �I�u�W�F�N�g���
	CObject *pObj = NULL;


	// ��ԑJ�ڃJ�E���^�[���Z
	//m_nCntState++;

	if (m_nCntState <= 0)
	{// �J�ڃJ�E���^�[��0�ɂȂ�����
		m_nCntState = 0;
	}

	if (m_pParent != NULL)
	{// �e������ꍇ

		// �e�̈ړ��ʎ擾
		D3DXVECTOR3 moveParent = m_pParent->GetMove();

		// �e�̈ʒu�擾
		D3DXVECTOR3 posParent = m_pParent->GetPosition();

		// �e�̌����擾
		D3DXVECTOR3 rotParent = m_pParent->GetRotation();

		// �ڕW�̈ʒu
		D3DXVECTOR3 posDest;
		posDest = posParent;

		// �ڕW�̊p�x�����߂�
		fRotDest = atan2f((pos.x - posDest.x), (pos.z - posDest.z));

		// �ڕW�Ƃ̍���
		fRotDiff = fRotDest - rot.y;

		//�p�x�̐��K��
		RotNormalize(fRotDiff);

		//�p�x�̕␳������
		rot.y += fRotDiff * 0.025f;

		// �p�x�̐��K��
		RotNormalize(rot.y);

		// �����ݒ�
		SetRotation(rot);

		if (CircleRange(pos, posDest, 25.0f, CHACE_DISTABCE) == true)
			{// ��苗���Ԃɐe����������
				bLen = true;	// ��������
			}

			// �����Ă�����Ƀ_�b�V��
			if (bLen == false)
			{// �������ۂ���Ă�����

				// �ǂ��|���ړ�����
				ChaseMove(fMove * 1.4f);
			}

	}
	else
	{// �������g���e�̎�

		float fRotDiff = 0.0f;	// ���݂ƖڕW�̍���

		// ��ԑJ�ڃJ�E���^�[�X�V
		m_nCntState = (m_nCntState + 1) % 120;

		// �ڕW�̊p�x�����߂�
		if (m_nCntState == 0)
		{
			fRotDest = Random(-31, 31) * 0.1f;
		}

		// �ڕW�Ƃ̍���
		fRotDiff = fRotDest - rot.y;

		//�p�x�̐��K��
		RotNormalize(fRotDiff);

		//�p�x�̕␳������
		rot.y += fRotDiff * 0.025f;

		// �p�x�̐��K��
		RotNormalize(rot.y);

		// �����ݒ�
		SetRotation(rot);

		// �ǂ��|���ړ�����
		ChaseMove(fMove);

#if _DEBUG
		// �F�ݒ�
		m_mMatcol = D3DXCOLOR(1.0f, 0.5f, 1.0f, 1.0f);
#endif
	}

	if (m_colorType == COLORTYPE_POISON ||
		m_colorType == COLORTYPE_CONFUSION)
	{// �ŃL�m�R�͍U���I

		// �v���C���[�Ǐ]�̔���
		TriggerChasePlayer();
	}

	// �ڕW�̌����ݒ�
	SetRotDest(fRotDest);
}

//==========================================================================
// ���_�ɋA��
//==========================================================================
void CEnemy::ReturnBase(void)
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
	CPlayer *pPlayer = CManager::GetScene()->GetPlayer();

	// �I�u�W�F�N�g���
	CObject *pObj = NULL;


	// �ڕW�̊p�x�����߂�
	fRotDest = atan2f((pos.x - m_posOrigin.x), (pos.z - m_posOrigin.z));

	// �ڕW�Ƃ̍���
	fRotDiff = fRotDest - rot.y;

	//�p�x�̐��K��
	RotNormalize(fRotDiff);

	//�p�x�̕␳������
	rot.y += fRotDiff * 0.025f;

	// �p�x�̐��K��
	RotNormalize(rot.y);

	if (CircleRange(pos, m_posOrigin, 25.0f, CHACE_DISTABCE) == true)
	{// ��苗���Ԃɐe����������
		bLen = true;	// ��������
		m_state = STATE_NONE;
		m_nCntState = 90;
	}

	// �����Ă�����Ƀ_�b�V��
	if (bLen == false)
	{// �������ۂ���Ă�����

		// �ǂ��|���ړ�����
		ChaseMove(fMove * 1.2f);
	}

	// �v���C���[�Ǐ]�̔���
	TriggerChasePlayer();

	// �ʒu�ݒ�
	SetPosition(pos);

	// �����ݒ�
	SetRotation(rot);

	// �ڕW�̌����ݒ�
	SetRotDest(fRotDest);
}

//==========================================================================
// �U������
//==========================================================================
void CEnemy::StateAttack(void)
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
	CPlayer *pPlayer = CManager::GetScene()->GetPlayer();

	// �I�u�W�F�N�g���
	CObject *pObj = NULL;


	// ���݂̎�ގ擾
	int nType = m_pMotion->GetType();

	if (m_pParent != NULL)
	{// �e������ꍇ

		// �e���ǂ��|����Ԃɂ���
		if (m_pParent->m_state != STATE_ATTACK)
		{
			if (m_pParent->m_state != STATE_DMG && m_pParent->m_state != STATE_DEAD)
			{
				m_pParent->m_state = STATE_PLAYERCHASE;
				m_pParent->m_nCntState = 60;
			}

			for (int nCntEnemy = 0; nCntEnemy < m_pParent->m_nNumChild; nCntEnemy++)
			{// �q�̐�����
				if (m_pParent->m_pChild[nCntEnemy] == NULL)
				{
					continue;
				}

				if (m_pParent->m_pChild[nCntEnemy]->m_state == STATE_DMG || m_pParent->m_pChild[nCntEnemy]->m_state == STATE_DEAD || m_pParent->m_pChild[nCntEnemy]->m_state == STATE_KNOCKBACK)
				{
					continue;
				}

				m_pParent->m_pChild[nCntEnemy]->m_state = STATE_PLAYERCHASE;
				m_pParent->m_pChild[nCntEnemy]->m_nCntState = 60;
			}
		}
	}
	else
	{// �������e�ȏꍇ

		for (int nCntEnemy = 0; nCntEnemy < m_nNumChild; nCntEnemy++)
		{// �q�̐�����

			if (m_pChild[nCntEnemy] == NULL)
			{// NULL��������
				continue;
			}

			if (m_pChild[nCntEnemy]->m_state != STATE_ATTACK)
			{// �U����Ԃ̎�

				if (m_pChild[nCntEnemy]->m_state != STATE_DMG && m_pChild[nCntEnemy]->m_state != STATE_DEAD && m_pChild[nCntEnemy]->m_state != STATE_KNOCKBACK)
				{
					m_pChild[nCntEnemy]->m_state = STATE_PLAYERCHASE;
					m_pChild[nCntEnemy]->m_nCntState = 60;
				}
			}
		}
	}

	if (nType == MOTION_DEF && pPlayer != NULL)
	{// �j���[�g�����ɖ߂��

		if (CircleRange(pos, pPlayer->GetPosition(), 400.0f, PLAYER_SERCH) == false)
		{// �v���C���[�Ɨ��ꂷ���Ă�����

			// �Ԋu���������Ԃɂ���
			m_state = STATE_PLAYERSEARCH;
			m_nCntState = 60 * 8;
		}
		else
		{// �܂��ǂ��鎞

			m_state = STATE_PLAYERCHASE;

			if (m_pParent != NULL)
			{// �e������ꍇ

				// �e���ǂ��|����Ԃɂ���
				if (m_pParent->m_state != STATE_DMG && m_pParent->m_state != STATE_DEAD && m_pParent->m_state != STATE_KNOCKBACK)
				{
					m_pParent->m_state = STATE_PLAYERCHASE;
				}

				for (int nCntEnemy = 0; nCntEnemy < m_pParent->m_nNumChild; nCntEnemy++)
				{// �q�̐�����

					if (m_pParent->m_pChild[nCntEnemy] == NULL)
					{
						continue;
					}

					if (m_pParent->m_pChild[nCntEnemy]->m_state == STATE_DMG || m_pParent->m_pChild[nCntEnemy]->m_state == STATE_DEAD || m_pParent->m_pChild[nCntEnemy]->m_state == STATE_KNOCKBACK)
					{
						continue;
					}

					m_pParent->m_pChild[nCntEnemy]->m_state = STATE_PLAYERCHASE;
				}
			}
			else
			{// �������e�ȏꍇ

				for (int nCntEnemy = 0; nCntEnemy < m_nNumChild; nCntEnemy++)
				{// �q�̐�����

					if (m_pChild[nCntEnemy] == NULL)
					{
						continue;
					}

					if (m_pChild[nCntEnemy]->m_state == STATE_DMG || m_pChild[nCntEnemy]->m_state == STATE_DEAD || m_pChild[nCntEnemy]->m_state == STATE_KNOCKBACK)
					{
						continue;
					}

					m_pChild[nCntEnemy]->m_state = STATE_PLAYERCHASE;
				}
			}
		}
	}

	// �ʒu�ݒ�
	SetPosition(pos);

	// �ړ��ʐݒ�
	SetMove(move);

	// �����ݒ�
	SetRotation(rot);

	// �ڕW�̌����ݒ�
	SetRotDest(fRotDest);


	// �F�ݒ�
#if _DEBUG
	m_mMatcol = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
#endif
}

//==========================================================================
// ��ޕʏ�ԍX�V����
//==========================================================================
void CEnemy::StateWait(void)
{
	return;
}

//==========================================================================
// �v���C���[�Ǐ]ON�ɂ���g���K�[
//==========================================================================
void CEnemy::TriggerChasePlayer(void)
{

	// �v���C���[���
	CPlayer *pPlayer = CManager::GetScene()->GetPlayer();

	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	if (pPlayer != NULL)
	{// NULL����Ȃ��Ƃ�

		float fRadius = PLAYER_SERCH;

		if (CircleRange(pos, pPlayer->GetPosition(), 200.0f, fRadius) == true)
		{// �v���C���[���͈͂ɓ����
			m_state = STATE_PLAYERCHASE;

			if (m_pParent != NULL)
			{// �e������ꍇ

				// �e���ǂ��|����Ԃɂ���
				if (m_pParent->m_state != STATE_DMG && m_pParent->m_state != STATE_DEAD && m_pParent->m_state != STATE_KNOCKBACK)
				{
					m_pParent->m_state = STATE_PLAYERCHASE;
				}

				for (int nCntEnemy = 0; nCntEnemy < m_pParent->m_nNumChild; nCntEnemy++)
				{// �q�̐�����

					if (m_pParent->m_pChild[nCntEnemy] == NULL)
					{
						continue;
					}

					if (m_pParent->m_pChild[nCntEnemy]->m_state == STATE_DMG || m_pParent->m_pChild[nCntEnemy]->m_state == STATE_DEAD || m_pParent->m_pChild[nCntEnemy]->m_state == STATE_KNOCKBACK)
					{
						continue;
					}

					// �����̐e�̎q���v���C���[�ǂ��|�����
					m_pParent->m_pChild[nCntEnemy]->m_state = STATE_PLAYERCHASE;
				}
			}
			else
			{// �������e�ȏꍇ

				for (int nCntEnemy = 0; nCntEnemy < m_nNumChild; nCntEnemy++)
				{// �q�̐�����

					if (m_pChild[nCntEnemy] == NULL)
					{
						continue;
					}

					if (m_pChild[nCntEnemy]->m_state == STATE_DMG || m_pChild[nCntEnemy]->m_state == STATE_DEAD || m_pChild[nCntEnemy]->m_state == STATE_KNOCKBACK)
					{
						continue;
					}

					m_pChild[nCntEnemy]->m_state = STATE_PLAYERCHASE;
				}
			}
		}
	}
}

//==========================================================================
// �U����Ԉڍs����
//==========================================================================
void CEnemy::ChangeToAttackState(void)
{
	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �v���C���[���
	CPlayer *pPlayer = CManager::GetScene()->GetPlayer();

	if (pPlayer == NULL)
	{
		return;
	}

	// �e�̈ʒu�擾
	D3DXVECTOR3 posPlayer = pPlayer->GetPosition();

	if (CircleRange(pos, posPlayer, 400.0f, pPlayer->GetRadius()) == true && m_sMotionFrag.bJump == false)
	{// ��苗���ԂɃv���C���[����������

		// �U����Ԃɂ���
		m_state = STATE_ATTACK;
		m_sMotionFrag.bATK = true;
	}
}

//==========================================================================
// �ǂ��|���ړ�
//==========================================================================
void CEnemy::ChaseMove(float fMove)
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
// �U��
//==========================================================================
void CEnemy::Atack(void)
{

	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �����擾
	D3DXVECTOR3 rot = GetRotation();

	// �ړ��ʎ擾
	D3DXVECTOR3 move = GetMove();

	// ���[�V�����̏��擾
	CMotion::Info aInfo = m_pMotion->GetInfo(m_pMotion->GetType());

	// �U�����̑����擾
	int nNumAttackInfo = aInfo.nNumAttackInfo;

	for (int nCntAttack = 0; nCntAttack < nNumAttackInfo; nCntAttack++)
	{
		if (aInfo.AttackInfo[nCntAttack] == NULL)
		{// NULL��������
			continue;
		}

		// �U�����ۑ�
		CMotion::AttackInfo atkInfo = *aInfo.AttackInfo[nCntAttack];

		if (m_pMotion->GetAllCount() == aInfo.AttackInfo[nCntAttack]->nInpactCnt)
		{// �Ռ��̃J�E���g�Ɠ����Ƃ�

			// �U��������
			AttackAction(atkInfo.nCollisionNum, atkInfo);
		}

		// ���[�V�����J�E���^�[�擾
		if (m_pMotion->GetAllCount() > aInfo.AttackInfo[nCntAttack]->nMinCnt && m_pMotion->GetAllCount() < aInfo.AttackInfo[nCntAttack]->nMaxCnt)
		{// �U�����蒆

			// �v���C���[���
			CPlayer *pPlayer = CManager::GetScene()->GetPlayer();

			if (pPlayer == NULL)
			{// NULL��������
				return;
			}

			// �G�̈ʒu�擾
			D3DXVECTOR3 TargetPos = pPlayer->GetPosition();

			// ���f�����擾
			CModel **pModel = GetObjectChara()->GetModel();
			D3DXMATRIX mtxTrans;	// �v�Z�p�}�g���b�N�X�錾

			// ����̃}�g���b�N�X�擾
			D3DXMATRIX mtxWepon;
			D3DXMatrixIdentity(&mtxWepon);

			if (pModel[aInfo.AttackInfo[nCntAttack]->nCollisionNum] == NULL)
			{// NULL��������
				return;
			}

			// ���肷��p�[�c�̃}�g���b�N�X�擾
			mtxWepon = pModel[aInfo.AttackInfo[nCntAttack]->nCollisionNum]->GetWorldMtx();

			// �ʒu�𔽉f����
			D3DXMatrixTranslation(&mtxTrans, aInfo.AttackInfo[nCntAttack]->Offset.x, aInfo.AttackInfo[nCntAttack]->Offset.y, aInfo.AttackInfo[nCntAttack]->Offset.z);
			D3DXMatrixMultiply(&mtxWepon, &mtxTrans, &mtxWepon);

			// ����̈ʒu
			D3DXVECTOR3 weponpos = D3DXVECTOR3(mtxWepon._41, mtxWepon._42, mtxWepon._43);
#if _DEBUG
			CEffect3D::Create(weponpos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), aInfo.AttackInfo[nCntAttack]->fRangeSize * 0.5f, 10, CEffect3D::MOVEEFFECT_NONE, CEffect3D::TYPE_NORMAL);
#endif
			// ����T�C�Y�擾
			float fRadius = pPlayer->GetObjectChara()->GetRadius();

			if (aInfo.AttackInfo[nCntAttack]->fRangeSize == 0.0f)
			{
				continue;
			}

			if (SphereRange(weponpos, TargetPos, aInfo.AttackInfo[nCntAttack]->fRangeSize, fRadius))
			{// ���̔���

#if 0
				int playerState = pPlayer->GetState();
				if (playerState != CPlayer::STATE_DMG &&
					playerState != CPlayer::STATE_KNOCKBACK &&
					playerState != CPlayer::STATE_DEAD &&
					playerState != CPlayer::STATE_INVINCIBLE)
				{
					D3DXVECTOR3 PlayerRot = pPlayer->GetRotation();

					// �^�[�Q�b�g�ƓG�Ƃ̌���
					float fRot = atan2f((pos.x - TargetPos.x), (pos.z - TargetPos.z));

					// �����𐳖ʂɂ���
					fRot = D3DX_PI + fRot;

					// �p�x�̐��K��
					RotNormalize(fRot);

					pPlayer->SetRotation(D3DXVECTOR3(PlayerRot.x, fRot, PlayerRot.z));
					fRot = pPlayer->GetRotation().y;

					// ������΂�
					pPlayer->SetMove(D3DXVECTOR3(
						sinf(fRot) * 4.0f,
						12.0f,
						cosf(fRot) * 4.0f));
				}
				if (pPlayer->Hit(aInfo.AttackInfo[nCntAttack]->nDamage) == true)
				{// ����ł���

					my_particle::Create(TargetPos, my_particle::TYPE_OFFSETTING);

					// �v���C���[���
					CPlayer *pPlayer = CManager::GetScene()->GetPlayer();
					CPlayer **ppPlayer = &pPlayer;

					// �v���C���[��NULL
					ppPlayer = NULL;
					int m = 0;
				}
#endif
			}
		}
	}

	// �����ݒ�
	SetRotation(rot);
}

//==========================================================================
// �U��������
//==========================================================================
void CEnemy::AttackAction(int nModelNum, CMotion::AttackInfo ATKInfo)
{
	return;
}

//==========================================================================
// �`�揈��
//==========================================================================
void CEnemy::Draw(void)
{
#if _DEBUG
	if (m_mMatcol != D3DXCOLOR(1.0f, 1.0f, 1.0f, m_mMatcol.a))
	{
		// �I�u�W�F�N�g�L�����̕`��
		CObjectChara::Draw(m_mMatcol);
	}
	else
	{
		// �I�u�W�F�N�g�L�����̕`��
		CObjectChara::Draw();
	}
#else
	//CObjectChara::Draw();

	if (m_mMatcol != D3DXCOLOR(1.0f, 1.0f, 1.0f, m_mMatcol.a))
	{
		// �I�u�W�F�N�g�L�����̕`��
		CObjectChara::Draw(m_mMatcol);
	}
	else
	{
		// �I�u�W�F�N�g�L�����̕`��
		CObjectChara::Draw();
	}
#endif
}

//==========================================================================
// ��Ԏ擾
//==========================================================================
int CEnemy::GetState(void)
{
	return m_state;
}

//==========================================================================
// ���̌���
//==========================================================================
void CEnemy::SetOriginRotation(D3DXVECTOR3 rot)
{
	m_rotOrigin = rot;
}

//==========================================================================
// ��Ԑݒ�
//==========================================================================
void CEnemy::SetState(int state)
{
	m_state = (STATE)state;
}

//==========================================================================
// ��Ԑݒ�
//==========================================================================
void CEnemy::SetState(int state, int nCntState)
{
	m_state = (STATE)state;
	m_nCntState = nCntState;
}

//==========================================================================
// ��ގ擾
//==========================================================================
int CEnemy::GetCharaType(void)
{
	return m_type;
}

//==========================================================================
// �G�̏��擾
//==========================================================================
CEnemy *CEnemy::GetEnemy(void)
{
	// �������g�̃|�C���^���擾
	return this;
}