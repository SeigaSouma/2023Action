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
#include "sound.h"
#include "mapmanager.h"
#include "blackframe.h"

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
	m_bControll = false;		// ����ł��邩
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
	m_bControll = true;		// ����ł��邩
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


			// BGM�X�g�b�v
			CManager::GetInstance()->GetSound()->StopSound(CSound::LABEL_BGM_GAME);

			// �Ǐ]�̎�ސݒ�
			m_SceneType = SCENE_BOSS;

			// ���Z�b�g����
			CGame::Reset();

			// �V�[���̃��Z�b�g
			CManager::GetInstance()->GetScene()->ResetScene();

			// �v���C���[���
			CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();
			if (pPlayer == NULL)
			{
				return;
			}
			// �}�b�v�}�l�[�W���̎擾
			CMapManager *pMapManager = CGame::GetMapManager();
			if (pMapManager == NULL)
			{// NULL��������
				return;
			}

			D3DXVECTOR3 pos = pMapManager->GetTargetPosition(0, 0.0f);
			pPlayer->SetPosition(pos);
			pPlayer->SetMapIndex(0);
			pPlayer->SetMapMoveValue(0.0f);

			// �J�����̏��擾
			CCamera *pCamera = CManager::GetInstance()->GetCamera();
			pCamera->ResetBoss();

			// ���t���[���N��
			CManager::GetInstance()->GetBlackFrame()->SetState(CBlackFrame::STATE_IN);

			// ����s�\��Ԃɂ���
			m_bControll = false;
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
