//=============================================================================
// 
//  マネージャ処理 [manager.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "sound.h"
#include "debugproc.h"
#include "object2D.h"
#include "object3D.h"
#include "objectBillboard.h"
#include "objectX.h"
#include "texture.h"
#include "calculation.h"

#include "pause.h"
#include "fade.h"
#include "instantfade.h"
#include "light.h"
#include "camera.h"
#include "bg.h"
#include "edit.h"
#include "resultmanager.h"
#include "rankingmanager.h"
#include "enemyfixedmove_manager.h"

//==========================================================================
// マクロ定義
//==========================================================================

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
CManager *CManager::m_pManager = NULL;					// マネージャのオブジェクト

//==========================================================================
// コンストラクタ
//==========================================================================
CManager::CManager()
{
	m_pRenderer = NULL;				// レンダラーのオブジェクト
	m_pInputKeyboard = NULL;		// キーボードのオブジェクト
	m_pInputGamepad = NULL;			// ゲームパッドのオブジェクト
	m_pSound = NULL;				// サウンドのオブジェクト
	m_pInputMouse = NULL;			// マウスのオブジェクト
	m_pDebugProc = NULL;			// デバッグ表示のオブジェクト
	m_pLight = NULL;				// ライトのオブジェクト
	m_pCamera = NULL;				// カメラのオブジェクト
	m_pTexture = NULL;				// テクスチャのオブジェクト
	m_pEdit = NULL;					// エディットのオブジェクト
	m_pScene = NULL;				// シーンのオブジェクト
	m_pFade = NULL;					// フェードのオブジェクト
	m_pInstantFade = NULL;			// 遷移なしフェードのオブジェクト
	m_pPause = NULL;				// ポーズのオブジェクト
	m_pResultManager = NULL;		// リザルトマネージャのオブジェクト
	m_pRankingManager = NULL;		// ランキングマネージャのオブジェクト
	m_pFixedMoveManager = NULL;		// 一定の行動マネージャのオブジェクト
	m_bWireframe = false;			// ワイヤーフレーム
	m_bHitStop = false;				// ヒットストップの判定
	m_nCntHitStop = 0;				// ヒットストップのカウンター
	m_OldMode = CScene::MODE_NONE;	// 前回のモード
	m_CurrentTime = 0;				// 現在時間
	m_OldTime = 0;					// 過去の時間
	m_fDeltaTime = 0.0f;			// 経過時間
}

//==========================================================================
// デストラクタ
//==========================================================================
CManager::~CManager()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CManager *CManager::Create(void)
{
	if (m_pManager == NULL)
	{// まだ生成していなかったら

		// マネージャのインスタンス生成
		m_pManager = DEBUG_NEW CManager;
	}
	else
	{
		// インスタンス取得
		m_pManager->GetInstance();
	}

	return m_pManager;
}

