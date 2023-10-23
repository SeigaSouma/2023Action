//=============================================================================
// 
//  �G�̋��_�G�f�B�^�[���� [edit_enemybase.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "edit_enemybase.h"
#include "enemybase.h"
#include "objectX.h"
#include "XLoad.h"
#include "manager.h"
#include "renderer.h"
#include "calculation.h"
#include "debugproc.h"
#include "input.h"
#include "elevation.h"
#include "mapmanager.h"
#include "game.h"
#include "enemymanager.h"
#include "enemy.h"
#include "enemyfixedmove_manager.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define MOVE		(10.0f)
#define MOVE_SLOW	(1.0f)
#define MARKOBJ	"data\\MODEL\\koko.x"

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CEditEnemyBase::CEditEnemyBase()
{
	// �[���N���A
	m_pos = mylib_const::DEFAULT_VECTOR3;	// �ʒu
	m_rot = mylib_const::DEFAULT_VECTOR3;			// �ʒu
	m_nEnemyType = 0;			// �Ǐ]�̎��
	m_nIdxMapPoint = 0;			// �}�b�v�|�C���g�̃C���f�b�N�X�ԍ�
	m_fPointRatio = 0.0f;		// �ړ�����
	m_fMoveValue = 0.0f;		// �ړ���
	m_MoveAngle = CObject::ANGLE_UP;	// �ړ�����
	memset(&m_pEnemy[0], NULL, sizeof(m_pEnemy));	// �G�ւ̃|�C���^
	m_apObjX = NULL;			// �I�u�W�F�N�gX
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CEditEnemyBase::~CEditEnemyBase()
{

}

//==========================================================================
// ��������
//==========================================================================
CEditEnemyBase *CEditEnemyBase::Create(void)
{
	// �����p�̃I�u�W�F�N�g
	CEditEnemyBase *pObjBase = NULL;

	if (pObjBase == NULL)
	{// NULL��������

		// �������̊m��
		pObjBase = DEBUG_NEW CEditEnemyBase;

		if (pObjBase != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ����������
			HRESULT hr = pObjBase->Init();
			if (FAILED(hr))
			{// ���s���Ă�����
				return NULL;
			}
		}

		return pObjBase;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CEditEnemyBase::Init(void)
{
	m_pos = CManager::GetInstance()->GetCamera()->GetPositionR();
	m_apObjX = CObjectX::Create(MARKOBJ, mylib_const::DEFAULT_VECTOR3, mylib_const::DEFAULT_VECTOR3, false);	// �I�u�W�F�N�gX
	m_apObjX->SetType(CObject::TYPE_BALLAST);
	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CEditEnemyBase::Uninit(void)
{
	
}

//==========================================================================
// �I������
//==========================================================================
void CEditEnemyBase::Release(void)
{
	if (m_apObjX != NULL)
	{
		m_apObjX->Uninit();
		m_apObjX = NULL;
	}
}

//==========================================================================
// �X�V����
//==========================================================================
void CEditEnemyBase::Update(void)
{
	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// ����
	Control(m_pos);
	ChangeType();

	// �ڈ�
	m_apObjX->SetPosition(m_pos);

	for (int nCntEnemy = 0; nCntEnemy < mylib_const::MAX_PATTEN_ENEMY; nCntEnemy++)
	{
		if (m_pEnemy[nCntEnemy] != NULL)
		{// NULL����Ȃ�������
			D3DXVECTOR3 posOrigin = m_pEnemy[nCntEnemy]->GetOriginPosition();
			m_pEnemy[nCntEnemy]->SetSpawnPosition(D3DXVECTOR3(posOrigin.x, m_pos.y, posOrigin.z));
			m_pEnemy[nCntEnemy]->SetMapIndexOrigin(m_nIdxMapPoint);
			m_pEnemy[nCntEnemy]->SetMapMoveValueOrigin(m_fMoveValue);
		}
	}


	// �}�b�v�}�l�[�W���̎擾
	CMapManager *pMapManager = CManager::GetInstance()->GetScene()->GetMapManager();
	if (pMapManager == NULL)
	{// NULL��������
		return;
	}

	// �Ȑ����ׂ�4�_
	int nP0 = m_nIdxMapPoint;
	int nP1 = m_nIdxMapPoint + 1;
	int nP2 = m_nIdxMapPoint + 2;

	// �ڕW�n�_
	D3DXVECTOR3 TargetPoint0 = pMapManager->GetControlPoint(nP0);
	D3DXVECTOR3 TargetPoint1 = pMapManager->GetControlPoint(nP1);
	D3DXVECTOR3 TargetPoint2 = pMapManager->GetControlPoint(nP2);

	int nAngle = 1;
	if (m_MoveAngle == CObject::ANGLE_LEFT)
	{
		nAngle = -1;
	}

	// ������̒n�_�擾
	float fMoveValue = m_fMoveValue + 50.0f * nAngle;
	D3DXVECTOR3 DestPoint = pMapManager->GetTargetPosition(m_nIdxMapPoint, fMoveValue);

	// �x�N�g��
	D3DXVECTOR3 vec = mylib_const::DEFAULT_VECTOR3;
	D3DXVECTOR3 newvec = mylib_const::DEFAULT_VECTOR3;

	// 2�_�Ԃ̋����擾
	float fPosLength = GetPosLength(TargetPoint1, TargetPoint2);

	// �x�N�g��
	if (m_fMoveValue < fMoveValue)
	{
		vec = DestPoint - m_pos;
	}
	else
	{
		vec = m_pos - DestPoint;
	}
	D3DXVec3Normalize(&vec, &vec);

	// 90�x�X����
	newvec.x = vec.z;
	newvec.z = -vec.x;

	// �ڕW�̎��_�̌���
	m_rot.y = atan2f((0.0f - newvec.x), (0.0f - newvec.z));
	m_apObjX->SetRotation(m_rot);




	// �͂ݏ���
	if (pInputKeyboard->GetPress(DIK_SPACE) == true)
	{// SPACE�Œ͂�
		Grab();
	}

	if (pInputKeyboard->GetTrigger(DIK_RETURN) == true)
	{// ENTER�Ŕz�u

		// �G�̋��_�̎擾
		CEnemyBase *pEnemyBase = CManager::GetInstance()->GetScene()->GetEnemyBase();
		if (pEnemyBase == NULL)
		{
			return;
		}
		// ����
		pEnemyBase->CreatePos(m_nEnemyType, m_nIdxMapPoint, m_fMoveValue, m_pos.y);
	}

	if (pInputKeyboard->GetTrigger(DIK_DELETE) == true)
	{// delete�ō폜
		Delete();
	}

	if (pInputKeyboard->GetTrigger(DIK_F9) == true)
	{// �Z�[�u

		// �G�̋��_�̎擾
		CEnemyBase *pEnemyBase = CManager::GetInstance()->GetScene()->GetEnemyBase();
		if (pEnemyBase == NULL)
		{
			return;
		}
		pEnemyBase->Save();
	}

	// �f�o�b�O���
	CManager::GetInstance()->GetDebugProc()->Print(
		"------------------[ �G�̋��_�G�f�B�^�[��� ]------------------\n"
		"<�z�u>         [ENTER]\n"
		"<�t�@�C���ۑ�> [F9] �ydata/BIN/enemybase.bin�z\n"
		"<�ړ�>         �����F[��,��,��,��]�@�ᑬ�F[W,A,S,D]\n"
		"<�㏸,���~>    [I,K]\n"
		"<��ޕύX>     [1, 2][%d]\n"
		"<�݈͂ړ�>     [SPACE]\n"
		"<�폜>         [DELETE]\n"
		"<�ʒu>         [X�F%f Y�F%f Z�F%f]\n"
		"\n", m_nEnemyType, m_pos.x, m_pos.y, m_pos.z);
}

//==========================================================================
// ���쏈��
//==========================================================================
void CEditEnemyBase::Control(D3DXVECTOR3 &pos)
{
	// �J�����̏��擾
	CCamera *pCamera = CManager::GetInstance()->GetCamera();

	// �J�����̌����擾
	D3DXVECTOR3 Camerarot = pCamera->GetRotation();

	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	if (pInputKeyboard->GetPress(DIK_LEFT) == true)
	{// ���L�[�������ꂽ,���ړ�

		m_fMoveValue -= MOVE;
		m_MoveAngle = CObject::ANGLE_LEFT;
	}
	else if (pInputKeyboard->GetPress(DIK_RIGHT) == true)
	{// D�L�[�������ꂽ,�E�ړ�

		m_fMoveValue += MOVE;
		m_MoveAngle = CObject::ANGLE_RIGHT;
	}

	if (pInputKeyboard->GetPress(DIK_A) == true)
	{// ���L�[�������ꂽ,���ړ�

		m_fMoveValue -= MOVE_SLOW;
	}
	else if (pInputKeyboard->GetPress(DIK_D) == true)
	{// D�L�[�������ꂽ,�E�ړ�

		m_fMoveValue += MOVE_SLOW;
	}

	if (pInputKeyboard->GetPress(DIK_I) == true)
	{// I�L�[�������ꂽ,�㏸

		pos.y += MOVE;
	}
	else if (pInputKeyboard->GetPress(DIK_K) == true)
	{// K�L�[�������ꂽ,���~

		pos.y -= MOVE;
	}


	// �}�b�v�}�l�[�W���̎擾
	CMapManager *pMapManager = CManager::GetInstance()->GetScene()->GetMapManager();
	if (pMapManager == NULL)
	{// NULL��������
		return;
	}

	// �ʒu�X�V
	pos = pMapManager->UpdateNowPosition(m_nIdxMapPoint, m_fPointRatio, m_fMoveValue, pos.y);

}


//==========================================================================
// ��ޕύX
//==========================================================================
void CEditEnemyBase::ChangeType(void)
{
	int nNumAll = CGame::GetEnemyManager()->GetPatternNum();

	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// �z�u����I�u�W�F�N�g�ύX
	bool bChange = false;
	if (pInputKeyboard->GetTrigger(DIK_1) == true)
	{// 1�������ꂽ

		// �F�̎�ލX�V
		m_nEnemyType = (m_nEnemyType + (nNumAll - 1)) % nNumAll;
		bChange = true;
	}
	else if (pInputKeyboard->GetTrigger(DIK_2) == true)
	{// 2�������ꂽ

		// �F�̎�ލX�V
		m_nEnemyType = (m_nEnemyType + 1) % nNumAll;
		bChange = true;
	}

	if (bChange)
	{
		for (int nCntEnemy = 0; nCntEnemy < mylib_const::MAX_PATTEN_ENEMY; nCntEnemy++)
		{
			if (m_pEnemy[nCntEnemy] != NULL)
			{// NULL�ɂ���
				m_pEnemy[nCntEnemy]->Kill();
				m_pEnemy[nCntEnemy]->Uninit();
				m_pEnemy[nCntEnemy] = NULL;
			}
		}

		CEnemyManager *pEnemyManager = CGame::GetEnemyManager();

		// �G�z�u
		CEnemy **ppEnemy = pEnemyManager->SetEnemy(m_pos, m_rot, m_nEnemyType);

		for (int nCntEnemy = 0; nCntEnemy < mylib_const::MAX_PATTEN_ENEMY; nCntEnemy++)
		{
			if (ppEnemy[nCntEnemy] != NULL)
			{
				m_pEnemy[nCntEnemy] = ppEnemy[nCntEnemy];
			}
		}
	}

}

//==========================================================================
// ���f���͂�
//==========================================================================
void CEditEnemyBase::Grab(void)
{

	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// �G�̋��_�̎擾
	CEnemyBase *pEnemyBase = CManager::GetInstance()->GetScene()->GetEnemyBase();
	if (pEnemyBase == NULL)
	{
		return;
	}

	bool bAll = false;
	if (pInputKeyboard->GetPress(DIK_RCONTROL) == true)
	{// �E�R���g���[���őS�I��
		bAll = true;
	}

	for (int i = 0; i < pEnemyBase->GetNumAll(); i++)
	{
		D3DXVECTOR3 TargetPoint = pEnemyBase->GetAxis(i);
		if (bAll == true || SphereRange(m_pos, TargetPoint, 50.0f, 50.0f))
		{// ���ɓ������Ă���

			// ����
			Control(TargetPoint);

			// �ʒu�ݒ�
			pEnemyBase->SetSpawnPoint(i, m_nIdxMapPoint, m_fMoveValue, TargetPoint.y);
		}
	}
}

//==========================================================================
// ���f���͂�
//==========================================================================
void CEditEnemyBase::Delete(void)
{
	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// �G�̋��_�̎擾
	CEnemyBase *pEnemyBase = CManager::GetInstance()->GetScene()->GetEnemyBase();
	if (pEnemyBase == NULL)
	{
		return;
	}

	for (int i = 0; i < pEnemyBase->GetNumAll(); i++)
	{
		D3DXVECTOR3 TargetPoint = pEnemyBase->GetAxis(i);
		if (SphereRange(m_pos, TargetPoint, 50.0f, 50.0f))
		{// ���ɓ������Ă���
			pEnemyBase->DeletePos(i);
		}
	}
}

//==========================================================================
// �`�揈��
//==========================================================================
void CEditEnemyBase::Draw(void)
{

}