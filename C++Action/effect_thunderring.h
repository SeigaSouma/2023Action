//=============================================================================
// 
//  雷のリングエフェクトヘッダー [effect_thunderring.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _EFFECT_THUNDERRING_H_
#define _EFFECT_THUNDERRING_H_	// 二重インクルード防止

#include "main.h"
#include "objectBillboard.h"

//==========================================================================
// クラス定義
//==========================================================================
// 雷のリングエフェクトクラス定義
class CThunderRing : public CObjectBillboard
{
public:

	CThunderRing(int nPriority = 7);
	~CThunderRing();

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtx(void);

	void SetDesableAutoDeath(void) { m_bAutoDeath = false; }	// 自動削除の判定削除
	bool IsFinish(void) { return m_bFinish; }					// 終了の判定
	static int GetNumAll(void);
	static CThunderRing *Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 size);	// 生成処理
private:

	// メンバ変数
	int m_nCntAnim;			// アニメーションカウンター
	int m_nPatternAnim;		// アニメーションパターンNo.
	bool m_bAutoDeath;		// 自動削除のフラグ
	bool m_bFinish;			// アニメーションが終わった判定
	static int m_nNumAll;	// 爆発の総数
	static int m_nTexIdx;	// テクスチャのインデックス番号
};



#endif