//=============================================================================
// 
//  エディットヘッダー [edit.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _EDIT_H_
#define _EDIT_H_	// 二重インクルード防止

#include "main.h"
#include "object.h"

class CObjectX;

//==========================================================================
// クラス定義
//==========================================================================
// エディットクラス定義
class CEdit : public CObject
{
public:

	CEdit(int nPriority = 0);
	~CEdit();

	// オーバーライドされた関数
	HRESULT Init(void);
	HRESULT Init(const char *pFileName);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CEdit *Create();
	void Release(void);
	static int GetNumAll(void);
protected:

private:
	HRESULT ReadText(void);
	void Control(CObjectX *pObjX);
	void ChangeType(void);
	void GrabModel(void);
	void DeleteModel(void);

	static CObjectX *m_pObjX;
	D3DXVECTOR3 m_posOld;	// 前回の位置
	static int m_nNumAll;	// 総数
	static int m_nType;		// タイプ
	static bool m_bShadow;	// 影を使うかどうか
};



#endif