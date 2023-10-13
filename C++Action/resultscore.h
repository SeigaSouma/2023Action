//=============================================================================
// 
//  リザルトスコアヘッダー [resultscore.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _RESULTSCORE_H_
#define _RESULTSCORE_H_	// 二重インクルード防止

#include "main.h"
#include "object.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define RESULTSCORE_DIGIT	(6)	// 桁数

//==========================================================================
// 前方宣言
//==========================================================================
class CMultiNumber;

//==========================================================================
// クラス定義
//==========================================================================
// 背景クラス定義
class CResultScore : public CObject
{
public:

	CResultScore(int nPriority = 7);
	~CResultScore();

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void SetAllArrival(void);	// 全ての到着設定
	void SetValue(int nScore);	// 値の設定処理
	static CResultScore *Create(void);
	bool IsSetBase(void) { return m_bSetBase; }
	bool IsSetToatal(void) { return m_bSetToatal; }
private:
	
	void CreateBaseScore(void);		// 元のスコアの生成
	void CreateToatalScore(void);	// 最終スコアの生成
	void SetBaseValue(void);		// 元のスコアの値設定処理
	void SetToatalValue(void);		// 最終スコアの値設定処理
	void MoveBaseScore(void);		// 元スコアの移動処理
	void MoveToatalScore(void);		// 最終スコアの移動処理

	int m_nBaseNum;					// 元のスコア
	int m_nBaseNumDest;				// 元のスコアの目標値
	int m_nToatalNum;				// 値
	int m_nToatalNumDest;			// 目標の値
	int m_nTexIdx;					// テクスチャのインデックス番号
	int m_nTexIdxNumber;			// 数字テクスチャのインデックス番号
	float m_fBasePosDest_X;			// 元スコアの目標位置
	float m_fToatalPosDest_X;		// 最終スコアの目標位置
	bool m_bArrivalBase;			// 元のスコアの到着判定
	bool m_bSetBase;				// 元のスコアの設定判定
	bool m_bArrivalToatal;			// 最終スコアの到着判定
	bool m_bSetToatal;				// 最終スコアの設定判定
	CObject2D *m_pBaseObj2D;		// 元のスコア2Dのオブジェクト
	CMultiNumber *m_pBaseScore;		// 元のスコア
	CObject2D *m_pToatalObj2D;		// オブジェクト2Dのオブジェクト
	CMultiNumber *m_pToatalScore;	// 数字のオブジェクト
};



#endif