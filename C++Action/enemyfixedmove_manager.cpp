//=============================================================================
// 
//  �G�̈��̓������� [enemyfixedmove_manager.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "debugproc.h"
#include "enemyfixedmove_manager.h"
#include "calculation.h"
#include "manager.h"
#include "renderer.h"
#include "enemy.h"
#include "mapmanager.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define SPAWN_STRENGTH	(100.0f)	// �X�|�[�����̍���
#define KILL_INT		(30)		// �|�����C���^�[�o��

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
CEnemyFixedMoveManager::sFixedMove CEnemyFixedMoveManager::m_aFxedMove[mylib_const::MAX_FIXEDMOVE] = {};	// ���̓����̏��
int CEnemyFixedMoveManager::m_nNumAll = 0;				// ���[�V�����̑���

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CEnemyFixedMoveManager::CEnemyFixedMoveManager()
{
	// �l�̃N���A
	m_nType = 0;				// ���݂̃��[�V�����̎��
	m_nPatternKey = 0;			// ���ڂ̃L�[��
	m_nCntFrame = 0;			// �t���[���̃J�E���g
	m_nCntAllFrame = 0;			// �S�ẴJ�E���g
	m_nMaxAllFrame = 0;			// �S�ẴJ�E���g�̍ő�l
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CEnemyFixedMoveManager::~CEnemyFixedMoveManager()
{

}

//==========================================================================
// ��������
//==========================================================================
CEnemyFixedMoveManager *CEnemyFixedMoveManager::Create(void)
{
	// �����p�̃I�u�W�F�N�g
	CEnemyFixedMoveManager *pModel = NULL;

	if (pModel == NULL)
	{// NULL��������

		// �������̊m��
		pModel = DEBUG_NEW CEnemyFixedMoveManager;

		if (pModel != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ����������
			HRESULT hr = pModel->Init();

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
HRESULT CEnemyFixedMoveManager::Init(void)
{
	return S_OK;
}

//==========================================================================
// �X�^�[�g���̐ݒ�
//==========================================================================
void CEnemyFixedMoveManager::StartSet(int nKey, int nFrame)
{
	// ���݂̃L�[�ύX
	m_nPatternKey = nKey;

	// �t���[���̃J�E���g
	m_nCntFrame = nFrame;

	while (1)
	{
		if (m_nCntFrame >= m_aFxedMove[m_nType].info[m_nPatternKey].nFrame)
		{// �t���[���̃J�E���g���t���[�����ɒB������

			// �t���[���̃J�E���g���[���ɖ߂�(?)
			m_nCntFrame -= m_aFxedMove[m_nType].info[m_nPatternKey].nFrame;

			// �p�^�[��NO.�X�V
			m_nPatternKey = (m_nPatternKey + 1) % m_aFxedMove[m_nType].nNumKey;

			if (m_nPatternKey == 0)
			{// �L�[���[���̎�

				// ���t���[�������Z�b�g
				m_nCntAllFrame = 0;
			}
		}

		if (m_nCntFrame < m_aFxedMove[m_nType].info[m_nPatternKey].nFrame)
		{
			break;
		}
	}
}

//==========================================================================
// �I������
//==========================================================================
void CEnemyFixedMoveManager::Uninit(void)
{
	
}

//==========================================================================
// �X�V����
//==========================================================================
void CEnemyFixedMoveManager::Update(void)
{

	
}

//==========================================================================
// �L�[�t���[���X�V
//==========================================================================
void CEnemyFixedMoveManager::UpdateKeyFrame(void)
{
	// �Đ��t���[���ۑ�
	int nFrame = m_aFxedMove[m_nType].info[m_nPatternKey].nFrame;

	// ���̃L�[
	int nNextKey = (m_nPatternKey + 1) % m_aFxedMove[m_nType].nNumKey;

	// �t���[���̃J�E���g�����Z
	m_nCntFrame++;
	m_nCntAllFrame++;

	if (m_nCntFrame >= nFrame)
	{// �t���[���̃J�E���g���t���[�����ɒB������

		// �t���[���̃J�E���g���[���ɖ߂�
		m_nCntFrame = 0;

		// �p�^�[��NO.�X�V
		m_nPatternKey = (m_nPatternKey + 1) % m_aFxedMove[m_nType].nNumKey;

		if (m_nPatternKey == 0)
		{// �L�[���[���̎�

			// ���t���[�������Z�b�g
			m_nCntAllFrame = 0;
		}
	}
}

//==========================================================================
// �ʒu�X�V
//==========================================================================
D3DXVECTOR3 CEnemyFixedMoveManager::UpdatePosition(CEnemy *pEnemy)
{
	// �ʒu�擾
	D3DXVECTOR3 pos = pEnemy->GetPosition();

	// �}�b�v�}�l�[�W���̎擾
	CMapManager *pMapManager = CManager::GetInstance()->GetScene()->GetMapManager();
	if (pMapManager == NULL)
	{// NULL��������
		return pos;
	}

	// �L�[�X�V
	UpdateKeyFrame();

	// �o���ʒu�擾
	D3DXVECTOR3 spawnPos = pEnemy->GetSpawnPosition();
	
	// �}�b�v���擾
	int nIdx = pEnemy->GetMapIndex();
	float fPointRatio = pEnemy->GetMapPointRatio();
	float fMoveValue = pEnemy->GetMapMoveValue();
	float fThusFarMoveX = 0.0f;
	float fThusFarMoveY = 0.0f;

	// ���܂ł̈ړ���
	for (int nCntKey = 0; nCntKey < m_nPatternKey + 1; nCntKey++)
	{
		int nFrame = m_aFxedMove[m_nType].info[nCntKey].nFrame;
		if (nCntKey == m_nPatternKey)
		{// �����L�[�������猻�݂̃J�E���g�܂�
			nFrame = m_nCntFrame;
		}
		fThusFarMoveX += m_aFxedMove[m_nType].info[nCntKey].move.x * nFrame;
		fThusFarMoveY += m_aFxedMove[m_nType].info[nCntKey].move.y * nFrame;
	}
	// �ʒu�X�V
	int nOriginIdx = pEnemy->GetMapIndexOrigin();
	fMoveValue = pEnemy->GetMapMoveValueOrigin() + fThusFarMoveX;
	nIdx = nOriginIdx;
	pos = pMapManager->UpdateNowPosition(nIdx, fPointRatio, fMoveValue, pos.y);
	pos.y = spawnPos.y + fThusFarMoveY;

	// �}�b�v���ݒ�
	pEnemy->SetMapIndex(nIdx);
	pEnemy->SetMapPointRatio(fPointRatio);
	pEnemy->SetMapMoveValue(fMoveValue);

	return pos;
}

//==========================================================================
// �ݒ菈��
//==========================================================================
void CEnemyFixedMoveManager::Set(int nType)
{
	if (nType > m_nNumAll)
	{// �����𒴂��Ă�����
		return;
	}

	m_nType = nType;		// ��ސݒ�
	m_nPatternKey = 0;		// ���ڂ̃L�[��
	m_nCntFrame = 0;		// �t���[���̃J�E���g
	m_nCntAllFrame = 0;		// �S�ẴJ�E���g
	m_nMaxAllFrame = 0;		// �S�ẴJ�E���g�̍ő�l

	for (int nCntKey = 0; nCntKey < m_aFxedMove[m_nType].nNumKey; nCntKey++)
	{
		m_nMaxAllFrame += m_aFxedMove[m_nType].info[nCntKey].nFrame;	// �S�ẴJ�E���g�̍ő�l
	}
}

//==========================================================================
// �e�L�X�g�ǂݍ��ݏ���
//==========================================================================
HRESULT CEnemyFixedMoveManager::ReadText(const std::string pTextFile)
{
	FILE *pFile = NULL;	// �t�@�C���|�C���^��錾

	// �t�@�C�����J��
	pFile = fopen(pTextFile.c_str(), "r");

	if (pFile == NULL)
	{//�t�@�C�����J�����ꍇ
		return E_FAIL;
	}

	char aComment[MAX_COMMENT];	// �R�����g
	int nType = 0;				// �z�u������

	while (1)
	{// END_SCRIPT������܂ŌJ��Ԃ�

		// ������̓ǂݍ���
		fscanf(pFile, "%s", &aComment[0]);

		//==================================================
		// �e���[�V�����̐ݒ�
		//==================================================
		if (strcmp(aComment, "FIXEDSET") == 0)
		{// ���[�V�������̓ǂݍ��݂��J�n

			// �s���̃J�E���^�[
			int nCntAct = 0;

			while (strcmp(aComment, "END_FIXEDSET") != 0)
			{// END_FIXEDSET������܂ŌJ��Ԃ�

				fscanf(pFile, "%s", &aComment[0]);	// �m�F����

				if (strcmp(aComment, "NUM_KEY") == 0)
				{// NUM_KEY��������L�[���ǂݍ���

					fscanf(pFile, "%s", &aComment[0]);	// =�̕�
					fscanf(pFile, "%d", &m_aFxedMove[m_nNumAll].nNumKey);	// �L�[��
				}

				//==================================================
				// �e�L�[�̐ݒ�
				//==================================================
				if (strcmp(aComment, "ACTSET") == 0)
				{// ACTSET�ŃL�[���̓ǂݍ��݊J�n

					while (strcmp(aComment, "END_ACTSET") != 0)
					{// END_ACTSET������܂ŌJ��Ԃ�

						fscanf(pFile, "%s", &aComment[0]);	// �m�F����

						if (strcmp(aComment, "FRAME") == 0)
						{// FRAME��������Đ��t���[�����ǂݍ���

							fscanf(pFile, "%s", &aComment[0]);		// =�̕�
							fscanf(pFile, "%d", &m_aFxedMove[m_nNumAll].info[nCntAct].nFrame);	// �Đ��t���[��
						}

						if (strcmp(aComment, "MOVE") == 0)
						{// MOVE��������ړ��ʓǂݍ���

							fscanf(pFile, "%s", &aComment[0]);		// =�̕�
							fscanf(pFile, "%f", &m_aFxedMove[m_nNumAll].info[nCntAct].move.x);	// X�ړ���
							fscanf(pFile, "%f", &m_aFxedMove[m_nNumAll].info[nCntAct].move.y);	// Y�ړ���
							fscanf(pFile, "%f", &m_aFxedMove[m_nNumAll].info[nCntAct].move.z);	// Z�ړ���
						}

					}// END_ACTSET�̂�����

					nCntAct++;	// �s���̃J�E���g���Z
				}

			}// END_FIXEDSET�̂�����

			// �f�[�^�����Z
			m_nNumAll++;
		}

		if (strcmp(aComment, "END_SCRIPT") == 0)
		{// �I�������Ń��[�v�𔲂���
			break;
		}
	}

	// �t�@�C�������
	fclose(pFile);

	return S_OK;
}
