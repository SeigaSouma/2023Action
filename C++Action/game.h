//=============================================================================
// 
//  �Q�[���w�b�_�[ [game.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _GAME_H_
#define _GAME_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "scene.h"

//==========================================================================
// �O���錾
//==========================================================================
class CPlayer;
class CScore;
class CTimer;
class CPowerGauge;
class CEditControlPoint;
class CBulletManager;
class CEditCameraAxis;
class CEditCameraChaseChanger;
class CEditEnemyBase;
class CStage;

//==========================================================================
// �N���X��`
//==========================================================================
// �Q�[���N���X��`
class CGame : public CScene
{
public:

	// �G�f�B�b�g�^�C�v
	enum EEditType
	{
		EDITTYPE_OFF = 0,			// �I�t
		EDITTYPE_MAPCONTROL,		// �}�b�v�O��
		EDITTYPE_CAMERACHASE,		// �J�����Ǐ]
		EDITTYPE_CAMERAAXIS,		// �J������
		EDITTYPE_ENEMYBASE,			// �G���_
		EDITTYPE_MAX
	};

	CGame();
	~CGame();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void Reset(void);
	static EEditType GetEditType(void) { return m_EditType; }
	static CScore *GetScore(void);
	static CTimer *GetTimer(void);
	static CPowerGauge *GetPowerGauge(void);
	static CEditControlPoint *GetEditControlPoint(void);
	static CEditEnemyBase *GetEditEnemyBase(void);
	static CBulletManager *GetBulletManager(void);
	static CStage *GetStage(void);

private:

	// �����o�֐�
	void EditReset(void);

	// �����o�ϐ�
	static CScore *m_pScore;					// �X�R�A�̃I�u�W�F�N�g
	static CTimer *m_pTimer;					// �^�C�}�[�̃I�u�W�F�N�g
	static CPowerGauge *m_pPowerGauge;			// �p���[�Q�[�W�̃I�u�W�F�N�g
	static CEditControlPoint *m_pEditControlPoint;	// ����_�G�f�B�^�[�̃I�u�W�F�N�g
	static CBulletManager *m_pBulletManager;		// �e�}�l�[�W���̃I�u�W�F�N�g
	static CEditCameraAxis *m_pEditCameraAxis;		// �J�������G�f�B�^�[�̃I�u�W�F�N�g
	static CEditCameraChaseChanger *m_pEditCmaeraChaseChanger;	// �J�����Ǐ]�ύX�҃G�f�B�^�[�̃I�u�W�F�N�g
	static CEditEnemyBase *m_pEditEnemyBase;		// �G�̋��_�G�f�B�^�[
	static CStage *m_pStage;	// �X�e�[�W�̃I�u�W�F�N�g
	static EEditType m_EditType;		// �G�f�B�b�g�̎��
};



#endif