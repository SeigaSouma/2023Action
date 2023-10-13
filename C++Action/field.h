//=============================================================================
// 
//  フィールドヘッダー [field.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _FIELD_H_
#define _FIELD_H_	// 二重インクルード防止

#include "main.h"
#include "object3D.h"

//==========================================================================
// クラス定義
//==========================================================================
// 弾クラス定義
class CField : public CObject3D
{
public:

	CField(int nPriority = 1);
	~CField();

	static HRESULT Load(void);
	static void Unload(void);
	static CField *Create();
	static CField *Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot);

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static int GetNumAll(void);

private:

#define TEXTURE			"data\\TEXTURE\\line.png"
#define WIDTH			(200.0f)						// 横幅
#define HEIGHT			(200.0f)						// 縦幅

	static int m_nNumAll;					// 総数
	int m_nTexIdx;				// テクスチャのインデックス番号
};


#endif