//==========================================================================
// インスタンス取得
//==========================================================================
CManager *CManager::GetInstance(void)
{
	return m_pManager;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CManager::Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{

	HRESULT hr;
	m_bHitStop = false;		// ヒットストップの判定
	m_nCntHitStop = 0;		// ヒットストップのカウンター

	//**********************************
	// キーボード
	//**********************************
	if (m_pInputKeyboard != NULL)
	{// 確保されていたら
		return E_FAIL;
	}

	// メモリ確保
	m_pInputKeyboard = DEBUG_NEW CInputKeyboard;

	if (m_pInputKeyboard != NULL)
	{// メモリの確保が出来ていたら

		// 初期化処理
		hr = m_pInputKeyboard->Init(hInstance, hWnd);
		if (FAILED(hr))
		{// 初期化処理が失敗した場合

			return E_FAIL;
		}
	}
	
	//**********************************
	// ゲームパッド
	//**********************************
	if (m_pInputGamepad != NULL)
	{// 確保されていたら
		return E_FAIL;
	}

	// メモリ確保
	m_pInputGamepad = DEBUG_NEW CInputGamepad;

	if (m_pInputGamepad != NULL)
	{// メモリの確保が出来ていたら

		// 初期化処理
		hr = m_pInputGamepad->Init(hInstance, hWnd);
		if (FAILED(hr))
		{// 初期化処理が失敗した場合

			return E_FAIL;
		}
	}


	//**********************************
	// マウス
	//**********************************
	if (m_pInputMouse != NULL)
	{// 確保されていたら
		return E_FAIL;
	}

	// メモリ確保
	m_pInputMouse = DEBUG_NEW CInputMouse;

	if (m_pInputMouse != NULL)
	{// メモリの確保が出来ていたら

		// 初期化処理
		hr = m_pInputMouse->Init(hInstance, hWnd);
		if (FAILED(hr))
		{// 初期化処理が失敗した場合

			return E_FAIL;
		}
	}


	//**********************************
	// レンダラー
	//**********************************
	if (m_pRenderer != NULL)
	{// 確保されていたら

		return E_FAIL;
	}

	// メモリ確保
	m_pRenderer = DEBUG_NEW CRenderer;

	if (m_pRenderer != NULL)
	{// メモリの確保が出来ていたら

		// 初期化処理
		hr = m_pRenderer->Init(hWnd, TRUE);
		if (FAILED(hr))
		{// 初期化処理が失敗した場合

			return E_FAIL;
		}
	}


	//**********************************
	// サウンド
	//**********************************
	if (m_pSound != NULL)
	{// 確保されていたら
		return E_FAIL;
	}

	// メモリ確保
	m_pSound = DEBUG_NEW CSound;

	if (m_pSound != NULL)
	{// メモリの確保が出来ていたら

		// 初期化処理
		hr = m_pSound->Init(hWnd);
		if (FAILED(hr))
		{// 初期化処理が失敗した場合

			return E_FAIL;
		}
	}


	//**********************************
	// デバッグ表示
	//**********************************
	if (m_pDebugProc != NULL)
	{// 確保されていたら
		return E_FAIL;
	}

	// メモリ確保
	m_pDebugProc = DEBUG_NEW CDebugProc;

	if (m_pDebugProc != NULL)
	{// メモリの確保が出来ていたら

		// 初期化処理
		hr = m_pDebugProc->Init(hInstance, hWnd);
		if (FAILED(hr))
		{// 初期化処理が失敗した場合

			return E_FAIL;
		}
	}


	//**********************************
	// ライト
	//**********************************
	if (m_pLight != NULL)
	{// 確保されていたら
		return E_FAIL;
	}

	// メモリ確保
	m_pLight = DEBUG_NEW CLight;

	if (m_pLight != NULL)
	{// メモリの確保が出来ていたら

		// 初期化処理
		hr = m_pLight->Init();
		if (FAILED(hr))
		{// 初期化処理が失敗した場合

			return E_FAIL;
		}
	}


	//**********************************
	// カメラ
	//**********************************
	if (m_pCamera != NULL)
	{// 確保されていたら
		return E_FAIL;
	}

	// メモリ確保
	m_pCamera = DEBUG_NEW CCamera;

	if (m_pCamera != NULL)
	{// メモリの確保が出来ていたら

		// 初期化処理
		hr = m_pCamera->Init();
		if (FAILED(hr))
		{// 初期化処理が失敗した場合

			return E_FAIL;
		}
	}

	//**********************************
	// 全てのテクスチャ読み込み
	//**********************************
	if (m_pTexture != NULL)
	{// 確保されていたら
		return E_FAIL;
	}

	// メモリ確保
	m_pTexture = DEBUG_NEW CTexture;

	if (m_pTexture != NULL)
	{// メモリの確保が出来ていたら

		// 初期化処理
		hr = m_pTexture->Load();
		if (FAILED(hr))
		{// 初期化処理が失敗した場合

			return E_FAIL;
		}

		// 初期化処理
		hr = m_pTexture->LoadAll();

		if (FAILED(hr))
		{// 初期化処理が失敗した場合

			return E_FAIL;
		}
	}

	//**********************************
	// 遷移なしフェード
	//**********************************
	m_pInstantFade = CInstantFade::Create();

	if (m_pInstantFade == NULL)
	{// 失敗していたら
		return E_FAIL;
	}

	//**********************************
	// ポーズ
	//**********************************
	m_pPause = CPause::Create();

	if (m_pPause == NULL)
	{// 失敗していたら
		return E_FAIL;
	}

	//**********************************
	// リザルトマネージャ
	//**********************************
	m_pResultManager = CResultManager::Create();

	if (m_pResultManager == NULL)
	{// 失敗していたら
		return E_FAIL;
	}

	//**********************************
	// ランキングマネージャ
	//**********************************
	m_pRankingManager = CRankingManager::Create();

	if (m_pRankingManager == NULL)
	{// 失敗していたら
		return E_FAIL;
	}

	//**********************************
	// フェード
	//**********************************
	m_pFade = CFade::Create();

	if (m_pFade == NULL)
	{// 失敗していたら
		return E_FAIL;
	}

	//**********************************
	// 一定の行動マネージャ
	//**********************************
	m_pFixedMoveManager = CEnemyFixedMoveManager::Create();
	if (m_pFixedMoveManager == NULL)
	{// 失敗していたら
		return E_FAIL;
	}
	m_pFixedMoveManager->ReadText("data\\TEXT\\enemy_act_fixed.txt");


	return S_OK;
}

//==========================================================================
// 次のモード設定
//==========================================================================
void CManager::SetMode(CScene::MODE mode)
{
	// 前回のモード設定
	m_OldMode = GetMode();

	if (mode == CScene::MODE_GAME && m_pResultManager != NULL)
	{// 次のモードがゲームだったら

		// スコア情報リセット
		m_pResultManager->Reset();
		m_pRankingManager->Reset();
	}

	// BGMストップ
	if (m_pSound != NULL && mode != CScene::MODE_RANKING)
	{
		m_pSound->StopSound();
	}

	// メモリの確保がされていたら
	if (m_pScene != NULL)
	{
		// 終了処理
		m_pScene->Uninit();
		delete m_pScene;
		m_pScene = NULL;
	}

	// 全てのオブジェクト破棄
	CObject::ReleaseAll();

	// ポーズ状況入れ替え
	if (m_pPause != NULL)
	{
		if (m_pPause->IsPause() == true)
		{// ポーズ中だったら
			m_pPause->SetPause();
		}
	}

	// カメラの情報リセット
	if (m_pCamera != NULL)
	{
		m_pCamera->Reset(mode);
	}

	// 生成処理
	m_pScene = CScene::Create(mode);

	// 初期化処理
	if (m_pScene != NULL)
	{
		m_pScene->Init();
	}

	m_bHitStop = false;		// ヒットストップの判定
	m_nCntHitStop = 0;		// ヒットストップのカウンター
}

//==========================================================================
// 現在のモード取得
//==========================================================================
CScene::MODE CManager::GetMode(void)
{
	// モード取得
	if (m_pScene != NULL)
	{
		return m_pScene->GetMode();
	}
	else
	{
		return CScene::MODE_NONE;
	}
}

//==========================================================================
// 終了処理
//==========================================================================
void CManager::Uninit(void)
{
	// 全てのオブジェクト破棄
	CObject::ReleaseAll();

	// BGMストップ
	m_pSound->StopSound();

	// キーボードの破棄
	if (m_pInputKeyboard != NULL)
	{// メモリの確保が出来ていたら

		// 終了処理
		m_pInputKeyboard->Uninit();

		// メモリの開放
		delete m_pInputKeyboard;
		m_pInputKeyboard = NULL;
	}

	// ゲームパッドの破棄
	if (m_pInputGamepad != NULL)
	{// メモリの確保が出来ていたら

		// 終了処理
		m_pInputGamepad->Uninit();

		// メモリの開放
		delete m_pInputGamepad;
		m_pInputGamepad = NULL;
	}

	// マウスの破棄
	if (m_pInputMouse != NULL)
	{// メモリの確保が出来ていたら

		// 終了処理
		m_pInputMouse->Uninit();

		// メモリの開放
		delete m_pInputMouse;
		m_pInputMouse = NULL;
	}

	// レンダラーの破棄
	if (m_pRenderer != NULL)
	{// メモリの確保が出来ていたら

		// 終了処理
		m_pRenderer->Uninit();

		// メモリの開放
		delete m_pRenderer;
		m_pRenderer = NULL;
	}

	// ライトの破棄
	if (m_pLight != NULL)
	{// メモリの確保が出来ていたら

		// 終了処理
		m_pLight->Uninit();

		// メモリの開放
		delete m_pLight;
		m_pLight = NULL;
	}

	// カメラの破棄
	if (m_pCamera != NULL)
	{// メモリの確保が出来ていたら

		// 終了処理
		m_pCamera->Uninit();

		// メモリの開放
		delete m_pCamera;
		m_pCamera = NULL;
	}

	// サウンドの破棄
	if (m_pSound != NULL)
	{// メモリの確保が出来ていたら

		// 終了処理
		m_pSound->Uninit();

		// メモリの開放
		delete m_pSound;
		m_pSound = NULL;
	}

	// デバッグ表示の破棄
	if (m_pDebugProc != NULL)
	{// メモリの確保が出来ていたら

		// 終了処理
		m_pDebugProc->Uninit();

		// メモリの開放
		delete m_pDebugProc;
		m_pDebugProc = NULL;
	}

	// エディットの破棄
	if (m_pEdit != NULL)
	{// メモリの確保が出来ていたら

		m_pEdit = NULL;
	}

	//**********************************
	// 全てのテクスチャ破棄
	//**********************************
	if (m_pTexture != NULL)
	{// メモリの確保が出来ていたら

		// 終了処理
		m_pTexture->Unload();

		// メモリの開放
		delete m_pTexture;
		m_pTexture = NULL;
	}

	if (m_pScene != NULL)
	{// メモリの確保が出来ていたら

		// 終了処理
		m_pScene->Uninit();
		delete m_pScene;
		m_pScene = NULL;
	}

	// フェードの破棄
	if (m_pFade != NULL)
	{// メモリの確保がされていたら

		// 終了処理
		m_pFade->Uninit();
		delete m_pFade;
		m_pFade = NULL;
	}

	// 遷移なしフェードの破棄
	if (m_pInstantFade != NULL)
	{// メモリの確保がされていたら

		// 終了処理
		m_pInstantFade->Uninit();
		delete m_pInstantFade;
		m_pInstantFade = NULL;
	}

	if (m_pPause != NULL)
	{// メモリの確保がされていたら

		// 終了処理
		m_pPause->Uninit();
		delete m_pPause;
		m_pPause = NULL;
	}

	if (m_pResultManager != NULL)
	{// メモリの確保がされていたら

		// 終了処理
		m_pResultManager->Uninit();
		delete m_pResultManager;
		m_pResultManager = NULL;
	}

	if (m_pRankingManager != NULL)
	{// メモリの確保がされていたら

		// 終了処理
		m_pRankingManager->Uninit();
		delete m_pRankingManager;
		m_pRankingManager = NULL;
	}

	if (m_pFixedMoveManager != NULL)
	{// メモリの確保がされていたら

		// 終了処理
		m_pFixedMoveManager->Uninit();
		delete m_pFixedMoveManager;
		m_pFixedMoveManager = NULL;
	}

}

//==========================================================================
// 更新処理
//==========================================================================
void CManager::Update(void)
{
	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = GetInputKeyboard();

	// 過去の時間保存
	m_OldTime = m_CurrentTime;

	// 現在時間取得
	m_CurrentTime = timeGetTime();

	m_fDeltaTime = (float)(m_CurrentTime - m_OldTime) / 1000;

	// フェードの更新処理
	m_pFade->Update();

	// 遷移なしフェードの更新処理
	m_pInstantFade->Update();

	// キーボードの更新処理
	m_pInputKeyboard->Update();

	// ゲームパッドの更新処理
	m_pInputGamepad->Update();

	// マウスの更新処理
	m_pInputMouse->Update();

	if ((pInputKeyboard->GetTrigger(DIK_P) == true || m_pInputGamepad->GetTrigger(CInputGamepad::BUTTON_START, 0) == true) &&
		m_pFade->GetState() == CFade::STATE_NONE &&
		GetMode() == CScene::MODE_GAME)
	{// フェード中じゃないとき

		// サウンド再生
		GetSound()->PlaySound(CSound::LABEL_SE_TUTORIALWINDOW);
		m_pPause->SetPause();
	}

	// ポーズの更新処理
	if (m_pPause->IsPause() == true)
	{// ポーズ中だったら
		m_pPause->Update();

#if _DEBUG

		// カメラの更新処理
		m_pCamera->Update();
#endif

		return;
	}

	if (m_bHitStop == true)
	{// ヒットストップ中は更新停止

		// カメラの更新処理
		m_pCamera->Update();

		// ヒットストップカウンター減算
		m_nCntHitStop--;

		if (m_nCntHitStop <= 0)
		{// カウンターがなくなったら
			m_bHitStop = false;
		}
		return;
	}
	else
	{
		m_nCntHitStop = 20;
	}

#if _DEBUG
	if (pInputKeyboard->GetTrigger(DIK_F2) == true)
	{// F2でワイヤーフレーム切り替え
		m_bWireframe = m_bWireframe ? false : true;
	}

	if (pInputKeyboard->GetTrigger(DIK_F8) == true)
	{// F8でエディットモード切替え

		if (m_pEdit == NULL)
		{// NULLだったら

			// エディットの生成処理
			m_pEdit = CEdit::Create();
		}
		else
		{
			// 終了させる
			m_pEdit->Release();
			m_pEdit = NULL;
		}
	}
#endif

	// レンダラーの更新処理
	m_pRenderer->Update();

	// ライトの更新処理
	m_pLight->Update();

	// カメラの更新処理
	m_pCamera->Update();

	// デバッグ表示の更新処理
	m_pDebugProc->Update();

	if (m_pEdit == NULL && m_pScene != NULL)
	{// メモリの確保が出来ていたら
		m_pScene->Update();
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CManager::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = m_pRenderer->GetDevice();

	if (m_bWireframe)
	{
		pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);	// 埋めるモード
	}

	// レンダラーの描画処理
	m_pRenderer->Draw();

	pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);	// 埋めるモード
}

