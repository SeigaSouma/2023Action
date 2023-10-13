//=============================================================================
// 
//  �G�̃}�l�[�W������ [enemymanager.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "debugproc.h"
#include "enemymanager.h"
#include "calculation.h"
#include "manager.h"
#include "game.h"
#include "renderer.h"
#include "enemy.h"
#include "particle.h"
#include "fade.h"
#include "elevation.h"
#include "player.h"
#include "impactwave.h"
#include "camera.h"
#include "sound.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define SPAWN_STRENGTH	(100.0f)	// �X�|�[�����̍���
#define KILL_INT		(30)		// �|�����C���^�[�o��

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
CEnemyManager::Pattern CEnemyManager::m_aPattern[mylib_const::MAX_PATTEN_ENEMY] = {};			// �ǂݍ��݃f�[�^
std::string CEnemyManager::sMotionFileName[mylib_const::MAX_PATTEN_ENEMY] = {};					// ���[�V�����t�@�C����
int CEnemyManager::m_nPatternNum = 0;	// �o���p�^�[����
CEnemy *CEnemyManager::m_pEnemy[mylib_const::MAX_OBJ] = {};
int CEnemyManager::m_nNumAll = 0;		// ����
int CEnemyManager::m_nNumChara = 0;		// �G�̎�ނ̑���
int CEnemyManager::m_nCntSpawn = 0;		// �o���J�E���g
bool CEnemyManager::m_bLoadPattern = false;	// �p�^�[���ǂݍ��ݔ���
bool CEnemyManager::m_bHitStop = false;	// �q�b�g�X�g�b�v�̔���
CEnemyManager::STATE CEnemyManager::m_state = CEnemyManager::STATE_NONE;		// ���

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CEnemyManager::CEnemyManager()
{
	// �l�̃N���A
	m_state = STATE_NONE;	// ���

	// �������Z�b�g
	m_nNumAll = 0;
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CEnemyManager::~CEnemyManager()
{

}

//==========================================================================
// ��������
//==========================================================================
CEnemyManager *CEnemyManager::Create(const std::string pTextFile)
{
	// �����p�̃I�u�W�F�N�g
	CEnemyManager *pModel = NULL;

	if (pModel == NULL)
	{// NULL��������

		// �������̊m��
		pModel = DEBUG_NEW CEnemyManager;

		if (pModel != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ����������
			HRESULT hr = pModel->ReadText(pTextFile);

			if (FAILED(hr))
			{// ���s���Ă�����
				return NULL;
			}

			// ����������
			hr = pModel->Init();

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
HRESULT CEnemyManager::Init(void)
{
	// �������Z�b�g
	m_nNumAll = 0;
	m_nCntSpawn = 0;

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CEnemyManager::Uninit(void)
{
	for (int nCntEnemy = 0; nCntEnemy < mylib_const::MAX_OBJ; nCntEnemy++)
	{
		if (m_pEnemy[nCntEnemy] != NULL)
		{
			m_pEnemy[nCntEnemy] = NULL;
		}
	}
}

//==========================================================================
// �j��
//==========================================================================
void CEnemyManager::Release(int nIdx)
{
	if (m_pEnemy[nIdx] != NULL)
	{
		m_pEnemy[nIdx] = NULL;
	}

	// �������Z
	m_nNumAll--;
}

//==========================================================================
// �X�V����
//==========================================================================
void CEnemyManager::Update(void)
{

	if (m_nNumAll <= 0)
	{
		SetEnemy(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 1);
	}

	// �e�L�X�g�̕`��
	CManager::GetDebugProc()->Print(
		"---------------- �G��� ----------------\n"
		"�y�c��l���z[%d]\n", m_nNumAll);
}

//==========================================================================
// �G�z�u
//==========================================================================
void CEnemyManager::SetEnemy(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nType)
{
	int nCntNULL = 0;
	int aParent[mylib_const::MAX_PATTEN_ENEMY] = {};

	for (int nCntNULL = 0; nCntNULL < mylib_const::MAX_OBJ; nCntNULL++)
	{
		if (m_pEnemy[nCntNULL] == NULL)
		{// NULL��������

			// �G�̐���
			m_pEnemy[nCntNULL] = CEnemy::Create(
				nCntNULL,						// �C���f�b�N�X�ԍ�
				sMotionFileName[nType].c_str(),	// �t�@�C����
				pos,							// �ʒu
				(CEnemy::TYPE)nType);			// ���

			if (m_pEnemy[nCntNULL] == NULL)
			{// ���s���Ă�����

				delete m_pEnemy[nCntNULL];
				m_pEnemy[nCntNULL] = NULL;
				break;
			}

			// �����ݒ�
			m_pEnemy[nCntNULL]->SetRotation(rot);
			m_pEnemy[nCntNULL]->SetRotDest(rot.y);

			// �e���f���̐ݒ�
			//if (m_aPattern[nPattern].EnemyData[nCntEnemy].nParent >= 0)
			//{
			//	m_pEnemy[nCntNULL]->SetParent(m_pEnemy[aParent[m_aPattern[nPattern].EnemyData[nCntEnemy].nParent]]);
			//}
			//else
			//{// �e�������̏ꍇ

			//	// NULL�ݒ�
			//	m_pEnemy[nCntNULL]->SetParent(NULL);
			//}

			// �e�ԍ��ۑ�
			//aParent[nCntEnemy] = nCntNULL;

			// �������Z
			m_nNumAll++;

			break;
		}
	}
}

//==========================================================================
// �p�^�[����
//==========================================================================
int CEnemyManager::GetPatternNum(void)
{
	return m_nPatternNum;
}

//==========================================================================
// �G�̑����擾
//==========================================================================
int CEnemyManager::GetNumAll(void)
{
	return m_nNumAll;
}

//==========================================================================
// �e�L�X�g�ǂݍ��ݏ���
//==========================================================================
HRESULT CEnemyManager::ReadText(const std::string pTextFile)
{

	if (m_bLoadPattern == true)
	{// ���ɓǂݍ��܂�Ă�����
		return S_OK;
	}

	FILE *pFile = NULL;	// �t�@�C���|�C���^��錾

	// �t�@�C�����J��
	pFile = fopen(pTextFile.c_str(), "r");

	if (pFile == NULL)
	{//�t�@�C�����J�����ꍇ
		return E_FAIL;
	}

	// �ǂݍ��ݔ���ON
	m_bLoadPattern = true;

	char aComment[MAX_COMMENT];	// �R�����g
	int nType = 0;				// �z�u������
	int nCntPatten = 0;			// �p�^�[���̃J�E���g
	int nCntFileName = 0;

	while (1)
	{// END_SCRIPT������܂ŌJ��Ԃ�

		// ������̓ǂݍ���
		fscanf(pFile, "%s", &aComment[0]);

		// �L�����N�^�[���̐ݒ�
		if (strcmp(aComment, "NUM_CHARACTER") == 0)
		{// NUM_CHARACTER��������

			fscanf(pFile, "%s", &aComment[0]);	// =�̕�
			fscanf(pFile, "%d", &m_nNumChara);	// �L�����N�^�[��
		}

		while (nCntFileName != m_nNumChara)
		{// ���f���̐����ǂݍ��ނ܂ŌJ��Ԃ�

			// ������̓ǂݍ���
			fscanf(pFile, "%s", &aComment[0]);

			// ���f�����̐ݒ�
			if (strcmp(aComment, "MOTION_FILENAME") == 0)
			{// NUM_MODEL��������

				fscanf(pFile, "%s", &aComment[0]);	// =�̕�
				fscanf(pFile, "%s", &aComment[0]);	// �t�@�C����

				// �t�@�C�����ۑ�
				sMotionFileName[nCntFileName] = aComment;

				nCntFileName++;	// �t�@�C�������Z
			}
		}

		// �e�p�^�[���̐ݒ�
		if (strcmp(aComment, "PATTERNSET") == 0)
		{// �z�u���̓ǂݍ��݂��J�n

			int nCntEnemy = 0;			// �G�̔z�u�J�E���g

			while (strcmp(aComment, "END_PATTERNSET") != 0)
			{// END_PATTERNSET������܂ŌJ��Ԃ�

				fscanf(pFile, "%s", &aComment[0]);	//�m�F����

				if (strcmp(aComment, "ENEMYSET") == 0)
				{// ENEMYSET�œG���̓ǂݍ��݊J�n

					while (strcmp(aComment, "END_ENEMYSET") != 0)
					{// END_ENEMYSET������܂ŌJ��Ԃ�

						fscanf(pFile, "%s", &aComment[0]);	// �m�F����

						if (strcmp(aComment, "TYPE") == 0)
						{// TYPE��������L�����t�@�C���ԍ��ǂݍ���

							fscanf(pFile, "%s", &aComment[0]);	// =�̕�
							fscanf(pFile, "%d", &m_aPattern[nCntPatten].EnemyData[nCntEnemy].nType);	// �L�����t�@�C���ԍ�
						}

						if (strcmp(aComment, "PARENT") == 0)
						{// PARENT��������e�̔ԍ��ǂݍ���

							fscanf(pFile, "%s", &aComment[0]);	// =�̕�
							fscanf(pFile, "%d", &m_aPattern[nCntPatten].EnemyData[nCntEnemy].nParent);	// �e�̔ԍ�
						}

						if (strcmp(aComment, "WAITTIME") == 0)
						{// WAITTIME��������ҋ@���ԓǂݍ���

							fscanf(pFile, "%s", &aComment[0]);	// =�̕�
							fscanf(pFile, "%d", &m_aPattern[nCntPatten].EnemyData[nCntEnemy].nWaitTime);	// �ҋ@����
						}

						if (strcmp(aComment, "POS") == 0)
						{// POS��������ʒu�ǂݍ���

							fscanf(pFile, "%s", &aComment[0]);		// =�̕�
							fscanf(pFile, "%f", &m_aPattern[nCntPatten].EnemyData[nCntEnemy].pos.x);	// X���W
							fscanf(pFile, "%f", &m_aPattern[nCntPatten].EnemyData[nCntEnemy].pos.y);	// Y���W
							fscanf(pFile, "%f", &m_aPattern[nCntPatten].EnemyData[nCntEnemy].pos.z);	// Z���W
						}

					}// END_ENEMYSET�̂�����

					nCntEnemy++;	// �G�̃J�E���g�����Z
					m_aPattern[nCntPatten].nNumEnemy++;	// �G�̃J�E���g�����Z
				}
			}// END_PATTERNSET�̂�����

			nCntPatten++;	// �p�^�[�����Z
		}

		if (strcmp(aComment, "END_SCRIPT") == 0)
		{// �I�������Ń��[�v�𔲂���
			break;
		}
	}

	// �p�^�[����
	m_nPatternNum = nCntPatten - 1;

	// �t�@�C�������
	fclose(pFile);

	return S_OK;
}
