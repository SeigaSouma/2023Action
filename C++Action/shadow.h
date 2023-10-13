//=============================================================================
// 
//  影ヘッダー [shadow.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _SHADOW_H_
#define _SHADOW_H_	// 二重インクルード防止

#include "main.h"
#include "object3D.h"

//==========================================================================
// クラス定義
//==========================================================================
// 影クラス定義
class CShadow : public CObject3D
{
public:
	CShadow(int nPriority = 7);
	~CShadow();

	static CShadow *Create();
	static CShadow *Create(D3DXVECTOR3 pos, float size = 50.0f);

	// オーバーライドされた関数ああ
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static int GetNumAll(void);

private:

	CObject *m_pObject;		// オブジェクトのポインタ
	static int m_nNumAll;	// 総数
	int m_nTexIdx;			// テクスチャのインデックス番号
};


#endif