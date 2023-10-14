//=============================================================================
// 
//  �����w�b�_�[ [multinumber.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _MULTINUMBER_H_
#define _MULTINUMBER_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "object2D.h"
#include "number.h"

//==========================================================================
// �O���錾
//==========================================================================
class CNumber;

//==========================================================================
// �N���X��`
//==========================================================================
// �w�i�N���X��`
class CMultiNumber
{
public:

	CMultiNumber(int nPriority = 6);
	~CMultiNumber();

	static CMultiNumber *Create(D3DXVECTOR3 pos, D3DXVECTOR2 size, int nNum, CNumber::EObjectType objtype);

	// �����o�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void AddNumber(int nValue);
	void SetValue(void);		// �l�̐ݒ菈��
	void SetValue(int nValue);	// �l�̐ݒ菈��(�I�[�o�[���[�h)
	void SetPosition(const D3DXVECTOR3 pos);	// �ʒu�ݒ�
	D3DXVECTOR3 GetPosition(void) const;		// �ʒu�擾
	void SetColor(const D3DXCOLOR col);			// �F�ݒ�
	D3DXCOLOR GetColor(void) const;				// �F�擾
	void Release(void);	// �J������

private:

	// �����o�ϐ�
	D3DXVECTOR3 m_pos;				// �ʒu
	D3DXCOLOR m_col;				// �F
	D3DXVECTOR2 size;				// �����̃T�C�Y
	int m_nNum;						// ����
	int m_nNumNumber;				// �����̐�
	CNumber **m_ppMultiNumber;		// �����̃I�u�W�F�N�g
	CNumber::EObjectType m_objType;	// �I�u�W�F�N�g�̎��

};

#endif