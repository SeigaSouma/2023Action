//=============================================================================
// 
//  �V�[���w�b�_�[ [scene.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _SCENE_H_
#define _SCENE_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "elevation.h"

//==========================================================================
// �O���錾
//==========================================================================
class CXLoad;
class CEnemyManager;
class CMapManager;
class CPlayer;
class CCameraAxis;
class CCameraChaseChanger;
class CEnemyBase;

//==========================================================================
// �N���X��`
//==========================================================================
// �V�[���N���X��`
class CScene
{
public:
	// ���[�h�̗񋓌^��`
	enum MODE
	{
		MODE_NONE = 0,	// �^�C�g��
		MODE_TITLE,		// �^�C�g��
		MODE_TUTORIAL,	// �`���[�g���A��
		MODE_GAME,		// �Q�[��
		MODE_RESULT,	// ���U���g
		MODE_RANKING,	// �����L���O
		MODE_MAX
	};

	CScene();
	~CScene();

	// �I�[�o�[���C�h����֐�
	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);

	void ResetScene(void);	// �V�[���̃��Z�b�g
	static CScene *Create(MODE mode);
	static CXLoad *GetXLoad(void);				// X�t�@�C���̃f�[�^�擾
	static CElevation *GetElevation(void);	// �I�u�W�F�N�g3D�̎擾
	static CEnemyManager *GetEnemyManager(void);
	static CMapManager *GetMapManager(void);	// �}�b�v�}�l�[�W���擾
	static CCameraAxis *GetCameraAxis(void);	// �J�����̎��擾
	static CCameraChaseChanger *GetCameraChaseChanger(void);	// �J�����Ǐ]��ނ̕ύX�Ҏ擾
	static CEnemyBase *GetEnemyBase(void);		// �G�̋��_
	CPlayer *GetPlayer(void);
	void UninitPlayer();// �v���C���[�̏��擾
	MODE GetMode(void);		// ���݂̃��[�h�擾
private:

	MODE m_mode;				// ���݂̃��[�h
	CPlayer *m_pPlayer;					// �v���C���[�̃I�u�W�F�N�g
	static CXLoad *m_pXLoad;	// X�t�@�C���̃I�u�W�F�N�g
	static CElevation *m_pObject3DMesh;			// �I�u�W�F�N�g3D���b�V���̃I�u�W�F�N�g
	static CEnemyManager *m_pEnemyManager;		// �G�}�l�[�W���̃I�u�W�F�N�g
	static CMapManager *m_pMapManager;			// �}�b�v�}�l�[�W���̃I�u�W�F�N�g
	static CCameraAxis *m_pCameraAxis;	// �J�����̎��̃I�u�W�F�N�g
	static CCameraChaseChanger *m_pCameraChaseChanger;	// �J�����Ǐ]�ύX�҂̃I�u�W�F�N�g
	static CEnemyBase *m_pEnemyBase;	// �G�̋��_
};



#endif