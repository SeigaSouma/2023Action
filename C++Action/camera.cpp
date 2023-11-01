//=============================================================================
// 
// �J�������� [camera.cpp]
// Author : ���n�Ή�
// 
//=============================================================================
#include "debugproc.h"
#include "manager.h"
#include "renderer.h"
#include "game.h"
#include "gamemanager.h"
#include "camera.h"
#include "input.h"
#include "calculation.h"
#include "player.h"
#include "elevation.h"
#include "enemymanager.h"
#include "title.h"
#include "instantfade.h"
#include "mapmanager.h"
#include "calculation.h"
#include "light.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define MOVE			(2.5f)				// �ړ���
#define MAX_LENGTH		(20000.0f)			// �ő勗��
#define MIN_LENGTH		(10.0f)				// �ŏ�����
//#define START_CAMERALEN	(5100.0f)			// ���̋���
#define START_CAMERALEN	(1200.0f)			// ���̋���
#define ROT_MOVE_MOUSE	(0.01f)				// ��]�ړ���
#define ROT_MOVE_STICK	(0.0015f)			// ��]�ړ���
#define ROT_MOVE		(0.025f)			// ��]�ړ���
#define MIN_ROT			(-D3DX_PI * 0.49f)	// �J�����Œ�p
#define MAX_ROT			(D3DX_PI * 0.49f)	// �J�����Œ�p
#define BACKFOLLOW_TIME	(20)				// �w�ʕ␳�܂ł̎���
#define TITLE_POSR_DEST	(D3DXVECTOR3(0.0f, 0.0f, 0.0f))
#define TITLE_LEN_DEST	(6700.0f)
#define TITLECAMERAROT_NONE		(D3DXVECTOR3(0.0f, -0.79f, -0.25f))
#define TITLECAMERAROT_ENEMY	(D3DXVECTOR3(0.0f, -0.79f, -0.12f))
#define TITLESTATE_CHANGE	(60 * 14)
#define TITLESTATE_CHASE	(60 * 20)
#define RESULT_LEN	(6700.0f)
//#define RESULT_LEN	(1000.0f)
#define RANKINGROT_NONE		(D3DXVECTOR3(0.0f, -0.79f + D3DX_PI, -0.30f))
#define ROTHOSEI	(0.01f)	// �����̕␳�W��
#define POSV_HOSEI	(0.12f * 0.8f)
#define POSR_HOSEI	(0.08f * 0.8f)
#define CHASEDISTANCE_DEST	(250.0f)
#define ROTCHANGEHOSEI		(0.3f)
#define RANKNIG_LEN_DEST	(7500.0f)

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CCamera::CCamera()
{
	m_viewport.X = 0;							// �`�悷���ʂ̍���X���W
	m_viewport.Y = 0;							// �`�悷���ʂ̍���Y���W
	m_viewport.Width = 0;						// �`�悷���ʂ̕�
	m_viewport.Height = 0;						// �`�悷���ʂ̍���
	m_viewport.Width = 0;						// �`�悷���ʂ̕�
	m_viewport.Height = 0;						// �`�悷���ʂ̍���
	m_viewport.MinZ = 0.0f;
	m_viewport.MaxZ = 0.0f;
	m_posR = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// �����_(�������ꏊ)
	m_posV = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// ���_(�J�����̈ʒu)
	m_posVDest = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// �ڕW�̎��_
	m_posRDest = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// �ڕW�̒����_
	m_vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);		// ������x�N�g��
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// �ړ���
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// ����
	m_rotVDest = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// �ڕW�̎��_�̌���
	m_TargetPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);// �ڕW�̈ʒu
	m_fDistance = 0.0f;							// ����
	m_fDestDistance = 0.0f;						// �ڕW�̋���
	m_fOriginDistance = 0.0f;					// ���̋���
	m_nShakeLength = 0.0f;						// �h��̒���
	m_nShakeLengthY = 0.0f;						// Y�̗h��̒���
	m_fMoveShake = 0.0f;						// �h��̈ړ���
	m_fMoveShakeY = 0.0f;						// Y�̗h��̈ړ���
	m_fHeightMaxDest = 0.0f;					// �J�����̍ő卂���̖ڕW
	m_fHeightMax = 0.0f;			// �J�����̍ő卂��
	m_fDiffHeight = 0.0f;						// �����̍���
	m_fDiffHeightSave = 0.0f;					// �����̍����ۑ��p
	m_fDiffHeightDest = 0.0f;					// �ڕW�̍����̍���
	m_fChaseLerpStart = 0.0f;					// �Ǐ]�␳�̏����l
	m_fChaseDistance = 0.0f;					// �Ǐ]�̊Ԋu
	m_fDestChaseDistance = 0.0f;				// �ڕW�̒Ǐ]�̊Ԋu
	m_bFollow = false;							// �Ǐ]���邩�ǂ���
	m_state = CAMERASTATE_NONE;					// ���
	m_nCntState = 0;							// ��ԃJ�E���^�[
	m_nCntDistance = 0;							// �����J�E���^�[
	m_nOriginCntDistance = 0;					// ���̋����J�E���^�[
	m_fDistanceCorrection = 0.0f;				// �����̊����␳�W��
	m_fDistanceDecrementValue = 0.0f;			// �����̌����W��
	m_ChaseType = CHASETYPE_NORMAL;				// �Ǐ]�̎��
	m_OldChaseType = CHASETYPE_NORMAL;			// �O��̒Ǐ]�̎��
	m_nCntChaseType = 0;			// �Ǐ]�̃J�E���^�[
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CCamera::~CCamera()
{

}

//==================================================================================
// �J�����̏���������
//==================================================================================
HRESULT CCamera::Init(void)
{
	m_viewport.X = 0;										// �`�悷���ʂ̍���X���W
	m_viewport.Y = 0;										// �`�悷���ʂ̍���Y���W
	m_viewport.Width = SCREEN_WIDTH;						// �`�悷���ʂ̕�
	m_viewport.Height = SCREEN_HEIGHT;						// �`�悷���ʂ̍���
	m_viewport.MinZ = 0.0f;
	m_viewport.MaxZ = 1.0f;
	m_posR = D3DXVECTOR3(0.0f, 200.0f, 0.0f);				// �����_(�������ꏊ)
	m_posV = D3DXVECTOR3(0.0f, 300.0f, m_posR.z + -400.0f);	// ���_(�J�����̈ʒu)
	m_posVDest = m_posV;									// �ڕW�̎��_
	m_posRDest = m_posR;									// �ڕW�̒����_
	m_vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);					// ������x�N�g��
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);					// �ړ���
	m_rot = D3DXVECTOR3(0.0f, 0.0f, -0.20f);					// ����
	m_rotVDest = m_rot;										// �ڕW�̎��_�̌���
	m_TargetPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);			// �ڕW�̈ʒu
	m_fDistance = START_CAMERALEN;							// ����
	m_fDestDistance = START_CAMERALEN;						// �ڕW�̋���
	m_fOriginDistance = START_CAMERALEN;					// ���̋���
	m_fDiffHeight = 0.0f;									// �����̍���
	m_fDiffHeightSave = 0.0f;								// �����̍����ۑ��p
	m_fDiffHeightDest = 0.0f;								// �ڕW�̍����̍���
	m_fChaseLerpStart = 0.0f;								// �Ǐ]�␳�̏����l
	m_fChaseDistance = 0.0f;								// �Ǐ]�̊Ԋu
	m_fDestChaseDistance = 0.0f;							// �ڕW�̒Ǐ]�̊Ԋu
	m_bFollow = true;										// �Ǐ]���邩�ǂ���
	m_state = CAMERASTATE_NONE;								// ���
	m_nCntState = 0;							// ��ԃJ�E���^�[
	m_nCntDistance = 0;							// �����J�E���^�[
	m_nOriginCntDistance = 0;					// ���̋����J�E���^�[
	m_fDistanceCorrection = 0;					// �����̊����␳�W��
	m_fDistanceDecrementValue = 0.0f;			// �����̌����W��
	m_fHeightMaxDest = 0.0f;					// �J�����̍ő卂���̖ڕW
	m_ChaseType = CHASETYPE_MAP;				// �Ǐ]�̎��
	m_OldChaseType = m_ChaseType;			// �O��̒Ǐ]�̎��
	m_nCntChaseType = 0;			// �Ǐ]�̃J�E���^�[

	// ���_�̑������
	SetCameraV();

	return S_OK;
}

