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
#include "bulletmanager.h"
#include "stage.h"

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
CStage *CGame::m_pStage = NULL;	// �X�e�[�W�̃I�u�W�F�N�g

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

	// BGM�Đ�
	CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_BGM_GAME);

	//**********************************
	// �v���C���[
	//**********************************
	if (CManager::GetInstance()->GetScene()->GetPlayer() != NULL)
	{
		CManager::GetInstance()->GetScene()->GetPlayer()->SetPosition(D3DXVECTOR3(0.0f, 0.0f, -1000.0f));
		CManager::GetInstance()->GetScene()->GetPlayer()->SetRotation(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	}

	//**********************************
	// �e�}�l�[�W��
	//**********************************
	m_pBulletManager = CBulletManager::Create();

	// �X�e�[�W
	m_pStage = CStage::Create();


	// �X�R�A�̐�������
	m_pScore = CScore::Create(D3DXVECTOR3(1000.0f, 50.0f, 0.0f));

	// �^�C�}�[�̐�������
	m_pTimer = CTimer::Create(D3DXVECTOR3(200.0f, 50.0f, 0.0f));

	CManager::GetInstance()->GetCamera()->Reset(CScene::MODE_GAME);

	// ����
	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CGame::Uninit(void)
{
	
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

	if (m_pBulletManager != NULL)
	{
		// �I��������
		m_pBulletManager->Uninit();
		delete m_pBulletManager;
		m_pBulletManager = NULL;
	}

	// �I������
	CScene::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CGame::Update(void)
{
	CManager::GetInstance()->GetDebugProc()->Print(
		"���݂̃��[�h�F�y�Q�[���z\n"
		"�؂�ւ��F�y F �z\n\n");

	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// �Q�[���p�b�h���擾
	CInputGamepad *pInputGamepad = CManager::GetInstance()->GetInputGamepad();

#if 1
	// �X�R�A�̍X�V����
	m_pScore->Update();

	if (CManager::GetInstance()->GetEdit() == NULL &&
		m_pEditControlPoint == NULL &&
		m_pEditCameraAxis == NULL &&
		GetEnemyManager()->GetState() != CEnemyManager::STATE_COMBOANIM)
	{
		// �^�C�}�[�̍X�V����
		m_pTimer->Update();
	}
#endif

	if (GetEnemyManager() != NULL)
	{// �G�}�l�[�W���̍X�V����
		GetEnemyManager()->Update();
	}

	if (pInputKeyboard->GetTrigger(DIK_F4))
	{// F4�Ń}�b�v�G�f�B�b�g

		if (m_pEditControlPoint == NULL)
		{// NULL��������

			// �G�f�B�b�g�̐�������
			m_pEditControlPoint = CEditControlPoint::Create();
		}
		else
		{
			// �I��������
			m_pEditControlPoint->Uninit();
			delete m_pEditControlPoint;
			m_pEditControlPoint = NULL;
		}
	}

	if (m_pEditControlPoint != NULL)
	{// ����_�}�l�[�W���̍X�V����
		m_pEditControlPoint->Update();
	}

	if (pInputKeyboard->GetTrigger(DIK_F5) == true)
	{// F5�������ꂽ,�J�����Ǐ]�ύX��
		if (m_pEditCmaeraChaseChanger == NULL)
		{// NULL��������

			// �G�f�B�b�g�̐�������
			m_pEditCmaeraChaseChanger = CEditCameraChaseChanger::Create();
		}
		else
		{
			// �I��������
			m_pEditCmaeraChaseChanger->Uninit();
			delete m_pEditCmaeraChaseChanger;
			m_pEditCmaeraChaseChanger = NULL;
		}
	}

	if (m_pEditCmaeraChaseChanger != NULL)
	{// �J�����Ǐ]�ύX�҃}�l�[�W���̍X�V����
		m_pEditCmaeraChaseChanger->Update();
	}


	if (pInputKeyboard->GetTrigger(DIK_F6))
	{// F6�ŃJ�������G�f�B�b�g

		if (m_pEditCameraAxis == NULL)
		{// NULL��������

			// �G�f�B�b�g�̐�������
			m_pEditCameraAxis = CEditCameraAxis::Create();
		}
		else
		{
			// �I��������
			m_pEditCameraAxis->Uninit();
			delete m_pEditCameraAxis;
			m_pEditCameraAxis = NULL;
		}
	}

	if (m_pEditCameraAxis != NULL)
	{// �J�������G�f�B�^�[�̍X�V����
		m_pEditCameraAxis->Update();
	}

#if _DEBUG

	if (pInputKeyboard->GetTrigger(DIK_F))
	{
		// ���[�h�ݒ�
		CManager::GetInstance()->GetFade()->SetFade(CScene::MODE_RESULT);
	}
#endif

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
// ���Z�b�g����
//==========================================================================
void CGame::Reset(void)
{

}