//==========================================================================
// 経過時間取得
//==========================================================================
float CManager::DeltaTime(void)
{
	return m_fDeltaTime;
}

//==========================================================================
// ヒットストップの設定
//==========================================================================
void CManager::SetEnableHitStop(int nCntHitStop)
{
	m_bHitStop = true;
	m_nCntHitStop = nCntHitStop;
}

//==========================================================================
// レンダラーの取得
//==========================================================================
CRenderer *CManager::GetRenderer(void)
{
	return m_pRenderer;
}

//==========================================================================
// キーボードの取得
//==========================================================================
CInputKeyboard *CManager::GetInputKeyboard(void)
{
	return m_pInputKeyboard;
}

//==========================================================================
// ゲームパッドの取得
//==========================================================================
CInputGamepad *CManager::GetInputGamepad(void)
{
	return m_pInputGamepad;
}

//==========================================================================
// マウスの取得
//==========================================================================
CInputMouse *CManager::GetInputMouse(void)
{
	return m_pInputMouse;
}

//==========================================================================
// サウンドの取得
//==========================================================================
CSound *CManager::GetSound(void)
{
	return m_pSound;
}

//==========================================================================
// デバッグ表示の取得
//==========================================================================
CDebugProc *CManager::GetDebugProc(void)
{
	return m_pDebugProc;
}

