//=============================================================================
// 
//  数字ヘッダー [number.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _NUMBER_H_
#define _NUMBER_H_	// 二重インクルード防止

#include "main.h"
#include "object2D.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CObject2D;
class CObjectBillboard;

//==========================================================================
// クラス定義
//==========================================================================
// 背景クラス定義
class CNumber
{
public:

	// 列挙型定義
	enum EObjectType
	{
		OBJECTTYPE_2D = 0,		// 2D
		OBJECTTYPE_BILLBOARD,	// ビルボード
		OBJECTTYPE_MAX
	};

	CNumber(int nPriority = 6);
	~CNumber();

	static CNumber *Create(EObjectType objtype);

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void Release(void);	// 開放処理
	CObject2D *GetObject2D(void);
	CObjectBillboard *GetObjectBillboard(void);

private:
	EObjectType m_objType;				// オブジェクトの種類
	CObject2D *m_aObject2D;				// オブジェクト2Dのオブジェクト
	CObjectBillboard *m_pObjBillboard;	// オブジェクトビルボードのオブジェクト
};

#endif