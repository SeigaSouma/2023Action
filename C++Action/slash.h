//=============================================================================
// 
//  �a���w�b�_�[ [slash.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _SLASH_H_
#define _SLASH_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "impactwave.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �a���N���X��`
class CSlash : public CImpactWave
{
public:
	CSlash(int nPriority = 7);
	~CSlash();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CSlash *Create(D3DXVECTOR3 pos, D3DXVECTOR3 Parentrot, D3DXVECTOR3 rot, D3DXCOLOR col, float fWidth, float fCenterDistance, int nLife, float fAddSizeValue, int nTexType, bool bAddBlend, ANGLE angle);

private:

	void UpdateTex(void);
	void Collision(void);	// �����蔻��
	bool IsHit(D3DXVECTOR3 TargetPos, float fTargetRadius);	// �p�x�t���̔���

	int m_nCntAnim;			// �A�j���[�V�����J�E���^�[
	int m_nPatternAnim;		// �A�j���[�V�����p�^�[��No.
	int m_nTexIdx;	// �e�N�X�`���̃C���f�b�N�X�ԍ�
	D3DXCOLOR m_colOrigin;	// ���̐F
	float m_fSizeDest;		// �ڕW�̃T�C�Y
	D3DXVECTOR3 collisionRotation;	// �����蔻��̌���
	CObject3D *m_pObj3D;
};



#endif