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
class CEnemyFixedMoveManager;

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

	static CManager *Create(void);		// 生成処理
	static CManager *GetInstance(void);	// インスタンス取得

	CRenderer *GetRenderer(void);
	CInputKeyboard *GetInputKeyboard(void);
	CInputGamepad *GetInputGamepad(void);
	CInputMouse *GetInputMouse(void);
	CSound *GetSound(void);
	CDebugProc *GetDebugProc(void);
	CLight *GetLight(void);
	CCamera *GetCamera(void);
	CTexture *GetTexture(void);
	CEdit *GetEdit(void);
	CFade *GetFade(void);
	CInstantFade *GetInstantFade(void);	// 遷移なしフェード取得
	CPause *GetPause(void);
	CResultManager *GetResultManager(void);	// リザルトマネージャ取得
	CRankingManager *GetRankingManager(void);	// ランキングマネージャのオブジェクト
	CScene *GetScene(void);		// シーン取得
	CEnemyFixedMoveManager *GetFixedManager(void);	// 一定の行動マネージャ取得

	float DeltaTime(void);								// 経過時間取得
	void SetMode(CScene::MODE mode);					// 次のモード設定
	CScene::MODE GetMode(void);							// 現在のモード取得
	void SetEnableHitStop(int nCntHitStop);				// ヒットストップの設定
	bool IsWireframe(void) { return m_bWireframe; }		// ワイヤーフレーム
	CScene::MODE GetOldMode(void) { return m_OldMode; }	// 前回のモード取得
private:

	CRenderer *m_pRenderer;				// レンダラーのオブジェクト
	CInputKeyboard *m_pInputKeyboard;	// キーボードのオブジェクト
	CInputGamepad *m_pInputGamepad;		// ゲームパッドのオブジェクト
	CInputMouse *m_pInputMouse;			// マウスのオブジェクト
	CSound *m_pSound;					// サウンドのオブジェクト
	CDebugProc *m_pDebugProc;			// デバッグ表示のオブジェクト
	CLight *m_pLight;					// ライトのオブジェクト
	CCamera *m_pCamera;					// カメラのオブジェクト
	CTexture *m_pTexture;				// テクスチャのオブジェクト
	CEdit *m_pEdit;						// エディットのオブジェクト
	CScene *m_pScene;					// シーンのオブジェクト
	CFade *m_pFade;						// フェードのオブジェクト
	CInstantFade *m_pInstantFade;		// 遷移なしフェードのオブジェクト
	CPause *m_pPause;					// ポーズのオブジェクト
	CResultManager *m_pResultManager;	// リザルトマネージャのオブジェクト
	CRankingManager *m_pRankingManager;	// ランキングマネージャのオブジェクト
	CEnemyFixedMoveManager *m_pFixedMoveManager;	// 一定の行動マネージャのオブジェクト

	bool m_bWireframe;					// ワイヤーフレーム
	bool m_bHitStop;						// ヒットストップの判定
	int m_nCntHitStop;					// ヒットストップのカウンター
	CScene::MODE m_OldMode;				// 前回のモード
	DWORD m_CurrentTime;				// 現在時間
	DWORD m_OldTime;					// 過去の時間
	float m_fDeltaTime;					// 経過時間

	static CManager *m_pManager;	// マネージャのオブジェクト
};


#endif