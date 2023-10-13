//=============================================================================
// 
//  �������� [multinumber.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "multinumber.h"
#include "number.h"
#include "manager.h"
#include "renderer.h"
#include "object2D.h"
#include "objectBillboard.h"
#include "texture.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define TEXTURE			"data\\TEXTURE\\number_blackclover_01.png"	// �e�N�X�`���̃t�@�C��
#define TEX_U			(0.1f)							// U�̕���

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CMultiNumber::CMultiNumber(int nPriority)
{
	// �l�̃N���A
	m_nNum = 0;			// ����
	m_nNumNumber = 0;	// �����̐�
	m_ppMultiNumber = NULL;	// �����̃I�u�W�F�N�g
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// �ʒu
	size = D3DXVECTOR2(0.0f, 0.0f);	// �����̃T�C�Y
	m_objType = CNumber::OBJECTTYPE_2D;	// �I�u�W�F�N�g�̎��
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CMultiNumber::~CMultiNumber()
{

}

//==========================================================================
// ��������
//==========================================================================
CMultiNumber *CMultiNumber::Create(D3DXVECTOR3 pos, D3DXVECTOR2 size, int nNum, CNumber::EObjectType objtype)
{
	// �����p�̃I�u�W�F�N�g
	CMultiNumber *pNumber = NULL;

	if (pNumber == NULL)
	{// NULL��������

		// �������̊m��
		pNumber = DEBUG_NEW CMultiNumber;

		if (pNumber != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// �I�u�W�F�N�g�̎��
			pNumber->m_objType = objtype;

			// �T�C�Y
			pNumber->size = size;

			// �ʒu
			pNumber->m_pos = pos;

			// �����̐�
			pNumber->m_nNumNumber = nNum;

			// ����������
			pNumber->Init();
		}
		else
		{
			delete pNumber;
			pNumber = NULL;
		}

		return pNumber;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CMultiNumber::Init(void)
{
	// �����������m��
	m_ppMultiNumber = DEBUG_NEW (CNumber*[m_nNumNumber]);

	// ��������
	int nTex = CManager::GetTexture()->Regist(TEXTURE);
	for (int nCntNum = 0; nCntNum < m_nNumNumber; nCntNum++)
	{
		// ��������
		m_ppMultiNumber[nCntNum] = CNumber::Create(m_objType);

		// �e��ϐ��̏�����
		switch (m_objType)
		{
		case CNumber::OBJECTTYPE_2D:
			m_ppMultiNumber[nCntNum]->GetObject2D()->SetSize(size);	// �T�C�Y
			m_ppMultiNumber[nCntNum]->GetObject2D()->SetPosition(D3DXVECTOR3(m_pos.x + size.y * nCntNum, m_pos.y, m_pos.z));	// �ʒu

			// ��ނ̐ݒ�
			m_ppMultiNumber[nCntNum]->GetObject2D()->SetType(CObject::TYPE_SCORE);

			// �e�N�X�`���̊��蓖��
			m_ppMultiNumber[nCntNum]->GetObject2D()->BindTexture(nTex);
			break;

		case CNumber::OBJECTTYPE_BILLBOARD:
			m_ppMultiNumber[nCntNum]->GetObjectBillboard()->SetSize(size);	// �T�C�Y
			m_ppMultiNumber[nCntNum]->GetObjectBillboard()->SetPosition(D3DXVECTOR3(m_pos.x + size.y * nCntNum, m_pos.y, m_pos.z));	// �ʒu

			// ��ނ̐ݒ�
			m_ppMultiNumber[nCntNum]->GetObjectBillboard()->SetType(CObject::TYPE_SCORE);

			// �e�N�X�`���̊��蓖��
			m_ppMultiNumber[nCntNum]->GetObjectBillboard()->BindTexture(nTex);
			break;

		default:
			break;
		}
	}

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CMultiNumber::Uninit(void)
{
	// �I������
	if (m_ppMultiNumber != NULL)
	{// NULL����Ȃ�������

		// �e�����I�u�W�F�N�g�̔j��
		for (int nCntNum = 0; nCntNum < m_nNumNumber; nCntNum++)
		{
			if (m_ppMultiNumber[nCntNum] != NULL)
			{
				m_ppMultiNumber[nCntNum]->Uninit();
				delete m_ppMultiNumber[nCntNum];
				m_ppMultiNumber[nCntNum] = NULL;
			}
		}
		delete[] m_ppMultiNumber;
		m_ppMultiNumber = NULL;
	}
}

//==========================================================================
// �������
//==========================================================================
void CMultiNumber::Release(void)
{
	// �I������
	if (m_ppMultiNumber != NULL)
	{// NULL����Ȃ�������

		// �e�����I�u�W�F�N�g�̔j��
		for (int nCntNum = 0; nCntNum < m_nNumNumber; nCntNum++)
		{
			if (m_ppMultiNumber[nCntNum] != NULL)
			{
				m_ppMultiNumber[nCntNum]->Release();
				delete m_ppMultiNumber[nCntNum];
				m_ppMultiNumber[nCntNum] = NULL;
			}
		}
		delete m_ppMultiNumber;
		m_ppMultiNumber = NULL;
	}
}

//==========================================================================
// �X�V����
//==========================================================================
void CMultiNumber::Update(void)
{
	// �X�V����
}

//==========================================================================
// �`�揈��
//==========================================================================
void CMultiNumber::Draw(void)
{
	for (int nCntNum = 0; nCntNum < m_nNumNumber; nCntNum++)
	{
		if (m_ppMultiNumber[nCntNum] == NULL)
		{// NULL��������
			continue;
		}
		m_ppMultiNumber[nCntNum]->Draw();
	}
}

//==========================================================================
// �l�̐ݒ菈��
//==========================================================================
void CMultiNumber::AddNumber(int nValue)
{
	m_nNum += nValue;

	// �l�̐ݒ菈��
	SetValue();
}

//==========================================================================
// �l�̐ݒ菈��
//==========================================================================
void CMultiNumber::SetValue(int nValue)
{
	m_nNum = nValue;

	// �l�̐ݒ菈��
	SetValue();
}

//==========================================================================
// �l�̐ݒ菈��
//==========================================================================
void CMultiNumber::SetValue(void)
{
	for (int nCntNum = 0; nCntNum < m_nNumNumber; nCntNum++)
	{
		if (m_ppMultiNumber[nCntNum] == NULL)
		{// NULL��������
			continue;
		}

		int aTexU = m_nNum % (int)std::powf(10, m_nNumNumber + 1 - nCntNum) / ((int)std::powf(10, m_nNumNumber - nCntNum) / 10);

		if (aTexU < 0)
		{
			aTexU = 0;
		}

		// �e�N�X�`���|�C���^�擾
		D3DXVECTOR2 *pTex;
		switch (m_objType)
		{
		case CNumber::OBJECTTYPE_2D:
			// �e�N�X�`���|�C���^�擾
			pTex = m_ppMultiNumber[nCntNum]->GetObject2D()->GetTex();

			// �e�N�X�`�����W�̐ݒ�
			pTex[0] = D3DXVECTOR2(aTexU * TEX_U, 0.0f);
			pTex[1] = D3DXVECTOR2(aTexU * TEX_U + TEX_U, 0.0f);
			pTex[2] = D3DXVECTOR2(aTexU * TEX_U, 1.0f);
			pTex[3] = D3DXVECTOR2(aTexU * TEX_U + TEX_U, 1.0f);

			// ���_�ݒ�
			m_ppMultiNumber[nCntNum]->GetObject2D()->SetVtx();
			break;

		case CNumber::OBJECTTYPE_BILLBOARD:
			// �e�N�X�`���|�C���^�擾
			pTex = m_ppMultiNumber[nCntNum]->GetObjectBillboard()->GetTex();

			// �e�N�X�`�����W�̐ݒ�
			pTex[0] = D3DXVECTOR2(aTexU * TEX_U, 0.0f);
			pTex[1] = D3DXVECTOR2(aTexU * TEX_U + TEX_U, 0.0f);
			pTex[2] = D3DXVECTOR2(aTexU * TEX_U, 1.0f);
			pTex[3] = D3DXVECTOR2(aTexU * TEX_U + TEX_U, 1.0f);

			// ���_�ݒ�
			m_ppMultiNumber[nCntNum]->GetObjectBillboard()->SetVtx();
			break;
		}
	}
}

//==========================================================================
// �ʒu�ݒ�
//==========================================================================
void CMultiNumber::SetPosition(const D3DXVECTOR3 pos)
{
	// �ʒu�ݒ�
	m_pos = pos;

	switch (m_objType)
	{
	case CNumber::OBJECTTYPE_2D:
		for (int nCntNum = 0; nCntNum < m_nNumNumber; nCntNum++)
		{
			if (m_ppMultiNumber[nCntNum] != NULL)
			{
				m_ppMultiNumber[nCntNum]->GetObject2D()->SetPosition(D3DXVECTOR3(m_pos.x + size.y * nCntNum, m_pos.y, m_pos.z));	// �ʒu
			}
		}
		break;

	case CNumber::OBJECTTYPE_BILLBOARD:
		for (int nCntNum = 0; nCntNum < m_nNumNumber; nCntNum++)
		{
			if (m_ppMultiNumber[nCntNum] != NULL)
			{
				m_ppMultiNumber[nCntNum]->GetObjectBillboard()->SetPosition(D3DXVECTOR3(m_pos.x + size.y * nCntNum, m_pos.y, m_pos.z));	// �ʒu
			}
		}
		break;
	}
}

//==========================================================================
// �ʒu�擾
//==========================================================================
D3DXVECTOR3 CMultiNumber::GetPosition(void) const
{
	return m_pos;
}