//=============================================================================
// 
//  �J�����̎����� [camerachasechanger.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "debugproc.h"
#include "camerachasechanger.h"
#include "calculation.h"
#include "game.h"
#include "3D_effect.h"
#include "debugpointnumber.h"
#include "objectX.h"
#include "mapmanager.h"
#include "stage.h"
#include "manager.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define MARKOBJ	"data\\MODEL\\crowd_01.x"

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
int CCameraChaseChanger::m_nNumChaseAxis = 0;		// ���Ǐ]�̑���
int CCameraChaseChanger::m_nNumChaseMap = 0;		// �}�b�v�Ǐ]�̑���
int CCameraChaseChanger::m_nNumAll = 0;		// ����

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CCameraChaseChanger::CCameraChaseChanger()
{
	// �l�̃N���A
	memset(&m_pMultiNumber[0], NULL, sizeof(m_pMultiNumber));
	memset(&m_apObjX[0], NULL, sizeof(m_apObjX));	// �I�u�W�F�N�gX
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CCameraChaseChanger::~CCameraChaseChanger()
{

}

//==========================================================================
// ��������
//==========================================================================
CCameraChaseChanger *CCameraChaseChanger::Create(const std::string pFileName)
{
	// �����p�̃I�u�W�F�N�g
	CCameraChaseChanger *pModel = NULL;

	if (pModel == NULL)
	{// NULL��������

		// �������̊m��
		pModel = DEBUG_NEW CCameraChaseChanger;

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
HRESULT CCameraChaseChanger::Init(void)
{
	return S_OK;
}

//==========================================================================
// �ʒu�쐬
//==========================================================================
void CCameraChaseChanger::CreatePos(CCamera::CHASETYPE type, int nIdx, float fMoveValue)
{
	sChaseChangeInfo InitInfo;
	memset(&InitInfo, NULL, sizeof(InitInfo));

	// �ʒu����
	m_ChaseChangeInfo.push_back(InitInfo);
	m_ChaseChangeInfo[m_nNumAll].nMapIdx = nIdx;
	m_ChaseChangeInfo[m_nNumAll].fMapMoveValue = fMoveValue;
	m_ChaseChangeInfo[m_nNumAll].chaseType = type;

	// �������Z
	switch (type)
	{
	case CCamera::CHASETYPE_NORMAL:
		m_ChaseChangeInfo[m_nNumAll].nByTypeIdx = m_nNumChaseAxis;
		m_nNumChaseAxis++;
		break;
	case CCamera::CHASETYPE_MAP:
		m_ChaseChangeInfo[m_nNumAll].nByTypeIdx = m_nNumChaseMap;
		m_nNumChaseMap++;
		break;
	}

	// �f�o�b�O�p�����̐���
	m_pMultiNumber[m_nNumAll] = CDebugPointNumber::Create(m_ChaseChangeInfo[m_nNumAll].nByTypeIdx);
	m_apObjX[m_nNumAll] = CObjectX::Create(MARKOBJ, mylib_const::DEFAULT_VECTOR3, mylib_const::DEFAULT_VECTOR3, false);	// �I�u�W�F�N�gX
	m_apObjX[m_nNumAll]->SetType(CObject::TYPE_BALLAST);

	// �������Z
	m_nNumAll++;

}

//==========================================================================
// �ʒu�폜
//==========================================================================
void CCameraChaseChanger::DeletePos(int nIdx)
{
	// �\�[�g����
	CalSort(&m_ChaseChangeInfo[0], nIdx, m_nNumAll);

	// �ʒu����
	m_ChaseChangeInfo.pop_back();

	// �������Z
	m_nNumAll--;
}

//==========================================================================
// �I������
//==========================================================================
void CCameraChaseChanger::Uninit(void)
{
	for (int i = 0; i < mylib_const::MAX_CAMERAAXIS; i++)
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
void CCameraChaseChanger::Update(void)
{
#if _DEBUG

	// �}�b�v�}�l�[�W���̎擾
	CMapManager *pMapManager = CGame::GetMapManager();
	if (pMapManager == NULL)
	{// NULL��������
		return;
	}

	// X�t�@�C���Ƃ̔���
	CStage *pStage = CGame::GetStage();
	if (pStage == NULL)
	{// NULL��������
		return;
	}

	for (int i = 0; i < m_nNumAll; i++)
	{
		if (m_apObjX[i] == NULL)
		{
			m_apObjX[i] = CObjectX::Create(MARKOBJ, mylib_const::DEFAULT_VECTOR3, mylib_const::DEFAULT_VECTOR3, false);	// �I�u�W�F�N�gX
			m_apObjX[i]->SetType(CObject::TYPE_BALLAST);
		}

		// �}�b�v��񂩂�ʒu�擾
		D3DXVECTOR3 pos = pMapManager->GetTargetPosition(m_ChaseChangeInfo[i].nMapIdx, m_ChaseChangeInfo[i].fMapMoveValue);

		float fHeight = 0.0f;
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

		m_apObjX[i]->SetPosition(pos);
		m_pMultiNumber[i]->SetPosition(D3DXVECTOR3(pos.x, pos.y + 50.0f, pos.z));
	}
#endif
}

//==========================================================================
// �e�L�X�g�ǂݍ��ݏ���
//==========================================================================
HRESULT CCameraChaseChanger::ReadText(const std::string pFileName)
{

	// �t�@�C�����J��
	FILE *pFile = fopen(pFileName.c_str(), "rb");

	if (pFile != NULL)
	{// �t�@�C�����J�����ꍇ

		// �t�@�C���ɐ��l��ǂݍ���
		fread(&m_nNumAll, sizeof(int), 1, pFile);
		fread(&m_nNumChaseAxis, sizeof(int), 1, pFile);
		fread(&m_nNumChaseMap, sizeof(int), 1, pFile);

		// ��������
		for (int i = 0; i < m_nNumAll; i++)
		{
			// �ʒu����
			sChaseChangeInfo InitInfo;
			memset(&InitInfo, NULL, sizeof(InitInfo));
			m_ChaseChangeInfo.push_back(InitInfo);
			m_pMultiNumber[i] = CDebugPointNumber::Create(i);
		}

		if (m_ChaseChangeInfo.size() > 0)
		{
			fread(&m_ChaseChangeInfo[0], sizeof(sChaseChangeInfo), m_nNumAll, pFile);
		}

		// �}�b�v�}�l�[�W���̎擾
		CMapManager *pMapManager = CGame::GetMapManager();
		if (pMapManager == NULL)
		{// NULL��������
			return S_OK;
		}
		for (int i = 0; i < m_nNumAll; i++)
		{
			m_ChaseChangeInfo[i].pos = pMapManager->GetTargetPosition(m_ChaseChangeInfo[i].nMapIdx, m_ChaseChangeInfo[i].fMapMoveValue);
		}

		// �t�@�C�������
		fclose(pFile);
	}
	else
	{// �t�@�C�����J���Ȃ������ꍇ

		m_nNumAll = 0;
		m_nNumChaseAxis = 0;
		m_nNumChaseMap = 0;

		// ��������
		for (int i = 0; i < m_nNumAll; i++)
		{
			// �ʒu����
			sChaseChangeInfo InitInfo;
			memset(&InitInfo, NULL, sizeof(InitInfo));
			m_ChaseChangeInfo.push_back(InitInfo);
			m_pMultiNumber[i] = CDebugPointNumber::Create(i);
		}

		// �S���[��
		if (m_ChaseChangeInfo.size() > 0)
		{
			memset(&m_ChaseChangeInfo[0], NULL, sizeof(m_ChaseChangeInfo));
		}

		// �Z�[�u
		Save(pFileName);
	}

	return S_OK;
}

//==========================================================================
// �O���t�@�C�������o������
//==========================================================================
void CCameraChaseChanger::Save(const std::string pFileName)
{
	// �t�@�C�����J��
	FILE *pFile = fopen(pFileName.c_str(), "wb");

	if (pFile != NULL)
	{// �t�@�C�����J�����ꍇ

		// �t�@�C���ɐ��l�������o��
		fwrite(&m_nNumAll, sizeof(int), 1, pFile);
		fwrite(&m_nNumChaseAxis, sizeof(int), 1, pFile);
		fwrite(&m_nNumChaseMap, sizeof(int), 1, pFile);

		if (m_ChaseChangeInfo.size() > 0)
		{
			fwrite(&m_ChaseChangeInfo[0], sizeof(sChaseChangeInfo), m_nNumAll, pFile);
		}

		// �t�@�C�������
		fclose(pFile);
	}
}

//==========================================================================
// �����擾
//==========================================================================
int CCameraChaseChanger::GetAxisNum(void)
{
	return m_ChaseChangeInfo.size() - 1;
}

//==========================================================================
// ���擾
//==========================================================================
D3DXVECTOR3 CCameraChaseChanger::GetAxis(int nIdx)
{
	if (nIdx < 0)
	{
		nIdx = 0;
	}

	// �}�b�v�}�l�[�W���̎擾
	CMapManager *pMapManager = CGame::GetMapManager();
	if (pMapManager == NULL)
	{// NULL��������
		return mylib_const::DEFAULT_VECTOR3;
	}

	D3DXVECTOR3 pos = mylib_const::DEFAULT_VECTOR3;

	if (nIdx >= (int)m_ChaseChangeInfo.size())
	{// �v�f���𒴂��Ă�����

		int nMaxIdx = (int)m_ChaseChangeInfo.size() - 1;

		// �}�b�v��񂩂�ʒu�擾
		pos = pMapManager->GetTargetPosition(m_ChaseChangeInfo[nMaxIdx].nMapIdx, m_ChaseChangeInfo[nMaxIdx].fMapMoveValue);
		return pos;
	}

	// �}�b�v��񂩂�ʒu�擾
	pos = pMapManager->GetTargetPosition(m_ChaseChangeInfo[nIdx].nMapIdx, m_ChaseChangeInfo[nIdx].fMapMoveValue);

	return pos;
}

//==========================================================================
// ���ݒ�
//==========================================================================
void CCameraChaseChanger::SetAxis(int nIdx, int nMapIdx, float fMapMoveValue)
{
	if (nIdx < 0)
	{
		return;
	}

	// ���n��
	m_ChaseChangeInfo[nIdx].nMapIdx = nMapIdx;
	m_ChaseChangeInfo[nIdx].fMapMoveValue = fMapMoveValue;
}

//==========================================================================
// �ύX�̏��擾
//==========================================================================
CCameraChaseChanger::sChaseChangeInfo CCameraChaseChanger::GetChaseChangeInfo(int nIdx)
{
	sChaseChangeInfo InitInfo;
	memset(&InitInfo, NULL, sizeof(InitInfo));

	if (m_ChaseChangeInfo.size() <= 0 || (int)m_ChaseChangeInfo.size() <= nIdx)
	{// �T�C�Y����
		return InitInfo;
	}

	return m_ChaseChangeInfo[nIdx];
}
