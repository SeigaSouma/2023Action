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
#include "enemyfixedmove_manager.h"

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
		SetEnemy(D3DXVECTOR3(0.0f, 200.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 4);
	}

	// �e�L�X�g�̕`��
	CManager::GetInstance()->GetDebugProc()->Print(
		"---------------- �G��� ----------------\n"
		"�y�c��l���z[%d]\n", m_nNumAll);
}

//==========================================================================
// �G�z�u
//==========================================================================
void CEnemyManager::SetEnemy(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nPattern)
{
	int nNumSpawn = m_aPattern[nPattern].nNumEnemy;	// �X�|�[�����鐔
	int nCntNULL = 0;
	int nCntStart = 0;
	Pattern NowPattern = m_aPattern[nPattern];

	for (int nCntEnemy = 0; nCntEnemy < nNumSpawn; nCntEnemy++)
	{
		for (nCntNULL = nCntStart; nCntNULL < mylib_const::MAX_OBJ; nCntNULL++, nCntStart++)
		{
			if (m_pEnemy[nCntNULL] != NULL)
			{// ��񂪓����Ă���
				continue;
			}

			int nType = NowPattern.EnemyData[nCntEnemy].nType;

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
			m_pEnemy[nCntNULL]->SetMapMoveValueOrigin(NowPattern.EnemyData[nCntEnemy].fStartMoveValue);

			// �G�̈��̓����}�l�[�W���|�C���^�擾
			CEnemyFixedMoveManager *pFixed = m_pEnemy[nCntNULL]->GetFixedManager();
			if (pFixed == NULL)
			{// ���s���Ă�����
				return;
			}
			pFixed->Set(NowPattern.nFixedType);
			pFixed->StartSet(NowPattern.EnemyData[nCntEnemy].nStartKey, NowPattern.EnemyData[nCntEnemy].nStartFrame);

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

				if (strcmp(aComment, "FIXEDMOVE") == 0)
				{// FIXEDMOVE����������̓����̎�ޓǂݍ���

					fscanf(pFile, "%s", &aComment[0]);	// =�̕�
					fscanf(pFile, "%d", &m_aPattern[nCntPatten].nFixedType);	// ���̓����̎��
				}

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

						if (strcmp(aComment, "STARTKEY") == 0)
						{// STARTKEY�������珉���L�[�ǂݍ���

							fscanf(pFile, "%s", &aComment[0]);	// =�̕�
							fscanf(pFile, "%d", &m_aPattern[nCntPatten].EnemyData[nCntEnemy].nStartKey);	// �����L�[
						}

						if (strcmp(aComment, "STARTFRAME") == 0)
						{// STARTFRAME�������珉���t���[���ǂݍ���

							fscanf(pFile, "%s", &aComment[0]);	// =�̕�
							fscanf(pFile, "%d", &m_aPattern[nCntPatten].EnemyData[nCntEnemy].nStartFrame);	// �����t���[��
						}

						if (strcmp(aComment, "STARTMOVEVALUE") == 0)
						{// STARTMOVEVALUE�������珉���}�b�v�ړ��ʓǂݍ���

							fscanf(pFile, "%s", &aComment[0]);	// =�̕�
							fscanf(pFile, "%f", &m_aPattern[nCntPatten].EnemyData[nCntEnemy].fStartMoveValue);	// �����}�b�v�ړ���
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

//==========================================================================
// �G�擾
//==========================================================================
CEnemy **CEnemyManager::GetEnemy(void)
{
	return &m_pEnemy[0];
}