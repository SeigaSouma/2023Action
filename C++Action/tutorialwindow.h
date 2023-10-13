//=============================================================================
// 
//  チュートリアルウィンドウヘッダー [tutorialwindow.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _TUTORIALWINDOW_H_
#define _TUTORIALWINDOW_H_	// 二重インクルード防止

#include "main.h"
#include "object2D.h"
#include "tutorialstep.h"

//==========================================================================
// 前方宣言
//==========================================================================

//==========================================================================
// マクロ定義
//==========================================================================

//==========================================================================
// クラス定義
//==========================================================================
// 背景クラス定義
class CTutorialWindow : public CObject2D
{
public:

	CTutorialWindow(int nPriority = 7);
	~CTutorialWindow();

	static CTutorialWindow *Create(CTutorialStep::STEP step);

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);

private:
	
	// メンバ関数

	// メンバ変数
	int m_nCntWait;					// 待機時間
	CTutorialStep::STEP m_NowStep;	// 今回のステップ
	static const char *m_apTextureFile[CTutorialStep::STEP_MAX];	// テクスチャのファイル
	static int m_nTexIdx[CTutorialStep::STEP_MAX];	// テクスチャのインデックス番号
};



#endif