//==================================================================================
// �J�����̏I������
//==================================================================================
void CCamera::Uninit(void)
{

}

//==================================================================================
// �J�����̍X�V����
//==================================================================================
void CCamera::Update(void)
{
	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	//if (m_state == CAMERASTATE_NONE)
	{
		// �����̍������Z�b�g
		m_fDiffHeightSave = 0.0f;

		if ((m_OldChaseType != CHASETYPE_NONE && m_ChaseType == CHASETYPE_NONE) ||
			(m_OldChaseType == CHASETYPE_NONE && m_ChaseType != CHASETYPE_NONE))
		{// �O�񂪒Ǐ]���Ăč��񂪒Ǐ]���ĂȂ�
			m_nCntChaseType = 150;
		}

		m_nCntChaseType--;
		if (m_nCntChaseType <= 0)
		{
			m_nCntChaseType = -1;
		}

		// ���_�E�����_�ړ�
		MoveCameraFollow();
		MoveCameraInput();
		MoveCameraVR();
		MoveCameraR();
		MoveCameraV();
		MoveCameraDistance();
		MoveCameraDistance();
		UpdateByMode();
		UpdateSpotLightVec();

		if (m_state == CAMERASTATE_SHAKE)
		{
			UpdateState();
		}
	}

//#ifdef _DEBUG
	
	if (pInputKeyboard->GetTrigger(DIK_F7) == true)
	{// F7�������ꂽ,�Ǐ]�؂�ւ�

		m_bFollow = m_bFollow ? false : true;
	}

//#endif

	// �O��̒Ǐ]�̎��
	m_OldChaseType = m_ChaseType;

	// �e�L�X�g�̕`��
	CManager::GetInstance()->GetDebugProc()->Print(
		"---------------- �J������� ----------------\n"
		"�y�����z[X�F%f Y�F%f Z�F%f]\n"
		"�y�����z[%f]\n"
		"�y���_�z[X�F%f Y�F%f Z�F%f]\n"
		"�y�����_�z[X�F%f Y�F%f Z�F%f]\n",
		m_rot.x, m_rot.y, m_rot.z,
		m_fDistance,
		m_posV.x, m_posV.y, m_posV.z,
		m_posR.x, m_posR.y, m_posR.z);
}

//==================================================================================
// ���[�h�ʍX�V����
//==================================================================================
void CCamera::UpdateByMode(void)
{
	switch (CManager::GetInstance()->GetMode())
	{
	case CScene::MODE_TITLE:
		
		// ��]������
		m_rot.y += D3DX_PI * 0.001f;
		RotNormalize(m_rot.y);
		break;

	case CScene::MODE_GAME:
		break;

	default:
		break;
	}
}

//==================================================================================
// ���͋@��̃J�����ړ�
//==================================================================================
void CCamera::MoveCameraInput(void)
{
#ifdef _DEBUG
	// �}�E�X�ł̈ړ�����
	MoveCameraMouse();
#endif

	// �X�e�B�b�N�ł̈ړ�����
	MoveCameraStick();
}

//==================================================================================
// �R���g���[���[�ł̈ړ�����
//==================================================================================
void CCamera::MoveCameraStick(void)
{
	// �Q�[���p�b�h���擾
	CInputGamepad *pInputGamepad = CManager::GetInstance()->GetInputGamepad();

#if 0
	m_rot.y += pInputGamepad->GetStickMoveR(0).x * ROT_MOVE_STICK;
	m_rot.z += pInputGamepad->GetStickMoveR(0).y * ROT_MOVE_STICK;
#endif
	// �p�x�̐��K��
	RotNormalize(m_rot.y);
	RotNormalize(m_rot.z);

	// ���_�̑������
	SetCameraV();
}

//==================================================================================
// �}�E�X�ł̈ړ�����
//==================================================================================
void CCamera::MoveCameraMouse(void)
{
	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// �L�[�{�[�h���擾
	CInputMouse *pInputMouse = CManager::GetInstance()->GetInputMouse();

	if (pInputMouse->GetPress(CInputMouse::BUTTON_LEFT) == true &&
		pInputMouse->GetPress(CInputMouse::BUTTON_RIGHT) == true)
	{// ���E��������

//#if _DEBUG
		m_move.x += (pInputMouse->GetMouseMove().x * sinf(-D3DX_PI * MOVE_LR + m_rot.y) * MOVE) -
			(pInputMouse->GetMouseMove().y * cosf(-D3DX_PI * MOVE_LR + m_rot.y) * MOVE);

		m_move.z += (pInputMouse->GetMouseMove().x * cosf(-D3DX_PI * MOVE_LR + m_rot.y) * MOVE) +
			(pInputMouse->GetMouseMove().y * sinf(-D3DX_PI * MOVE_LR + m_rot.y) * MOVE);
//#endif

		// �ړ��ʕ␳
		MoveCameraVR();

		// �p�x�̐��K��
		RotNormalize(m_rot.y);
		RotNormalize(m_rot.z);

		// �����_�ݒ�
		SetCameraR();
	}
	else if (pInputMouse->GetPress(CInputMouse::BUTTON_LEFT) == true)
	{// ���N���b�N���Ă�Ƃ�,���_��]

		m_rot.y += pInputMouse->GetMouseMove().x * ROT_MOVE_MOUSE;

//#if _DEBUG

		m_rot.z += pInputMouse->GetMouseMove().y * ROT_MOVE_MOUSE;	
//#endif

		// �p�x�̐��K��
		RotNormalize(m_rot.y);
		RotNormalize(m_rot.z);

		// �l�̐��K��
		ValueNormalize(m_rot.z, MAX_ROT, MIN_ROT);

		// ���_�̑������
		SetCameraV();

	}
	else if (pInputMouse->GetPress(CInputMouse::BUTTON_RIGHT) == true)
	{// �E�N���b�N���Ă�Ƃ�,�����_��]

		m_rot.y += pInputMouse->GetMouseMove().x * ROT_MOVE_MOUSE;

//#if _DEBUG
		m_rot.z += pInputMouse->GetMouseMove().y * ROT_MOVE_MOUSE;
//#endif

		// �p�x�̐��K��
		RotNormalize(m_rot.y);
		RotNormalize(m_rot.z);

		// �l�̐��K��
		ValueNormalize(m_rot.z, MAX_ROT, MIN_ROT);

		// �����_�̈ʒu�X�V
		SetCameraR();
	}

	// �}�E�X�z�C�[���ŋ�������
	m_fDistance += pInputMouse->GetMouseMove().z * (MOVE * 0.3f);
	m_fDestDistance += pInputMouse->GetMouseMove().z * (MOVE * 0.3f);
	m_fOriginDistance += pInputMouse->GetMouseMove().z * (MOVE * 0.3f);

	// ���_�̑������
	SetCameraV();
}

//==================================================================================
// �J�����̎��_�ړ�
//==================================================================================
void CCamera::MoveCameraV(void)
{
	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

#ifdef _DEBUG
	// ���_�ړ�
	if (pInputKeyboard->GetPress(DIK_Y) == true)
	{// Y�L�[�������ꂽ,���_��ړ�

		m_rot.z -= ROT_MOVE;
	}
	else if (pInputKeyboard->GetPress(DIK_N) == true)
	{// N�L�[�������ꂽ,���_���ړ�

		m_rot.z += ROT_MOVE;
	}

	if (pInputKeyboard->GetPress(DIK_Z) == true)
	{// Z�L�[�������ꂽ

		m_rot.y += ROT_MOVE;
	}
	if (pInputKeyboard->GetPress(DIK_C) == true)
	{// C�L�[�������ꂽ

		m_rot.y -= ROT_MOVE;
	}
#endif

#if 0
	if (pInputKeyboard->GetPress(DIK_D) == true)
	{// Z�L�[�������ꂽ

		m_rot.y += ROT_MOVE * 1.8f;
	}
	if (pInputKeyboard->GetPress(DIK_A) == true)
	{// C�L�[�������ꂽ

		m_rot.y -= ROT_MOVE * 1.8f;
	}
#endif


	// �p�x�̐��K��
	RotNormalize(m_rot.y);
	RotNormalize(m_rot.z);

	// �l�̐��K��
	ValueNormalize(m_rot.z, MAX_ROT, MIN_ROT);

	// ���_�̑������
	SetCameraV();

}

