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

//==========================================================================
// マクロ定義
//==========================================================================

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
CRenderer *CManager::m_pRenderer = NULL;				// レンダラーのオブジェクト
CInputKeyboard *CManager::m_pInputKeyboard = NULL;		// キーボードのオブジェクト
CInputGamepad *CManager::m_pInputGamepad = NULL;		// ゲームパッドのオブジェクト
CSound *CManager::m_pSound = NULL;						// サウンドのオブジェクト
CInputMouse *CManager::m_pInputMouse = NULL;			// マウスのオブジェクト
CDebugProc *CManager::m_pDebugProc = NULL;				// デバッグ表示のオブジェクト
CBG *CManager::m_pBg = NULL;							// 背景のオブジェクト
CLight *CManager::m_pLight = NULL;						// ライトのオブジェクト
CCamera *CManager::m_pCamera = NULL;					// カメラのオブジェクト
CMap *CManager::m_pMap = NULL;							// マップのオブジェクト
CTexture *CManager::m_pTexture = NULL;					// テクスチャのオブジェクト
CEdit *CManager::m_pEdit = NULL;						// エディットのオブジェクト
CScene *CManager::m_pScene = NULL;						// シーンのオブジェクト
CFade *CManager::m_pFade = NULL;						// フェードのオブジェクト
CInstantFade *CManager::m_pInstantFade = NULL;			// 遷移なしフェードのオブジェクト
CPause *CManager::m_pPause = NULL;						// ポーズのオブジェクト
CResultManager *CManager::m_pResultManager = NULL;		// リザルトマネージャのオブジェクト
CRankingManager *CManager::m_pRankingManager = NULL;	// ランキングマネージャのオブジェクト

bool CManager::m_bWireframe = false;					// ワイヤーフレーム
bool CManager::m_bHitStop = false;						// ヒットストップの判定
int CManager::m_nCntHitStop = 0;						// ヒットストップのカウンター
CScene::MODE CManager::m_OldMode = CScene::MODE_NONE;	// 前回のモード

//==========================================================================
// コンストラクタ
//==========================================================================
CManager::CManager()
{

}

//==========================================================================
// デストラクタ
//==========================================================================
CManager::~CManager()
{

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
	// マップ
	//**********************************


	//**********************************
	// オブジェクト生成
	//**********************************
	CObject2D *pObject2D = NULL;


#if 0
	// ブロックの生成処理
	pObject2D = CBlock::Create(D3DXVECTOR3(640.0f, 550.0f, 0.0f), D3DXVECTOR2(50.0f, 50.0f), 2, 1);
	pObject2D = CBlock::Create(D3DXVECTOR3(340.0f, 550.0f, 0.0f), D3DXVECTOR2(50.0f, 50.0f), 2, 1);
	pObject2D = CBlock::Create(D3DXVECTOR3(100.0f, 650.0f, 0.0f), D3DXVECTOR2(120.0f, 120.0f));

	pObject2D = CBlock::Create(D3DXVECTOR3(640.0f, 400.0f, 0.0f), D3DXVECTOR2(30.0f, 30.0f));
	pObject2D = CBlock::Create(D3DXVECTOR3(450.0f, 150.0f, 0.0f), D3DXVECTOR2(30.0f, 30.0f), 1, 8);
	pObject2D = CBlock::Create(D3DXVECTOR3(810.0f, 150.0f, 0.0f), D3DXVECTOR2(30.0f, 30), 1, 8);
	pObject2D = CBlock::Create(D3DXVECTOR3(480.0f, 150.0f, 0.0f), D3DXVECTOR2(30.0f, 30.0f), 11, 1);


	for (int nCnt = 0; nCnt < 30; nCnt++)
	{
		CBlock::Create(D3DXVECTOR3(nCnt * 50.0f, 700.0f, 0.0f), D3DXVECTOR2(50.0f, 50.0f));
	}

	// 下から順番
	for (int nCnt = 0; nCnt < 8; nCnt++)
	{
		CBlock::Create(D3DXVECTOR3(1000.0f, 550.0f - nCnt * 50.0f, 0.0f), D3DXVECTOR2(50.0f, 50.0f));
	}

	// 上から順番
	for (int nCnt = 0; nCnt < 8; nCnt++)
	{
		CBlock::Create(D3DXVECTOR3(1200.0f, 200.0f + nCnt * 50.0f, 0.0f), D3DXVECTOR2(50.0f, 50.0f));
	}

	// ブロックの生成処理
	pObject2D = CItem::Create(CItem::TYPE_ITEM, D3DXVECTOR3(640.0f, 500.0f, 0.0f));
	pObject2D = CItem::Create(CItem::TYPE_ITEM, D3DXVECTOR3(340.0f, 500.0f, 0.0f));
	pObject2D = CItem::Create(CItem::TYPE_ITEM, D3DXVECTOR3(800.0f, 600.0f, 0.0f));
#endif

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

	// 背景の破棄
	if (m_pBg != NULL)
	{// メモリの確保が出来ていたら

		m_pBg = NULL;
	}

	// マップの破棄
	if (m_pMap != NULL)
	{// メモリの確保が出来ていたら

		m_pMap = NULL;
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

}

//==========================================================================
// 更新処理
//==========================================================================
void CManager::Update(void)
{
	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();


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
	CManager::GetSound()->PlaySound(CSound::LABEL_SE_TUTORIALWINDOW);
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

	if (m_pScene != NULL)
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
// 背景の取得
//==========================================================================
CBG *CManager::GetBg(void)
{
	return m_pBg;
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
// シーン取得
//==========================================================================
CScene *CManager::GetScene(void)
{
	return m_pScene;
}

//--------------------------------------------------------------------------[ CSceneの処理 ]--------------------------------------------------------------------------//
