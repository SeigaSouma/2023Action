//=============================================================================
// 
//  ���U���g�}�l�[�W������ [resultmanager.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "debugproc.h"
#include "resultmanager.h"
#include "calculation.h"

//==========================================================================
// �}�N����`
//==========================================================================

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CResultManager::CResultManager()
{
	// �l�̃N���A
	memset(&m_nScoreNum[0], 0, sizeof(m_nScoreNum));	// �X�R�A�̎��
	m_nBaseScore = 0;		// �X�R�A
	m_nToatalScore = 0;	// �ŏI�X�R�A

}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CResultManager::~CResultManager()
{

}

//==========================================================================
// ��������
//==========================================================================
CResultManager *CResultManager::Create(void)
{
	// �����p�̃I�u�W�F�N�g
	CResultManager *pManager = NULL;

	if (pManager == NULL)
	{// NULL��������

		// �������̊m��
		pManager = DEBUG_NEW CResultManager;

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
HRESULT CResultManager::Init(void)
{
	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CResultManager::Uninit(void)
{

}

//==========================================================================
// ���Z�b�g
//==========================================================================
void CResultManager::Reset(void)
{
	// �l�̃N���A
	memset(&m_nScoreNum[0], 0, sizeof(m_nScoreNum));	// �X�R�A�̎��
	m_nBaseScore = 0;		// �X�R�A
	m_nToatalScore = 0;	// �ŏI�X�R�A
}

//==========================================================================
// �X�V����
//==========================================================================
void CResultManager::Update(void)
{
	
}

//==========================================================================
// �X�R�A���Z
//==========================================================================
void CResultManager::AddScore(int nValue)
{
	m_nBaseScore += nValue;
	m_nToatalScore += nValue;
}

//==========================================================================
// �X�R�A�̎�މ��Z
//==========================================================================
void CResultManager::AddScoreNum(CResultManager::ADDTYPE type)
{
	// �����̎�ނ����Z
	m_nScoreNum[type]++;

	// �l���Z
	m_nToatalScore += 0;
}

//==========================================================================
// ��ޖ��̐��擾
//==========================================================================
int CResultManager::GetScoreTypeNum(ADDTYPE type)
{
	return m_nScoreNum[type];
}

//==========================================================================
// �X�R�A�̎擾����
//==========================================================================
int CResultManager::GetBaseScore(void)
{
	return m_nBaseScore;
}

//==========================================================================
// �X�R�A�̎擾����
//==========================================================================
int CResultManager::GetToatalScore(void)
{
	return m_nToatalScore;
}