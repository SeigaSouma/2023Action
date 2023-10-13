//=============================================================================
// 
//  遷移なしフェードヘッダー [instantfade.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _INSTANTFADE_H_
#define _INSTANTFADE_H_	// 二重インクルード防止

#include "main.h"
#include "object2D.h"

//==========================================================================
// 前方宣言
//==========================================================================

//==========================================================================
// クラス定義
//==========================================================================
// 遷移なしフェードクラス定義
class CInstantFade
{
public:

	// 列挙型定義
	enum STATE
	{
		STATE_NONE = 0,			// 何もしていない
		STATE_FADEOUT,			// フェードアウト
		STATE_FADEIN,			// フェードイン
		STATE_FADECOMPLETION,	// フェード完了
		STATE_MAX
	};

	CInstantFade();
	~CInstantFade();

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	CObject2D *GetObject2D(void);
	static CInstantFade *Create(void);
	void SetFade(D3DXCOLOR FadeColor = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f), int nDuration = 30);	// フェード設定
	STATE GetState(void);	// 状態取得

private:

	int m_nDuration;			// フェードまでの時間
	CObject2D *m_aObject2D;		// オブジェクト2Dのオブジェクト
	STATE m_state;				// 状態
};

#endif