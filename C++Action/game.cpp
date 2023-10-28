//=============================================================================
// 
//  �Q�[������ [game.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "manager.h"
#include "game.h"
#include "renderer.h"
#include "calculation.h"
#include "debugproc.h"
#include "fade.h"
#include "camera.h"

#include "input.h"
#include "player.h"
#include "enemy.h"
#include "score.h"
#include "timer.h"
#include "map.h"
#include "elevation.h"
#include "sound.h"
#include "edit_controlpoint.h"
#include "edit_cameraaxis.h"
#include "edit_camerachasechanger.h"
#include "edit_enemybase.h"
#include "bulletmanager.h"
#include "stage.h"
#include "gamemanager.h"
#include "hitscore.h"

#include "enemymanager.h"
#include "mapmanager.h"
#include "player.h"
#include "cameraaxis.h"
#include "camerachasechanger.h"
#include "enemybase.h"

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
CScore *CGame::m_pScore = NULL;					// �X�R�A�̃I�u�W�F�N�g
CTimer *CGame::m_pTimer = NULL;					// �^�C�}�[�̃I�u�W�F�N�g
CPowerGauge *CGame::m_pPowerGauge = NULL;		// �p���[�Q�[�W�̃I�u�W�F�N�g
CEditControlPoint *CGame::m_pEditControlPoint = NULL;	// ����_�G�f�B�^�[�̃I�u�W�F�N�g
CBulletManager *CGame::m_pBulletManager = NULL;		// �e�}�l�[�W���̃I�u�W�F�N�g
CEditCameraAxis *CGame::m_pEditCameraAxis = NULL;		// �J�������G�f�B�^�[�̃I�u�W�F�N�g
CEditCameraChaseChanger *CGame::m_pEditCmaeraChaseChanger = NULL;	// �J�����Ǐ]�ύX�҃G�f�B�^�[�̃I�u�W�F�N�g
CEditEnemyBase *CGame::m_pEditEnemyBase = NULL;		// �G�̋��_�G�f�B�^�[
CStage *CGame::m_pStage = NULL;						// �X�e�[�W�̃I�u�W�F�N�g
CGameManager *CGame::m_pGameManager = NULL;			// �Q�[���}�l�[�W���̃I�u�W�F�N�g
CGame::EEditType CGame::m_EditType = EDITTYPE_OFF;		// �G�f�B�b�g�̎��

CEnemyManager *CGame::m_pEnemyManager = NULL;	// �G�}�l�[�W���̃I�u�W�F�N�g
CMapManager *CGame::m_pMapManager = NULL;		// �}�b�v�}�l�[�W���̃I�u�W�F�N�g
CCameraAxis *CGame::m_pCameraAxis = NULL;		// �J�����̎��̃I�u�W�F�N�g
CCameraChaseChanger *CGame::m_pCameraChaseChanger = NULL;	// �J�����Ǐ]�ύX�҂̃I�u�W�F�N�g
CEnemyBase *CGame::m_pEnemyBase = NULL;			// �G�̋��_
CHitScore *CGame::m_pHitScore = NULL;			// �q�b�g�X�R�A

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CGame::CGame()
{
	// �l�̃N���A
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CGame::~CGame()
{

}

//==========================================================================
// ����������
//==========================================================================
HRESULT CGame::Init(void)
{

	// ����������
	if (FAILED(CScene::Init()))
	{// ���s�����ꍇ
		return E_FAIL;
	}

	//**********************************
	// �Q�[���}�l�[�W��
	//**********************************
	m_pGameManager = CGameManager::Create();

	//**********************************
	// �}�b�v�}�l�[�W��
	//**********************************
	m_pMapManager = CMapManager::Create("data\\BIN\\maptarget.bin");

	if (m_pMapManager == NULL)
	{// NULL��������
		return E_FAIL;
	}

	//**********************************
	// �G�}�l�[�W��
	//**********************************
	m_pEnemyManager = CEnemyManager::Create("data\\TEXT\\enemy_manager.txt");

	if (m_pEnemyManager == NULL)
	{// NULL��������
		return E_FAIL;
	}

	//**********************************
	// �J�����̎�
	//**********************************
	m_pCameraAxis = CCameraAxis::Create("data\\BIN\\cameraaxis.bin");
	if (m_pCameraAxis == NULL)
	{// NULL��������
		return E_FAIL;
	}

	//**********************************
	// �J�����Ǐ]�ύX��
	//**********************************
	m_pCameraChaseChanger = CCameraChaseChanger::Create("data\\BIN\\camerachanger.bin");
	if (m_pCameraChaseChanger == NULL)
	{// NULL��������
		return E_FAIL;
	}

	//**********************************
	// �G�̋��_
	//**********************************
	m_pEnemyBase = CEnemyBase::Create();
	if (m_pEnemyBase == NULL)
	{// NULL��������
		return E_FAIL;
	}

	// BGM�Đ�
	CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_BGM_GAME);

	//**********************************
	// �v���C���[
	//**********************************
	if (CManager::GetInstance()->GetScene()->GetPlayer() != NULL)
	{
		CManager::GetInstance()->GetScene()->GetPlayer()->SetPosition(D3DXVECTOR3(0.0f, 1000.0f, -1000.0f));
		CManager::GetInstance()->GetScene()->GetPlayer()->SetRotation(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	}

	//**********************************
	// �e�}�l�[�W��
	//**********************************
	m_pBulletManager = CBulletManager::Create();

	// �X�e�[�W
	m_pStage = CStage::Create("data\\TEXT\\stage_info.txt");

	// �X�R�A�̐�������
	m_pScore = CScore::Create(D3DXVECTOR3(1000.0f, 50.0f, 0.0f));

	// �^�C�}�[�̐�������
	m_pTimer = CTimer::Create(D3DXVECTOR3(200.0f, 50.0f, 0.0f));

	// �q�b�g�X�R�A
	m_pHitScore = CHitScore::Create(D3DXVECTOR3(1300.0f, 600.0f, 0.0f));

	CManager::GetInstance()->GetCamera()->Reset(CScene::MODE_GAME);

	// ����
	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CGame::Uninit(void)
{
	if (m_pGameManager != NULL)
	{
		// �I������
		m_pGameManager->Uninit();
		delete m_pGameManager;
		m_pGameManager = NULL;
	}

	// �X�R�A�̔j��
	if (m_pScore != NULL)
	{// �������̊m�ۂ��o���Ă�����

		// �I������
		m_pScore->Uninit();

		// �������̊J��
		delete m_pScore;
		m_pScore = NULL;
	}

	// �^�C�}�[�̔j��
	if (m_pTimer != NULL)
	{// �������̊m�ۂ��o���Ă�����

		// �I������
		m_pTimer->Uninit();

		// �������̊J��
		delete m_pTimer;
		m_pTimer = NULL;
	}

	// �q�b�g�X�R�A�̔j��
	if (m_pHitScore != NULL)
	{
		// �I������
		m_pHitScore->Uninit();
		delete m_pHitScore;
		m_pHitScore = NULL;
	}

	// �X�e�[�W�̔j��
	if (m_pStage != NULL)
	{// �������̊m�ۂ��o���Ă�����

		// �I������
		m_pStage->Uninit();
		delete m_pStage;
		m_pStage = NULL;
	}

	if (m_pEditControlPoint != NULL)
	{
		// �I��������
		m_pEditControlPoint->Uninit();
		delete m_pEditControlPoint;
		m_pEditControlPoint = NULL;
	}
	
	if (m_pEditCameraAxis != NULL)
	{
		// �I��������
		m_pEditCameraAxis->Uninit();
		delete m_pEditCameraAxis;
		m_pEditCameraAxis = NULL;
	}

	if (m_pEditCmaeraChaseChanger != NULL)
	{
		// �I��������
		m_pEditCmaeraChaseChanger->Uninit();
		delete m_pEditCmaeraChaseChanger;
		m_pEditCmaeraChaseChanger = NULL;
	}

	if (m_pEditEnemyBase != NULL)
	{
		// �I��������
		m_pEditEnemyBase->Uninit();
		delete m_pEditEnemyBase;
		m_pEditEnemyBase = NULL;
	}

	if (m_pBulletManager != NULL)
	{
		// �I��������
		m_pBulletManager->Uninit();
		delete m_pBulletManager;
		m_pBulletManager = NULL;
	}

	// �}�b�v�}�l�[�W��
	if (m_pMapManager != NULL)
	{
		m_pMapManager->Uninit();
		delete m_pMapManager;
		m_pMapManager = NULL;
	}

	// �J�����̎�
	if (m_pCameraAxis != NULL)
	{
		m_pCameraAxis->Uninit();
		delete m_pCameraAxis;
		m_pCameraAxis = NULL;
	}

	// �J�����Ǐ]�ύX��
	if (m_pCameraChaseChanger != NULL)
	{
		m_pCameraChaseChanger->Uninit();
		delete m_pCameraChaseChanger;
		m_pCameraChaseChanger = NULL;
	}

	// �G�̋��_
	if (m_pEnemyBase != NULL)
	{
		m_pEnemyBase->Uninit();
		delete m_pEnemyBase;
		m_pEnemyBase = NULL;
	}

	// �G�}�l�[�W��
	if (m_pEnemyManager != NULL)
	{
		m_pEnemyManager->Uninit();
		delete m_pEnemyManager;
		m_pEnemyManager = NULL;
	}

	// �I������
	CScene::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CGame::Update(void)
{
	// �Q�[���}�l�[�W��
	if (m_pGameManager != NULL)
	{
		// �X�V����
		m_pGameManager->Update();
	}

	CManager::GetInstance()->GetDebugProc()->Print(
		"���݂̃��[�h�F�y�Q�[���z\n"
		"�؂�ւ��F�y F �z\n\n");

	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// �Q�[���p�b�h���擾
	CInputGamepad *pInputGamepad = CManager::GetInstance()->GetInputGamepad();

#if 1
	if (CManager::GetInstance()->GetEdit() == NULL &&
		m_pEditControlPoint == NULL &&
		m_pEditCameraAxis == NULL &&
		GetEnemyManager()->GetState() != CEnemyManager::STATE_COMBOANIM)
	{
		// �^�C�}�[�̍X�V����
		m_pTimer->Update();

		// �X�R�A�̍X�V����
		m_pScore->Update();

		// �q�b�g�X�R�A�̍X�V����
		if (m_pHitScore != NULL)
		{
			// �X�V����
			m_pHitScore->Update();
		}
	}
#endif

	if (pInputKeyboard->GetTrigger(DIK_F4))
	{// F4�ŃG�f�B�b�g�؂�ւ�

		// �؂�ւ�
		m_EditType = (EEditType)(((int)m_EditType + 1) % (int)EDITTYPE_MAX);	// �Ǐ]�̎��

		EditReset();

		switch (m_EditType)
		{
		case CGame::EDITTYPE_OFF:	// �S���I�t

			break;

		case CGame::EDITTYPE_MAPCONTROL:

			if (m_pEditControlPoint == NULL)
			{// NULL��������

				// �G�f�B�b�g�̐�������
				m_pEditControlPoint = CEditControlPoint::Create();
			}
			break;

		case CGame::EDITTYPE_CAMERACHASE:

			if (m_pEditCmaeraChaseChanger == NULL)
			{// NULL��������

				// �G�f�B�b�g�̐�������
				m_pEditCmaeraChaseChanger = CEditCameraChaseChanger::Create();
			}
			break;

		case CGame::EDITTYPE_CAMERAAXIS:

			if (m_pEditCameraAxis == NULL)
			{// NULL��������

				// �G�f�B�b�g�̐�������
				m_pEditCameraAxis = CEditCameraAxis::Create();
			}
			break;

		case EDITTYPE_ENEMYBASE:
			if (m_pEditEnemyBase == NULL)
			{// NULL��������

				// �G�f�B�b�g�̐�������
				m_pEditEnemyBase = CEditEnemyBase::Create();
			}
			break;

		}
	}

	if (GetEnemyManager() != NULL)
	{// �G�}�l�[�W���̍X�V����
		GetEnemyManager()->Update();
	}

	if (m_pEditControlPoint != NULL)
	{// ����_�}�l�[�W���̍X�V����
		m_pEditControlPoint->Update();
	}

	if (m_pEditCmaeraChaseChanger != NULL)
	{// �J�����Ǐ]�ύX�҃}�l�[�W���̍X�V����
		m_pEditCmaeraChaseChanger->Update();
	}

	if (m_pEditCameraAxis != NULL)
	{// �J�������G�f�B�^�[�̍X�V����
		m_pEditCameraAxis->Update();
	}

	if (m_pEditEnemyBase != NULL)
	{// �G�̋��_�G�f�B�^�[�̍X�V����
		m_pEditEnemyBase->Update();
	}

	// �}�b�v�}�l�[�W���̍X�V����
	m_pMapManager->Update();

	// �J�����̎��̍X�V����
	m_pCameraAxis->Update();

	// �J�����Ǐ]�ύX��
	m_pCameraChaseChanger->Update();

	// �G�̋��_
	m_pEnemyBase->Update();

#if _DEBUG

	if (pInputKeyboard->GetTrigger(DIK_F))
	{
		// ���[�h�ݒ�
		CManager::GetInstance()->GetFade()->SetFade(CScene::MODE_RESULT);
	}
#endif

	// �V�[���̍X�V
	CScene::Update();

}

//==========================================================================
// �`�揈��
//==========================================================================
void CGame::Draw(void)
{

}

//==========================================================================
// �X�R�A�̎擾
//==========================================================================
CScore *CGame::GetScore(void)
{
	return m_pScore;
}

//==========================================================================
// �^�C�}�[�̎擾
//==========================================================================
CTimer *CGame::GetTimer(void)
{
	return m_pTimer;
}

//==========================================================================
// �q�b�g�X�R�A�̎擾
//==========================================================================
CHitScore *CGame::GetHitScore(void)
{
	return m_pHitScore;
}

//==========================================================================
// �p���[�Q�[�W�̎擾
//==========================================================================
CPowerGauge *CGame::GetPowerGauge(void)
{
	return m_pPowerGauge;
}

//==========================================================================
// ����_�G�f�B�^�[�̎擾
//==========================================================================
CEditControlPoint *CGame::GetEditControlPoint(void)
{
	return m_pEditControlPoint;
}

//==========================================================================
// �e�}�l�[�W���̎擾
//==========================================================================
CBulletManager *CGame::GetBulletManager(void)
{
	return m_pBulletManager;
}

//==========================================================================
// �X�e�[�W�̎擾
//==========================================================================
CStage *CGame::GetStage(void)
{
	return m_pStage;
}

//==========================================================================
// �Q�[���}�l�[�W���̎擾
//==========================================================================
CGameManager *CGame::GetGameManager(void)
{
	return m_pGameManager;
}


//==========================================================================
// �G�}�l�[�W���̎擾
//==========================================================================
CEnemyManager *CGame::GetEnemyManager(void)
{
	return m_pEnemyManager;
}

//==========================================================================
// �}�b�v�}�l�[�W���̎擾
//==========================================================================
CMapManager *CGame::GetMapManager(void)
{
	return m_pMapManager;
}

//==========================================================================
// �J�����̎��擾
//==========================================================================
CCameraAxis *CGame::GetCameraAxis(void)
{
	return m_pCameraAxis;
}

//==========================================================================
// �J�����Ǐ]�ύX�҂̎擾
//==========================================================================
CCameraChaseChanger *CGame::GetCameraChaseChanger(void)
{
	return m_pCameraChaseChanger;
}

//==========================================================================
// �G�̋��_
//==========================================================================
CEnemyBase *CGame::GetEnemyBase(void)
{
	return m_pEnemyBase;
}

//==========================================================================
// ���Z�b�g����
//==========================================================================
void CGame::Reset(void)
{
	// �X�e�[�W�̔j��
	if (m_pStage != NULL)
	{// �������̊m�ۂ��o���Ă�����

		// �I������
		m_pStage->Release();
		delete m_pStage;
		m_pStage = NULL;
	}

	// �}�b�v�}�l�[�W��
	if (m_pMapManager != NULL)
	{
		m_pMapManager->Release();
		delete m_pMapManager;
		m_pMapManager = NULL;
	}

	// �J�����̎�
	if (m_pCameraAxis != NULL)
	{
		m_pCameraAxis->Uninit();
		delete m_pCameraAxis;
		m_pCameraAxis = NULL;
	}

	// �J�����Ǐ]�ύX��
	if (m_pCameraChaseChanger != NULL)
	{
		m_pCameraChaseChanger->Uninit();
		delete m_pCameraChaseChanger;
		m_pCameraChaseChanger = NULL;
	}

	// �G�̋��_
	if (m_pEnemyBase != NULL)
	{
		m_pEnemyBase->Uninit();
		delete m_pEnemyBase;
		m_pEnemyBase = NULL;
	}

	// �G�}�l�[�W��
	if (m_pEnemyManager != NULL)
	{
		m_pEnemyManager->Kill();
	}

	// �X�e�[�W
	m_pStage = CStage::Create("data\\TEXT\\bossstage_info.txt");

	//**********************************
	// �}�b�v�}�l�[�W��
	//**********************************
	m_pMapManager = CMapManager::Create("data\\BIN\\maptarget_boss.bin");

	if (m_pMapManager == NULL)
	{// NULL��������
		return;
	}
	m_pMapManager->SetEnableCrawl();

	//**********************************
	// �J�����̎�
	//**********************************
	m_pCameraAxis = CCameraAxis::Create("data\\BIN\\cameraaxis_boss.bin");
	if (m_pCameraAxis == NULL)
	{// NULL��������
		return;
	}

	//**********************************
	// �J�����Ǐ]�ύX��
	//**********************************
	m_pCameraChaseChanger = CCameraChaseChanger::Create("data\\BIN\\camerachanger_boss.bin");
	if (m_pCameraChaseChanger == NULL)
	{// NULL��������
		return;
	}

	//**********************************
	// �G�̋��_
	//**********************************
	m_pEnemyBase = CEnemyBase::Create();
	if (m_pEnemyBase == NULL)
	{// NULL��������
		return;
	}
}

//==========================================================================
// �G�f�B�^�[���Z�b�g����
//==========================================================================
void CGame::EditReset(void)
{
	if (m_pEditControlPoint != NULL)
	{
		// �I��������
		m_pEditControlPoint->Uninit();
		delete m_pEditControlPoint;
		m_pEditControlPoint = NULL;
	}

	if (m_pEditCameraAxis != NULL)
	{
		// �I��������
		m_pEditCameraAxis->Uninit();
		delete m_pEditCameraAxis;
		m_pEditCameraAxis = NULL;
	}

	if (m_pEditCmaeraChaseChanger != NULL)
	{
		// �I��������
		m_pEditCmaeraChaseChanger->Uninit();
		delete m_pEditCmaeraChaseChanger;
		m_pEditCmaeraChaseChanger = NULL;
	}

	if (m_pEditEnemyBase != NULL)
	{
		// �I��������
		m_pEditEnemyBase->Release();
		m_pEditEnemyBase->Uninit();
		delete m_pEditEnemyBase;
		m_pEditEnemyBase = NULL;
	}

}
