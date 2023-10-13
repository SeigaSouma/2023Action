//=============================================================================
// 
//  マネージャヘッダー [manager.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _MANAGER_H_
#define _MANAGER_H_	// 二重インクルード防止

#include "main.h"
#include "object.h"
#include "scene.h"

// マクロ定義
#define MOVE_LRUP			(0.75f)	// 斜め上移動
#define MOVE_LR				(0.5f)	// 水平移動
#define MOVE_LRDW			(0.25f)	// 斜め上移動

// 前方宣言
class CRenderer;
class CLight;
class CCamera;
class CInputKeyboard;
class CInputGamepad;
class CInputMouse;
class CSound;
class CDebugProc;
class CPlayer;
class CScore;
class CTimer;
class CBG;
class CMap;
class CTexture;
class CXLoad;
class CEdit;
class CElevation;
class CEnemyManager;
class CFade;
class CInstantFade;
class CPause;
class CPowerGauge;
class CResultManager;
class CRankingManager;

//==========================================================================
// クラス定義
//==========================================================================
// マネージャクラス定義
class CManager
{
public:
	CManager();
	~CManager();

	HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CRenderer *GetRenderer(void);
	static CInputKeyboard *GetInputKeyboard(void);
	static CInputGamepad *GetInputGamepad(void);
	static CInputMouse *GetInputMouse(void);
	static CSound *GetSound(void);
	static CDebugProc *GetDebugProc(void);
	static CBG *GetBg(void);
	static CLight *GetLight(void);
	static CCamera *GetCamera(void);
	static CMap *GetMap(void);
	static CTexture *GetTexture(void);
	static CEdit *GetEdit(void);
	static CFade *GetFade(void);
	static CInstantFade *GetInstantFade(void);	// 遷移なしフェード取得
	static CPause *GetPause(void);
	static CResultManager *GetResultManager(void);	// リザルトマネージャ取得
	static CRankingManager *GetRankingManager(void);	// ランキングマネージャのオブジェクト
	static CScene *GetScene(void);		// シーン取得

	static void SetMode(CScene::MODE mode);						// 次のモード設定
	static CScene::MODE GetMode(void);							// 現在のモード取得
	static void SetEnableHitStop(int nCntHitStop);				// ヒットストップの設定
	static bool IsWireframe(void) { return m_bWireframe; }		// ワイヤーフレーム
	static CScene::MODE GetOldMode(void) { return m_OldMode; }	// 前回のモード取得
private:

	static CRenderer *m_pRenderer;				// レンダラーのオブジェクト
	static CInputKeyboard *m_pInputKeyboard;	// キーボードのオブジェクト
	static CInputGamepad *m_pInputGamepad;		// ゲームパッドのオブジェクト
	static CInputMouse *m_pInputMouse;			// マウスのオブジェクト
	static CSound *m_pSound;					// サウンドのオブジェクト
	static CDebugProc *m_pDebugProc;			// デバッグ表示のオブジェクト
	static CBG *m_pBg;							// 背景のオブジェクト
	static CLight *m_pLight;					// ライトのオブジェクト
	static CCamera *m_pCamera;					// カメラのオブジェクト
	static CMap *m_pMap;						// マップのオブジェクト
	static CTexture *m_pTexture;				// テクスチャのオブジェクト
	static CEdit *m_pEdit;						// エディットのオブジェクト
	static CScene *m_pScene;					// シーンのオブジェクト
	static CFade *m_pFade;						// フェードのオブジェクト
	static CInstantFade *m_pInstantFade;		// 遷移なしフェードのオブジェクト
	static CPause *m_pPause;					// ポーズのオブジェクト
	static CResultManager *m_pResultManager;	// リザルトマネージャのオブジェクト
	static CRankingManager *m_pRankingManager;	// ランキングマネージャのオブジェクト

	static bool m_bWireframe;					// ワイヤーフレーム
	static bool m_bHitStop;						// ヒットストップの判定
	static int m_nCntHitStop;					// ヒットストップのカウンター
	static CScene::MODE m_OldMode;				// 前回のモード
};


#endif