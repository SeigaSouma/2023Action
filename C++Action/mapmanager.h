//=============================================================================
// 
//  �}�b�v�}�l�[�W���w�b�_�[ [mapmanager.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _MAPMANAGER_H_
#define _MAPMANAGER_H_	// ��d�C���N���[�h�h�~

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
// �}�b�v�}�l�[�W���N���X��`
class CMapManager
{
public:
	CMapManager();
	~CMapManager();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);

	void CreatePos(D3DXVECTOR3 pos);	// �ʒu�쐬
	void DeletePos(int nIdx);			// �ʒu�폜
	HRESULT ReadText(const std::string pFileName);	// �O���t�@�C���ǂݍ��ݏ���
	void Save(const std::string pFileName);			// �O���t�@�C�������o������

	static CMapManager *Create(const std::string pFileName);
	int GetControlPointNum(void);			// ����_���擾
	D3DXVECTOR3 GetControlPoint(void);		// ����_�擾
	D3DXVECTOR3 GetControlPoint(int nIdx);	// ����_�擾
	void SetControlPoint(D3DXVECTOR3 pos, int nIdx);	// ����_�ݒ�
	D3DXVECTOR3 GetTargetPosition(int nIdx, float fMoveValue);	// �ڕW�̈ʒu�擾
	D3DXVECTOR3 UpdateNowPosition(int& nIdx, float& fRatio, float& fMoveValue, float PosY);
	D3DXVECTOR3 UpdateNowPosition(int& nIdx, float& fRatio, float& fMoveValue, float PosY, CObject::ANGLE MoveAngle, float fMove);	// ���ݒn�X�V
	CObject::ANGLE GetTargetAngle(int myIdx, int TargetIdx, float myMoveValue, float TargetMoveValue);	// �ڕW�������̂ǂ����ɂ��邩�̔���
	CObject::ANGLE GetTargetAngle(CObject *pMyObj, CObject *pTargetObj);	// �ڕW�������̂ǂ����ɂ��邩�̔���
	int ConversionToIndex(D3DXVECTOR3 pos);			// �ʒu���}�b�v�̃C���f�b�N�X�ϊ�
	float GetThusFarMoveValue(int nIdx);		// ����܂ł̃}�b�v�ړ��ʎ擾
	static int GetNumAll(void) { return m_nNumAll; }	// �����擾
private:

	std::vector<D3DXVECTOR3> m_posAll;		// �ʒu

	D3DXVECTOR3 m_pos;			// �ʒu
	static CMapManager *m_pTop;	// �擪�̃|�C���^
	static CMapManager *m_pCur;	// �Ō���̃|�C���^
	CMapManager *m_pPrev;		// �O�̃I�u�W�F�N�g�ւ̃|�C���^
	CMapManager *m_pNext;		// ���̃I�u�W�F�N�g�ւ̃|�C���^
	static int m_nNumAll;		// ����
	CDebugPointNumber *m_pMultiNumber[mylib_const::MAX_CAMERAAXIS];
};



#endif