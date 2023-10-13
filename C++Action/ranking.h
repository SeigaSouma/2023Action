//=============================================================================
// 
//  ランキングヘッダー [ranking.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _RANKING_H_
#define _RANKING_H_	// 二重インクルード防止

#include "main.h"
#include "manager.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CRankingScore;

//==========================================================================
// クラス定義
//==========================================================================
// ランキングクラス定義
class CRanking : public CScene
{
public:

	CRanking();
	~CRanking();

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static void SetEnableArrival(void);
	static int GetRandStage(void) { return m_nRandStage; }
private:
	int m_nCntSwitch;		// 切り替えのカウンター
	static bool m_bAllArrival;		// 全て到着した判定
	static int m_nRandStage;	// ステージのランダムインデックス番号
	static CRankingScore *m_pRankingScore;	// ランキングスコアのオブジェクト
};



#endif