//==========================================================================
// ライトの取得
//==========================================================================
CLight *CManager::GetLight(void)
{
	return m_pLight;
}

//==========================================================================
// カメラの取得
//==========================================================================
CCamera *CManager::GetCamera(void)
{
	return m_pCamera;
}

//==========================================================================
// テクスチャの取得
//==========================================================================
CTexture *CManager::GetTexture(void)
{
	return m_pTexture;
}

//==========================================================================
// エディットの取得
//==========================================================================
CEdit *CManager::GetEdit(void)
{
	return m_pEdit;
}

//==========================================================================
// フェードの取得
//==========================================================================
CFade *CManager::GetFade(void)
{
	return m_pFade;
}

//==========================================================================
// 遷移なしフェードの取得
//==========================================================================
CInstantFade *CManager::GetInstantFade(void)
{
	return m_pInstantFade;
}

//==========================================================================
// ポーズの取得
//==========================================================================
CPause *CManager::GetPause(void)
{
	return m_pPause;
}

//==========================================================================
// リザルトマネージャ取得
//==========================================================================
CResultManager *CManager::GetResultManager(void)
{
	return m_pResultManager;
}

//==========================================================================
// ランキングマネージャのオブジェクト
//==========================================================================
CRankingManager *CManager::GetRankingManager(void)
{
	return m_pRankingManager;
}

//==========================================================================
// 一定の行動マネージャ取得
//==========================================================================
CEnemyFixedMoveManager *CManager::GetFixedManager(void)
{
	return m_pFixedMoveManager;
}

//==========================================================================
// シーン取得
//==========================================================================
CScene *CManager::GetScene(void)
{
	return m_pScene;
}

//--------------------------------------------------------------------------[ CSceneの処理 ]--------------------------------------------------------------------------//