//==================================================================================
// �J�����̒����_�ړ�
//==================================================================================
void CCamera::MoveCameraR(void)
{
	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

#ifdef _DEBUG
	// ����
	if (pInputKeyboard->GetPress(DIK_Q) == true)
	{// Q�L�[�������ꂽ,������

		m_rot.y -= ROT_MOVE;
	}
	else if (pInputKeyboard->GetPress(DIK_E) == true)
	{// E�L�[�������ꂽ,������

		m_rot.y += ROT_MOVE;
	}

	// �㏸���~
	if (pInputKeyboard->GetPress(DIK_T) == true)
	{// T�L�[�������ꂽ

		m_rot.z += ROT_MOVE;
	}
	else if (pInputKeyboard->GetPress(DIK_B) == true)
	{// B�L�[�������ꂽ

		m_rot.z -= ROT_MOVE;
	}
#endif

	// �p�x�̐��K��
	RotNormalize(m_rot.y);
	RotNormalize(m_rot.z);

	// �l�̐��K��
	ValueNormalize(m_rot.z, MAX_ROT, MIN_ROT);

	// �����_�̈ʒu�X�V
	SetCameraR();
}

//==================================================================================
// �J�����̎��_�E�����_�ړ�
//==================================================================================
void CCamera::MoveCameraVR(void)
{

	// �ړ��ʕ������Z
	m_posV.x += m_move.x;
	m_posV.z += m_move.z;

	// �ړ��ʂ����Z�b�g
	m_move.x = 0.0f;
	m_move.z = 0.0f;

	// �����_�̈ʒu�X�V
	SetCameraR();
}

//==================================================================================
//  ���_�E�����_�ԋ���
//==================================================================================
void CCamera::MoveCameraDistance(void)
{
	// �����J�E���^�[���Z
	m_nCntDistance--;

	if (m_nCntDistance <= 0)
	{
		m_nCntDistance = 0;

		if (m_fDestDistance >= m_fOriginDistance)
		{// �J�������߂Â��Ă���
			m_fDestDistance -= m_fDistanceDecrementValue;
		}

		if (m_fDestDistance <= m_fOriginDistance)
		{// �␳����������߂�
			m_fDestDistance = m_fOriginDistance;
		}
	}

	// ���E�l�␳
	if (m_fDistance >= MAX_LENGTH)
	{
		m_fDistance = MAX_LENGTH;
	}
	else if (m_fDistance <= MIN_LENGTH)
	{
		m_fDistance = MIN_LENGTH;
	}

	// �ڕW�̋����֕␳
	InertiaCorrection(m_fDistance, m_fDestDistance, m_fDistanceCorrection);

}

//==================================================================================
// �J�����̎��_�������
//==================================================================================
void CCamera::SetCameraV(void)
{

	switch (CManager::GetInstance()->GetMode())
	{
	case CScene::MODE_TITLE:
		SetCameraVTitle();
		break;

	case CScene::MODE_TUTORIAL:
		SetCameraVGame();
		break;

	case CScene::MODE_GAME:
		SetCameraVGame();
		break;

	case CScene::MODE_RESULT:
		SetCameraVResult();
		break;

	default:
		break;
	}

}

//==================================================================================
// �J�����̎��_�������(�^�C�g��)
//==================================================================================
void CCamera::SetCameraVTitle(void)
{
	if (m_bFollow == false)
	{// �Ǐ]���Ȃ��Ƃ�

		// ���_�̑������
		m_posV.x = m_posR.x + cosf(m_rot.z) * sinf(m_rot.y) * -m_fDistance;
		m_posV.z = m_posR.z + cosf(m_rot.z) * cosf(m_rot.y) * -m_fDistance;
		m_posV.y = m_posR.y + sinf(m_rot.z) * -m_fDistance;
	}
	else if (m_bFollow == true)
	{// �Ǐ]ON

		// ���_�̑������
		m_posVDest.x = m_posR.x + cosf(m_rot.z) * sinf(m_rot.y) * -m_fDistance;
		m_posVDest.z = m_posR.z + cosf(m_rot.z) * cosf(m_rot.y) * -m_fDistance;
		m_posVDest.y = m_posR.y + sinf(m_rot.z) * -m_fDistance;

		float fDistance = 0.0f;
		m_fHeightMaxDest = m_posVDest.y;
		// �ڕW�̊p�x�����߂�
		float fRotDest = atan2f((m_posVDest.x - m_posR.x), (m_posVDest.z - m_posR.z));
		while (1)
		{
			
			// ���z�̒e�̈ʒu
			float fPosBulletX = m_TargetPos.x + cosf(m_rot.z) * sinf(m_rot.y) * -fDistance;
			float fPosBulletZ = m_TargetPos.z + cosf(m_rot.z) * cosf(m_rot.y) * -fDistance;

			// �����擾
			bool bLand = false;
			float fHeight = CGame::GetElevation()->GetHeight(D3DXVECTOR3(fPosBulletX, 0.0f, fPosBulletZ), bLand);

			if (m_fHeightMaxDest <= fHeight)
			{// �ő�̍������X�V������

				// �����̉����������ۑ�
				float fDistanceRatio = fDistance / (m_fDistance);

				// �ڕW�̍ő卂���ۑ�
				m_fHeightMaxDest = fHeight * (fDistanceRatio + 1.0f);
			}

			// �������Z
			fDistance += 10.0f;

			if (fDistance >= m_fDistance)
			{// �����𒴂�����I���
				break;
			}
		}

		// �ڕW�̎��_�X�V
		if (m_fHeightMaxDest > m_posVDest.y)
		{
			// �ő�̍����␳
			m_fHeightMax += (m_fHeightMaxDest - m_fHeightMax) * 0.08f;

			m_posVDest.y = m_fHeightMax;

			// �����̍���
			m_fDiffHeightSave += m_fHeightMax - m_posV.y;
		}

		// �␳����
		m_posV += (m_posVDest - m_posV) * 0.12f;
	}
}

//==================================================================================
// �J�����̎��_�������(�Q�[��)
//==================================================================================
void CCamera::SetCameraVGame(void)
{
	if (m_bFollow == false)
	{// �Ǐ]���Ȃ��Ƃ�

		// ���_�̑������
		m_posV.x = m_posR.x + cosf(m_rot.z) * sinf(m_rot.y) * -m_fDistance;
		m_posV.z = m_posR.z + cosf(m_rot.z) * cosf(m_rot.y) * -m_fDistance;
		m_posV.y = m_posR.y + sinf(m_rot.z) * -m_fDistance;
	}
	else if (m_bFollow == true)
	{// �Ǐ]ON

		//���݂ƖڕW�̍��������߂�
		float fRotDiff = m_rotVDest.z - m_rot.z;
		RotNormalize(fRotDiff);

		m_rot.z += fRotDiff * 0.1f;
		RotNormalize(m_rot.z);

		switch (m_ChaseType)
		{
		case CCamera::CHASETYPE_NORMAL:
			ChaseNormal();
			break;

		case CCamera::CHASETYPE_MAP:
			ChaseMap();
			break;

		default:
			ChaseNone();
			break;
		}
	}
}

//==================================================================================
// �J�����̎��_�������(���U���g)
//==================================================================================
void CCamera::SetCameraVResult(void)
{
	// ���_�̑������
	m_posV.x = m_posR.x + cosf(m_rot.z) * sinf(m_rot.y) * -m_fDistance;
	m_posV.z = m_posR.z + cosf(m_rot.z) * cosf(m_rot.y) * -m_fDistance;
	m_posV.y = m_posR.y + sinf(m_rot.z) * -m_fDistance;
}

