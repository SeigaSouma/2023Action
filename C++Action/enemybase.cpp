//=============================================================================
// 
//  �J�����̎����� [enemybase.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "debugproc.h"
#include "enemybase.h"
#include "calculation.h"
#include "game.h"
#include "3D_effect.h"
#include "debugpointnumber.h"
#include "objectX.h"
#include "mapmanager.h"
#include "enemymanager.h"
#include "manager.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define MARKOBJ	"data\\MODEL\\koko.x"
#define FILE_ENEMYBASE	"data\\BIN\\enemybase.bin"

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
int CEnemyBase::m_nNumAll = 0;		// ����

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CEnemyBase::CEnemyBase()
{
	// �l�̃N���A
		memset(&m_pMultiNumber[0], NULL, sizeof(m_pMultiNumber));	// �I�u�W�F�N�gX
	memset(&m_apObjX[0], NULL, sizeof(m_apObjX));	// �I�u�W�F�N�gX
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CEnemyBase::~CEnemyBase()
{

}

//==========================================================================
// ��������
//==========================================================================
CEnemyBase *CEnemyBase::Create(void)
{
	// �����p�̃I�u�W�F�N�g
	CEnemyBase *pModel = NULL;

	if (pModel == NULL)
	{// NULL��������

		// �������̊m��
		pModel = DEBUG_NEW CEnemyBase;

		if (pModel != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ����������
			HRESULT hr = pModel->ReadText();

			if (FAILED(hr))
			{// ���s���Ă�����
				return NULL;
			}

			// ����������
			hr = pModel->Init();
			if (FAILED(hr))
			{// ���s���Ă�����
				delete pModel;
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
HRESULT CEnemyBase::Init(void)
{
	//// �������Z�b�g
	//m_ppMapManager = DEBUG_NEW (CEnemyBase*);
	//*m_ppMapManager = NULL;

	CEnemyManager *pEnemyManager = CGame::GetEnemyManager();

	// ��������
	for (int i = 0; i < m_nNumAll; i++)
	{
		// �f�o�b�O�p�����̐���
		m_pMultiNumber[i] = CDebugPointNumber::Create(i);

		//if (m_ChaseChangeInfo[i].nRush == 0)
		//{// ���b�V���p����Ȃ�������
		//	pEnemyManager->SetEnemy(
		//		D3DXVECTOR3(0.0f, m_ChaseChangeInfo[i].fSpawnPosY, 0.0f),
		//		m_ChaseChangeInfo[i].nMapIdx,
		//		m_ChaseChangeInfo[i].fMapMoveValue,
		//		m_ChaseChangeInfo[i].nPattern);
		//}
	}

	return S_OK;
}

//==========================================================================
// �ʒu�쐬
//==========================================================================
void CEnemyBase::CreatePos(int nPattern, int nMapIdx, float fMapMoveValue, int nRush, float PosY)
{
	sInfo InitInfo;
	memset(&InitInfo, NULL, sizeof(InitInfo));

	// �ʒu����
	m_ChaseChangeInfo.push_back(InitInfo);
	m_ChaseChangeInfo[m_nNumAll].nPattern = nPattern;	// ���
	m_ChaseChangeInfo[m_nNumAll].nMapIdx = nMapIdx;
	m_ChaseChangeInfo[m_nNumAll].fMapMoveValue = fMapMoveValue;
	m_ChaseChangeInfo[m_nNumAll].fSpawnPosY = PosY;
	m_ChaseChangeInfo[m_nNumAll].nRush = nRush;

	// �ڈ󐶐�
	m_apObjX[m_nNumAll] = CObjectX::Create(MARKOBJ, mylib_const::DEFAULT_VECTOR3, mylib_const::DEFAULT_VECTOR3, false);	// �I�u�W�F�N�gX
	m_apObjX[m_nNumAll]->SetType(CObject::TYPE_BALLAST);
	m_apObjX[m_nNumAll]->SetPosition(D3DXVECTOR3(0.0f, PosY, 0.0f));

	// �������Z
	m_nNumAll++;

}

//==========================================================================
// �ʒu�폜
//==========================================================================
void CEnemyBase::DeletePos(int nIdx)
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
void CEnemyBase::Uninit(void)
{
	
}

//==========================================================================
// �X�V����
//==========================================================================
void CEnemyBase::Update(void)
{
#if _DEBUG

	// �}�b�v�}�l�[�W���̎擾
	CMapManager *pMapManager = CManager::GetInstance()->GetScene()->GetMapManager();
	if (pMapManager == NULL)
	{// NULL��������
		return;
	}

	for (int i = 0; i < m_nNumAll; i++)
	{
		if (m_apObjX[i] == NULL)
		{
			m_apObjX[i] = CObjectX::Create(MARKOBJ, mylib_const::DEFAULT_VECTOR3, mylib_const::DEFAULT_VECTOR3, false);	// �I�u�W�F�N�gX
			m_apObjX[i]->SetType(CObject::TYPE_BALLAST);
			//m_apObjX[i]->SetPositionD3DXVECTOR3(pos.x, m_apObjX[i]->GetPosition().y, pos.z);
		}

		// �}�b�v��񂩂�ʒu�擾
		D3DXVECTOR3 pos = pMapManager->GetTargetPosition(m_ChaseChangeInfo[i].nMapIdx, m_ChaseChangeInfo[i].fMapMoveValue);

		m_apObjX[i]->SetPosition(D3DXVECTOR3(pos.x, m_ChaseChangeInfo[i].fSpawnPosY, pos.z));

		if (m_pMultiNumber[i] != NULL)
		{
			m_pMultiNumber[i]->SetPosition(D3DXVECTOR3(pos.x, m_ChaseChangeInfo[i].fSpawnPosY + 50.0f, pos.z));
		}
	}
#endif
}

//==========================================================================
// �e�L�X�g�ǂݍ��ݏ���
//==========================================================================
HRESULT CEnemyBase::ReadText(void)
{

	// �t�@�C�����J��
	FILE *pFile = fopen(FILE_ENEMYBASE, "rb");

	if (pFile != NULL)
	{// �t�@�C�����J�����ꍇ

		// �t�@�C���ɐ��l��ǂݍ���
		fread(&m_nNumAll, sizeof(int), 1, pFile);

		// ��������
		for (int i = 0; i < m_nNumAll; i++)
		{
			// �ʒu����
			sInfo InitInfo;
			memset(&InitInfo, NULL, sizeof(InitInfo));
			m_ChaseChangeInfo.push_back(InitInfo);
		}

		if (m_ChaseChangeInfo.size() > 0)
		{
			fread(&m_ChaseChangeInfo[0], sizeof(sInfo), m_nNumAll, pFile);
		}

		// �t�@�C�������
		fclose(pFile);
	}
	else
	{// �t�@�C�����J���Ȃ������ꍇ

		m_nNumAll = 0;

		// ��������
		for (int i = 0; i < m_nNumAll; i++)
		{
			// �ʒu����
			sInfo InitInfo;
			memset(&InitInfo, NULL, sizeof(InitInfo));
			m_ChaseChangeInfo.push_back(InitInfo);
		}

		// �S���[��
		if (m_ChaseChangeInfo.size() > 0)
		{
			memset(&m_ChaseChangeInfo[0], NULL, sizeof(m_ChaseChangeInfo));
		}

		// �Z�[�u
		Save();
	}

	return S_OK;
}

//==========================================================================
// �O���t�@�C�������o������
//==========================================================================
void CEnemyBase::Save(void)
{
	// �t�@�C�����J��
	FILE *pFile = fopen(FILE_ENEMYBASE, "wb");

	if (pFile != NULL)
	{// �t�@�C�����J�����ꍇ

		// �t�@�C���ɐ��l�������o��
		fwrite(&m_nNumAll, sizeof(int), 1, pFile);

		if (m_ChaseChangeInfo.size() > 0)
		{
			fwrite(&m_ChaseChangeInfo[0], sizeof(sInfo), m_nNumAll, pFile);
		}

		// �t�@�C�������
		fclose(pFile);
	}
}

//==========================================================================
// �����擾
//==========================================================================
int CEnemyBase::GetAxisNum(void)
{
	return m_ChaseChangeInfo.size() - 1;
}

//==========================================================================
// ���擾
//==========================================================================
D3DXVECTOR3 CEnemyBase::GetAxis(int nIdx)
{
	if (nIdx < 0)
	{
		nIdx = 0;
	}

	// �}�b�v�}�l�[�W���̎擾
	CMapManager *pMapManager = CManager::GetInstance()->GetScene()->GetMapManager();
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
	pos.y = m_ChaseChangeInfo[nIdx].fSpawnPosY;

	return pos;
}

//==========================================================================
// ���ݒ�
//==========================================================================
void CEnemyBase::SetSpawnPoint(int nIdx, int nMapIdx, float fMapMoveValue, float PosY)
{
	if (nIdx < 0)
	{
		return;
	}

	// ���n��
	m_ChaseChangeInfo[nIdx].nMapIdx = nMapIdx;
	m_ChaseChangeInfo[nIdx].fMapMoveValue = fMapMoveValue;
	m_ChaseChangeInfo[nIdx].fSpawnPosY = PosY;
}

//==========================================================================
// �ύX�̏��擾
//==========================================================================
CEnemyBase::sInfo CEnemyBase::GetChaseChangeInfo(int nIdx)
{
	sInfo InitInfo;
	memset(&InitInfo, NULL, sizeof(InitInfo));

	if (m_ChaseChangeInfo.size() <= 0 || (int)m_ChaseChangeInfo.size() <= nIdx)
	{// �T�C�Y����
		return InitInfo;
	}

	return m_ChaseChangeInfo[nIdx];
}
