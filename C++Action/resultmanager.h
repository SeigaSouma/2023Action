//=============================================================================
// 
//  リザルトマネージャヘッダー [resultmanager.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _RESULTMANAGER_H_
#define _RESULTMANAGER_H_	// 二重インクルード防止

#include "main.h"
#include "constans.h"

// 前方宣言
class CEnemy;
class CBoss;

//==========================================================================
// クラス定義
//==========================================================================
// リザルトマネージャクラス定義
class CResultManager
{
public:

	enum ADDTYPE
	{
		ADDTYPE_NONE = 0,
		ADDTYPE_NORMAL,
		ADDTYPE_BLUE,
		ADDTYPE_GOLD,
		ADDTYPE_MAX
	};

	CResultManager();
	~CResultManager();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Reset(void);	// リセット

	int GetScoreTypeNum(ADDTYPE type);	// スコアの種類毎の数取得
	void AddScore(int nValue);			// スコア加算
	void AddScoreNum(ADDTYPE type);			// スコアの種類加算
	int GetBaseScore(void);			// 元のスコアの取得処理
	int GetToatalScore(void);			// 最終スコアの取得処理
	static CResultManager *Create(void);	// 生成処理
	
private:
	int m_nScoreNum[ADDTYPE_MAX];	// スコアの種類
	int m_nBaseScore;	// 元のスコア
	int m_nToatalScore;	// 最終スコア
};



#endif