//=============================================================================
// 
//  �v���C���[���� [player.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "game.h"
#include "player.h"
#include "camera.h"
#include "manager.h"
#include "debugproc.h"
#include "renderer.h"
#include "input.h"
#include "enemy.h"
#include "calculation.h"
#include "score.h"
#include "texture.h"
#include "Xload.h"
#include "model.h"
#include "motion.h"
#include "hp_gauge.h"
#include "objectChara.h"
#include "elevation.h"
#include "shadow.h"
#include "particle.h"
#include "3D_Effect.h"
#include "ballast.h"
#include "impactwave.h"
#include "sound.h"
#include "enemymanager.h"
#include "mapmanager.h"
#include "edit_controlpoint.h"
#include "bullet.h"
#include "slash.h"
#include "camerachasechanger.h"
#include "cameraaxis.h"
#include "stage.h"
#include "objectX.h"
#include "gamemanager.h"
#include "instantfade.h"

// �h����
#include "tutorialplayer.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define CHARAFILE		"data\\TEXT\\motion_set_player.txt"
#define JUMP			(20.0f * 1.5f)	// �W�����v�͏����l
#define MAX_LIFE		(100)			// �̗�
#define TARGET_LEN		(400.0f)		// �ڕW�܂ł̋���
#define WALK_INT		(30)			// ���s�̎���
#define INVINCIBLE_INT	(2)				// ���G�̊Ԋu
#define INVINCIBLE_TIME	(90)			// ���G�̎���
#define CONFUSIONTIME	(60 * 2)		// ��������

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CPlayer::CPlayer(int nPriority) : CObjectChara(nPriority)
{
	// �l�̃N���A
	m_state = STATE_NONE;	// ���
	m_Oldstate = m_state;	// �O��̏��
	m_atkRush = ATKRUSH_RIGHT;	// �A���A�^�b�N�̎��
	m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// �}�e���A���̐F
	m_posKnokBack = mylib_const::DEFAULT_VECTOR3;	// �m�b�N�o�b�N�̈ʒu
	m_KnokBackMove = mylib_const::DEFAULT_VECTOR3;	// �m�b�N�o�b�N�̈ړ���
	m_bLandOld = false;				// �ߋ��̒��n���
	m_bJump = false;				// �W�����v�����ǂ���
	m_sMotionFrag.bATK = false;		// ���[�V�����̃t���O
	m_sMotionFrag.bJump = false;		// ���[�V�����̃t���O
	m_sMotionFrag.bKnockBack = false;	// ���[�V�����̃t���O
	m_sMotionFrag.bMove = false;		// ���[�V�����̃t���O
	m_nAngle = 1;					// ����
	m_nCntState = 0;				// ��ԑJ�ڃJ�E���^�[
	m_nCntWalk = 0;					// ���s�J�E���^�[
	m_nCntConfusion = 0;			// �����J�E���^�[
	m_nTexIdx = 0;					// �e�N�X�`���̃C���f�b�N�X�ԍ�
	m_nIdxXFile = 0;				// X�t�@�C���̃C���f�b�N�X�ԍ�
	m_fAtkStickRot = 0.0f;			// �U�����̃X�e�B�b�N����
	m_fBodyRot = 0.0f;				// �U�����̐g�̌���
	m_fTruePosY = 0.0f;				// �{���̕␳��̈ʒu
	m_bStick = false;				// �X�e�B�b�N�|��������
	m_StickAngle = ANGLE_RIGHT;		// �X�e�B�b�N�̌���
	m_OldStickAngle = ANGLE_RIGHT;	// �O��̃X�e�B�b�N�̌���
	m_pMotion = NULL;				// ���[�V�����̏��
	m_pShadow = NULL;				// �e�̏��
	m_pTargetP = NULL;				// �ڕW�̒n�_
	m_pHPGauge = NULL;				// HP�Q�[�W�̏��
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CPlayer::~CPlayer()
{

}

//==========================================================================
// ��������
//==========================================================================
CPlayer *CPlayer::Create(void)
{
	// �����p�̃I�u�W�F�N�g
	CPlayer *pPlayer = NULL;

	if (pPlayer == NULL)
	{// NULL��������

		// �������̊m��
		switch (CManager::GetInstance()->GetMode())
		{
		case CScene::MODE_GAME:
			pPlayer = DEBUG_NEW CPlayer;
			break;

		case CScene::MODE_TUTORIAL:
			pPlayer = DEBUG_NEW CTutorialPlayer;
			break;

		default:
			return NULL;
			break;
		}

		if (pPlayer != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ����������
			pPlayer->Init();
		}

		return pPlayer;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CPlayer::Init(void)
{
	// ��ނ̐ݒ�
	SetType(TYPE_PLAYER);

	m_state = STATE_NONE;	// ���
	m_nCntState = 0;		// ��ԑJ�ڃJ�E���^�[
	m_bLandOld = true;

	// �L�����쐬
	HRESULT hr = SetCharacter(CHARAFILE);

	if (FAILED(hr))
	{// ���s���Ă�����
		return E_FAIL;
	}

	// ���[�V�����̐�������
	m_pMotion = CMotion::Create(CHARAFILE);

	// �I�u�W�F�N�g�L�����N�^�[�̏��擾
	CObjectChara *pObjChar = GetObjectChara();

	// ���[�V�����̐ݒ�
	m_pMotion->SetModel(pObjChar->GetModel(), pObjChar->GetNumModel(), CObjectChara::GetObjectChara());

	// �̗̓Q�[�W
	//m_pHPGauge = CHP_Gauge::Create(120.0f, m_nLife);

	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �e�̐���
	m_pShadow = CShadow::Create(pos, 50.0f);

	// �|�[�Y�̃��Z�b�g
	m_pMotion->ResetPose(MOTION_DEF);
	//m_atkRush = ATKRUSH_LEFT;	// �A���A�^�b�N�̎��
	//SetMapIndex(35);

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CPlayer::Uninit(void)
{
	if (m_pMotion != NULL)
	{
		m_pMotion->Uninit();
		delete m_pMotion;
		m_pMotion = NULL;
	}

	// HP�Q�[�W������
	if (m_pHPGauge != NULL)
	{
		//m_pHPGauge->Uninit();
		m_pHPGauge = NULL;
	}

	// �e������
	if (m_pShadow != NULL)
	{
		//m_pShadow->Uninit();
		m_pShadow = NULL;
	}

	// �I������
	CObjectChara::Uninit();

	// ���[�h�ʏI������
	UninitByMode();
}

//==========================================================================
// ���[�h�ʏI������
//==========================================================================
void  CPlayer::UninitByMode(void)
{
	CScene *pScene = CManager::GetInstance()->GetScene();
	if (pScene != NULL)
	{
		// �v���C���[���
		CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();
		CPlayer **ppPlayer = &pPlayer;

		// �v���C���[��NULL
		*ppPlayer = NULL;
		int n = 0;
		CManager::GetInstance()->GetScene()->UninitPlayer();
	}
}

//==========================================================================
// �I������
//==========================================================================
void CPlayer::Kill(void)
{

	my_particle::Create(GetPosition(), my_particle::TYPE_OFFSETTING);

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
void CPlayer::Update(void)
{
	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();
	if (pInputKeyboard->GetTrigger(DIK_F5) == true)
	{// F5�Ń��Z�b�g
		SetPosition(D3DXVECTOR3(0.0f, 0.0f, -1000.0f));
		SetMove(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	}

	// �G�f�B�b�g���͔�����
	if (CGame::GetElevation()->IsEdit())
	{
		return;
	}

	// �G�f�B�b�g���͔�����
	if (CGame::GetEditType() != CGame::EDITTYPE_OFF)
	{
		return;
	}

	// �R���{���o���͔�����
	if (CGame::GetEnemyManager()->GetState() == CEnemyManager::STATE_COMBOANIM)
	{
		return;
	}

	// �ߋ��̈ʒu�ۑ�
	SetOldPosition(GetPosition());

	// �ߋ��̈ړ��ʕۑ�
	SetOldMapMoveValue(GetMapMoveValue());

	// ����
	Controll();
	CollisionChaseChanger();

	// ���[�V�����̐ݒ菈��
	MotionSet();

	// ���[�V�����̍X�V����
	if (m_pMotion != NULL)
	{
		m_pMotion->Update();

		if (m_pMotion->GetType() == MOTION_ATK || m_pMotion->GetType() == MOTION_ATK2)
		{
#if 0
			// ���[�V�����̏��擾
			CMotion::Info aInfo = m_pMotion->GetInfo(m_pMotion->GetType());

			CModel **ppModel = GetObjectChara()->GetModel();
			D3DXVECTOR3 bodyrot = mylib_const::DEFAULT_VECTOR3;
			D3DXVECTOR3 playerRot = mylib_const::DEFAULT_VECTOR3;

			// �Q�[���p�b�h���擾
			CInputGamepad *pInputGamepad = CManager::GetInstance()->GetInputGamepad();

			// �ړ������擾
			CObject::ANGLE MoveAngle = GetMoveAngle();

			if (pInputGamepad->GetStickPositionRatioR(0).y >= 0.5f || pInputGamepad->GetStickPositionRatioR(0).y <= -0.5f ||
				pInputGamepad->GetStickPositionRatioR(0).x >= 0.5f || pInputGamepad->GetStickPositionRatioR(0).x <= -0.5f)
			{
#if 0
				switch (MoveAngle)
				{
				case CPlayer::ANGLE_UP:
					bodyrot.x = pInputGamepad->GetStickRotR(0);

					if (bodyrot.x > D3DX_PI * 0.15f)
					{
						MoveAngle = ANGLE_RIGHT;
						m_nAngle = 1;	// ����
					}
					else if (bodyrot.x < D3DX_PI * -0.15f)
					{
						MoveAngle = ANGLE_LEFT;
						m_nAngle = -1;	// ����
					}
					if (bodyrot.x > D3DX_PI * 0.85f || bodyrot.x < D3DX_PI * -0.85f)
					{
						MoveAngle = ANGLE_DOWN;
						bodyrot.x = 0.0f;
					}
					bodyrot.x = 0.0f;
					break;

				case CPlayer::ANGLE_RIGHT:
					bodyrot.x = -pInputGamepad->GetStickRotR(0);

					if (bodyrot.x <= D3DX_PI * -0.85f)
					{
						MoveAngle = ANGLE_DOWN;
					}
					else if (bodyrot.x > D3DX_PI * 0.15f && bodyrot.x < D3DX_PI * 0.85f)
					{
						MoveAngle = ANGLE_LEFT;
						m_nAngle = -1;	// ����
					}
					else if (bodyrot.x <= D3DX_PI * -0.75f)
					{
						bodyrot.x = D3DX_PI * -0.75f;
					}
					else if (bodyrot.x >= D3DX_PI * -0.15f)
					{
						MoveAngle = ANGLE_UP;
						bodyrot.x = D3DX_PI * -0.15f;
					}

					bodyrot.x += D3DX_PI;
					bodyrot.x -= (D3DX_PI * 0.5f);
					break;

				case CPlayer::ANGLE_DOWN:

					bodyrot.x = pInputGamepad->GetStickRotR(0);

					if (bodyrot.x >= 0.0f && bodyrot.x <= D3DX_PI * 0.85f)
					{
						MoveAngle = ANGLE_RIGHT;
						m_nAngle = 1;	// ����
					}
					else if (bodyrot.x < D3DX_PI * -0.85f)
					{
						MoveAngle = ANGLE_LEFT;
						m_nAngle = -1;	// ����
					}
					if (bodyrot.x < D3DX_PI * 0.15f && bodyrot.x > D3DX_PI * -0.15f)
					{
						MoveAngle = ANGLE_UP;
						bodyrot.x = 0.0f;
					}
					bodyrot.x = 0.0f;

					break;

				case CPlayer::ANGLE_LEFT:
					bodyrot.x = pInputGamepad->GetStickRotR(0);

					if (bodyrot.x >= D3DX_PI * 0.85f)
					{
						MoveAngle = ANGLE_DOWN;
						bodyrot.x = D3DX_PI * -0.85f;
					}
					else if (bodyrot.x < D3DX_PI * 0.85f && bodyrot.x > 0.0f)
					{
						bodyrot.x = D3DX_PI * -0.75f;
						MoveAngle = ANGLE_RIGHT;
						m_nAngle = 1;	// ����
					}
					else if (bodyrot.x <= D3DX_PI * -0.75f)
					{
						bodyrot.x = D3DX_PI * -0.75f;
					}
					else if (bodyrot.x >= D3DX_PI * -0.15f)
					{
						MoveAngle = ANGLE_UP;
						bodyrot.x = D3DX_PI * -0.15f;
					}

					bodyrot.x += D3DX_PI;
					bodyrot.x -= (D3DX_PI * 0.5f);
					break;

				default:
					break;
		}
#else
				bodyrot.x = m_fBodyRot;
#endif

				RotNormalize(bodyrot.x);
				ppModel[1]->SetRotation(ppModel[1]->GetRotation() + bodyrot);
	}

			// �ړ������ݒ�
			SetMoveAngle(MoveAngle);
#endif
}
	}

	// ���_���ݒ�
	SetVtx();

	// �U������
	Atack();

	// ��ԍX�V
	UpdateState();

	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �ړ��ʎ擾
	D3DXVECTOR3 move = GetMove();

	// �����擾
	D3DXVECTOR3 rot = GetRotation();

	// �e�̈ʒu�X�V
	if (m_pShadow != NULL)
	{
		m_pShadow->SetPosition(D3DXVECTOR3(pos.x, m_pShadow->GetPosition().y, pos.z));
	}

	// HP�Q�[�W�̈ʒu�X�V
	if (m_pHPGauge != NULL)
	{
		m_pHPGauge->UpdatePosition(GetPosition(), GetLife());
	}

	// �f�o�b�O�\��
	CManager::GetInstance()->GetDebugProc()->Print(
		"------------------[�v���C���[�̑���]------------------\n"
		"�ʒu�F�yX�F%f, Y�F%f, Z�F%f�z �yW / A / S / D�z\n"
		"�����F�yX�F%f, Y�F%f, Z�F%f�z �yZ / C�z\n"
		"�ړ��ʁF�yX�F%f, Y�F%f, Z�F%f�z\n", pos.x, pos.y, pos.z, rot.x, rot.y, rot.y, move.x, move.y, move.z);
}

//==========================================================================
// ���쏈��
//==========================================================================
void CPlayer::Controll(void)
{
	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// �Q�[���p�b�h���擾
	CInputGamepad *pInputGamepad = CManager::GetInstance()->GetInputGamepad();

	// �J�����̏��擾
	CCamera *pCamera = CManager::GetInstance()->GetCamera();

	// �J�����̌����擾
	D3DXVECTOR3 Camerarot = pCamera->GetRotation();

	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �ړ��ʎ擾
	D3DXVECTOR3 move = GetMove();

	// �����擾
	D3DXVECTOR3 rot = GetRotation();

	// �ڕW�̌����擾
	float fRotDest = GetRotDest();

	// ���݂̎�ގ擾
	int nMotionType = m_pMotion->GetType();

	// �ړ��ʎ擾
	float fMove = GetVelocity();

	// �ړ������擾
	CObject::ANGLE MoveAngle = GetMoveAngle();
	SetOldMoveAngle(MoveAngle);

	if (m_pMotion->IsGetMove(nMotionType) == 1)
	{// �ړ��\���[�V�����̎�

		// �ړ����ɂ���
		m_sMotionFrag.bMove = true;

		// �X�e�B�b�N�̌���
		float fStickRot = pInputGamepad->GetStickRotL(0);

		if (pInputKeyboard->GetPress(DIK_A) == true || pInputGamepad->GetStickMoveL(0).x < 0)
		{//���L�[�������ꂽ,���ړ�

			// �ړ����ɂ���
			m_sMotionFrag.bMove = true;

			move.x -= fMove;

			// ������
			MoveAngle = ANGLE_LEFT;
			m_nAngle = -1;	// ����
		}
		else if (pInputKeyboard->GetPress(DIK_D) == true || pInputGamepad->GetStickMoveL(0).x > 0)
		{//D�L�[�������ꂽ,�E�ړ�

			// �ړ����ɂ���
			m_sMotionFrag.bMove = true;

			move.x += fMove;

			// �E����
			MoveAngle = ANGLE_RIGHT;
			m_nAngle = 1;	// ����
		}
		else if (pInputKeyboard->GetPress(DIK_W) == true || pInputGamepad->GetStickMoveL(0).y > 0)
		{//W�������ꂽ�A��ړ�

			// �ړ����ɂ���
			m_sMotionFrag.bMove = true;

			// �����
			MoveAngle = ANGLE_UP;
		}
		else if (pInputKeyboard->GetPress(DIK_S) == true || pInputGamepad->GetStickMoveL(0).y < 0)
		{//S�������ꂽ�A���ړ�

			// �ړ����ɂ���
			m_sMotionFrag.bMove = true;

			// �����
			MoveAngle = ANGLE_DOWN;
		}
		else
		{
			// �ړ������ǂ���
			m_sMotionFrag.bMove = false;
		}


		if (pInputGamepad->GetStickPositionRatioL(0).y >= 0.5f || pInputGamepad->GetStickPositionRatioL(0).y <= -0.5f ||
			pInputGamepad->GetStickPositionRatioL(0).x >= 0.5f || pInputGamepad->GetStickPositionRatioL(0).x <= -0.5f)
		{// �U��

			if ((fStickRot >= 0.0f && fStickRot < D3DX_PI * 0.25f) ||
				(fStickRot <= 0.0f && fStickRot > -D3DX_PI * 0.25f))
			{// �����
				MoveAngle = ANGLE_UP;
				m_nAngle = 1;	// ����
			}
			else if (fStickRot >= D3DX_PI * 0.25f && fStickRot < D3DX_PI * 0.75f)
			{// �E����
				MoveAngle = ANGLE_RIGHT;
				m_nAngle = 1;	// ����
			}
			else if (
				fStickRot >= -D3DX_PI * 0.75f &&
				fStickRot < -D3DX_PI * 0.25f)
			{// ������
				MoveAngle = ANGLE_LEFT;
				m_nAngle = -1;	// ����
			}
			else if (
				(fStickRot > D3DX_PI * 0.75f && fStickRot <= D3DX_PI) ||
				(fStickRot < -D3DX_PI * 0.75f && fStickRot >= -D3DX_PI))
			{// ������
				MoveAngle = ANGLE_DOWN;
			}
		}

		if (m_bJump == false &&
			(pInputKeyboard->GetTrigger(DIK_SPACE) == true || pInputGamepad->GetTrigger(CInputGamepad::BUTTON_LB, 0)))
		{//SPACE�������ꂽ,�W�����v

			m_bJump = true;
			m_sMotionFrag.bJump = true;
			move.y += 17.0f;
		}
	}
	else
	{// �ړ��\���[�V��������Ȃ��ꍇ

		if (pInputKeyboard->GetPress(DIK_A) == true || pInputGamepad->GetStickMoveL(0).x < 0)
		{//���L�[�������ꂽ,���ړ�

			// ������
			MoveAngle = ANGLE_LEFT;
			m_nAngle = -1;	// ����
		}
		else if (pInputKeyboard->GetPress(DIK_D) == true || pInputGamepad->GetStickMoveL(0).x > 0)
		{//D�L�[�������ꂽ,�E�ړ�

			// �E����
			MoveAngle = ANGLE_RIGHT;
			m_nAngle = 1;	// ����
		}
		else if (pInputKeyboard->GetPress(DIK_W) == true || pInputGamepad->GetStickMoveL(0).y > 0)
		{//W�������ꂽ�A��ړ�

			// �����
			MoveAngle = ANGLE_UP;
		}
		else if (pInputKeyboard->GetPress(DIK_S) == true || pInputGamepad->GetStickMoveL(0).y < 0)
		{//S�������ꂽ�A���ړ�

			// ������
			MoveAngle = ANGLE_DOWN;
		}
	}

	// �}�b�v�}�l�[�W���̎擾
	CMapManager *pMapManager = CGame::GetMapManager();
	if (pMapManager == NULL)
	{// NULL��������
		return;
	}

	float fTimeDest = 0.0f;
	D3DXVECTOR3 sakiPos;
	float fMoveValue = GetMapMoveValue();

	// �ړ������ʉ��Z
	fMoveValue += move.x;

	// �Ȑ����ׂ�4�_
	int nIdxMapPoint = GetMapIndex();

	// �ʒu�̊����擾
	float fPointRatio = GetMapPointRatio();

	// �ʒu�X�V
	D3DXVECTOR3 newPosition = pos;
	newPosition = pMapManager->UpdateNowPosition(nIdxMapPoint, fPointRatio, fMoveValue, pos.y);

	// �ڕW�n�_
	float fDest = (fMoveValue + (fMove + GetRadius() * 0.5f) * m_nAngle);
	int nDestIdx = nIdxMapPoint;
	float fDestPointRatio = fPointRatio;
	sakiPos = pMapManager->UpdateNowPosition(nDestIdx, fDestPointRatio, fDest, pos.y);


	// �ڕW�̊p�x�����߂�
	fRotDest = atan2f((newPosition.x - sakiPos.x), (newPosition.z - sakiPos.z));

	if (MoveAngle == ANGLE_DOWN)
	{
		fRotDest = atan2f((pos.x - pCamera->GetPositionV().x), (pos.z - pCamera->GetPositionV().z));
	}
	else if (MoveAngle == ANGLE_UP)
	{
		// �J�����̏��擾
		CCamera *pCamera = CManager::GetInstance()->GetCamera();

		// �J�����̒Ǐ]��ގ擾
		CCamera::CHASETYPE CameraChaseType = pCamera->GetChaseType();

		if (CameraChaseType == CCamera::CHASETYPE_NORMAL)
		{
			fRotDest = atan2f((pos.x - pCamera->GetTargetPosition().x), (pos.z - pCamera->GetTargetPosition().z));
		}
		else
		{
			fRotDest = fRotDest - D3DX_PI * 0.5f;
		}

	}

	// �p�x�̐��K��
	RotNormalize(fRotDest);

	// ���݂ƖڕW�̍��������߂�
	float fRotDiff = fRotDest - rot.y;

	// �p�x�̐��K��
	RotNormalize(fRotDiff);

	// �p�x�̕␳������
	rot.y += fRotDiff * 0.15f;

	// �p�x�̐��K��
	RotNormalize(rot.y);

	// �d�͏���
	if (m_state != STATE_KNOCKBACK && m_state != STATE_DMG)
	{
		move.y -= mylib_const::GRAVITY;

		// �ʒu�X�V
		newPosition.y += move.y;
		sakiPos.y = newPosition.y;
	}

	//**********************************
	// �����蔻��
	//**********************************
	bool bLandStage = Collision(sakiPos, move);

	m_bHitStage;

	bool bMove = false;
	if (m_bLandOld == false && bLandStage == true)
	{// �O��͒��n���Ă��Ȃ��āA����͒��n���Ă���ꍇ

		bMove = false;
	}

	if (m_bLandOld == true && bLandStage == true)
	{// �O�����������n���Ă���ꍇ
		bMove = true;
	}

	// �ߋ��̒��n����ۑ�
	//m_bLandOld = bLandStage;

	if (m_bHitWall == false && 
		(bLandStage == false || bMove == true || m_bLandField == true || m_bJump == true || m_sMotionFrag.bKnockBack == true))
	{
		pos.x = newPosition.x;
		pos.z = newPosition.z;
		pos.y = sakiPos.y;
		Collision(pos, move);

		//if (m_bHitStage == true)
		{// �X�e�[�W�ňړ�����ꍇ

			// �O��͏���Ă����Ƃɂ���
			m_bLandOld = true;
		}
	}
	else
	{
		D3DXVECTOR3 posOld = GetOldPosition();
		pos.x = posOld.x;
		pos.z = posOld.z;
		pos = posOld;
		move.x = 0.0f;
		fMoveValue = GetOldMapMoveValue();

		// �O��͏���ĂȂ��������Ă��Ƃɂ���
		//m_bLandOld = false;
	}

	if (CGame::GetGameManager()->GetType() == CGameManager::SCENE_RUSH)
	{// �G�̃��b�V����

		// ���擾
		D3DXVECTOR3 posMapIndex = pMapManager->GetTargetPosition(37, 0.0f);

		if (CircleRange(pos, posMapIndex, GetRadius(), 900.0f) == false)
		{// �~����O�ꂽ��

			D3DXVECTOR3 posOld = GetOldPosition();
			pos.x = posOld.x;
			pos.z = posOld.z;
			pos = posOld;
			move.x = 0.0f;
			fMoveValue = GetOldMapMoveValue();
		}
	}

	// �ʒu�X�V
	//pos.y += move.y;

	// �����␳
	if (m_state != STATE_KNOCKBACK && m_state != STATE_DMG)
	{
		move.x += (0.0f - move.x) * 0.25f;
		move.z += (0.0f - move.z) * 0.25f;
	}

	// �ʒu�ݒ�
	SetPosition(pos);

	// �ړ��ʐݒ�
	SetMove(move);

	// �����ݒ�
	SetRotation(rot);

	// �ڕW�̌����ݒ�
	SetRotDest(fRotDest);

	// �}�b�v�̃C���f�b�N�X�ԍ��ݒ�
	SetMapIndex(nIdxMapPoint);

	// �}�b�v�|�C���g�Ԃ̊����ݒ�
	SetMapPointRatio(fPointRatio);

	// �}�b�v�|�C���g�Ԃ̈ړ��ʐݒ�
	SetMapMoveValue(fMoveValue);

	// �ړ������ݒ�
	SetMoveAngle(MoveAngle);


	if (pInputGamepad->GetStickPositionRatioR(0).y <= 0.5f && pInputGamepad->GetStickPositionRatioR(0).y >= -0.5f &&
		pInputGamepad->GetStickPositionRatioR(0).x <= 0.5f && pInputGamepad->GetStickPositionRatioR(0).x >= -0.5f)
	{

		// �X�e�B�b�N�|��������
		m_bStick = false;
	}
	
	if (/*m_sMotionFrag.bATK == false &&*/
		(
			pInputGamepad->GetStickPositionRatioR(0).y >= 0.5f || pInputGamepad->GetStickPositionRatioR(0).y <= -0.5f ||
			pInputGamepad->GetStickPositionRatioR(0).x >= 0.5f || pInputGamepad->GetStickPositionRatioR(0).x <= -0.5f))
	{// �U��

		if (pInputGamepad->GetStickPositionRatioR(0).x >= 0.2f)
		{
			m_StickAngle = ANGLE_RIGHT;		// �X�e�B�b�N�̌���
		}
		else if (pInputGamepad->GetStickPositionRatioR(0).x <= -0.2f)
		{
			m_StickAngle = ANGLE_LEFT;		// �X�e�B�b�N�̌���
		}
		else if (pInputGamepad->GetStickPositionRatioR(0).y >= 0.2f)
		{
			m_StickAngle = ANGLE_UP;		// �X�e�B�b�N�̌���
		}
		else if (pInputGamepad->GetStickPositionRatioR(0).y <= -0.2f)
		{
			m_StickAngle = ANGLE_DOWN;		// �X�e�B�b�N�̌���
		}

		// �U������ON
		if (m_StickAngle != m_OldStickAngle)
		{
			// �X�e�B�b�N�̌���
			m_fAtkStickRot = pInputGamepad->GetStickRotR(0);

			switch (MoveAngle)
			{
			case CPlayer::ANGLE_UP:

				if (m_fAtkStickRot <= 0.0f)
				{// ������
					m_fAtkStickRot *= -1;
				}
				m_fAtkStickRot -= D3DX_PI * 0.5f;	// ������
				break;

			case CPlayer::ANGLE_RIGHT:
				m_fAtkStickRot -= D3DX_PI * 0.5f;	// ������
				break;

			case CPlayer::ANGLE_DOWN:

				if (m_fAtkStickRot >= 0.0f)
				{// �E����
					m_fAtkStickRot *= -1;
					m_fAtkStickRot += D3DX_PI * 0.5f;
				}
				else
				{
					m_fAtkStickRot -= D3DX_PI * 0.5f;	// ������
				}
				break;

			case CPlayer::ANGLE_LEFT:
				m_fAtkStickRot -= D3DX_PI * 0.5f;	// ������
				break;

			default:
				break;
			}
			RotNormalize(m_fAtkStickRot);

			int nType = m_pMotion->GetType();
			if (nType == MOTION_JUMP || nType == MOTION_FALL)
			{
				m_pMotion->ToggleFinish(true);
			}

			m_sMotionFrag.bJump = false;
			m_sMotionFrag.bATK = true;

		}

		// �X�e�B�b�N�|��������
		m_bStick = true;

		m_OldStickAngle = m_StickAngle;	// �O��̃X�e�B�b�N�̌���
	}
	else
	{
		//m_atkRush = ATKRUSH_LEFT;
		m_OldStickAngle = ANGLE_MAX;

		// �X�e�B�b�N�|��������
		m_bStick = false;
	}
}

//==========================================================================
// ���[�V�����̐ݒ�
//==========================================================================
void CPlayer::MotionSet(void)
{
	if (m_pMotion == NULL)
	{// ���[�V������NULL��������
		return;
	}

	if (m_pMotion->IsFinish() == true)
	{// �I�����Ă�����

		// ���݂̎�ގ擾
		int nType = m_pMotion->GetType();
		int nOldType = m_pMotion->GetOldType();

		if (m_sMotionFrag.bMove == true && m_sMotionFrag.bATK == false && m_sMotionFrag.bKnockBack == false && m_bJump == false)
		{// �ړ����Ă�����

			m_sMotionFrag.bMove = false;	// �ړ�����OFF

			// �ړ����[�V����
			m_pMotion->Set(MOTION_WALK);
		}
		else if (m_sMotionFrag.bJump == true && m_sMotionFrag.bATK == false && m_sMotionFrag.bKnockBack == false)
		{// �W�����v��

			// �W�����v�̃t���OOFF
			m_sMotionFrag.bJump = false;

			// �W�����v���[�V����
			m_pMotion->Set(MOTION_JUMP);
		}
		else if (m_bJump == true && m_sMotionFrag.bJump == false && m_sMotionFrag.bATK == false && m_sMotionFrag.bKnockBack == false)
		{// �W�����v��&&�W�����v���[�V�������I����Ă鎞

			// �������[�V����
			m_pMotion->Set(MOTION_FALL);
		}
		else if (m_sMotionFrag.bKnockBack == true)
		{// ���ꒆ��������

			// ���ꃂ�[�V����
			m_pMotion->Set(MOTION_KNOCKBACK);
		}
		else if (m_sMotionFrag.bATK == true)
		{// �U�����Ă�����

			m_sMotionFrag.bATK = false;		// �U������OFF

			// �A���A�^�b�N�̎��
			m_atkRush = (ATKRUSH)(((int)m_atkRush + 1) % (int)ATKRUSH_MAX);

			int nType = MOTION_ATK + m_atkRush;
			m_pMotion->Set(nType, false);
		}
		else
		{
			// �j���[�g�������[�V����
			m_pMotion->Set(MOTION_DEF);
		}
	}
}

//==========================================================================
// �U��
//==========================================================================
void CPlayer::Atack(void)
{
	if (m_pMotion == NULL)
	{// ���[�V������NULL��������
		return;
	}

	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �����擾
	D3DXVECTOR3 rot = GetRotation();

	// �J�����̏��擾
	CCamera *pCamera = CManager::GetInstance()->GetCamera();

	// �J�����̌����擾
	D3DXVECTOR3 Camerarot = pCamera->GetRotation();

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

		if (m_pMotion->GetAllCount() == aInfo.AttackInfo[nCntAttack]->nInpactCnt)
		{// �Ռ��̃J�E���g�Ɠ����Ƃ�

			D3DXMATRIX mtxTrans;	// �v�Z�p�}�g���b�N�X�錾

			if (GetObjectChara()->GetModel()[aInfo.AttackInfo[nCntAttack]->nCollisionNum] == NULL)
			{// NULL��������
				return;
			}

			// ���肷��p�[�c�̃}�g���b�N�X�擾
			D3DXMATRIX mtxWepon = GetObjectChara()->GetModel()[aInfo.AttackInfo[nCntAttack]->nCollisionNum]->GetWorldMtx();

			// �ʒu�𔽉f����
			D3DXMatrixTranslation(&mtxTrans, aInfo.AttackInfo[nCntAttack]->Offset.x, aInfo.AttackInfo[nCntAttack]->Offset.y, aInfo.AttackInfo[nCntAttack]->Offset.z);
			D3DXMatrixMultiply(&mtxWepon, &mtxTrans, &mtxWepon);

			// ����̈ʒu
			D3DXVECTOR3 weponpos = D3DXVECTOR3(mtxWepon._41, mtxWepon._42, mtxWepon._43);

			// ��ޕ�
			switch (m_pMotion->GetType())
			{
			case MOTION_ATK:
			case MOTION_ATK2:
				//// �p�[�e�B�N������
				//my_particle::Create(weponpos, my_particle::TYPE_SUPERATTACK);

				//// �`���[�W�J�E���g���Z�b�g
				////CGame::GetPowerGauge()->SetChargeCount(0);

				//// �Ռ��g����
				//CImpactWave::Create
				//(
				//	D3DXVECTOR3(pos.x, pos.y + 80.0f, pos.z),	// �ʒu
				//	D3DXVECTOR3(0.0f, 0.0f, 0.0f),				// ����
				//	D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f),			// �F
				//	100.0f,										// ��
				//	20.0f,										// ����
				//	20,											// ����
				//	28.0f,										// ���̈ړ���
				//	CImpactWave::TYPE_BLACK2,					// �e�N�X�`���^�C�v
				//	true										// ���Z�������邩
				//);

				//CImpactWave::Create
				//(
				//	D3DXVECTOR3(pos.x, pos.y + 150.0f, pos.z),	// �ʒu
				//	D3DXVECTOR3(0.0f, 0.0f, D3DX_PI),				// ����
				//	D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.4f),			// �F
				//	180.0f,										// ��
				//	150.0f,										// ����
				//	14,											// ����
				//	4.0f,										// ���̈ړ���
				//	CImpactWave::TYPE_GIZAWHITE,				// �e�N�X�`���^�C�v
				//	false										// ���Z�������邩
				//);

				// �U��
				//CManager::GetInstance()->GetCamera()->SetShake(20, 10.0f, 0.0f);

				float fRotY = rot.y;
				int nA = 1;
				if (GetMoveAngle() == ANGLE_LEFT)
				{
					nA = -1;
					fRotY += D3DX_PI;
				}

				// �a������
				//CSlash::Create
				//(
				//	D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z),	// �ʒu
				//	D3DXVECTOR3(0.0f, 0.0f, 0.0f),		// ����
				//	D3DXVECTOR3(m_fAtkStickRot, D3DX_PI + fRotY, 0.0f),		// ����
				//	D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),	// �F
				//	200.0f,								// ��
				//	50.0f,								// ���S����̊Ԋu
				//	10,									// ����
				//	40.0f,								// ���̈ړ���
				//	CImpactWave::TYPE_PURPLE4,			// �e�N�X�`���̎��
				//	true,								// ���Z�������邩�ǂ���
				//	GetMoveAngle()
				//);
				CSlash *pSlash = CSlash::Create
				(
					D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z),	// �ʒu
					D3DXVECTOR3(0.0f, 0.0f, 0.0f),		// ����
					D3DXVECTOR3(m_fAtkStickRot, D3DX_PI + fRotY, 0.0f),		// ����
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),	// �F
					200.0f,								// ��
					50.0f,								// ���S����̊Ԋu
					10,									// ����
					40.0f,								// ���̈ړ���
					CImpactWave::TYPE_PURPLE4,			// �e�N�X�`���̎��
					true,								// ���Z�������邩�ǂ���
					GetMoveAngle()
				);
				pSlash->SetMapIndex(GetMapIndex());
				pSlash->SetMapMoveValue(GetMapMoveValue());

				// ���s���Đ�
				CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_IMPACT01);
				break;
			}
		}

		// ���[�V�����J�E���^�[�擾
		if (m_pMotion->GetAllCount() > aInfo.AttackInfo[nCntAttack]->nMinCnt && m_pMotion->GetAllCount() < aInfo.AttackInfo[nCntAttack]->nMaxCnt)
		{// �U�����蒆

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

			CEffect3D::Create(weponpos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), aInfo.AttackInfo[nCntAttack]->fRangeSize * 0.5f, 10, CEffect3D::MOVEEFFECT_NONE, CEffect3D::TYPE_NORMAL);

#if TOPCUR

			// �擪��ۑ�
			CObject *pObj = CObject::GetTop(mylib_const::ENEMY_PRIORITY);

			while (pObj != NULL)
			{// NULL������܂Ŗ������[�v

				// ���̃I�u�W�F�N�g���ꎞ�ۑ�
				CObject *pObjNext = pObj->GetNext();

				// ���S�̔���
				if (pObj->IsDeath() == true)
				{// ���S�t���O�������Ă�����

					// ���̃I�u�W�F�N�g����
					pObj = pObjNext;
					continue;
				}

				// ��ނ̎擾
				if (pObj->GetType() != CObject::TYPE_ENEMY)
				{// �G����Ȃ�������

					// ���̃I�u�W�F�N�g����
					pObj = pObjNext;
					continue;
				}

				// �G�̈ʒu�擾
				D3DXMATRIX mtxOrigin = pObj->GetObjectChara()->GetModel()[0]->GetWorldMtx();
				D3DXVECTOR3 TargetPos = D3DXVECTOR3(mtxOrigin._41, mtxOrigin._42, mtxOrigin._43);

				// ����T�C�Y�擾
				float fRadius = pObj->GetObjectChara()->GetRadius();

				if (SphereRange(weponpos, TargetPos, aInfo.AttackInfo[nCntAttack]->fRangeSize, fRadius))
				{// ���̔���

					if (pObj->Hit(aInfo.AttackInfo[nCntAttack]->nDamage) == true)
					{// ����ł���

						my_particle::Create(TargetPos, my_particle::TYPE_OFFSETTING);
					}
				}

				// ���̃I�u�W�F�N�g����
				pObj = pObjNext;
			}
#endif
		}
	}

	CManager::GetInstance()->GetDebugProc()->Print(
		"���[�V�����J�E���^�[�F%d\n", m_pMotion->GetAllCount());
}

//==========================================================================
// �����蔻��
//==========================================================================
bool CPlayer::Collision(D3DXVECTOR3 &pos, D3DXVECTOR3 &move)
{

	// �����擾
	D3DXVECTOR3 rot = GetRotation();

	// ���n�������ǂ���
	bool bLand = false;
	float fHeight = 0.0f;
	m_bLandField = false;
	m_bHitWall = false;			// �ǂ̓����蔻��

	// �����擾
	if (m_state != STATE_KNOCKBACK && m_state != STATE_DMG)
	{
		fHeight = CManager::GetInstance()->GetScene()->GetElevation()->GetHeight(pos, bLand);
	}
	else
	{
		fHeight = pos.y;
	}

	if (fHeight > pos.y)
	{// �n�ʂ̕���������荂��������

		// �n�ʂ̍����ɕ␳
		pos.y = fHeight;
		m_bLandField = true;

		if (bLand == true)
		{// ���n���Ă���

			// �W�����v�g�p�\�ɂ���
			m_bJump = false;
			move.y = 0.0f;
			m_bLandOld = false;
		}
	}


	// X�t�@�C���Ƃ̔���
	CStage *pStage = CGame::GetStage();
	if (pStage == NULL)
	{// NULL��������
		return false;
	}

	bool bNowLand = false;

	// �X�e�[�W�ɓ�����������
	m_bHitStage = false;
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
		fHeight = pObjX->GetHeight(pos, bLand);

		if (bLand == true && fHeight > pos.y)
		{// �n�ʂ̕���������荂��������

			// �n�ʂ̍����ɕ␳
			if (pos.y + 50.0f <= fHeight)
			{// �������ǂ���������
				m_bHitWall = true;
			}
			else
			{
				pos.y = fHeight;
			}

			m_bHitStage = true;
			m_bLandField = false;

			if (bLand == true)
			{// ���n���Ă���

				if ((m_sMotionFrag.bKnockBack || m_bJump == true) && GetPosition().y  >= fHeight)
				{
					m_bLandOld = true;
				}

				if (m_bJump == true)
				{// �W�����v����������
					m_pMotion->ToggleFinish(true);
				}

				// �W�����v�g�p�\�ɂ���
				m_bJump = false;
				move.y = 0.0f;
				bNowLand = true;
				m_sMotionFrag.bJump = false;
			}
		}
	}

	// �ߋ��̒��n����ۑ�
	//m_bLandOld = bNowLand;



	// �ʒu�擾
	D3DXVECTOR3 posOld = GetPosition();

	// ��
	float fLen = CGame::GetElevation()->GetWidthLen();
	int nBlock = CGame::GetElevation()->GetWidthBlock();
	nBlock /= 2;
	if (pos.x + GetRadius() >= fLen * nBlock) { pos.x = fLen * nBlock - GetRadius(); }
	if (pos.x - GetRadius() <= -fLen * nBlock) { pos.x = -fLen * nBlock + GetRadius(); }
	if (pos.z + GetRadius() >= fLen * nBlock) { pos.z = fLen * nBlock - GetRadius(); }
	if (pos.z - GetRadius() <= -fLen * nBlock) { pos.z = -fLen * nBlock + GetRadius(); }

	// �����ݒ�
	SetRotation(rot);

	// �ʒu�ݒ�
	//SetPosition(pos);

	return bNowLand;
}

