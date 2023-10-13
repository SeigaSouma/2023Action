//=============================================================================
// 
//  爆発ヘッダー [explosion.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _EXPLOSION_H_
#define _EXPLOSION_H_	// 二重インクルード防止

#include "main.h"
#include "objectBillboard.h"

//==========================================================================
// クラス定義
//==========================================================================
// 爆発クラス定義
class CExplosion : public CObjectBillboard
{
public:

	CExplosion(int nPriority = mylib_const::DEFAULT_PRIORITY);
	~CExplosion();

	static HRESULT Load(void);
	static void Unload(void);
	static CExplosion *Create();
	static CExplosion *Create(const D3DXVECTOR3 pos);

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtx(void);

	static int GetNumAll(void);

private:
	int m_nCntAnim;							// アニメーションカウンター
	int m_nPatternAnim;						// アニメーションパターンNo.
	static int m_nNumAll;					// 爆発の総数
	int m_nTexIdx;					// テクスチャのインデックス番号
};



#endif