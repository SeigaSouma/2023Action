//=============================================================================
// 
//  �J�����̎��w�b�_�[ [cameraaxis.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _CAMERAAXIS_H_
#define _CAMERAAXIS_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "constans.h"
#include "object.h"

//==========================================================================
// �O���錾
//==========================================================================
class CDebugPointNumber;

//==========================================================================
// �N���X��`
//==========================================================================
// �J�����̎��N���X��`
class CCameraAxis
{
public:
	CCameraAxis();
	~CCameraAxis();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);

	void CreatePos(D3DXVECTOR3 pos);	// �ʒu�쐬
	void DeletePos(int nIdx);			// �ʒu�폜
	HRESULT ReadText(const std::string pFileName);	// �O���t�@�C���ǂݍ��ݏ���
	void Save(const std::string pFileName);			// �O���t�@�C�������o������

	static CCameraAxis *Create(const std::string pFileName);
	int GetAxisNum(void);			// �����擾
	D3DXVECTOR3 GetAxis(int nIdx);	// ���擾
	void SetAxis(D3DXVECTOR3 pos, int nIdx);	// ���ݒ�
	static int GetNumAll(void) { return m_nNumAll; }	// �����擾
private:

	std::vector<D3DXVECTOR3> m_posAll;		// �ʒu
	CDebugPointNumber *m_pMultiNumber[mylib_const::MAX_CAMERAAXIS];
	CObjectX *m_apObjX[mylib_const::MAX_CAMERAAXIS];	// �I�u�W�F�N�gX

	static int m_nNumAll;		// ����
};



#endif