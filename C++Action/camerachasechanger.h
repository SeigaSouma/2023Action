//=============================================================================
// 
//  �J�����̎��w�b�_�[ [camerachasechanger.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _CAMERACHASECHANGER_H_
#define _CAMERACHASECHANGER_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "constans.h"
#include "object.h"
#include "camera.h"

//==========================================================================
// �O���錾
//==========================================================================
class CDebugPointNumber;

//==========================================================================
// �N���X��`
//==========================================================================
// �J�����̎��N���X��`
class CCameraChaseChanger
{
public:

	// �\���̒�`
	struct sChaseChangeInfo
	{
		int nByTypeIdx;					// ��ޕʃC���f�b�N�X�ԍ�
		D3DXVECTOR3 pos;				// �ʒu
		CCamera::CHASETYPE chaseType;	// �Ǐ]�̎��
	};

	CCameraChaseChanger();
	~CCameraChaseChanger();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);

	void CreatePos(CCamera::CHASETYPE type, D3DXVECTOR3 pos);	// �ʒu�쐬
	void DeletePos(int nIdx);			// �ʒu�폜
	HRESULT ReadText(const std::string pFileName);	// �O���t�@�C���ǂݍ��ݏ���
	void Save(const std::string pFileName);			// �O���t�@�C�������o������

	static CCameraChaseChanger *Create(const std::string pFileName);
	int GetAxisNum(void);			// �����擾
	D3DXVECTOR3 GetAxis(int nIdx);	// ���擾
	void SetAxis(D3DXVECTOR3 pos, int nIdx);	// ���ݒ�
	sChaseChangeInfo GetChaseChangeInfo(int nIdx);	// �ύX�̏��擾
	static int GetNumAll(void) { return m_nNumAll; }	// �����擾
private:

	std::vector<sChaseChangeInfo> m_ChaseChangeInfo;		// �Ǐ]�ύX�̏��
	CDebugPointNumber *m_pMultiNumber[mylib_const::MAX_CAMERAAXIS];
	CObjectX *m_apObjX[mylib_const::MAX_CAMERAAXIS];	// �I�u�W�F�N�gX

	static int m_nNumChaseAxis;		// ���Ǐ]�̑���
	static int m_nNumChaseMap;		// �}�b�v�Ǐ]�̑���
	static int m_nNumAll;		// ����
};



#endif