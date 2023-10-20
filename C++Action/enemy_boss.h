//=============================================================================
// 
//  �{�X�w�b�_�[ [enemy_boss.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _ENEMYBOSS_H_
#define _ENEMYBOSS_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "enemy.h"

// �O���錾

//==========================================================================
// �N���X��`
//==========================================================================
// �{�X�N���X��`
class CEnemyBoss : public CEnemy
{
public:

	CEnemyBoss(int nPriority = mylib_const::ENEMY_PRIORITY);
	~CEnemyBoss();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Kill(void) override;

	CEnemyBoss *GetEnemy(void);
protected:

private:
	typedef enum
	{
		MOTION_DEF = 0,		// �j���[�g�������[�V����
		MOTION_WALK,		// �ړ����[�V����
		MOTION_BULLETATK,			// �U��
		MOTION_KNOCKBACK,	// ���ꃂ�[�V����
		MOTION_FADEOUT,		// �A�҃��[�V����
		MOTION_MAX
	}MOTION;

	enum ATKTYPE
	{
		ATKTYPE_BULLET = 0,	// �e���ˍU��
		ATKTYPE_MAX
	};

	// �s���̍\���̒�`
	struct BOSSACT
	{
		int nCntWait;		// �ҋ@�J�E���^�[
		ATKTYPE AtkType;	// �U���̎��
	};

	// �I�[�o�[���C�h���ꂽ�֐�
	void UpdateByType(void) override;	// ��ޕʍX�V����
	void ProcessLanding(void) override;	// ���n������
	void AttackAction(int nModelNum, CMotion::AttackInfo ATKInfo) override;	// �U��������
	void StateAttack(void) override;	// �U������
	void MotionSet(void) override;		// ���[�V�����̐ݒ�
	void StateWait(void) override;		// �ҋ@����

	// ��ԍX�V�n
	void Spawn(void) override;		// �X�|�[��

	// �����o�֐�
	void ChaseMove(float fMove);		// �ǂ��|���ړ�
	void ChangeToAttackState(void);		// �U����Ԉڍs����
	void UpdateByAttack(void);			// �U���ʏ���
	void UpdateAttackBullet(void);		// �e�U��

	// �����o�ϐ�
	BOSSACT m_sAct;	// �{�X�̍s��
};



#endif