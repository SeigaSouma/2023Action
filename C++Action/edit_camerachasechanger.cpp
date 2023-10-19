//=============================================================================
// 
//  �J�����Ǐ]��ޕύX�G�f�B�b�g���� [edit_camerachasechanger.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "edit_camerachasechanger.h"
#include "camerachasechanger.h"
#include "objectX.h"
#include "XLoad.h"
#include "manager.h"
#include "renderer.h"
#include "calculation.h"
#include "debugproc.h"
#include "input.h"
#include "elevation.h"
#include "3D_effect.h"
#include "mapmanager.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define MOVE		(10.0f)
#define MOVE_SLOW	(1.0f)

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CEditCameraChaseChanger::CEditCameraChaseChanger()
{
	// �[���N���A
	m_pos = mylib_const::DEFAULT_VECTOR3;	// �ʒu
	m_ChaseType = CCamera::CHASETYPE_NORMAL;	// �Ǐ]�̎��
	m_nIdxMapPoint = 0;			// �}�b�v�|�C���g�̃C���f�b�N�X�ԍ�
	m_fPointRatio = 0.0f;		// �ړ�����
	m_fMoveValue = 0.0f;		// �ړ���
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CEditCameraChaseChanger::~CEditCameraChaseChanger()
{

}

//==========================================================================
// ��������
//==========================================================================
CEditCameraChaseChanger *CEditCameraChaseChanger::Create(void)
{
	// �����p�̃I�u�W�F�N�g
	CEditCameraChaseChanger *pObjectX = NULL;

	if (pObjectX == NULL)
	{// NULL��������

		// �������̊m��
		pObjectX = DEBUG_NEW CEditCameraChaseChanger;

		if (pObjectX != NULL)
		{// �������̊m�ۂ��o���Ă�����

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
HRESULT CEditCameraChaseChanger::Init(void)
{
	m_pos = CManager::GetInstance()->GetCamera()->GetPositionR();
	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CEditCameraChaseChanger::Uninit(void)
{

}

//==========================================================================
// �X�V����
//==========================================================================
void CEditCameraChaseChanger::Update(void)
{
	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// ����
	Control(m_pos);

	switch (m_ChaseType)
	{
	case CCamera::CHASETYPE_NORMAL:
		CEffect3D::Create(m_pos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f), 50.0f, 2, CEffect3D::MOVEEFFECT_NONE, CEffect3D::TYPE_NORMAL);
		break;

	case CCamera::CHASETYPE_MAP:
		CEffect3D::Create(m_pos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f), 50.0f, 2, CEffect3D::MOVEEFFECT_NONE, CEffect3D::TYPE_NORMAL);
		break;
	}

	// �͂ݏ���
	if (pInputKeyboard->GetPress(DIK_SPACE) == true)
	{// SPACE�Œ͂�
		Grab();
	}

	if (pInputKeyboard->GetTrigger(DIK_C) == true)
	{// C�L�[�ŒǏ]��ޕύX
		m_ChaseType = (CCamera::CHASETYPE)(((int)m_ChaseType + 1) % (int)CCamera::CHASETYPE_MAX);	// �Ǐ]�̎��
	}

	if (pInputKeyboard->GetTrigger(DIK_RETURN) == true)
	{// ENTER�Ŕz�u

		// �}�b�v�}�l�[�W���̎擾
		CCameraChaseChanger *pCmaeraChanger = CManager::GetInstance()->GetScene()->GetCameraChaseChanger();
		if (pCmaeraChanger == NULL)
		{
			return;
		}
		pCmaeraChanger->CreatePos(m_ChaseType, m_nIdxMapPoint, m_fMoveValue);
	}

	if (pInputKeyboard->GetTrigger(DIK_DELETE) == true)
	{// delete�ō폜
		Delete();
	}

	if (pInputKeyboard->GetTrigger(DIK_F9) == true)
	{// �Z�[�u

		// �}�b�v�}�l�[�W���̎擾
		if (CManager::GetInstance()->GetScene()->GetMapManager() == NULL)
		{
			return;
		}
		CCameraChaseChanger *pCmaeraChanger = CManager::GetInstance()->GetScene()->GetCameraChaseChanger();
		pCmaeraChanger->Save("data\\BIN\\camerachanger.bin");
	}

	// �f�o�b�O���
	CManager::GetInstance()->GetDebugProc()->Print(
		"------------------[ �Ǐ]��ޕύX��� ]------------------\n"
		"<�z�u>         [ENTER]\n"
		"<�t�@�C���ۑ�> [F9] �ydata/TEXT/edit_info.txt�z\n"
		"<���>         [C�F%d]\n"
		"<�ړ�>         �����F[��,��,��,��]�@�ᑬ�F[W,A,S,D]\n"
		"<�㏸,���~>    [I,K]\n"
		"<�݈͂ړ�>     [SPACE]\n"
		"<�폜>         [DELETE]\n"
		"<�ʒu>         [X�F%f Y�F%f Z�F%f]\n"
		"\n", m_ChaseType, m_pos.x, m_pos.y, m_pos.z);
}

//==========================================================================
// ���쏈��
//==========================================================================
void CEditCameraChaseChanger::Control(D3DXVECTOR3 &pos)
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
	}
	else if (pInputKeyboard->GetPress(DIK_RIGHT) == true)
	{// D�L�[�������ꂽ,�E�ړ�

		m_fMoveValue += MOVE;
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


	// ���n�������ǂ���
	bool bLand = false;

	// �����擾
	float fHeight = CManager::GetInstance()->GetScene()->GetElevation()->GetHeight(pos, bLand);

	if (bLand == true)
	{
		// �������
		pos.y = fHeight;
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
// ���f���͂�
//==========================================================================
void CEditCameraChaseChanger::Grab(void)
{

	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// �}�b�v�}�l�[�W���擾
	CCameraChaseChanger *pCmaeraChanger = CManager::GetInstance()->GetScene()->GetCameraChaseChanger();
	if (pCmaeraChanger == NULL)
	{// NULL�Ŕ�����
		return;
	}

	bool bAll = false;
	if (pInputKeyboard->GetPress(DIK_RCONTROL) == true)
	{// �E�R���g���[���őS�I��
		bAll = true;
	}

	for (int i = 0; i < pCmaeraChanger->GetNumAll(); i++)
	{
		D3DXVECTOR3 TargetPoint = pCmaeraChanger->GetAxis(i);
		if (bAll == true || SphereRange(m_pos, TargetPoint, 50.0f, 50.0f))
		{// ���ɓ������Ă���

			// ����
			Control(TargetPoint);

			// �ʒu�ݒ�
			pCmaeraChanger->SetAxis(i, m_nIdxMapPoint, m_fMoveValue);
		}
	}
}

//==========================================================================
// ���f���͂�
//==========================================================================
void CEditCameraChaseChanger::Delete(void)
{
	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	if (CManager::GetInstance()->GetScene()->GetMapManager() == NULL)
	{// NULL�Ŕ�����
		return;
	}

	// �}�b�v�}�l�[�W���擾
	CCameraChaseChanger *pCmaeraChanger = CManager::GetInstance()->GetScene()->GetCameraChaseChanger();

	for (int i = 0; i < pCmaeraChanger->GetNumAll(); i++)
	{
		D3DXVECTOR3 TargetPoint = pCmaeraChanger->GetAxis(i);
		if (SphereRange(m_pos, TargetPoint, 50.0f, 50.0f))
		{// ���ɓ������Ă���
			pCmaeraChanger->DeletePos(i);
		}
	}
}

//==========================================================================
// �`�揈��
//==========================================================================
void CEditCameraChaseChanger::Draw(void)
{

}
