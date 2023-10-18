//=============================================================================
// 
//  ゲーム処理 [game.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "manager.h"
#include "game.h"
#include "renderer.h"
#include "calculation.h"
#include "debugproc.h"
#include "fade.h"
#include "camera.h"

#include "input.h"
#include "player.h"
#include "enemy.h"
#include "score.h"
#include "timer.h"
#include "map.h"
#include "elevation.h"
#include "sound.h"
#include "edit_controlpoint.h"
#include "bulletmanager.h"

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
CScore *CGame::m_pScore = NULL;					// スコアのオブジェクト
CTimer *CGame::m_pTimer = NULL;					// タイマーのオブジェクト
CPowerGauge *CGame::m_pPowerGauge = NULL;		// パワーゲージのオブジェクト
CEditControlPoint *CGame::m_pEditControlPoint = NULL;	// 制御点エディターのオブジェクト
CBulletManager *CGame::m_pBulletManager = NULL;		// 弾マネージャのオブジェクト

//==========================================================================
// コンストラクタ
//==========================================================================
CGame::CGame()
{
	// 値のクリア
}

//==========================================================================
// デストラクタ
//==========================================================================
CGame::~CGame()
{

}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CGame::Init(void)
{

	// 初期化処理
	if (FAILED(CScene::Init()))
	{// 失敗した場合
		return E_FAIL;
	}

	// BGM再生
	CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_BGM_GAME);

	//**********************************
	// プレイヤー
	//**********************************
	if (CManager::GetInstance()->GetScene()->GetPlayer() != NULL)
	{
		CManager::GetInstance()->GetScene()->GetPlayer()->SetPosition(D3DXVECTOR3(0.0f, 0.0f, -1000.0f));
		CManager::GetInstance()->GetScene()->GetPlayer()->SetRotation(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	}

	//**********************************
	// 弾マネージャ
	//**********************************
	m_pBulletManager = CBulletManager::Create();


	// スコアの生成処理
	m_pScore = CScore::Create(D3DXVECTOR3(1000.0f, 50.0f, 0.0f));

	// タイマーの生成処理
	m_pTimer = CTimer::Create(D3DXVECTOR3(200.0f, 50.0f, 0.0f));

	CManager::GetInstance()->GetCamera()->Reset(CScene::MODE_GAME);

	// 成功
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CGame::Uninit(void)
{
	
	// スコアの破棄
	if (m_pScore != NULL)
	{// メモリの確保が出来ていたら

		// 終了処理
		m_pScore->Uninit();

		// メモリの開放
		delete m_pScore;
		m_pScore = NULL;
	}

	// タイマーの破棄
	if (m_pTimer != NULL)
	{// メモリの確保が出来ていたら

		// 終了処理
		m_pTimer->Uninit();

		// メモリの開放
		delete m_pTimer;
		m_pTimer = NULL;
	}

	if (m_pEditControlPoint != NULL)
	{
		// 終了させる
		m_pEditControlPoint->Uninit();
		delete m_pEditControlPoint;
		m_pEditControlPoint = NULL;
	}

	if (m_pBulletManager != NULL)
	{
		// 終了させる
		m_pBulletManager->Uninit();
		delete m_pBulletManager;
		m_pBulletManager = NULL;
	}

	// 終了処理
	CScene::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CGame::Update(void)
{
	CManager::GetInstance()->GetDebugProc()->Print(
		"現在のモード：【ゲーム】\n"
		"切り替え：【 F 】\n\n");

	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// ゲームパッド情報取得
	CInputGamepad *pInputGamepad = CManager::GetInstance()->GetInputGamepad();

#if 1
	// スコアの更新処理
	m_pScore->Update();

	if (CManager::GetInstance()->GetEdit() == NULL &&
		m_pEditControlPoint == NULL &&
		GetEnemyManager()->GetState() != CEnemyManager::STATE_COMBOANIM)
	{
		// タイマーの更新処理
		m_pTimer->Update();
	}
#endif

	if (GetEnemyManager() != NULL)
	{// 敵マネージャの更新処理
		GetEnemyManager()->Update();
	}

	if (pInputKeyboard->GetTrigger(DIK_F4))
	{// F4でマップエディット

		if (m_pEditControlPoint == NULL)
		{// NULLだったら

			// エディットの生成処理
			m_pEditControlPoint = CEditControlPoint::Create();
		}
		else
		{
			// 終了させる
			m_pEditControlPoint->Uninit();
			delete m_pEditControlPoint;
			m_pEditControlPoint = NULL;
		}
	}

	if (m_pEditControlPoint != NULL)
	{// 制御点マネージャの更新処理
		m_pEditControlPoint->Update();
	}

#if _DEBUG

	if (pInputKeyboard->GetTrigger(DIK_F))
	{
		// モード設定
		CManager::GetInstance()->GetFade()->SetFade(CScene::MODE_RESULT);
	}
#endif

	CScene::Update();

}

//==========================================================================
// 描画処理
//==========================================================================
void CGame::Draw(void)
{

}

//==========================================================================
// スコアの取得
//==========================================================================
CScore *CGame::GetScore(void)
{
	return m_pScore;
}

//==========================================================================
// タイマーの取得
//==========================================================================
CTimer *CGame::GetTimer(void)
{
	return m_pTimer;
}

//==========================================================================
// パワーゲージの取得
//==========================================================================
CPowerGauge *CGame::GetPowerGauge(void)
{
	return m_pPowerGauge;
}

//==========================================================================
// 制御点エディターの取得
//==========================================================================
CEditControlPoint *CGame::GetEditControlPoint(void)
{
	return m_pEditControlPoint;
}

//==========================================================================
// 弾マネージャの取得
//==========================================================================
CBulletManager *CGame::GetBulletManager(void)
{
	return m_pBulletManager;
}

//==========================================================================
// リセット処理
//==========================================================================
void CGame::Reset(void)
{

}