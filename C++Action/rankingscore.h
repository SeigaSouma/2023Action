//=============================================================================
// 
//  ランキングスコアヘッダー [rankingscore.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _RANKINGSCORE_H_
#define _RANKINGSCORE_H_	// 二重インクルード防止

#include "main.h"
#include "object.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define RANKINGSCORE_DIGIT	(6)	// 桁数
#define RANKINGNUM	(5)	// 桁数

//==========================================================================
// 前方宣言
//==========================================================================
class CNumber;

//==========================================================================
// クラス定義
//==========================================================================
// ランキングスコアクラス定義
class CRankingScore : public CObject
{
public:

	CRankingScore(int nPriority = 7);
	~CRankingScore();

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void SetAllArrival(void);	// 全ての到着設定
	void UpdateNewRecord(void);	// ニューレコードの更新処理
	void SetValue(int nCntRanking);	// 値の設定処理
	static CRankingScore *Create(void);
private:

	// 列挙型定義
	enum VTX
	{
		VTX_NUM = 0,		// リザルトロゴ
		VTX_LOGO,
		VTX_MAX
	};

	void Load(void);	// ロード
	void Save(void);	// セーブ
	void Sort(void);	// ソート
	void Moving(int nCntRanking);	// 移動処理
	
	int m_nNumRanking;				// ランキング数
	int m_nScore[RANKINGNUM];		// スコア
	int m_nNowScore;				// 今回のスコア
	int m_nTexIdx[VTX_MAX];					// テクスチャのインデックス番号
	int m_nTexIdxNumber;			// 数字テクスチャのインデックス番号
	int m_nIdxNewRecord;			// ニューレコードのインデックス番号
	int m_nCntNewRecord;			// ニューレコードのカウンター
	float m_fPosDestX[RANKINGNUM];	// 目標の位置
	bool m_bNewRecord;				// ニューレコードのフラグ
	bool m_bArrival[RANKINGNUM];	// 到着判定
	CObject2D *m_pObj2D[VTX_MAX];	// オブジェクト2Dのオブジェクト
	CNumber *m_pScore[RANKINGNUM][RANKINGSCORE_DIGIT];			// 数字のオブジェクト
	static const char *m_apTextureFile[VTX_MAX];	// テクスチャのファイル
};



#endif