//==========================================================================
// �Ǐ]�̕ύX�҂Ƃ̔���
//==========================================================================
void CPlayer::CollisionChaseChanger(void)
{
	// �Ǐ]�̕ύX�Ҏ擾
	CCameraChaseChanger *pCameraChaseChanger = CGame::GetCameraChaseChanger();
	if (pCameraChaseChanger == NULL)
	{// NULL��������
		return;
	}

	// �}�b�v�}�l�[�W���̎擾
	CMapManager *pMapManager = CGame::GetMapManager();
	if (pMapManager == NULL)
	{// NULL��������
		return;
	}

	// �J�����̏��擾
	CCamera *pCamera = CManager::GetInstance()->GetCamera();

	if (CGame::GetGameManager()->GetType() == CGameManager::SCENE_BOSS)
	{// �{�X��
		pCamera->SetChaseType(CCamera::CHASETYPE_NORMAL);
		pCamera->SetTargetPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		return;
	}

	// ���擾
	CCameraChaseChanger::sChaseChangeInfo ChaseChangerInfo;
	CCamera::CHASETYPE CameraChaseType = CCamera::CHASETYPE_MAP;

	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();
	int nMapIdx = GetMapIndex();
	int nChangeNumAll = pCameraChaseChanger->GetNumAll();

	if (nMapIdx >= 32 && CGame::GetGameManager()->IsRushEnd() == false)
	{// �G���b�V���̎�

		// ���擾
		D3DXVECTOR3 posMapIndex = pMapManager->GetTargetPosition(37, 0.0f);

		// �~�̔���
		if (CircleRange(pos, posMapIndex, GetRadius(), 900.0f))
		{
			// �Ǐ]�̎�ސݒ�
			CGame::GetGameManager()->SetType(CGameManager::SCENE_RUSH);
			pCamera->SetChaseType(CCamera::CHASETYPE_NONE);
			pCamera->SetTargetPosition(D3DXVECTOR3(posMapIndex.x, posMapIndex.y + 150.0f, posMapIndex.z));
			pCamera->SetLenDest(pCamera->GetOriginDistance() + 500.0f);
			return;
		}
	}

	if (nMapIdx >= 39 && CGame::GetGameManager()->GetType() != CGameManager::SCENE_TRANSITION)
	{// �Ō�̓����̋߂�

		// ���擾
		D3DXVECTOR3 posMapIndex = pMapManager->GetTargetPosition(40, 0.0f);

		// �~�̔���
		if (CircleRange(pos, posMapIndex, GetRadius(), 400.0f))
		{
			// �Ǐ]�̎�ސݒ�
			CGame::GetGameManager()->SetType(CGameManager::SCENE_TRANSITIONWAIT);
			pCamera->SetChaseType(CCamera::CHASETYPE_NONE);
			pCamera->SetTargetPosition(D3DXVECTOR3(posMapIndex.x, posMapIndex.y + 150.0f, posMapIndex.z));
			pCamera->SetLenDest(pCamera->GetOriginDistance() - 200.0f);

			if (CircleRange(pos, posMapIndex, GetRadius(), 300.0f))
			{// �X�ɒ���

				// �Q�[���p�b�h���擾
				CInputGamepad *pInputGamepad = CManager::GetInstance()->GetInputGamepad();

				if (pInputGamepad->GetStickPositionRatioL(0).y >= 0.5f)
				{// ���ɐi��
					CGame::GetGameManager()->SetType(CGameManager::SCENE_TRANSITION);
					CManager::GetInstance()->GetInstantFade()->SetFade();
				}
			}
			return;
		}
	}

	for (int i = 0; i < nChangeNumAll + 1; i++)
	{
		// ���擾
		ChaseChangerInfo = pCameraChaseChanger->GetChaseChangeInfo(i);

		if (nMapIdx == ChaseChangerInfo.nMapIdx && i != nChangeNumAll)
		{// �ꏏ�ȏꍇ
			if (pMapManager->GetTargetAngle(nMapIdx, ChaseChangerInfo.nMapIdx, GetMapMoveValue(), ChaseChangerInfo.fMapMoveValue) == CObject::ANGLE_LEFT)
			{// ���ɂ�����
				continue;
			}
		}
		else if (nMapIdx > ChaseChangerInfo.nMapIdx && i != nChangeNumAll)
		{// �����̃}�b�v�C���f�b�N�X��菬�����ꍇ && �I�[����Ȃ�
			continue;
		}
		else if (i == nChangeNumAll)
		{// �Ō�
			ChaseChangerInfo.chaseType = CCamera::CHASETYPE_NORMAL;
		}

		// ��ޕʏ���
		switch (ChaseChangerInfo.chaseType)
		{
		case CCamera::CHASETYPE_NORMAL:
		
			CameraChaseType = CCamera::CHASETYPE_MAP;
			pCamera->SetChaseType(CCamera::CHASETYPE_MAP);
		
			break;

		case CCamera::CHASETYPE_MAP:
			pCamera->SetChaseType(CCamera::CHASETYPE_NORMAL);
			D3DXVECTOR3 AxisPos = CGame::GetCameraAxis()->GetAxis(ChaseChangerInfo.nByTypeIdx);
			pCamera->SetTargetPosition(AxisPos);
			CameraChaseType = CCamera::CHASETYPE_NORMAL;
			break;
		}

		break;
	}

	// �f�o�b�O�\��
	CManager::GetInstance()->GetDebugProc()->Print(
		"------------------[�}�b�v�̒Ǐ]]------------------\n"
		"��ށF�y%d�z\n", CameraChaseType);

}

