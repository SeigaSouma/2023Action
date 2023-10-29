//=============================================================================
// 
//  敵スポーンエフェクトヘッダー [effect_enemyspawn.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _EFFECT_ENEMYSPAWN_H_
#define _EFFECT_ENEMYSPAWN_H_	// 二重インクルード防止

#include "main.h"
#include "objectBillboard.h"

//==========================================================================
// クラス定義
//==========================================================================
// 敵スポーンエフェクトクラス定義
class CEffectEnemySpawn : public CObjectBillboard
{
public:

	CEffectEnemySpawn(int nPriority = mylib_const::DEFAULT_PRIORITY);
	~CEffectEnemySpawn();

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtx(void);

	static CEffectEnemySpawn *Create(const D3DXVECTOR3 pos);	// 生成処理
	static void LoadTexture(void);	// テクスチャ読み込み
private:

	// メンバ変数
	int m_nCntAnim;			// アニメーションカウンター
	int m_nPatternAnim;		// アニメーションパターンNo.
	static int m_nTexIdx;	// テクスチャのインデックス番号
};



#endif