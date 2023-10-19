//=============================================================================
// 
//  �J�����̎����� [cameraaxis.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "debugproc.h"
#include "cameraaxis.h"
#include "calculation.h"
#include "game.h"
#include "3D_effect.h"
#include "debugpointnumber.h"
#include "objectX.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define MARKOBJ	"data\\MODEL\\map\\mark_01.x"

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
int CCameraAxis::m_nNumAll = 0;		// ����

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CCameraAxis::CCameraAxis()
{
	// �l�̃N���A
	memset(&m_pMultiNumber[0], NULL, sizeof(m_pMultiNumber));
	memset(&m_apObjX[0], NULL, sizeof(m_apObjX));	// �I�u�W�F�N�gX
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CCameraAxis::~CCameraAxis()
{

}

//==========================================================================
// ��������
//==========================================================================
CCameraAxis *CCameraAxis::Create(const std::string pFileName)
{
	// �����p�̃I�u�W�F�N�g
	CCameraAxis *pModel = NULL;

	if (pModel == NULL)
	{// NULL��������

		// �������̊m��
		pModel = DEBUG_NEW CCameraAxis;

		if (pModel != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ����������
			HRESULT hr = pModel->Init();
			if (FAILED(hr))
			{// ���s���Ă�����
				delete pModel;
				return NULL;
			}

			// ����������
			hr = pModel->ReadText(pFileName);

			if (FAILED(hr))
			{// ���s���Ă�����
				return NULL;
			}
		}

		return pModel;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CCameraAxis::Init(void)
{
	//// �������Z�b�g
	//m_ppMapManager = DEBUG_NEW (CCameraAxis*);
	//*m_ppMapManager = NULL;

	return S_OK;
}

//==========================================================================
// �ʒu�쐬
//==========================================================================
void CCameraAxis::CreatePos(D3DXVECTOR3 pos)
{
	// �ʒu����
	m_posAll.push_back(pos);

	// �f�o�b�O�p�����̐���
	m_pMultiNumber[m_nNumAll] = CDebugPointNumber::Create(m_nNumAll);
	m_apObjX[m_nNumAll] = CObjectX::Create(MARKOBJ, mylib_const::DEFAULT_VECTOR3, mylib_const::DEFAULT_VECTOR3, false);	// �I�u�W�F�N�gX
	m_apObjX[m_nNumAll]->SetType(CObject::TYPE_BALLAST);

	// �������Z
	m_nNumAll++;

}

//==========================================================================
// �ʒu�폜
//==========================================================================
void CCameraAxis::DeletePos(int nIdx)
{
	// �\�[�g����
	CalSort(&m_posAll[0], nIdx, m_nNumAll);

	// �ʒu����
	m_posAll.pop_back();

	// �������Z
	m_nNumAll--;
}

//==========================================================================
// �I������
//==========================================================================
void CCameraAxis::Uninit(void)
{
	for (int i = 0; i < 32; i++)
	{
		if (m_pMultiNumber[i] != NULL)
		{// NULL����Ȃ�������
			//m_pMultiNumber[i]->Uninit();
			m_pMultiNumber[i] = NULL;
		}
	}
}

//==========================================================================
// �X�V����
//==========================================================================
void CCameraAxis::Update(void)
{
#if _DEBUG
	for (int i = 0; i < m_nNumAll; i++)
	{
		CEffect3D::Create(m_posAll[i], D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), 50.0f, 2, CEffect3D::MOVEEFFECT_NONE, CEffect3D::TYPE_NORMAL);
		m_apObjX[i]->SetPosition(m_posAll[i]);
		m_pMultiNumber[i]->SetPosition(D3DXVECTOR3(m_posAll[i].x, m_posAll[i].y + 50.0f, m_posAll[i].z));
	}
#endif
}

//==========================================================================
// �e�L�X�g�ǂݍ��ݏ���
//==========================================================================
HRESULT CCameraAxis::ReadText(const std::string pFileName)
{

	// �t�@�C�����J��
	FILE *pFile = fopen(pFileName.c_str(), "rb");

	if (pFile != NULL)
	{// �t�@�C�����J�����ꍇ

		// �t�@�C���ɐ��l��ǂݍ���
		fread(&m_nNumAll, sizeof(int), 1, pFile);

		// ��������
		for (int i = 0; i < m_nNumAll; i++)
		{
			// �ʒu����
			m_posAll.push_back(mylib_const::DEFAULT_VECTOR3);
			m_pMultiNumber[i] = CDebugPointNumber::Create(i);
			m_apObjX[i] = CObjectX::Create(MARKOBJ, mylib_const::DEFAULT_VECTOR3, mylib_const::DEFAULT_VECTOR3, false);	// �I�u�W�F�N�gX
			m_apObjX[i]->SetType(CObject::TYPE_BALLAST);
		}

		fread(&m_posAll[0], sizeof(D3DXVECTOR3), m_nNumAll, pFile);

		// �t�@�C�������
		fclose(pFile);
	}
	else
	{// �t�@�C�����J���Ȃ������ꍇ

		m_nNumAll = 1;

		// ��������
		for (int i = 0; i < m_nNumAll; i++)
		{
			// �ʒu����
			m_posAll.push_back(mylib_const::DEFAULT_VECTOR3);
			m_pMultiNumber[i] = CDebugPointNumber::Create(i);
			m_apObjX[i] = CObjectX::Create(MARKOBJ, mylib_const::DEFAULT_VECTOR3, mylib_const::DEFAULT_VECTOR3, false);	// �I�u�W�F�N�gX
			m_apObjX[i]->SetType(CObject::TYPE_BALLAST);
		}

		for (int nCount = 0; nCount < m_nNumAll; nCount++)
		{// �S���[��
			m_posAll[nCount] = mylib_const::DEFAULT_VECTOR3;
		}

		// �Z�[�u
		Save(pFileName);
	}

	return S_OK;
}

//==========================================================================
// �O���t�@�C�������o������
//==========================================================================
void CCameraAxis::Save(const std::string pFileName)
{
	// �t�@�C�����J��
	FILE *pFile = fopen(pFileName.c_str(), "wb");

	if (pFile != NULL)
	{// �t�@�C�����J�����ꍇ

		// �t�@�C���ɐ��l�������o��
		fwrite(&m_nNumAll, sizeof(int), 1, pFile);
		fwrite(&m_posAll[0], sizeof(D3DXVECTOR3), m_nNumAll, pFile);

		// �t�@�C�������
		fclose(pFile);
	}
}

//==========================================================================
// �����擾
//==========================================================================
int CCameraAxis::GetAxisNum(void)
{
	return m_posAll.size() - 1;
}

//==========================================================================
// ���ݒ�
//==========================================================================
void CCameraAxis::SetAxis(D3DXVECTOR3 pos, int nIdx)
{
	m_posAll[nIdx] = pos;
}

//==========================================================================
// ���擾
//==========================================================================
D3DXVECTOR3 CCameraAxis::GetAxis(int nIdx)
{
	if (nIdx < 0)
	{
		nIdx = 0;
	}

	if (nIdx >= (int)m_posAll.size())
	{// �v�f���𒴂��Ă�����

		int nMaxIdx = (int)m_posAll.size() - 1;
		return m_posAll[nMaxIdx];
	}
	return m_posAll[nIdx];
}

