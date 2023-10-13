//=============================================================================
// 
//  �|�C���g�̐������� [debugpointnumber.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "debugpointnumber.h"
#include "manager.h"
#include "renderer.h"
#include "calculation.h"
#include "debugproc.h"
#include "input.h"
#include "3D_effect.h"
#include "multinumber.h"
#include "mapmanager.h"

//==========================================================================
// �}�N����`
//==========================================================================

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CDebugPointNumber::CDebugPointNumber(int nPriority) : CObject(nPriority)
{
	// �[���N���A
	m_nIdxParent = 0;		// �e�̔ԍ�
	m_pMultiNumber = NULL;	// �����̃I�u�W�F�N�g

}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CDebugPointNumber::~CDebugPointNumber()
{

}

//==========================================================================
// ��������
//==========================================================================
CDebugPointNumber *CDebugPointNumber::Create(int nIdx)
{
	// �����p�̃I�u�W�F�N�g
	CDebugPointNumber *pObjectX = NULL;

	if (pObjectX == NULL)
	{// NULL��������

		// �������̊m��
		pObjectX = DEBUG_NEW CDebugPointNumber;

		if (pObjectX != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// �e�̔ԍ�
			pObjectX->m_nIdxParent = nIdx;

			// ����������
			HRESULT hr = pObjectX->Init();
			if (FAILED(hr))
			{// ���s���Ă�����
				return NULL;
			}
		}

		return pObjectX;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CDebugPointNumber::Init(void)
{
	//if (CManager::GetScene()->GetMapManager() == NULL)
	//{// NULL�Ŕ�����
	//	return E_FAIL;
	//}

	//// �}�b�v�}�l�[�W���擾
	//CMapManager *pMapManager = CManager::GetScene()->GetMapManager();

	// ��������
	m_pMultiNumber = CMultiNumber::Create(
		D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		D3DXVECTOR2(50.0f, 60.0f),
		GetDigit(m_nIdxParent),
		CNumber::OBJECTTYPE_BILLBOARD);	// �����̃I�u�W�F�N�g

	// ��ސݒ�
	SetType(CObject::TYPE_NUMBER);

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CDebugPointNumber::Uninit(void)
{
	// �����̃I�u�W�F�N�g�̏I������
	if (m_pMultiNumber != NULL)
	{
		m_pMultiNumber->Uninit();
		delete m_pMultiNumber;
		m_pMultiNumber = NULL;
	}

	// �I������
	Release();
}

//==========================================================================
// �X�V����
//==========================================================================
void CDebugPointNumber::Update(void)
{
	// �}�b�v�}�l�[�W���擾
	CMapManager *pMapManager = CManager::GetScene()->GetMapManager();
	if (pMapManager == NULL)
	{// NULL�Ŕ�����
		return;
	}

	// �l�̐ݒ菈��
	m_pMultiNumber->SetValue(m_nIdxParent);

	// �ʒu�X�V
	D3DXVECTOR3 pos = pMapManager->GetControlPoint(m_nIdxParent + 1);
	pos.y += 50.0f;
	m_pMultiNumber->SetPosition(pos);
}

//==========================================================================
// �`�揈��
//==========================================================================
void CDebugPointNumber::Draw(void)
{

}
