//=============================================================================
// 
//  ポイントの数字ヘッダー [debugpointnumber.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _DEBUG_POINTNUMBER_H_
#define _DEBUG_POINTNUMBER_H_	// 二重インクルード防止

#include "main.h"
#include "object.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CMultiNumber;

//==========================================================================
// クラス定義
//==========================================================================
// エディットクラス定義
class CDebugPointNumber : public CObject
{
public:

	CDebugPointNumber(int nPriority = 7);
	~CDebugPointNumber();

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CDebugPointNumber *Create(int nIdx);

private:
	int m_nIdxParent;	// 親の番号
	CMultiNumber *m_pMultiNumber;	// 数字のオブジェクト
};



#endif