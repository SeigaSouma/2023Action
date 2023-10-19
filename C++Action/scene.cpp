//=============================================================================
// 
//  �V�[������ [scene.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "scene.h"
#include "renderer.h"
#include "calculation.h"
#include "Xload.h"
#include "map.h"
#include "fade.h"
#include "elevation.h"
#include "enemymanager.h"
#include "mapmanager.h"
#include "player.h"
#include "cameraaxis.h"
#include "camerachasechanger.h"

// �J�ڐ�
#include "game.h"
#include "title.h"
#include "tutorial.h"
#include "result.h"
#include "ranking.h"

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
CXLoad *CScene::m_pXLoad = NULL;				// X�t�@�C���̃I�u�W�F�N�g
CElevation *CScene::m_pObject3DMesh = NULL;		// �I�u�W�F�N�g3D���b�V���̃I�u�W�F�N�g
CEnemyManager *CScene::m_pEnemyManager = NULL;	// �G�}�l�[�W���̃I�u�W�F�N�g
CMapManager *CScene::m_pMapManager = NULL;		// �}�b�v�}�l�[�W���̃I�u�W�F�N�g
CCameraAxis *CScene::m_pCameraAxis = NULL;		// �J�����̎��̃I�u�W�F�N�g
CCameraChaseChanger *CScene::m_pCameraChaseChanger = NULL;	// �J�����Ǐ]�ύX�҂̃I�u�W�F�N�g

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CScene::CScene()
{
	// �ϐ��̃N���A
	m_mode = MODE_TITLE;
	m_pPlayer = NULL;
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CScene::~CScene()
{

}

//==========================================================================
// ��������
//==========================================================================
CScene *CScene::Create(CScene::MODE mode)
{
	// �����p�̃I�u�W�F�N�g
	CScene *pScene = NULL;

	if (pScene == NULL)
	{// NULL��������

		// ��������
		switch (mode)
		{
		case CScene::MODE_TITLE:
			pScene = DEBUG_NEW CTitle;
			break;

		case CScene::MODE_TUTORIAL:
			pScene = DEBUG_NEW CTutorial;
			break;

		case CScene::MODE_GAME:
			pScene = DEBUG_NEW CGame;
			break;

		case CScene::MODE_RESULT:
			pScene = DEBUG_NEW CResult;
			break;

		case CScene::MODE_RANKING:
			pScene = DEBUG_NEW CRanking;
			break;
		}

		if (pScene != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ���[�h�̐ݒ�
			pScene->m_mode = mode;
		}

		return pScene;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CScene::Init(void)
{
	HRESULT hr;

	//**********************************
	// X�t�@�C��
	//**********************************
	if (m_pXLoad != NULL)
	{// �m�ۂ���Ă�����
		return E_FAIL;
	}

	// �������m��
	m_pXLoad = DEBUG_NEW CXLoad;

	if (m_pXLoad == NULL)
	{// �������̊m�ۂ��o���Ă��Ȃ�������
		return E_FAIL;
	}
	m_pXLoad->Init();

	//**********************************
	// �}�b�v�}�l�[�W��
	//**********************************
	m_pMapManager = CMapManager::Create("data\\BIN\\maptarget.bin");

	if (m_pMapManager == NULL)
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
	// �G�}�l�[�W��
	//**********************************
	m_pEnemyManager = CEnemyManager::Create("data\\TEXT\\enemy_manager.txt");

	if (m_pEnemyManager == NULL)
	{// NULL��������
		return E_FAIL;
	}

	//**********************************
	// �}�b�v�̐���
	//**********************************
	if (FAILED(map::Create()))
	{// ���s�����ꍇ
		return E_FAIL;
	}

	//**********************************
	// �N���̒n��
	//**********************************
	m_pObject3DMesh = CElevation::Create("data\\TEXT\\elevation_field.txt");

	// ����������
	hr = m_pXLoad->Init();

	if (FAILED(hr))
	{// ���������������s�����ꍇ
		return E_FAIL;
	}

	// �������m��
	m_pPlayer = CPlayer::Create();

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CScene::Uninit(void)
{
	// X�t�@�C���̔j��
	if (m_pXLoad != NULL)
	{// �������̊m�ۂ��o���Ă�����

		// �I������
		m_pXLoad->Uninit();

		// �������̊J��
		delete m_pXLoad;
		m_pXLoad = NULL;
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

	// �G�}�l�[�W��
	if (m_pEnemyManager != NULL)
	{
		m_pEnemyManager->Uninit();
		delete m_pEnemyManager;
		m_pEnemyManager = NULL;
	}

	// �v���C���[�̔j��
	if (m_pPlayer != NULL)
	{// �������̊m�ۂ��o���Ă�����

		m_pPlayer->Uninit();
		m_pPlayer = NULL;
	}

}

//==========================================================================
// �X�V����
//==========================================================================
void CScene::Update(void)
{
	// �}�b�v�}�l�[�W���̍X�V����
	m_pMapManager->Update();

	// �J�����̎��̍X�V����
	m_pCameraAxis->Update();

	// �J�����Ǐ]�ύX��
	m_pCameraChaseChanger->Update();
}

//==========================================================================
// �`�揈��
//==========================================================================
void CScene::Draw(void)
{
	
}

//==========================================================================
// ���݂̃��[�h�擾
//==========================================================================
CScene::MODE CScene::GetMode(void)
{
	return m_mode;
}

//==========================================================================
// X�t�@�C���̎擾
//==========================================================================
CXLoad *CScene::GetXLoad(void)
{
	return m_pXLoad;
}

//==========================================================================
// 3D�I�u�W�F�N�g�̎擾
//==========================================================================
CElevation *CScene::GetElevation(void)
{
	return m_pObject3DMesh;
}

//==========================================================================
// �G�}�l�[�W���̎擾
//==========================================================================
CEnemyManager *CScene::GetEnemyManager(void)
{
	return m_pEnemyManager;
}

//==========================================================================
// �}�b�v�}�l�[�W���̎擾
//==========================================================================
CMapManager *CScene::GetMapManager(void)
{
	return m_pMapManager;
}

//==========================================================================
// �J�����̎��擾
//==========================================================================
CCameraAxis *CScene::GetCameraAxis(void)
{
	return m_pCameraAxis;
}

//==========================================================================
// �J�����Ǐ]�ύX�҂̎擾
//==========================================================================
CCameraChaseChanger *CScene::GetCameraChaseChanger(void)
{
	return m_pCameraChaseChanger;
}

//==========================================================================
// �v���C���[�̎擾
//==========================================================================
CPlayer *CScene::GetPlayer(void)
{
	return m_pPlayer;
}

//==========================================================================
// �v���C���[�̏I��
//==========================================================================
void CScene::UninitPlayer()
{
	m_pPlayer = NULL;
}