//==================================================================================
// �J�����̒����_�������
//==================================================================================
void CCamera::SetCameraR(void)
{

	switch (CManager::GetInstance()->GetMode())
	{
	case CScene::MODE_TITLE:
		SetCameraRTitle();
		break;

	case CScene::MODE_TUTORIAL:
		SetCameraRGame();
		break;

	case CScene::MODE_GAME:
		SetCameraRGame();
		break;

	case CScene::MODE_RESULT:
		SetCameraRResult();
		break;

	default:
		break;
	}

}

//==================================================================================
// �J�����̒����_�������(�^�C�g��)
//==================================================================================
void CCamera::SetCameraRTitle(void)
{
	if (m_bFollow == false)
	{// �Ǐ]���Ȃ��Ƃ�

		// �����_�̑������
		m_posR.x = m_posV.x + cosf(m_rot.z) * sinf(m_rot.y) * m_fDistance;
		m_posR.z = m_posV.z + cosf(m_rot.z) * cosf(m_rot.y) * m_fDistance;
		m_posR.y = m_posV.y + sinf(m_rot.z) * m_fDistance;
	}
	else
	{// �Ǐ]ON

		float fYcamera = 100.0f;

		if (m_TargetPos.y >= 50.0f)
		{
			fYcamera = (m_TargetPos.y - 50.0f) + 100.0f;
		}
		else if (m_TargetPos.y <= -50.0f)
		{
			fYcamera = (m_TargetPos.y + 50.0f) + 100.0f;
		}

		fYcamera = m_TargetPos.y + 100.0f;

		if (fYcamera <= 0.0f)
		{
			m_fDiffHeightSave += -fYcamera;
		}

		// �ڕW�̍����̍�������
		m_fDiffHeightDest = m_fDiffHeightSave;

		// �����̍�����␳����
		m_fDiffHeight += (m_fDiffHeightDest - m_fDiffHeight) * 0.001f;


		// �����_�̑������
		m_posRDest.x = m_TargetPos.x;
		m_posRDest.z = m_TargetPos.z;
		m_posRDest.y = fYcamera - m_fDiffHeight;

		// �␳����
		m_posR += (m_posRDest - m_posR) * POSR_HOSEI;
	}
}

//==================================================================================
// �J�����̒����_�������(�Q�[��)
//==================================================================================
void CCamera::SetCameraRGame(void)
{
	if (m_bFollow == false)
	{// �Ǐ]���Ȃ��Ƃ�

		// �����_�̑������
		m_posR.x = m_posV.x + cosf(m_rot.z) * sinf(m_rot.y) * m_fDistance;
		m_posR.z = m_posV.z + cosf(m_rot.z) * cosf(m_rot.y) * m_fDistance;
		m_posR.y = m_posV.y + sinf(m_rot.z) * m_fDistance;
	}
	else
	{// �Ǐ]ON

		if (m_ChaseType == CHASETYPE_NONE)
		{
			ChaseNone();
			return;
		}

		// �v���C���[�̏��擾
		CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();
		if (pPlayer == NULL)
		{// NULL��������
			return;
		}

		D3DXVECTOR3 PlayerPos = pPlayer->GetPosition();
		D3DXVECTOR3 PlayerRot = pPlayer->GetRotation();
		float fYcamera = 100.0f;

		if (PlayerPos.y >= 50.0f)
		{
			fYcamera = (PlayerPos.y - 50.0f) + 100.0f;
		}
		else if (PlayerPos.y <= -50.0f)
		{
			fYcamera = (PlayerPos.y + 50.0f) + 100.0f;
		}

		fYcamera = PlayerPos.y + 100.0f;

		if (fYcamera <= 0.0f)
		{
			m_fDiffHeightSave += -fYcamera;
		}

		// �ڕW�̍����̍�������
		m_fDiffHeightDest = m_fDiffHeightSave;

		// �����̍�����␳����
		m_fDiffHeight += (m_fDiffHeightDest - m_fDiffHeight) * 0.001f;



		// �����_
		D3DXVECTOR3 DestPoint = mylib_const::DEFAULT_VECTOR3;
		if (CGame::GetGameManager()->GetType() != CGameManager::SCENE_BOSS)
		{// �{�X����Ȃ��Ƃ�


			// �}�b�v�}�l�[�W���̎擾
			CMapManager *pMapManager = CGame::GetMapManager();
			if (pMapManager == NULL)
			{// NULL��������
				return;
			}

			// �}�b�v���擾
			int nMapIdx = pPlayer->GetMapIndex();
			float fMapRatio = pPlayer->GetMapPointRatio();
			float fMapMoveValue = pPlayer->GetMapMoveValue();

			// �Ȑ����ׂ�4�_
			int nP0 = nMapIdx;
			int nP1 = nMapIdx + 1;
			int nP2 = nMapIdx + 2;

			// �ڕW�n�_
			D3DXVECTOR3 TargetPoint0 = pMapManager->GetControlPoint(nP0);
			D3DXVECTOR3 TargetPoint1 = pMapManager->GetControlPoint(nP1);
			D3DXVECTOR3 TargetPoint2 = pMapManager->GetControlPoint(nP2);

			int nAngle = 1;
			if (pPlayer->GetMoveAngle() == CObject::ANGLE_LEFT)
			{
				nAngle = -1;
			}

			static float time = 0.0f;
			if (pPlayer->GetMoveAngle() == CObject::ANGLE_RIGHT && pPlayer->GetOldMoveAngle() != CObject::ANGLE_RIGHT)
			{// �ڕW���E
				time = 0.0f;
				m_fChaseLerpStart = m_fChaseDistance;	// �Ǐ]�␳�̏����l
				m_fDestChaseDistance = CHASEDISTANCE_DEST;
			}
			else if (pPlayer->GetMoveAngle() == CObject::ANGLE_LEFT && pPlayer->GetOldMoveAngle() != CObject::ANGLE_LEFT)
			{// �ڕW���E
				time = 0.0f;
				m_fChaseLerpStart = m_fChaseDistance;	// �Ǐ]�␳�̏����l
				m_fDestChaseDistance = -CHASEDISTANCE_DEST;
			}
			else if (
				(pPlayer->GetMoveAngle() == CObject::ANGLE_UP && pPlayer->GetOldMoveAngle() != CObject::ANGLE_UP) ||
				(pPlayer->GetMoveAngle() == CObject::ANGLE_DOWN && pPlayer->GetOldMoveAngle() != CObject::ANGLE_DOWN))
			{// �ڕW���E
				time = 0.0f;
				m_fChaseLerpStart = m_fChaseDistance;	// �Ǐ]�␳�̏����l
				m_fDestChaseDistance = 0.0f;
			}
			time += CManager::GetInstance()->DeltaTime() * 0.5f;
#if 0
			m_fChaseDistance = Lerp(m_fChaseLerpStart, m_fDestChaseDistance, time);
#else
			m_fChaseDistance += (m_fDestChaseDistance - m_fChaseDistance) * 0.02f;
#endif
			// ������̒n�_�擾
			float fMoveValue = fMapMoveValue + m_fChaseDistance;
			DestPoint = pMapManager->GetTargetPosition(nMapIdx, fMoveValue);
		}
		else
		{
			DestPoint = PlayerPos;
		}

		// �����_�̑������
		m_posRDest.x = DestPoint.x;
		m_posRDest.z = DestPoint.z;
		m_posRDest.y = fYcamera - m_fDiffHeight;

		// �␳����
		float fHosei = POSR_HOSEI;
		if (m_nCntChaseType != -1)
		{
			float fBairitu = 1.0f - ((float)m_nCntChaseType / 150.0f);
			fHosei *= fBairitu;
		}

		m_posR += (m_posRDest - m_posR) * fHosei;

	}
}

