//=============================================================================
// 
//  タイマーヘッダー [timer.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _TIMER_H_
#define _TIMER_H_	// 二重インクルード防止

#include "main.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CMultiNumber;

//==========================================================================
// クラス定義
//==========================================================================
// 背景クラス定義
class CTimer
{
public:

	CTimer(int nPriority = 6);
	~CTimer();

	static HRESULT Load(void);
	static void Unload(void);
	static CTimer *Create(void);
	static CTimer *Create(D3DXVECTOR3 pos);

	// オーバーライドされた関数
	HRESULT Init(void);
	HRESULT Init(D3DXVECTOR3 pos);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void Add(int nValue);

private:
	//==============================
	// マクロ定義
	//=============================
#define NUM_TIMER	(4)			// 桁数
#define MAX_TIME	(60 * 99)	// タイマーの最大数
#define START_TIME	(60 * 50)	// タイマーの初期値

	void SetValue(void);

	int m_nNum;							// 値
	int m_nCntTime;						// 時間のカウント
	CMultiNumber *m_apNumber;			// 数字のオブジェクト
	int m_nTexIdx;						// テクスチャのインデックス番号
};



#endif