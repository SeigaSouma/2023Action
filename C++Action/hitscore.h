//=============================================================================
// 
//  �q�b�g�X�R�A�w�b�_�[ [hitscore.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _HITSCORE_H_
#define _HITSCORE_H_	// ��d�C���N���[�h�h�~

//==========================================================================
// �O���錾
//==========================================================================
class CMultiNumber;
class CObject2D;

//==========================================================================
// �N���X��`
//==========================================================================
// �q�b�g�X�R�A�N���X��`
class CHitScore
{
public:

	CHitScore(int nPriority = 7);
	~CHitScore();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void);
	HRESULT Init(D3DXVECTOR3 pos);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void Add(int nValue);
	void Reset(void);	// ���Z�b�g
	static CHitScore *Create(void);
	static CHitScore *Create(D3DXVECTOR3 pos);

private:

	// �����o�֐�
	void UpdateMove(void);
	void CalScore(void);	// �X�R�A�v�Z

	// �����o�ϐ�
	CMultiNumber *m_apNumber;	// �����̃I�u�W�F�N�g
	CObject2D *m_pObj2D;		// �I�u�W�F�N�g2D�̃I�u�W�F�N�g
	D3DXVECTOR3 m_posOrigin;	// ���̈ʒu
	int m_nTexIdx;				// �e�N�X�`���̃C���f�b�N�X�ԍ�
	int m_nCntCooltime;			// �N�[���^�C���̃J�E���^�[
};



#endif