//==================================================================================
// �J�����̒����_�������(���U���g)
//==================================================================================
void CCamera::SetCameraRResult(void)
{
	// �����_�̑������
	m_posR.x = m_posV.x + cosf(m_rot.z) * sinf(m_rot.y) * m_fDistance;
	m_posR.z = m_posV.z + cosf(m_rot.z) * cosf(m_rot.y) * m_fDistance;
	m_posR.y = m_posV.y + sinf(m_rot.z) * m_fDistance;
}

//==================================================================================
// �X�|�b�g���C�g�̃x�N�g���X�V
//==================================================================================
void CCamera::UpdateSpotLightVec(void)
{
	// �����x�N�g��
	D3DXVECTOR3 vec = mylib_const::DEFAULT_VECTOR3;

	// ���_���璍���_�ւ̌���
	vec = m_posR - m_posV;

	// ���K��
	D3DXVec3Normalize(&vec, &vec);

	// �X�|�b�g���C�g�̕����ݒ�
	CManager::GetInstance()->GetLight()->UpdateSpotLightDirection(vec);

}

//==================================================================================
// �ʏ�̒Ǐ]
//==================================================================================
void CCamera::ChaseNormal(void)
{
	// �v���C���[�̏��擾
	CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();

	if (pPlayer == NULL)
	{
		return;
	}

	D3DXVECTOR3 PlayerPos = pPlayer->GetPosition();
	D3DXVECTOR3 PlayerRot = pPlayer->GetRotation();

	float fYcamera = 100.0f;

	// �ڕW�̎��_�̌���
	m_rotVDest.y = atan2f((m_TargetPos.x - PlayerPos.x), (m_TargetPos.z - PlayerPos.z));

	//���݂ƖڕW�̍��������߂�
	float fRotDiff = m_rotVDest.y - m_rot.y;
	RotNormalize(fRotDiff);

	float fHosei = ROTHOSEI;
	if (m_nCntChaseType != -1)
	{
		float fBairitu = 1.0f - ((float)m_nCntChaseType / 150.0f);
		fHosei *= fBairitu;
	}

	m_rot.y += fRotDiff * fHosei;
	RotNormalize(m_rot.y);

	// ���_�̑������
	m_posVDest.x = m_posR.x + cosf(m_rot.z) * sinf(m_rot.y) * -m_fDistance;
	m_posVDest.z = m_posR.z + cosf(m_rot.z) * cosf(m_rot.y) * -m_fDistance;
	m_posVDest.y = m_posR.y + sinf(m_rot.z) * -m_fDistance;

	float fDistance = 0.0f;
	m_fHeightMaxDest = m_posVDest.y;
	// �ڕW�̊p�x�����߂�
	float fRotDest = atan2f((m_posVDest.x - m_posR.x), (m_posVDest.z - m_posR.z));
	while (1)
	{
		// �v���C���[�̏��擾
		CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();

		if (pPlayer == NULL)
		{
			return;
		}

		D3DXVECTOR3 PlayerPos = pPlayer->GetPosition();

		// ���z�̒e�̈ʒu
		float fPosBulletX = PlayerPos.x + cosf(m_rot.z) * sinf(m_rot.y) * -fDistance;
		float fPosBulletZ = PlayerPos.z + cosf(m_rot.z) * cosf(m_rot.y) * -fDistance;

		// �����擾
		bool bLand = false;
		float fHeight = CGame::GetElevation()->GetHeight(D3DXVECTOR3(fPosBulletX, 0.0f, fPosBulletZ), bLand);

		if (m_fHeightMaxDest <= fHeight)
		{// �ő�̍������X�V������

			// �����̉����������ۑ�
			float fDistanceRatio = fDistance / (m_fDistance);

			// �ڕW�̍ő卂���ۑ�
			m_fHeightMaxDest = fHeight * (fDistanceRatio + 1.0f);
		}

		// �������Z
		fDistance += 10.0f;

		if (fDistance >= m_fDistance)
		{// �����𒴂�����I���
			break;
		}
	}

	// �ڕW�̎��_�X�V
	if (m_fHeightMaxDest > m_posVDest.y)
	{
		// �ő�̍����␳
		m_fHeightMax += (m_fHeightMaxDest - m_fHeightMax) * 0.08f;

		m_posVDest.y = m_fHeightMax;

		// �����̍���
		m_fDiffHeightSave += m_fHeightMax - m_posV.y;
	}

	// �␳����
	fHosei = POSV_HOSEI;
	if (m_nCntChaseType != -1)
	{
		float fBairitu = 1.0f - ((float)m_nCntChaseType / 150.0f);
		fHosei *= fBairitu;
	}
	m_posV += (m_posVDest - m_posV) * fHosei;
}

//==================================================================================
// �}�b�v�ɉ������Ǐ]
//==================================================================================
void CCamera::ChaseMap(void)
{
	// �v���C���[�̏��擾
	CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();
	if (pPlayer == NULL)
	{// NULL��������
		return;
	}

	D3DXVECTOR3 PlayerPos = pPlayer->GetPosition();
	// �}�b�v�}�l�[�W���̎擾
	CMapManager *pMapManager = CGame::GetMapManager();
	if (pMapManager == NULL)
	{// NULL��������
		return;
	}

	// �}�b�v���擾
	int nMapIdx = pPlayer->GetMapIndex();
	float fMapRatio = pPlayer->GetMapPointRatio();
	float fMapMoveValue = pPlayer->GetMapMoveValue();

	// �Ȑ����ׂ�4�_
	int nP0 = nMapIdx;
	int nP1 = nMapIdx + 1;
	int nP2 = nMapIdx + 2;

	// �ڕW�n�_
	D3DXVECTOR3 TargetPoint0 = pMapManager->GetControlPoint(nP0);
	D3DXVECTOR3 TargetPoint1 = pMapManager->GetControlPoint(nP1);
	D3DXVECTOR3 TargetPoint2 = pMapManager->GetControlPoint(nP2);

	int nAngle = 1;
	if (pPlayer->GetMoveAngle() == CObject::ANGLE_LEFT)
	{
		nAngle = -1;
	}

	// ������̒n�_�擾
	float fMoveValue = fMapMoveValue + 50.0f * nAngle;
	D3DXVECTOR3 DestPoint = pMapManager->GetTargetPosition(nMapIdx, fMoveValue);

	// �x�N�g��
	D3DXVECTOR3 vec = mylib_const::DEFAULT_VECTOR3;
	D3DXVECTOR3 newvec = mylib_const::DEFAULT_VECTOR3;

	// 2�_�Ԃ̋����擾
	float fPosLength = GetPosLength(TargetPoint1, TargetPoint2);

	// �x�N�g��
	if (fMapMoveValue < fMoveValue)
	{
		vec = DestPoint - PlayerPos;
	}
	else
	{
		vec = PlayerPos - DestPoint;
	}
	D3DXVec3Normalize(&vec, &vec);

	// 90�x�X����
	newvec.x = vec.z;
	newvec.z = -vec.x;

	// �ڕW�̎��_�̌���
	m_rotVDest.y = atan2f((0.0f - newvec.x), (0.0f - newvec.z));

	//���݂ƖڕW�̍��������߂�
	float fRotDiff = m_rotVDest.y - m_rot.y;
	RotNormalize(fRotDiff);

	float fHosei = ROTHOSEI;
	if (m_nCntChaseType != -1)
	{
		float fBairitu = 1.0f - ((float)m_nCntChaseType / 150.0f);
		fHosei *= fBairitu;
	}

	m_rot.y += fRotDiff * fHosei;
	RotNormalize(m_rot.y);

	// ���_�̑������
	m_posVDest.x = m_posR.x + cosf(m_rot.z) * sinf(m_rot.y) * -m_fDistance;
	m_posVDest.z = m_posR.z + cosf(m_rot.z) * cosf(m_rot.y) * -m_fDistance;
	m_posVDest.y = m_posR.y + sinf(m_rot.z) * -m_fDistance;


	float fDistance = 0.0f;
	m_fHeightMaxDest = m_posVDest.y;
	// �ڕW�̊p�x�����߂�
	float fRotDest = atan2f((m_posVDest.x - m_posR.x), (m_posVDest.z - m_posR.z));

	// �n�ʂƂ̐ڐG����
	while (1)
	{
		// �v���C���[�̏��擾
		CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();

		if (pPlayer == NULL)
		{
			return;
		}

		D3DXVECTOR3 PlayerPos = pPlayer->GetPosition();

		// ���z�̒e�̈ʒu
		float fPosBulletX = PlayerPos.x + cosf(m_rot.z) * sinf(m_rot.y) * -fDistance;
		float fPosBulletZ = PlayerPos.z + cosf(m_rot.z) * cosf(m_rot.y) * -fDistance;

		// �����擾
		bool bLand = false;
		float fHeight = CGame::GetElevation()->GetHeight(D3DXVECTOR3(fPosBulletX, 0.0f, fPosBulletZ), bLand);

		if (m_fHeightMaxDest <= fHeight)
		{// �ő�̍������X�V������

			// �����̉����������ۑ�
			float fDistanceRatio = fDistance / (m_fDistance);

			// �ڕW�̍ő卂���ۑ�
			m_fHeightMaxDest = fHeight * (fDistanceRatio + 1.0f);
		}

		// �������Z
		fDistance += 10.0f;

		if (fDistance >= m_fDistance)
		{// �����𒴂�����I���
			break;
		}
	}

	// �ڕW�̎��_�X�V
	if (m_fHeightMaxDest > m_posVDest.y)
	{
		// �ő�̍����␳
		m_fHeightMax += (m_fHeightMaxDest - m_fHeightMax) * 0.08f;

		m_posVDest.y = m_fHeightMax;

		// �����̍���
		m_fDiffHeightSave += m_fHeightMax - m_posV.y;
	}

	fHosei = POSV_HOSEI;
	if (m_nCntChaseType != -1)
	{
		float fBairitu = 1.0f - ((float)m_nCntChaseType / 150.0f);
		fHosei *= fBairitu;
	}
	// �␳����
	m_posV += (m_posVDest - m_posV) * fHosei;

}

