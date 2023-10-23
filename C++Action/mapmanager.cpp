//=============================================================================
// 
//  �}�b�v�}�l�[�W������ [mapmanager.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "debugproc.h"
#include "mapmanager.h"
#include "calculation.h"
#include "game.h"
#include "3D_effect.h"
#include "debugpointnumber.h"
#include "stage.h"
#include "objectX.h"

//==========================================================================
// �}�N����`
//==========================================================================

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
CMapManager *CMapManager::m_pTop = NULL;	// �擪�̃|�C���^
CMapManager *CMapManager::m_pCur = NULL;	// �Ō���̃|�C���^
int CMapManager::m_nNumAll = 0;		// ����

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CMapManager::CMapManager()
{
	// �l�̃N���A
	m_pPrev = NULL;		// �O�̃I�u�W�F�N�g�ւ̃|�C���^
	m_pNext = NULL;		// ���̃I�u�W�F�N�g�ւ̃|�C���^
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// �ʒu
	memset(&m_pMultiNumber[0], NULL, sizeof(m_pMultiNumber));
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CMapManager::~CMapManager()
{

}

//==========================================================================
// ��������
//==========================================================================
CMapManager *CMapManager::Create(const std::string pFileName)
{
	// �����p�̃I�u�W�F�N�g
	CMapManager *pModel = NULL;

	if (pModel == NULL)
	{// NULL��������

		// �������̊m��
		pModel = DEBUG_NEW CMapManager;

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
HRESULT CMapManager::Init(void)
{
	//// �������Z�b�g
	//m_ppMapManager = DEBUG_NEW (CMapManager*);
	//*m_ppMapManager = NULL;

	return S_OK;
}

//==========================================================================
// �ʒu�쐬
//==========================================================================
void CMapManager::CreatePos(D3DXVECTOR3 pos)
{
#if 0
	// �Ō����ۑ�
	CMapManager *pObjCur = m_pCur;

	if (pObjCur != NULL)
	{// �Ō�������݂��Ă�����

		pObjCur->m_pNext = this;	// �Ō���̎��Ɏ������g����
		m_pCur = this;				// �Ō�����������g�ɍX�V
		m_pCur->m_pPrev = pObjCur;	// �Ō���̑O�ɉߋ��̍Ō������
	}
	else
	{// �Ō��������������
		m_pCur = this;	// �Ō�����������g�ɍX�V
	}

	if (m_pTop == NULL)
	{// �擪�ɉ����Ȃ�������
		m_pTop = this;	// �擪�Ɏ������g�̃|�C���^��n��
	}
#else
	//// ��������
	//m_ppMapManager[m_nNumAll] = DEBUG_NEW CMapManager;
	//m_ppMapManager[m_nNumAll] = NULL;

	//// �ʒu���
	//m_ppMapManager[m_nNumAll]->m_pos = pos;
#endif
	// �ʒu����
	m_posAll.push_back(pos);


	// �f�o�b�O�p�����̐���
	m_pMultiNumber[m_nNumAll] = CDebugPointNumber::Create(m_nNumAll - 1);

	// �������Z
	m_nNumAll++;

}

//==========================================================================
// �ʒu�폜
//==========================================================================
void CMapManager::DeletePos(int nIdx)
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
void CMapManager::Uninit(void)
{
	//if (m_posAll != NULL)
	//{// �ʒu����������
	//	delete[] m_posAll;
	//	m_posAll = NULL;
	//}

	//if (*m_ppMapManager != NULL)
	//{
	//	//for (int nCntPoint = 0; nCntPoint < m_nNumAll; nCntPoint++)
	//	//{
	//	//	if (m_ppMapManager[nCntPoint] != NULL)
	//	//	{// �}�l�[�W������������
	//	//		delete m_ppMapManager[nCntPoint];
	//	//		m_ppMapManager[nCntPoint] = NULL;
	//	//	}
	//	//}

	//	delete m_ppMapManager;
	//	m_ppMapManager = NULL;
	//}
}

//==========================================================================
// �X�V����
//==========================================================================
void CMapManager::Update(void)
{
#if _DEBUG
	for (int i = 0; i < m_nNumAll; i++)
	{
		//// X�t�@�C���Ƃ̔���
		//CStage *pStage = CGame::GetStage();
		//if (pStage == NULL)
		//{// NULL��������
		//	return;
		//}

		//float fHeight = 0.0f;
		//for (int nCntStage = 0; nCntStage < pStage->GetNumAll(); nCntStage++)
		//{
		//	// �I�u�W�F�N�g�擾
		//	CObjectX *pObjX = pStage->GetObj(nCntStage);

		//	if (pObjX == NULL)
		//	{// NULL��������
		//		continue;
		//	}

		//	// �����擾
		//	bool bLand = false;
		//	m_posAll[i].y = pObjX->GetHeight(m_posAll[i], bLand);
		//}

		CEffect3D::Create(m_posAll[i], D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), 50.0f, 2, CEffect3D::MOVEEFFECT_NONE, CEffect3D::TYPE_NORMAL);
		m_pMultiNumber[i]->SetPosition(D3DXVECTOR3(m_posAll[i].x, m_posAll[i].y + 50.0f, m_posAll[i].z));

	}
#endif
}

//==========================================================================
// �e�L�X�g�ǂݍ��ݏ���
//==========================================================================
HRESULT CMapManager::ReadText(const std::string pFileName)
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
			m_pMultiNumber[i] = CDebugPointNumber::Create(i - 1);
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
			m_pMultiNumber[i] = CDebugPointNumber::Create(i - 1);
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
void CMapManager::Save(const std::string pFileName)
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
// ����_���擾
//==========================================================================
int CMapManager::GetControlPointNum(void)
{
	return m_posAll.size() - 1;
}

//==========================================================================
// ����_�ݒ�
//==========================================================================
void CMapManager::SetControlPoint(D3DXVECTOR3 pos, int nIdx)
{
	m_posAll[nIdx] = pos;
}

//==========================================================================
// ����_�擾
//==========================================================================
D3DXVECTOR3 CMapManager::GetControlPoint(int nIdx)
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

//==========================================================================
// �ڕW�̈ʒu�擾
//==========================================================================
D3DXVECTOR3 CMapManager::GetTargetPosition(int nIdx, float fMoveValue)
{
	D3DXVECTOR3 pos = mylib_const::DEFAULT_VECTOR3;
	float fRatio = 0.0f;

	// �Ȑ����ׂ�4�_
	int nP0, nP1, nP2, nP3;
	nP0 = nIdx;
	nP1 = nIdx + 1;
	nP2 = nIdx + 2;
	nP3 = nIdx + 3;

	// �ڕW�n�_
	D3DXVECTOR3 TargetPoint0 = GetControlPoint(nP0);
	D3DXVECTOR3 TargetPoint1 = GetControlPoint(nP1);
	D3DXVECTOR3 TargetPoint2 = GetControlPoint(nP2);
	D3DXVECTOR3 TargetPoint3 = GetControlPoint(nP3);

	// 2�_�Ԃ̋����擾
	float fPosLength = GetPosLength(TargetPoint1, TargetPoint2);

	// ���������߂�
	fRatio = fMoveValue / fPosLength;

	if (fRatio >= 1.0f && nIdx < GetControlPointNum() - 1)
	{
		// �}�b�v�̈ʒu���Z
		nIdx++;
		if (nP1 < GetControlPointNum() - 1)
		{
			fMoveValue = fMoveValue - fPosLength;
			fRatio = fMoveValue / GetPosLength(TargetPoint2, TargetPoint3);
		}
	}
	else if (fRatio < 0.0f)
	{
		// �}�b�v�̈ʒu���Z
		nIdx--;
		if (nIdx < -1)
		{
			nIdx = -1;
		}

		fMoveValue = GetPosLength(TargetPoint0, TargetPoint1) - (-fMoveValue);
		fRatio = fMoveValue / GetPosLength(TargetPoint0, TargetPoint1);
	}

	// �␳��4�_�X�V
	nP0 = nIdx;
	nP1 = nIdx + 1;
	nP2 = nIdx + 2;
	nP3 = nIdx + 3;

	// 4�_�̈ʒu���X�V
	TargetPoint0 = GetControlPoint(nP0);
	TargetPoint1 = GetControlPoint(nP1);
	TargetPoint2 = GetControlPoint(nP2);
	TargetPoint3 = GetControlPoint(nP3);

	pos = CatmullRomSplineInterp(TargetPoint0, TargetPoint1, TargetPoint2, TargetPoint3, fRatio);

	return pos;
}

//==========================================================================
// ���ݒn�擾
//==========================================================================
D3DXVECTOR3 CMapManager::UpdateNowPosition(int& nIdx, float& fRatio, float& fMoveValue, float PosY)
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 sakiPos;

	// �Ȑ����ׂ�4�_
	int nP0, nP1, nP2, nP3;
	nP0 = nIdx;
	nP1 = nIdx + 1;
	nP2 = nIdx + 2;
	nP3 = nIdx + 3;

	// �ڕW�n�_
	D3DXVECTOR3 TargetPoint0 = GetControlPoint(nP0);
	D3DXVECTOR3 TargetPoint1 = GetControlPoint(nP1);
	D3DXVECTOR3 TargetPoint2 = GetControlPoint(nP2);
	D3DXVECTOR3 TargetPoint3 = GetControlPoint(nP3);

	// 2�_�Ԃ̋����擾
	float fPosLength = GetPosLength(TargetPoint1, TargetPoint2);

	// �ʒu�̊���
	if (fPosLength != 0.0f)
	{
		fRatio = fMoveValue / fPosLength;
	}
	else if (nIdx >= GetControlPointNum() - 1/* && move.x < 0*/)
	{// �Ō�
		fRatio = 1.0f;
	}

	bool bLeftArrival = false, bRightArrival = false;
	while (1)
	{
		bLeftArrival = false, bRightArrival = false;
		if (fRatio >= 1.0f && nIdx < GetControlPointNum() - 1)
		{
			// �}�b�v�̈ʒu���Z
			nIdx++;
			if (nP1 < GetControlPointNum() - 1)
			{
				fMoveValue = fMoveValue - fPosLength;
				fRatio = fMoveValue / GetPosLength(TargetPoint2, TargetPoint3);
				bRightArrival = true;
			}
		}
		else if (fRatio < 0.0f)
		{
			// �}�b�v�̈ʒu���Z
			nIdx--;

			if (nIdx < -1)
			{
				nIdx = -1;
			}
			fMoveValue = GetPosLength(TargetPoint0, TargetPoint1);
			fRatio = fMoveValue / GetPosLength(TargetPoint0, TargetPoint1);
			bLeftArrival = true;
		}

		if (fRatio >= 0.0f && fRatio <= 1.0f)
		{
			break;
		}
		else
		{
			nP0 = nIdx;
			nP1 = nIdx + 1;
			nP2 = nIdx + 2;
			nP3 = nIdx + 3;

			// �ڕW�n�_
			TargetPoint0 = GetControlPoint(nP0);
			TargetPoint1 = GetControlPoint(nP1);
			TargetPoint2 = GetControlPoint(nP2);
			TargetPoint3 = GetControlPoint(nP3);
		}
	}

	if (bLeftArrival == true || bRightArrival == true)
	{// ���E�ǂ���������

		// �␳��4�_�X�V
		nP0 = nIdx;
		nP1 = nIdx + 1;
		nP2 = nIdx + 2;
		nP3 = nIdx + 3;

		// 4�_�̈ʒu���X�V
		TargetPoint0 = GetControlPoint(nP0);
		TargetPoint1 = GetControlPoint(nP1);
		TargetPoint2 = GetControlPoint(nP2);
		TargetPoint3 = GetControlPoint(nP3);

		if (bLeftArrival == true)
		{
			// �ړ��̒l���I�[
			int nIdx1 = nP1, nIdx2 = nP2;
			fMoveValue = GetPosLength(GetControlPoint(nIdx1), GetControlPoint(nIdx2));
			if (nIdx <= -1)
			{
				nIdx1 = nP0 + 1, nIdx2 = nP1 + 1;
				TargetPoint0 = GetControlPoint(0);
				TargetPoint1 = GetControlPoint(0);
				TargetPoint2 = GetControlPoint(1);
				TargetPoint3 = GetControlPoint(2);
				fMoveValue = GetPosLength(GetControlPoint(nIdx1), GetControlPoint(nIdx2));
			}
		}
	}

	if (nIdx <= -1 && fRatio < 1.0f && fRatio > 0.0f)
	{
		fRatio = 1.0f;
		int nIdx1 = nP0 + 1, nIdx2 = nP1 + 1;
		fMoveValue = fPosLength;
	}
	else if (nIdx >= GetControlPointNum() - 1)
	{
		fRatio = 0.0f;
		fMoveValue = 0.0f;
	}

	// �Ȑ��̈ʒu
	pos = CatmullRomSplineInterp(TargetPoint0, TargetPoint1, TargetPoint2, TargetPoint3, fRatio);
	pos.y = PosY;

	return pos;
}

//==========================================================================
// ���ݒn�X�V
//==========================================================================
D3DXVECTOR3 CMapManager::UpdateNowPosition(int& nIdx, float& fRatio, float& fMoveValue, float PosY, CObject::ANGLE MoveAngle, float fMove)
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 sakiPos;

	// �ړ��p�̌���
	int nAngle = 1;

	// �ړ������ʉ��Z
	if (MoveAngle == CObject::ANGLE_LEFT)
	{// ���͋t�ړ�
		nAngle = -1;
	}

	// �ړ�
	fMoveValue += fMove * nAngle;

	// �Ȑ����ׂ�4�_
	int nP0, nP1, nP2, nP3;
	nP0 = nIdx;
	nP1 = nIdx + 1;
	nP2 = nIdx + 2;
	nP3 = nIdx + 3;

	// �ڕW�n�_
	D3DXVECTOR3 TargetPoint0 = GetControlPoint(nP0);
	D3DXVECTOR3 TargetPoint1 = GetControlPoint(nP1);
	D3DXVECTOR3 TargetPoint2 = GetControlPoint(nP2);
	D3DXVECTOR3 TargetPoint3 = GetControlPoint(nP3);

	// 2�_�Ԃ̋����擾
	float fPosLength = GetPosLength(TargetPoint1, TargetPoint2);

	// �ʒu�̊���
	if (fPosLength != 0.0f)
	{
		fRatio = fMoveValue / fPosLength;
	}
	else if (nIdx >= GetControlPointNum() - 1/* && move.x < 0*/)
	{// �Ō�
		fRatio = 1.0f;
	}

	bool bLeftArrival = false, bRightArrival = false;
	while (1)
	{
		bLeftArrival = false, bRightArrival = false;
		if (fRatio >= 1.0f && nIdx < GetControlPointNum() - 1)
		{
			// �}�b�v�̈ʒu���Z
			nIdx++;
			if (nP1 < GetControlPointNum() - 1)
			{
				fMoveValue = fMoveValue - fPosLength;
				fRatio = fMoveValue / GetPosLength(TargetPoint2, TargetPoint3);
				bRightArrival = true;
			}
		}
		else if (fRatio < 0.0f)
		{
			// �}�b�v�̈ʒu���Z
			nIdx--;

			if (nIdx < -1)
			{
				nIdx = -1;
			}
			fMoveValue = GetPosLength(TargetPoint0, TargetPoint1);
			fRatio = fMoveValue / GetPosLength(TargetPoint0, TargetPoint1);
			bLeftArrival = true;
		}

		if (fRatio >= 0.0f && fRatio <= 1.0f)
		{
			break;
		}
	}

	if (bLeftArrival == true || bRightArrival == true)
	{// ���E�ǂ���������

		// �␳��4�_�X�V
		nP0 = nIdx;
		nP1 = nIdx + 1;
		nP2 = nIdx + 2;
		nP3 = nIdx + 3;

		// 4�_�̈ʒu���X�V
		TargetPoint0 = GetControlPoint(nP0);
		TargetPoint1 = GetControlPoint(nP1);
		TargetPoint2 = GetControlPoint(nP2);
		TargetPoint3 = GetControlPoint(nP3);

		if (bLeftArrival == true)
		{
			// �ړ��̒l���I�[
			int nIdx1 = nP1, nIdx2 = nP2;
			fMoveValue = GetPosLength(GetControlPoint(nIdx1), GetControlPoint(nIdx2));
			if (nIdx <= -1)
			{
				nIdx1 = nP0 + 1, nIdx2 = nP1 + 1;
				TargetPoint0 = GetControlPoint(0);
				TargetPoint1 = GetControlPoint(0);
				TargetPoint2 = GetControlPoint(1);
				TargetPoint3 = GetControlPoint(2);
				fMoveValue = GetPosLength(GetControlPoint(nIdx1), GetControlPoint(nIdx2));
			}
		}
	}

	if (nIdx <= -1 && fRatio < 1.0f && fRatio > 0.0f)
	{
		fRatio = 1.0f;
		int nIdx1 = nP0 + 1, nIdx2 = nP1 + 1;
		fMoveValue = fPosLength;
	}
	else if (nIdx >= GetControlPointNum() - 1)
	{
		fRatio = 0.0f;
		fMoveValue = 0.0f;
	}

	// �Ȑ��̈ʒu
	pos = CatmullRomSplineInterp(TargetPoint0, TargetPoint1, TargetPoint2, TargetPoint3, fRatio);
	pos.y = PosY;

	return pos;
}

