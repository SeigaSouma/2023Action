//=============================================================================
// 
//  �X�e�[�W�w�b�_�[ [stage.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _STAGE_H_
#define _STAGE_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "object.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �X�e�[�W�N���X��`
class CStage
{
public:

	CStage();
	~CStage();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	CObjectX *GetObj(int nIdx);
	int GetNumAll(void);		// �����擾
	static CStage *Create(void);

private:

	// �����o�֐�
	HRESULT ReadXFile(void);	// ���ł�ǂݍ��ݏ���
	HRESULT ReadText(void);		// �O���t�@�C���ǂݍ��ݏ���
	HRESULT SaveText(void);		// �O���t�@�C���������ݏ���

	// �����o�ϐ�
	int m_nNumAll;
	std::vector<std::string> ModelFile;		// ���f���t�@�C����
	CObjectX *m_pObjX[mylib_const::MAX_STAGE];	// �I�u�W�F�N�gX�̃|�C���^

};


#endif