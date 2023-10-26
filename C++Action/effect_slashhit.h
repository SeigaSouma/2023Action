//=============================================================================
// 
//  斬撃ヒットエフェクトヘッダー [effect_slashhit.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _EFFECT_SLASHHIT_H_
#define _EFFECT_SLASHHIT_H_	// 二重インクルード防止

#include "main.h"
#include "objectBillboard.h"

//==========================================================================
// クラス定義
//==========================================================================
// 斬撃ヒットエフェクトクラス定義
class CEffectSlashHit : public CObjectBillboard
{
public:

	CEffectSlashHit(int nPriority = mylib_const::DEFAULT_PRIORITY);
	~CEffectSlashHit();

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtx(void);

	static int GetNumAll(void);
	static CEffectSlashHit *Create(const D3DXVECTOR3 pos);	// 生成処理
private:

	// メンバ変数
	int m_nCntAnim;			// アニメーションカウンター
	int m_nPatternAnim;		// アニメーションパターンNo.
	static int m_nNumAll;	// 爆発の総数
	static int m_nTexIdx;	// テクスチャのインデックス番号
};



#endif