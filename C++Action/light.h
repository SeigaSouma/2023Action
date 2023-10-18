//=============================================================================
//
// ���C�g���� [light.h]
// Author : ���n�Ή�
//
//=============================================================================
#ifndef _LIGHT_H_		// ���̃}�N����`������Ă��Ȃ�������
#define _LIGHT_H_		// ��d�C���N���[�h�h�~�̃}�N�����`����

#include "main.h"

//==========================================================================
// �N���X��`
//==========================================================================
// ���C�g�N���X��`
class CLight
{
public:

	CLight();
	~CLight();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);

private:

	// �}�N����`
#define MAX_LIGHT	(3)	// ���C�g�̍ő吔
	
	D3DLIGHT9 m_aLight[MAX_LIGHT];		// ���C�g�\����
};

#endif