//=============================================================================
// 
//  �G�̈��̓����}�l�[�W�� [enemyfixedmove_manager.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _ENEMYFIXEDMOVE_MANAGER_H_
#define _ENEMYFIXEDMOVE_MANAGER_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "constans.h"
#include "object.h"

// �O���錾
class CEnemy;

//==========================================================================
// �N���X��`
//==========================================================================
// �G�̈��̓����}�l�[�W���N���X��`
class CEnemyFixedMoveManager
{
protected:

	// �\���̒�`
	struct sFixedInfo
	{
		int nFrame;			// �t���[����
		D3DXVECTOR3 move;	// �ړ���
	};

	// ���s��
	struct sFixedMove
	{
		sFixedInfo info[mylib_const::MAX_FIXEDMOVE_INFO];	// �s�����
		int nNumKey;		// �L�[�̐�
	};

public:

	CEnemyFixedMoveManager();
	~CEnemyFixedMoveManager();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);

	void Set(int nType);	// �ݒ菈��
	void StartSet(int nKey, int nFrame);	// �X�^�[�g���̐ݒ�
	D3DXVECTOR3 UpdatePosition(CEnemy *pEnemy);	// �ʒu�X�V
	static CEnemyFixedMoveManager *Create(void);
	static HRESULT ReadText(const std::string pTextFile);	// �O���t�@�C���ǂݍ��ݏ���

private:

	// �����o�֐�
	void UpdateKeyFrame(void);	// �L�[�t���[���X�V

	// �����o�ϐ�
	static sFixedMove m_aFxedMove[mylib_const::MAX_FIXEDMOVE];	// ���̓����̏��
	int m_nType;				// ���݂̃��[�V�����̎��
	int m_nPatternKey;			// ���ڂ̃L�[��
	int m_nCntFrame;			// �t���[���̃J�E���g
	int m_nCntAllFrame;			// �S�ẴJ�E���g
	int m_nMaxAllFrame;			// �S�ẴJ�E���g�̍ő�l
	static int m_nNumAll;				// ���[�V�����̑���
};



#endif