//==================================================================================
// �Ǐ]�Ȃ�
//==================================================================================
void CCamera::ChaseNone(void)
{


	// �}�b�v�}�l�[�W���̎擾
	CMapManager *pMapManager = CGame::GetMapManager();
	if (pMapManager == NULL)
	{// NULL��������
		return;
	}

	// �}�b�v���擾
	int nMapIdx = 37;
	float fMapRatio = 0.0f;
	float fMapMoveValue = 0.0f;

	// �Ȑ����ׂ�4�_
	int nP0 = nMapIdx;
	int nP1 = nMapIdx + 1;
	int nP2 = nMapIdx + 2;

	// �ڕW�n�_
	D3DXVECTOR3 TargetPoint0 = pMapManager->GetControlPoint(nP0);
	D3DXVECTOR3 TargetPoint1 = pMapManager->GetControlPoint(nP1);
	D3DXVECTOR3 TargetPoint2 = pMapManager->GetControlPoint(nP2);

	int nAngle = 1;

	// ������̒n�_�擾
	float fMoveValue = fMapMoveValue + 50.0f * nAngle;
	D3DXVECTOR3 DestPoint = pMapManager->GetTargetPosition(nMapIdx, fMoveValue);

	// �x�N�g��
	D3DXVECTOR3 vec = mylib_const::DEFAULT_VECTOR3;
	D3DXVECTOR3 newvec = mylib_const::DEFAULT_VECTOR3;

	// 2�_�Ԃ̋����擾
	float fPosLength = GetPosLength(TargetPoint1, TargetPoint2);

	// �x�N�g��
	vec = TargetPoint2 - TargetPoint1;
	D3DXVec3Normalize(&vec, &vec);

	// 90�x�X����
	newvec.x = vec.z;
	newvec.z = -vec.x;

	// �ڕW�̎��_�̌���
	m_rotVDest.y = atan2f((0.0f - newvec.x), (0.0f - newvec.z));

	//���݂ƖڕW�̍��������߂�
	float fRotDiff = m_rotVDest.y - m_rot.y;
	RotNormalize(fRotDiff);

	m_rot.y += fRotDiff * (ROTHOSEI * 0.1f);
	RotNormalize(m_rot.y);






	// �����_�̑������
	m_posRDest.x = m_TargetPos.x;
	m_posRDest.z = m_TargetPos.z;
	m_posRDest.y = m_TargetPos.y + 100.0f;

	// �␳����
	m_posR += (m_posRDest - m_posR) * (POSR_HOSEI * 0.1f);

	// ���_�̑������
	m_posVDest.x = m_posR.x + cosf(m_rot.z) * sinf(m_rot.y) * -m_fDistance;
	m_posVDest.z = m_posR.z + cosf(m_rot.z) * cosf(m_rot.y) * -m_fDistance;
	m_posVDest.y = m_posR.y + sinf(m_rot.z) * -m_fDistance;

	// �␳����
	m_posV += (m_posVDest - m_posV) * (POSV_HOSEI * 0.1f);
}

//==================================================================================
// �J�����̔w�ʎ����Ǐ]
//==================================================================================
void CCamera::MoveCameraFollow(void)
{

}

//==================================================================================
// �J�����̐ݒ菈��
//==================================================================================
void CCamera::SetCamera(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// �r���[�|�[�g�̐ݒ�
	pDevice->SetViewport(&m_viewport);

	// �v���W�F�N�V�����}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_mtxProjection);

	// �v���W�F�N�V�����}�g���b�N�X�̍쐬
	D3DXMatrixPerspectiveFovLH(&m_mtxProjection,
								D3DXToRadian(45.0f),	// ����p
								(float)m_viewport.Width / (float)m_viewport.Height,	// �A�X�y�N�g��
								10.0f,		// ���s���̐���
								20000.0f);	// ���s���̐���

	// �v���W�F�N�V�����}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_PROJECTION, &m_mtxProjection);

	// �r���[�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_mtxView);

	// �r���[�}�g���b�N�X�̍쐬
	D3DXMatrixLookAtLH(&m_mtxView,
		&m_posV,	// ���_
		&m_posR,	// �����_
		&m_vecU);	// ������x�N�g��

	// �r���[�}�g���b�N�X�̐ݒ�
	pDevice->SetTransform(D3DTS_VIEW, &m_mtxView);

}

//==================================================================================
// �ڕW�̒����ݒ�
//==================================================================================
void CCamera::SetLenDest(float fLength, int nCntTime, float DecrementValue, float fCorrection)
{
	// �ڕW�̋����ݒ�
	m_fDestDistance = fLength;
	m_nCntDistance = nCntTime;
	m_nOriginCntDistance = m_nCntDistance;	// ���̋����J�E���^�[
	m_fDistanceCorrection = fCorrection;	// �����␳�W��
	m_fDistanceDecrementValue = DecrementValue;	// ������
}

//==================================================================================
// �J�����̐U���ݒ菈��
//==================================================================================
void CCamera::SetShake(int nTime, float fLength, float fLengthY)
{
	// �U����Ԃɐݒ�
	m_state = CAMERASTATE_SHAKE;
	m_nCntState = nTime;		// ��ԑJ�ڃJ�E���^�[
	m_nShakeLength = fLength;	// �h��̑傫��
	m_nShakeLengthY = fLengthY;	// Y�̗h��̑傫��
}

//==================================================================================
// �ڕW�̈ʒu�ݒ�
//==================================================================================
void CCamera::SetTargetPosition(const D3DXVECTOR3 pos)
{
	// �ڕW�̈ʒu
	m_TargetPos = pos;
}

//==================================================================================
// �ڕW�̈ʒu�擾
//==================================================================================
D3DXVECTOR3 CCamera::GetTargetPosition(void)
{
	return m_TargetPos;
}

//==================================================================================
// �Ǐ]�̎�ސݒ�
//==================================================================================
void CCamera::SetChaseType(CHASETYPE type)
{
	m_ChaseType = type;
}

//==================================================================================
// �Ǐ]�̎�ގ擾
//==================================================================================
CCamera::CHASETYPE CCamera::GetChaseType(void)
{
	return m_ChaseType;
}


