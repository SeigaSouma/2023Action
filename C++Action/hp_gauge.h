//=============================================================================
// 
//  体力ゲージヘッダー [hp_gauge.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _HP_GAUGE_H_
#define _HP_GAUGE_H_	// 二重インクルード防止

#include "main.h"
#include "objectBillboard.h"

//==========================================================================
// クラス定義
//==========================================================================
// 体力ゲージクラス定義
class CHP_Gauge : public CObject
{
public:

	CHP_Gauge(int nPriority = mylib_const::DEFAULT_PRIORITY);
	~CHP_Gauge();

	static CHP_Gauge *Create(float fPosLen, int nMaxLife, float fSizeBuff = 1.0f);

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtx(void);
	void SetVtx(int nCntGauge);

	void UpdatePosition(D3DXVECTOR3 pos, int nLife);
	static int GetNumAll(void);

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
		CObjectBillboard *pObjBillboard;	// ビルボードのオブジェクト
		float fMaxWidth;					// 幅の最大値
		float fWidthDest;					// 幅の差分
		float fMaxHeight;					// 高さの最大値
	};

	void ChangeColor(int nCntGauge);		// 色変更
	void GaugeDecrement(int nCntGauge);		// 減少処理

	static int m_nNumAll;					// 総数
	static const char *m_apTextureFile[];	// テクスチャのファイル
	SHP_Gauge m_HPGauge[VTXTYPE_MAX];		// HPゲージの情報
	float m_fPosLength;						// 原点からの長さ
	int m_nLife;							// 体力
	int m_nMaxLife;							// 最大体力
	int m_nTexIdx[VTXTYPE_MAX];				// テクスチャのインデックス番号
};


#endif