//==========================================================================
// �q�b�g����
//==========================================================================
bool CPlayer::Hit(const int nValue)
{
	// �̗͎擾
	int nLife = GetLife();

	if (m_state != STATE_DEAD && m_state != STATE_DMG && m_state != STATE_KNOCKBACK && m_state != STATE_INVINCIBLE)
	{// �_���[�W��t��Ԃ̎�

		// �̗͌��炷
		nLife -= nValue;

		m_KnokBackMove.y += 18.0f;
		m_bHitStage = false;

		// �̗͐ݒ�
		SetLife(nLife);

		if (nLife <= 0)
		{// �̗͂��Ȃ��Ȃ�����

			// �����
			m_state = STATE_DEAD;

			// �̗͐ݒ�
			SetLife(0);

			// ���S����
			Kill();

			// �I������
			Uninit();

			// ����
			return true;
		}

		// �ߋ��̏�ԕۑ�
		m_Oldstate = m_state;

		// �_���[�W��Ԃɂ���
		m_state = STATE_DMG;

		// �J�ڃJ�E���^�[�ݒ�
		m_nCntState = 0;

		// �m�b�N�o�b�N�̈ʒu�X�V
		D3DXVECTOR3 pos = GetPosition();
		D3DXVECTOR3 rot = GetRotation();
		m_posKnokBack = pos;

		// �m�b�N�o�b�N����ɂ���
		m_sMotionFrag.bKnockBack = true;

		// ���ꃂ�[�V����
		m_pMotion->Set(MOTION_KNOCKBACK);

		// �Ռ��g����
		CImpactWave::Create
		(
			D3DXVECTOR3(pos.x, pos.y + 80.0f, pos.z),	// �ʒu
			D3DXVECTOR3(D3DX_PI * 0.5f, D3DX_PI + rot.y, D3DX_PI),				// ����
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.3f),			// �F
			80.0f,										// ��
			80.0f,										// ����
			20,											// ����
			10.0f,										// ���̈ړ���
			CImpactWave::TYPE_GIZAWHITE,				// �e�N�X�`���^�C�v
			false										// ���Z�������邩
		);

		CManager::GetInstance()->SetEnableHitStop(12);

		// �U��
		CManager::GetInstance()->GetCamera()->SetShake(12, 25.0f, 0.0f);

		// �T�E���h�Đ�
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_PLAYERDMG);
	}

	// ����łȂ�
	return false;
}

