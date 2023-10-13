//=============================================================================
// 
//  ����_�G�f�B�b�g�w�b�_�[ [edit_controlpoint.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _EDIT_CONTROLPOINT_H_
#define _EDIT_CONTROLPOINT_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "object.h"


//==========================================================================
// �N���X��`
//==========================================================================
// �G�f�B�b�g�N���X��`
class CEditControlPoint
{
public:

	CEditControlPoint();
	~CEditControlPoint();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void);
	HRESULT Init(const char *pFileName);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CEditControlPoint *Create();

private:

	void Control(D3DXVECTOR3 &pos);	// ����
	void Grab(void);
	void Delete(void);

	D3DXVECTOR3 m_pos;	// �ʒu
};



#endif