//=============================================================================
// 
//  リザルト画面ヘッダー [result_screen.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _RESULTSCREEN_H_
#define _RESULTSCREEN_H_	// 二重インクルード防止

#include "main.h"
#include "object.h"
#include "resultmanager.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CObject2D;
class CMultiNumber;
class CResultScore;

//==========================================================================
// マクロ定義
//==========================================================================
#define SCORETYPE_DIGIT	(3)	// スコア種類の桁数
#define MAX_SCORETYPE	(3)	// スコア種類

//==========================================================================
// クラス定義
//==========================================================================
// 背景クラス定義
class CResultScreen : public CObject
{
public:

	CResultScreen(int nPriority = 0);
	~CResultScreen();

	static CResultScreen *Create(void);

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtx(void);
	void SetVtx(int nCntSelect);

	void SetAllArrival(void);	// 全ての到着設定
	bool IsSetNumber(void) { return m_bSetNumber; }

private:
	// 列挙型定義
	enum VTX
	{
		VTX_LOGO = 0,		// リザルトロゴ
		VTX_RESULTSCORE_NORMAL,	// リザルトスコア
		VTX_RESULTSCORE_RED,	// リザルトスコア
		VTX_RESULTSCORE_GOLD,	// リザルトスコア
		VTX_LOGO_MAX
	};

	// メンバ関数
	void MoveObj2D(int nCntType);
	void MoveNumber(int nCntType);

	static const char *m_apTextureFile[VTX_LOGO_MAX];	// テクスチャのファイル
	int m_nTexIdx[VTX_LOGO_MAX];						// テクスチャのインデックス番号
	int m_nTexIdxNumber;								// 数字テクスチャのインデックス番号
	float m_fPosDestObj2D[MAX_SCORETYPE];				// オブジェ2Dの目標位置
	float m_fPosDestNumber[MAX_SCORETYPE];				// 数字の目標位置
	int m_nNumDest[MAX_SCORETYPE];							// ボーナス数の目標値
	float m_fNum[MAX_SCORETYPE];							// ボーナス数
	bool m_nArrival;										// 到着したかの判定
	bool m_bSetNumber;										// 設定したかの判定
	CObject2D *m_pObj2D[VTX_LOGO_MAX];						// オブジェクト2Dのオブジェクト
	CMultiNumber *m_apNumber[MAX_SCORETYPE];				// 数字のオブジェクト
	CResultScore *m_pResultScore;							// リザルトスコアのオブジェクト
};



#endif