//==========================================================================
// ��ԍX�V����
//==========================================================================
void CPlayer::UpdateState(void)
{
	switch (m_state)
	{
	case STATE_NONE:
		// �F�ݒ�
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		break;

	case STATE_INVINCIBLE:
		Invincible();
		break;

	case STATE_DMG:
		Damage();
		break;

	case STATE_KNOCKBACK:
		KnockBack();
		break;
	}
}

//==========================================================================
// ���G
//==========================================================================
void CPlayer::Invincible(void)
{
	// ��ԑJ�ڃJ�E���^�[���Z
	m_nCntState--;

	if (m_nCntState % INVINCIBLE_INT == 0)
	{// �K��Ԋu
		
		// �F�ݒ�
		if (m_mMatcol.a == 1.0f)
		{
			m_mMatcol.a = 0.4f;
		}
		else
		{
			m_mMatcol.a = 1.0f;
		}
	}

	if (m_nCntState <= 0)
	{// �J�ڃJ�E���^�[��0�ɂȂ�����

		// �Ȃɂ��Ȃ���Ԃɂ���
		m_state = STATE_NONE;
		m_nCntState = 0;

		// �F�ݒ�
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	}

}

//==========================================================================
// �_���[�W
//==========================================================================
void CPlayer::Damage(void)
{
	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �ړ��ʎ擾
	D3DXVECTOR3 move = GetMove();

	// �����擾
	D3DXVECTOR3 rot = GetRotation();

	// ��ԑJ�ڃJ�E���^�[���Z
	m_nCntState++;

	// �F�ݒ�
	m_mMatcol = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);

	// �ʒu�X�V
	pos.y = (-0.4f * (float)(m_nCntState * m_nCntState) + m_KnokBackMove.y * (float)m_nCntState) + m_posKnokBack.y;
	/*pos.x += move.x;
	pos.z += move.z;*/

	// �N���Ƃ̔���
	if (m_bHitStage && m_nCntState >= 10)
	{// �n�ʂƓ������Ă�����
		m_state = STATE_INVINCIBLE;
		m_nCntState = INVINCIBLE_TIME;
		m_KnokBackMove.y = 0.0f;	// �ړ��ʃ[��
		m_bLandOld = true;

		// �F�ݒ�
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// �m�b�N�o�b�N�������
		m_sMotionFrag.bKnockBack = false;
		m_pMotion->ToggleFinish(true);


		// X�t�@�C���Ƃ̔���
		CStage *pStage = CGame::GetStage();
		if (pStage == NULL)
		{// NULL��������
			return;
		}

		// �X�e�[�W�ɓ�����������
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
			pos.y = pObjX->GetHeight(pos, bLand);
		}


	}
	else if (m_nCntState >= 30)
	{// �J�ڃJ�E���^�[��30�ɂȂ�����

		// �m�b�N�o�b�N��Ԃɂ���
		m_state = STATE_KNOCKBACK;
	}


	// �ʒu�ݒ�
	SetPosition(pos);

	// �ړ��ʐݒ�
	SetMove(move);

	// �����ݒ�
	SetRotation(rot);
}

