//=============================================================================
// 
//  ����_�G�f�B�b�g���� [edit_controlpoint.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "edit_controlpoint.h"
#include "objectX.h"
#include "XLoad.h"
#include "manager.h"
#include "renderer.h"
#include "calculation.h"
#include "debugproc.h"
#include "input.h"
#include "elevation.h"
#include "camera.h"
#include "mapmanager.h"
#include "3D_effect.h"

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
CEditControlPoint::CEditControlPoint()
{
	// �[���N���A
	m_pos = mylib_const::DEFAULT_VECTOR3;	// �ʒu

}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CEditControlPoint::~CEditControlPoint()
{

}

//==========================================================================
// ��������
//==========================================================================
CEditControlPoint *CEditControlPoint::Create(void)
{
	// �����p�̃I�u�W�F�N�g
	CEditControlPoint *pObjectX = NULL;

	if (pObjectX == NULL)
	{// NULL��������

		// �������̊m��
		pObjectX = DEBUG_NEW CEditControlPoint;

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
HRESULT CEditControlPoint::Init(void)
{
	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CEditControlPoint::Uninit(void)
{

}

//==========================================================================
// �X�V����
//==========================================================================
void CEditControlPoint::Update(void)
{
	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// ����
	Control(m_pos);
	CEffect3D::Create(m_pos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 0.6f, 0.1f, 1.0f), 50.0f, 2, CEffect3D::MOVEEFFECT_NONE, CEffect3D::TYPE_NORMAL);

	// �͂ݏ���
	if (pInputKeyboard->GetPress(DIK_SPACE) == true)
	{// SPACE�Œ͂�
		Grab();
	}

	if (pInputKeyboard->GetTrigger(DIK_RETURN) == true)
	{// ENTER�Ŕz�u

		// �}�b�v�}�l�[�W���̎擾
		if (CManager::GetInstance()->GetScene()->GetMapManager() == NULL)
		{
			return;
		}
		CMapManager *pMapManager = CManager::GetInstance()->GetScene()->GetMapManager();
		pMapManager->CreatePos(m_pos);
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
		CMapManager *pMapManager = CManager::GetInstance()->GetScene()->GetMapManager();
		pMapManager->Save("data\\BIN\\maptarget.bin");
	}

	// �f�o�b�O���
	CManager::GetInstance()->GetDebugProc()->Print(
		"------------------[ �G�f�B�b�g��� ]------------------\n"
		"<�z�u>         [ENTER]\n"
		"<�t�@�C���ۑ�> [F9] �ydata/TEXT/edit_info.txt�z\n"
		"<�ړ�>         �����F[��,��,��,��]�@�ᑬ�F[W,A,S,D]\n"
		"<�㏸,���~>    [I,K]\n"
		"<�݈͂ړ�>     [SPACE]\n"
		"<�폜>         [DELETE]\n"
		"<�ʒu>         [X�F%f Y�F%f Z�F%f]\n"
		"\n", m_pos.x, m_pos.y, m_pos.z);
}

//==========================================================================
// ���쏈��
//==========================================================================
void CEditControlPoint::Control(D3DXVECTOR3 &pos)
{
	// �J�����̏��擾
	CCamera *pCamera = CManager::GetInstance()->GetCamera();

	// �J�����̌����擾
	D3DXVECTOR3 Camerarot = pCamera->GetRotation();

	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	if (pInputKeyboard->GetPress(DIK_LEFT) == true)
	{// ���L�[�������ꂽ,���ړ�

		if (pInputKeyboard->GetPress(DIK_UP) == true)
		{// A+W,����ړ�

			pos.x += sinf(-D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE;
			pos.z += cosf(-D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE;
		}
		else if (pInputKeyboard->GetPress(DIK_DOWN) == true)
		{// A+S,�����ړ�

			pos.x += sinf(-D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE;
			pos.z += cosf(-D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE;
		}
		else
		{// A,���ړ�

			pos.x += sinf(-D3DX_PI * MOVE_LR + Camerarot.y) * MOVE;
			pos.z += cosf(-D3DX_PI * MOVE_LR + Camerarot.y) * MOVE;
		}
	}
	else if (pInputKeyboard->GetPress(DIK_RIGHT) == true)
	{// D�L�[�������ꂽ,�E�ړ�

		if (pInputKeyboard->GetPress(DIK_UP) == true)
		{// D+W,�E��ړ�

			pos.x += sinf(D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE;
			pos.z += cosf(D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE;
		}
		else if (pInputKeyboard->GetPress(DIK_DOWN) == true)
		{// D+S,�E���ړ�

			pos.x += sinf(D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE;
			pos.z += cosf(D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE;
		}
		else
		{// D,�E�ړ�

			pos.x += sinf(D3DX_PI * MOVE_LR + Camerarot.y) * MOVE;
			pos.z += cosf(D3DX_PI * MOVE_LR + Camerarot.y) * MOVE;
		}
	}
	else if (pInputKeyboard->GetPress(DIK_UP) == true)
	{// W�������ꂽ�A���ړ�

		pos.x += sinf(Camerarot.y) * MOVE;
		pos.z += cosf(Camerarot.y) * MOVE;
	}
	else if (pInputKeyboard->GetPress(DIK_DOWN) == true)
	{// S�������ꂽ�A��O�ړ�

		pos.x += sinf(D3DX_PI + Camerarot.y) * MOVE;
		pos.z += cosf(D3DX_PI + Camerarot.y) * MOVE;
	}

	if (pInputKeyboard->GetPress(DIK_A) == true)
	{// ���L�[�������ꂽ,���ړ�

		if (pInputKeyboard->GetPress(DIK_W) == true)
		{// A+W,����ړ�

			pos.x += sinf(-D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE_SLOW;
			pos.z += cosf(-D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE_SLOW;
		}
		else if (pInputKeyboard->GetPress(DIK_S) == true)
		{// A+S,�����ړ�

			pos.x += sinf(-D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE_SLOW;
			pos.z += cosf(-D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE_SLOW;
		}
		else
		{// A,���ړ�

			pos.x += sinf(-D3DX_PI * MOVE_LR + Camerarot.y) * MOVE_SLOW;
			pos.z += cosf(-D3DX_PI * MOVE_LR + Camerarot.y) * MOVE_SLOW;
		}
	}
	else if (pInputKeyboard->GetPress(DIK_D) == true)
	{// D�L�[�������ꂽ,�E�ړ�

		if (pInputKeyboard->GetPress(DIK_W) == true)
		{// D+W,�E��ړ�

			pos.x += sinf(D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE_SLOW;
			pos.z += cosf(D3DX_PI * MOVE_LRDW + Camerarot.y) * MOVE_SLOW;
		}
		else if (pInputKeyboard->GetPress(DIK_S) == true)
		{// D+S,�E���ړ�

			pos.x += sinf(D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE_SLOW;
			pos.z += cosf(D3DX_PI * MOVE_LRUP + Camerarot.y) * MOVE_SLOW;
		}
		else
		{// D,�E�ړ�

			pos.x += sinf(D3DX_PI * MOVE_LR + Camerarot.y) * MOVE_SLOW;
			pos.z += cosf(D3DX_PI * MOVE_LR + Camerarot.y) * MOVE_SLOW;
		}
	}
	else if (pInputKeyboard->GetPress(DIK_W) == true)
	{// W�������ꂽ�A���ړ�

		pos.x += sinf(Camerarot.y) * MOVE_SLOW;
		pos.z += cosf(Camerarot.y) * MOVE_SLOW;
	}
	else if (pInputKeyboard->GetPress(DIK_S) == true)
	{// S�������ꂽ�A��O�ړ�

		pos.x += sinf(D3DX_PI + Camerarot.y) * MOVE_SLOW;
		pos.z += cosf(D3DX_PI + Camerarot.y) * MOVE_SLOW;
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
}

//==========================================================================
// ���f���͂�
//==========================================================================
void CEditControlPoint::Grab(void)
{

	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	if (CManager::GetInstance()->GetScene()->GetMapManager() == NULL)
	{// NULL�Ŕ�����
		return;
	}

	// �}�b�v�}�l�[�W���擾
	CMapManager *pMapManager = CManager::GetInstance()->GetScene()->GetMapManager();

	for (int i = 0; i < pMapManager->GetNumAll(); i++)
	{
		D3DXVECTOR3 TargetPoint = pMapManager->GetControlPoint(i);
		if (SphereRange(m_pos, TargetPoint, 50.0f, 50.0f))
		{// ���ɓ������Ă���

			// ����
			Control(TargetPoint);

			// �ʒu�ݒ�
			pMapManager->SetControlPoint(TargetPoint, i);
		}
	}
}

//==========================================================================
// ���f���͂�
//==========================================================================
void CEditControlPoint::Delete(void)
{
	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	if (CManager::GetInstance()->GetScene()->GetMapManager() == NULL)
	{// NULL�Ŕ�����
		return;
	}

	// �}�b�v�}�l�[�W���擾
	CMapManager *pMapManager = CManager::GetInstance()->GetScene()->GetMapManager();

	for (int i = 0; i < pMapManager->GetNumAll(); i++)
	{
		D3DXVECTOR3 TargetPoint = pMapManager->GetControlPoint(i);
		if (SphereRange(m_pos, TargetPoint, 50.0f, 50.0f))
		{// ���ɓ������Ă���
			pMapManager->DeletePos(i);
		}
	}
}

//==========================================================================
// �`�揈��
//==========================================================================
void CEditControlPoint::Draw(void)
{

}
