//=============================================================================
// 
//  �J�������G�f�B�b�g���� [edit_cameraaxis.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "edit_cameraaxis.h"
#include "objectX.h"
#include "XLoad.h"
#include "manager.h"
#include "renderer.h"
#include "calculation.h"
#include "debugproc.h"
#include "input.h"
#include "elevation.h"
#include "camera.h"
#include "cameraaxis.h"
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
CEditCameraAxis::CEditCameraAxis()
{
	// �[���N���A
	m_pos = mylib_const::DEFAULT_VECTOR3;	// �ʒu

}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CEditCameraAxis::~CEditCameraAxis()
{

}

//==========================================================================
// ��������
//==========================================================================
CEditCameraAxis *CEditCameraAxis::Create(void)
{
	// �����p�̃I�u�W�F�N�g
	CEditCameraAxis *pObjectX = NULL;

	if (pObjectX == NULL)
	{// NULL��������

		// �������̊m��
		pObjectX = DEBUG_NEW CEditCameraAxis;

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
HRESULT CEditCameraAxis::Init(void)
{
	m_pos = CManager::GetInstance()->GetCamera()->GetPositionR();
	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CEditCameraAxis::Uninit(void)
{

}

//==========================================================================
// �X�V����
//==========================================================================
void CEditCameraAxis::Update(void)
{
	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// ����
	Control(m_pos);
	CEffect3D::Create(m_pos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXCOLOR(0.1f, 0.6f, 1.0f, 1.0f), 50.0f, 2, CEffect3D::MOVEEFFECT_NONE, CEffect3D::TYPE_SMOKE);

	// �͂ݏ���
	if (pInputKeyboard->GetPress(DIK_SPACE) == true)
	{// SPACE�Œ͂�
		Grab();
	}

	if (pInputKeyboard->GetTrigger(DIK_RETURN) == true)
	{// ENTER�Ŕz�u

		// �}�b�v�}�l�[�W���̎擾
		CCameraAxis *pCameraAxis = CManager::GetInstance()->GetScene()->GetCameraAxis();
		if (pCameraAxis == NULL)
		{
			return;
		}
		pCameraAxis->CreatePos(m_pos);
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
		CCameraAxis *pCameraAxis = CManager::GetInstance()->GetScene()->GetCameraAxis();
		pCameraAxis->Save("data\\BIN\\cameraaxis.bin");
	}

	// �f�o�b�O���
	CManager::GetInstance()->GetDebugProc()->Print(
		"------------------[ �J�������G�f�B�^ ]------------------\n"
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
void CEditCameraAxis::Control(D3DXVECTOR3 &pos)
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
void CEditCameraAxis::Grab(void)
{

	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// �}�b�v�}�l�[�W���擾
	CCameraAxis *pCameraAxis = CManager::GetInstance()->GetScene()->GetCameraAxis();
	if (pCameraAxis == NULL)
	{// NULL�Ŕ�����
		return;
	}

	bool bAll = false;
	if (pInputKeyboard->GetPress(DIK_RCONTROL) == true)
	{// �E�R���g���[���őS�I��
		bAll = true;
	}

	for (int i = 0; i < pCameraAxis->GetNumAll(); i++)
	{
		D3DXVECTOR3 TargetPoint = pCameraAxis->GetAxis(i);
		if (bAll == true || CircleRange(m_pos, TargetPoint, 50.0f, 50.0f))
		{// ���ɓ������Ă���

			// ����
			Control(TargetPoint);

			// �ʒu�ݒ�
			pCameraAxis->SetAxis(TargetPoint, i);
		}
	}
}

//==========================================================================
// ���f���͂�
//==========================================================================
void CEditCameraAxis::Delete(void)
{
	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	if (CManager::GetInstance()->GetScene()->GetMapManager() == NULL)
	{// NULL�Ŕ�����
		return;
	}

	// �}�b�v�}�l�[�W���擾
	CCameraAxis *pCameraAxis = CManager::GetInstance()->GetScene()->GetCameraAxis();

	for (int i = 0; i < pCameraAxis->GetNumAll(); i++)
	{
		D3DXVECTOR3 TargetPoint = pCameraAxis->GetAxis(i);
		if (SphereRange(m_pos, TargetPoint, 50.0f, 50.0f))
		{// ���ɓ������Ă���
			pCameraAxis->DeletePos(i);
		}
	}
}

//==========================================================================
// �`�揈��
//==========================================================================
void CEditCameraAxis::Draw(void)
{

}
