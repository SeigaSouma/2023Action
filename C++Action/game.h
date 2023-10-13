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

//==========================================================================
// �N���X��`
//==========================================================================
// �Q�[���N���X��`
class CGame : public CScene
{
public:

	CGame();
	~CGame();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void Reset(void);
	static CScore *GetScore(void);
	static CTimer *GetTimer(void);
	static CPowerGauge *GetPowerGauge(void);
	static CEditControlPoint *GetEditControlPoint(void);
	static CBulletManager *GetBulletManager(void);

private:
	
	static CScore *m_pScore;					// �X�R�A�̃I�u�W�F�N�g
	static CTimer *m_pTimer;					// �^�C�}�[�̃I�u�W�F�N�g
	static CPowerGauge *m_pPowerGauge;			// �p���[�Q�[�W�̃I�u�W�F�N�g
	static CEditControlPoint *m_pEditControlPoint;	// ����_�G�f�B�^�[�̃I�u�W�F�N�g
	static CBulletManager *m_pBulletManager;		// �e�}�l�[�W���̃I�u�W�F�N�g
};



#endif