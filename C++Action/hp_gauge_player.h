//=============================================================================
// 
//  プレイヤーの体力ゲージヘッダー [hp_gauge_player.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _HP_GAUGE_PLAYER_H_
#define _HP_GAUGE_PLAYER_H_	// 二重インクルード防止

#include "main.h"
#include "object2D.h"

//==========================================================================
// クラス定義
//==========================================================================
// プレイヤーの体力ゲージクラス定義
class CHP_GaugePlayer : public CObject
{
public:

	CHP_GaugePlayer(int nPriority = 7);
	~CHP_GaugePlayer();

	static CHP_GaugePlayer *Create(D3DXVECTOR3 pos, int nMaxLife);

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtx(void);
	void SetVtx(int nCntGauge);

	void Kill(void);
	void SetLife(int nLife);

private:

	// 列挙型定義
	typedef enum
	{
		VTXTYPE_BLACK = 0,	// 黒ゲージ
		VTXTYPE_PINK,		// ピンクゲージ
		VTXTYPE_FRAM,		// 枠
		VTXTYPE_MAX
	}VTXTYPE;

	// 構造体定義
	struct SHP_Gauge
	{
		CObject2D *pObj2D;	// オブジェクト2Dのオブジェクト
		float fMaxWidth;			// 幅の最大値
		float fWidthDest;			// 幅の差分
		float fMaxHeight;			// 高さの最大値
	};

	// メンバ関数
	void ChangeColor(int nCntGauge);		// 色変更
	void GaugeDecrement(int nCntGauge);		// 減少処理

	// メンバ変数
	static const char *m_apTextureFile[];	// テクスチャのファイル
	SHP_Gauge m_HPGauge[VTXTYPE_MAX];		// HPゲージの情報
	int m_nLife;							// 体力
	int m_nMaxLife;							// 最大体力
	int m_nTexIdx[VTXTYPE_MAX];				// テクスチャのインデックス番号
};


#endif