//==========================================================================
// �m�b�N�o�b�N
//==========================================================================
void CPlayer::KnockBack(void)
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


	// ��ԑJ�ڃJ�E���^�[���Z
	m_nCntState++;

	// �ʒu�X�V
	pos.y = (-0.4f * (float)(m_nCntState * m_nCntState) + m_KnokBackMove.y * (float)m_nCntState) + m_posKnokBack.y;
	/*pos.x += move.x;
	pos.z += move.z;*/

	// �N���Ƃ̔���
	if (m_bHitStage)
	{// �n�ʂƓ������Ă�����
		m_state = STATE_INVINCIBLE;
		m_nCntState = INVINCIBLE_TIME;
		m_KnokBackMove.y = 0.0f;	// �ړ��ʃ[��
		m_bLandOld = true;

		// �F�ݒ�
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		
		// �m�b�N�o�b�N�������
		m_sMotionFrag.bKnockBack = false;
		m_pMotion->ToggleFinish(true);

		// X�t�@�C���Ƃ̔���
		CStage *pStage = CGame::GetStage();
		if (pStage == NULL)
		{// NULL��������
			return;
		}

		// �X�e�[�W�ɓ�����������
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
			pos.y = pObjX->GetHeight(pos, bLand);
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
}

//==========================================================================
// �`�揈��
//==========================================================================
void CPlayer::Draw(void)
{
#if _DEBUG
	CObjectChara::Draw();
#else
	CObjectChara::Draw();
#endif
}

//==========================================================================
// ��Ԏ擾
//==========================================================================
int CPlayer::GetState(void)
{
	return m_state;
}