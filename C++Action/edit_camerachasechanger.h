//=============================================================================
// 
//  �J�����Ǐ]��ޕύX�G�f�B�b�g�w�b�_�[ [edit_camerachasechanger.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _EDIT_CAMERACHASECHANGER_H_
#define _EDIT_CAMERACHASECHANGER_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "object.h"
#include "camera.h"


//==========================================================================
// �N���X��`
//==========================================================================
// �J�����Ǐ]��ޕύX�G�f�B�b�g�N���X��`
class CEditCameraChaseChanger
{
public:

	CEditCameraChaseChanger();
	~CEditCameraChaseChanger();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void);
	HRESULT Init(const char *pFileName);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CEditCameraChaseChanger *Create(void);	// ��������

private:

	void Control(D3DXVECTOR3 &pos);	// ����
	void Grab(void);	// �͂�
	void Delete(void);	// �폜

	int m_nIdxMapPoint;			// �}�b�v�|�C���g�̃C���f�b�N�X�ԍ�
	float m_fPointRatio;		// �ړ�����
	float m_fMoveValue;			// �ړ���
	D3DXVECTOR3 m_pos;	// �ʒu
	CCamera::CHASETYPE m_ChaseType;			// �Ǐ]�̎��
};



#endif