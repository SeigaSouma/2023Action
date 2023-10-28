//=============================================================================
// 
// �Q�[���}�l�[�W������ [gamemanager.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "gamemanager.h"
#include "game.h"
#include "debugproc.h"
#include "manager.h"
#include "renderer.h"
#include "instantfade.h"
#include "player.h"
#include "camera.h"

//==========================================================================
// �}�N����`
//==========================================================================

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CGameManager::CGameManager()
{
	// �l�̃N���A
	m_SceneType = SCENE_MAIN;	// �V�[���̎��
	m_bEndRush = false;			// ���b�V�����I��������
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CGameManager::~CGameManager()
{

}

//==========================================================================
// ��������
//==========================================================================
CGameManager *CGameManager::Create(void)
{
	// �����p�̃I�u�W�F�N�g
	CGameManager *pManager = NULL;

	if (pManager == NULL)
	{// NULL��������

		// �������̊m��
		pManager = DEBUG_NEW CGameManager;

		if (pManager != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ����������
			HRESULT hr = pManager->Init();

			if (FAILED(hr))
			{// ���s���Ă�����
				return NULL;
			}
		}

		return pManager;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CGameManager::Init(void)
{
	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CGameManager::Uninit(void)
{
	
}

//==========================================================================
// �X�V����
//==========================================================================
void CGameManager::Update(void)
{
	if (m_SceneType == SCENE_TRANSITION)
	{// �J�ڒ�

		// �J�ڂȂ��t�F�[�h�̏�Ԏ擾
		CInstantFade::STATE fadestate = CManager::GetInstance()->GetInstantFade()->GetState();

		if (fadestate == CInstantFade::STATE_FADECOMPLETION)
		{// ���������u��
			
			// �Ǐ]�̎�ސݒ�
			m_SceneType = SCENE_BOSS;

			// ���Z�b�g����
			CGame::Reset();

			// �V�[���̃��Z�b�g
			CManager::GetInstance()->GetScene()->ResetScene();

			// �J�����̏��擾
			CCamera *pCamera = CManager::GetInstance()->GetCamera();
			pCamera->ResetBoss();

			CManager::GetInstance()->GetScene()->GetPlayer()->SetPosition(D3DXVECTOR3(0.0f, 2000.0f, 0.0f));
		}
	}
	
}

//==========================================================================
// �V�[���̎�ސݒ�
//==========================================================================
void CGameManager::SetType(SceneType type)
{
	m_SceneType = type;
}

//==========================================================================
// �V�[���̎�ގ擾
//==========================================================================
CGameManager::SceneType CGameManager::GetType(void)
{
	return m_SceneType;
}
