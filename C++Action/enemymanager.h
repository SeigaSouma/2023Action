//=============================================================================
// 
//  �G�̃}�l�[�W���w�b�_�[ [enemymanager.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _ENEMYMANAGER_H_
#define _ENEMYMANAGER_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "constans.h"

// �O���錾
class CEnemy;

//==========================================================================
// �N���X��`
//==========================================================================
// �G�̃}�l�[�W���N���X��`
class CEnemyManager
{
public:

	// �\���̒�`
	struct EnemyData
	{
		int nType;	// �L�����N�^�[���
		int nStartFrame;	// �����t���[��
		int nStartKey;		// �����L�[
		float fStartMoveValue;	// �����}�b�v�ړ���
	};

	struct Pattern
	{
		int nNumEnemy;	// �G�̐�
		int nFixedType;	// ���̓����̎��
		EnemyData EnemyData[mylib_const::MAX_PATTEN_ENEMY];
	};

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

	// �񋓌^��`
	enum STATE
	{
		STATE_NONE = 0,		// �����Ȃ����
		STATE_COMBOANIM,	// �R���{���̃A�j���[�V����
		STATE_MAX
	};

	CEnemyManager();
	~CEnemyManager();

	HRESULT Init(void);
	void Uninit(void);
	virtual void Update(void);

	static CEnemyManager *Create(const std::string pTextFile);
	HRESULT ReadText(const std::string pTextFile);	// �O���t�@�C���ǂݍ��ݏ���
	CEnemy **SetEnemy(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nPattern);	// �G�z�u
	CEnemy **SetEnemy(D3DXVECTOR3 pos, int nMapIndex, float fMapMoveValue, int nPattern);	// �G�z�u
	static int GetPatternNum(void);
	static void Release(int nIdx);		// �j��
	STATE GetState(void) { return m_state; }	// ��Ԏ擾
	int GetNumAll(void);		// �G�̑����擾
	Pattern GetPattern(int nPattern);	// �p�^�[���擾
	CEnemy **GetEnemy(void);	// �G�擾
	static const char *GetMotionFilename(int nType);
protected:


private:

	static int m_nNumAll;		// �G�̑���
	static CEnemy *m_pEnemy[mylib_const::MAX_OBJ];						// �G�ւ̃|�C���^
	static Pattern m_aPattern[mylib_const::MAX_PATTEN_ENEMY];			// �z�u�̎��
	static std::string sMotionFileName[mylib_const::MAX_PATTEN_ENEMY];	// ���[�V�����t�@�C����
	static bool m_bHitStop;		// �q�b�g�X�g�b�v�̔���
	static STATE m_state;		// ���
	static int m_nCntSpawn;		// �o���J�E���g
	static int m_nPatternNum;	// �o���p�^�[����
	static int m_nNumChara;		// �G�̎�ނ̑���
	static bool m_bLoadPattern;	// �p�^�[���ǂݍ��ݔ���
};



#endif