//==================================================================================
// �J�����̐U������
//==================================================================================
void CCamera::Shake(void)
{
	// �����擾
	int nLength = (int)m_nShakeLength;
	int nLengthY = (int)m_nShakeLengthY;

	m_fMoveShake = (float)Random(-nLength, nLength) * sinf((float)Random(-314, 314) * 0.01f);	// �h��̈ړ���
	m_fMoveShakeY = (float)Random(-nLengthY, nLengthY);						// Y�̗h��̈ړ���

	// ���_�̑������
	m_posV.x += (float)Random(-nLength, nLength) * sinf((float)Random(-314, 314) * 0.01f);
	m_posV.y += (float)Random(-nLengthY, nLengthY);
	m_posV.z += (float)Random(-nLength, nLength) * cosf((float)Random(-314, 314) * 0.01f);

	// �����_�̑������
	m_posR.x += (float)Random(-nLength, nLength) * sinf((float)Random(-314, 314) * 0.01f);
	m_posR.y += (float)Random(-nLengthY, nLengthY);
	m_posR.z += (float)Random(-nLength, nLength) * cosf((float)Random(-314, 314) * 0.01f);

	// �����␳
	InertiaCorrection(m_nShakeLength, 0.0f, 0.1f);
	InertiaCorrection(m_nShakeLengthY, 0.0f, 0.1f);
}

//==================================================================================
// �J�����̏�ԍX�V����
//==================================================================================
void CCamera::UpdateState(void)
{
	switch (m_state)
	{
	case CAMERASTATE_NONE:
		break;

	case CAMERASTATE_SHAKE:

		if (m_nCntState % 3 == 0)
		{
			// ��ʗh��
			Shake();
		}
		else
		{
			// ���_�̑������
			m_posV.x += m_fMoveShake;
			m_posV.y += m_fMoveShakeY;
			m_posV.z += m_fMoveShake;

			// �����_�̑������
			m_posR.x += m_fMoveShake;
			m_posR.y += m_fMoveShakeY;
			m_posR.z += m_fMoveShake;

			// �����␳
			InertiaCorrection(m_nShakeLength, 0.0f, 0.01f);
			InertiaCorrection(m_nShakeLengthY, 0.0f, 0.01f);
		}

		// ��ԑJ�ڃJ�E���^�[���Z
		m_nCntState--;

		if (m_nCntState <= 0)
		{
			m_nCntState = 0;
			m_state = CAMERASTATE_NONE;
		}
		break;
	}
}

//==========================================================================
// ���Z�b�g
//==========================================================================
void CCamera::Reset(CScene::MODE mode)
{
	
	m_bFollow = true;										// �Ǐ]���邩�ǂ���

	switch (mode)
	{
	case CScene::MODE_NONE:
		break;

	case CScene::MODE_TITLE:
		ResetTitle();
		break;

	case CScene::MODE_TUTORIAL:
		ResetGame();
		break;

	case CScene::MODE_GAME:
		ResetGame();
		break;

	case CScene::MODE_RESULT:
		ResetResult();
		break;

	case CScene::MODE_RANKING:
		ResetRanking();
		break;

	case CScene::MODE_MAX:
		break;

	default:
		break;
	}

	// ���_�̑������
	SetCameraV();
}

//==========================================================================
// �Q�[���̃��Z�b�g
//==========================================================================
void CCamera::ResetGame(void)
{
	m_posR = D3DXVECTOR3(0.0f, 200.0f, 0.0f);				// �����_(�������ꏊ)
	m_posV = D3DXVECTOR3(0.0f, 300.0f, m_posR.z + -400.0f);	// ���_(�J�����̈ʒu)
	m_posVDest = m_posV;									// �ڕW�̎��_
	m_posRDest = m_posR;									// �ڕW�̒����_
	m_vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);					// ������x�N�g��
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);					// �ړ���
	m_rot = D3DXVECTOR3(0.0f, 0.0f, -0.15f);					// ����
	m_rotVDest = m_rot;										// �ڕW�̎��_�̌���
	m_TargetPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);			// �ڕW�̈ʒu
	m_fDistance = START_CAMERALEN;							// ����
	m_fDestDistance = START_CAMERALEN;						// �ڕW�̋���
	m_fOriginDistance = START_CAMERALEN;					// ���̋���
	m_fDiffHeight = 0.0f;									// �����̍���
	m_fDiffHeightSave = 0.0f;								// �����̍����ۑ��p
	m_fDiffHeightDest = 0.0f;								// �ڕW�̍����̍���
	m_state = CAMERASTATE_NONE;								// ���
	m_nCntState = 0;							// ��ԃJ�E���^�[
	m_nCntDistance = 0;							// �����J�E���^�[
	m_nOriginCntDistance = 0;					// ���̋����J�E���^�[
	m_fDistanceCorrection = 0;					// �����̊����␳�W��
	m_fDistanceDecrementValue = 0.0f;			// �����̌����W��
	m_fHeightMaxDest = 0.0f;					// �J�����̍ő卂���̖ڕW
	m_ChaseType = CHASETYPE_MAP;				// �Ǐ]�̎��
	m_OldChaseType = m_ChaseType;			// �O��̒Ǐ]�̎��
}

//==========================================================================
// ���Z�b�g
//==========================================================================
void CCamera::ResetBoss(void)
{
	m_posR = CManager::GetInstance()->GetScene()->GetPlayer()->GetPosition();				// �����_(�������ꏊ)
	m_posV = D3DXVECTOR3(0.0f, 300.0f, m_posR.z + -400.0f);	// ���_(�J�����̈ʒu)
	m_posVDest = m_posV;									// �ڕW�̎��_
	m_posRDest = m_posR;									// �ڕW�̒����_
	m_vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);					// ������x�N�g��
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);					// �ړ���
	m_rot = D3DXVECTOR3(0.0f, 0.0f, -0.25f);					// ����
	m_rotVDest = m_rot;										// �ڕW�̎��_�̌���
	m_TargetPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);			// �ڕW�̈ʒu
	m_fDistance = 1600.0f;							// ����
	m_fDestDistance = m_fDistance;						// �ڕW�̋���
	m_fOriginDistance = m_fDistance;					// ���̋���
	m_fDiffHeight = 0.0f;									// �����̍���
	m_fDiffHeightSave = 0.0f;								// �����̍����ۑ��p
	m_fDiffHeightDest = 0.0f;								// �ڕW�̍����̍���
	m_state = CAMERASTATE_NONE;								// ���
	m_nCntState = 0;							// ��ԃJ�E���^�[
	m_nCntDistance = 0;							// �����J�E���^�[
	m_nOriginCntDistance = 0;					// ���̋����J�E���^�[
	m_fDistanceCorrection = 0;					// �����̊����␳�W��
	m_fDistanceDecrementValue = 0.0f;			// �����̌����W��
	m_fHeightMaxDest = 0.0f;					// �J�����̍ő卂���̖ڕW
	m_ChaseType = CHASETYPE_MAP;				// �Ǐ]�̎��
	m_OldChaseType = m_ChaseType;			// �O��̒Ǐ]�̎��

	// �v���C���[�̏��擾
	CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();
	if (pPlayer == NULL)
	{// NULL��������
		return;
	}

	D3DXVECTOR3 PlayerPos = pPlayer->GetPosition();

	// �����_�̑��
	m_posR = D3DXVECTOR3(PlayerPos.x, PlayerPos.y + 100.0f, PlayerPos.z);	// �����_(�������ꏊ)

	// �ڕW�̎��_�̌���
	m_rot.y = atan2f((m_TargetPos.x - PlayerPos.x), (m_TargetPos.z - PlayerPos.z));
	m_rotVDest.y = m_rot.y;

	// ���_�̑��
	m_posV.x = m_posR.x + cosf(m_rot.z) * sinf(m_rot.y) * -m_fDistance;
	m_posV.z = m_posR.z + cosf(m_rot.z) * cosf(m_rot.y) * -m_fDistance;
	m_posV.y = m_posR.y + sinf(m_rot.z) * -m_fDistance;
	m_posVDest = m_posV;								// �ڕW�̎��_
	m_posRDest = m_posR;								// �ڕW�̒����_
}