//==========================================================================
// �ڕW�������̂ǂ����ɂ��邩�̔���
//==========================================================================
CObject::ANGLE CMapManager::GetTargetAngle(int myIdx, int TargetIdx, float myMoveValue, float TargetMoveValue)
{
	CObject::ANGLE angle = CObject::ANGLE_UP;

	if (myIdx > TargetIdx)
	{// ������荶�ɂ���
		angle = CObject::ANGLE_LEFT;
	}
	else if (myIdx < TargetIdx)
	{// �������E�ɂ���
		angle = CObject::ANGLE_RIGHT;
	}
	else if (myIdx == TargetIdx)
	{// �����Ɠ���

		if (myMoveValue > TargetMoveValue)
		{// ������荶�ɂ���
			angle = CObject::ANGLE_LEFT;
		}
		else
		{// �������E�ɂ���
			angle = CObject::ANGLE_RIGHT;
		}
	}

	return angle;
}

//==========================================================================
// �ڕW�������̂ǂ����ɂ��邩�̔���
//==========================================================================
CObject::ANGLE CMapManager::GetTargetAngle(CObject *pMyObj, CObject *pTargetObj)
{
	CObject::ANGLE angle = CObject::ANGLE_UP;
	if (pMyObj == NULL || pTargetObj == NULL)
	{// NULL��������
		return angle;
	}

	int nMyIdx = pMyObj->GetMapIndex();
	int nTargetIdx = pTargetObj->GetMapIndex();
	float fMyMoveValue = pMyObj->GetMapMoveValue();
	float fTargetMoveValue = pTargetObj->GetMapMoveValue();

	if (nMyIdx > nTargetIdx)
	{// ������荶�ɂ���
		angle = CObject::ANGLE_LEFT;
	}
	else if (nMyIdx < nTargetIdx)
	{// �������E�ɂ���
		angle = CObject::ANGLE_RIGHT;
	}
	else if (nMyIdx == nTargetIdx)
	{// �����Ɠ���

		if (fMyMoveValue > fTargetMoveValue)
		{// ������荶�ɂ���
			angle = CObject::ANGLE_LEFT;
		}
		else
		{// �������E�ɂ���
			angle = CObject::ANGLE_RIGHT;
		}
	}

	return angle;
}

//==========================================================================
// ����܂ł̃}�b�v�ړ��ʎ擾
//==========================================================================
float CMapManager::GetThusFarMoveValue(int nIdx)
{

	int nCalIdx = -1;
	float fMoveValue = 0.0f;

	// �Ȑ����ׂ�4�_
	int nP0, nP1;

	while (1)
	{
		nP0 = nCalIdx;
		nP1 = nCalIdx + 1;

		if (nP0 >= nIdx)
		{// �ŏI�_��������I��
			break;
		}

		// �ڕW�n�_
		D3DXVECTOR3 TargetPoint0 = GetControlPoint(nP0);
		D3DXVECTOR3 TargetPoint1 = GetControlPoint(nP1);

		// �������Z
		fMoveValue += GetPosLength(TargetPoint0, TargetPoint1);
		nCalIdx++;
	}

	return fMoveValue;
}

//==========================================================================
// ����_�擾
//==========================================================================
D3DXVECTOR3 CMapManager::GetControlPoint(void)
{
	return m_pos;
}