//==========================================================================
// �^�C�g���̃��Z�b�g
//==========================================================================
void CCamera::ResetTitle(void)
{
	m_posR = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// �����_(�������ꏊ)
	m_posV = D3DXVECTOR3(2401.44f, 1542.82f, -2355.46f);	// ���_(�J�����̈ʒu)
	m_posVDest = m_posV;								// �ڕW�̎��_
	m_posRDest = m_posR;								// �ڕW�̒����_
	m_vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);				// ������x�N�g��
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// �ړ���
	m_rot = TITLECAMERAROT_NONE;						// ����
	m_rotVDest = m_rot;									// �ڕW�̎��_�̌���
	m_TargetPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// �ڕW�̈ʒu
	m_fDistance = TITLE_LEN_DEST;						// ����
	m_fDestDistance = TITLE_LEN_DEST;					// �ڕW�̋���
	m_fOriginDistance = TITLE_LEN_DEST;					// ���̋���
	m_fDiffHeight = 0.0f;								// �����̍���
	m_fDiffHeightSave = 0.0f;							// �����̍����ۑ��p
	m_fDiffHeightDest = 0.0f;							// �ڕW�̍����̍���
	m_state = CAMERASTATE_NONE;							// ���
	m_nCntState = 0;									// ��ԃJ�E���^�[
	m_nCntDistance = 0;									// �����J�E���^�[
	m_nOriginCntDistance = 0;							// ���̋����J�E���^�[
	m_fDistanceCorrection = 0;							// �����̊����␳�W��
	m_fDistanceDecrementValue = 0.0f;					// �����̌����W��
	m_fHeightMaxDest = 0.0f;							// �J�����̍ő卂���̖ڕW
	m_ChaseType = CHASETYPE_MAP;				// �Ǐ]�̎��
	m_OldChaseType = m_ChaseType;			// �O��̒Ǐ]�̎��
}

//==========================================================================
// ���U���g���Z�b�g
//==========================================================================
void CCamera::ResetResult(void)
{
	m_vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);				// ������x�N�g��
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// �ړ���
	//m_rot = D3DXVECTOR3(0.0f, 0.0f, -0.35f);			// ����
	m_rot = D3DXVECTOR3(0.0f, 0.03f, -0.15f);			// ����
	m_rotVDest = m_rot;									// �ڕW�̎��_�̌���
	m_TargetPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// �ڕW�̈ʒu
	m_fDistance = RESULT_LEN;						// ����
	m_fDestDistance = RESULT_LEN;					// �ڕW�̋���
	m_fOriginDistance = RESULT_LEN;					// ���̋���
	m_fDiffHeight = 0.0f;								// �����̍���
	m_fDiffHeightSave = 0.0f;							// �����̍����ۑ��p
	m_fDiffHeightDest = 0.0f;							// �ڕW�̍����̍���
	m_state = CAMERASTATE_NONE;							// ���
	m_nCntState = 0;									// ��ԃJ�E���^�[
	m_nCntDistance = 0;									// �����J�E���^�[
	m_nOriginCntDistance = 0;							// ���̋����J�E���^�[
	m_fDistanceCorrection = 0;							// �����̊����␳�W��
	m_fDistanceDecrementValue = 0.0f;					// �����̌����W��
	m_fHeightMaxDest = 0.0f;							// �J�����̍ő卂���̖ڕW
	m_ChaseType = CHASETYPE_MAP;				// �Ǐ]�̎��
	m_OldChaseType = m_ChaseType;			// �O��̒Ǐ]�̎��

	// �����_�̑��
	m_posR = D3DXVECTOR3(-57.76f, 312.0f, 76.42f);	// �����_(�������ꏊ)

	// ���_�̑��
	m_posV.x = m_posR.x + cosf(m_rot.z) * sinf(m_rot.y) * -m_fDistance;
	m_posV.z = m_posR.z + cosf(m_rot.z) * cosf(m_rot.y) * -m_fDistance;
	m_posV.y = m_posR.y + sinf(m_rot.z) * -m_fDistance;
	m_posVDest = m_posV;								// �ڕW�̎��_
	m_posRDest = m_posR;								// �ڕW�̒����_
}

//==========================================================================
// �����L���O�̃��Z�b�g
//==========================================================================
void CCamera::ResetRanking(void)
{
	m_posR = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// �����_(�������ꏊ)
	m_posV = D3DXVECTOR3(2401.44f, 1542.82f, -2355.46f);	// ���_(�J�����̈ʒu)
	m_posVDest = m_posV;								// �ڕW�̎��_
	m_posRDest = m_posR;								// �ڕW�̒����_
	m_vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);				// ������x�N�g��
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// �ړ���
	m_TargetPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// �ڕW�̈ʒu
	m_fDistance = RANKNIG_LEN_DEST;						// ����
	m_fDestDistance = RANKNIG_LEN_DEST;					// �ڕW�̋���
	m_fOriginDistance = RANKNIG_LEN_DEST;				// ���̋���
	m_fDiffHeight = 0.0f;								// �����̍���
	m_fDiffHeightSave = 0.0f;							// �����̍����ۑ��p
	m_fDiffHeightDest = 0.0f;							// �ڕW�̍����̍���
	m_state = CAMERASTATE_NONE;							// ���
	m_nCntState = 0;									// ��ԃJ�E���^�[
	m_nCntDistance = 0;									// �����J�E���^�[
	m_nOriginCntDistance = 0;							// ���̋����J�E���^�[
	m_fDistanceCorrection = 0;							// �����̊����␳�W��
	m_fDistanceDecrementValue = 0.0f;					// �����̌����W��
	m_fHeightMaxDest = 0.0f;							// �J�����̍ő卂���̖ڕW
	m_ChaseType = CHASETYPE_MAP;				// �Ǐ]�̎��
	m_OldChaseType = m_ChaseType;			// �O��̒Ǐ]�̎��

	m_rot = RANKINGROT_NONE;							// ����
	m_rotVDest = m_rot;									// �ڕW�̎��_�̌���

	// �����_�̑��
	m_posR = D3DXVECTOR3(0.0f, 400.0f, 0.0f);			// �����_(�������ꏊ)

	// ���_�̑��
	m_posV.x = m_posR.x + cosf(m_rot.z) * sinf(m_rot.y) * -m_fDistance;
	m_posV.z = m_posR.z + cosf(m_rot.z) * cosf(m_rot.y) * -m_fDistance;
	m_posV.y = m_posR.y + sinf(m_rot.z) * -m_fDistance;
	m_posVDest = m_posV;								// �ڕW�̎��_
	m_posRDest = m_posR;								// �ڕW�̒����_
}

//==========================================================================
// �J�����̈ʒu�擾
//==========================================================================
D3DXVECTOR3 CCamera::GetPositionV(void) const
{
	return m_posV;
}

//==========================================================================
// �J�����̒����_�擾
//==========================================================================
D3DXVECTOR3 CCamera::GetPositionR(void) const
{
	return m_posR;
}

//==========================================================================
// �����ݒ�
//==========================================================================
void CCamera::SetRotation(const D3DXVECTOR3 rot)
{
	m_rot = rot;
}

//==========================================================================
// �����擾
//==========================================================================
D3DXVECTOR3 CCamera::GetRotation(void) const
{
	return m_rot;
}

//==========================================================================
// �ڕW�̌����ݒ�
//==========================================================================
void CCamera::SetDestRotation(const D3DXVECTOR3 rot)
{
	m_rotVDest = rot;
}

//==========================================================================
// �ڕW�̌����擾
//==========================================================================
D3DXVECTOR3 CCamera::GetDestRotation(void)
{
	return m_rotVDest;
}

//==========================================================================
// ���ɂȂ�J�����̋����ݒ�
//==========================================================================
void CCamera::SetOriginDistance(float fDistance)
{
	m_fOriginDistance = fDistance;
}

//==========================================================================
// ���ɂȂ�J�����̋����擾
//==========================================================================
float CCamera::GetOriginDistance(void)
{
	return m_fOriginDistance;
}
