//=============================================================================
// 
//  チュートリアルステップ処理 [tutorialstep.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "tutorialstep.h"
#include "tutorialwindow.h"
#include "manager.h"
#include "input.h"
#include "fade.h"
#include "sound.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define SPEEDTIME	(150)	// スピード変更の時間

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CTutorialStep::CTutorialStep()
{
	// 値のクリア
	m_nCntWait = 0;			// 待機のカウンター
	m_nCntSpeedUP = 0;		// スピードアップのカウンター
	m_nCntSpeedDOWN = 0;	// スピードダウンのカウンター
	m_nCntImpactWave = 0;	// 円形波の回数
	m_nCntDirectWave = 0;	// 直線波の回数
	m_bEndStep = false;		// ステップの終了判定
	m_bSetOK = false;		// OKの設定判定
	m_step = STEP_WAIT;		// 現在のステップ
}

//==========================================================================
// デストラクタ
//==========================================================================
CTutorialStep::~CTutorialStep()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CTutorialStep *CTutorialStep::Create(void)
{
	// 生成用のオブジェクト
	CTutorialStep *pScene = NULL;

	if (pScene == NULL)
	{// NULLだったら

		// 生成処理
		pScene = DEBUG_NEW CTutorialStep;

		if (pScene != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			if (FAILED(pScene->Init()))
			{// 失敗していたら
				return NULL;
			}
		}

		return pScene;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CTutorialStep::Init(void)
{
	// 各種変数の初期化
	m_nCntWait = 0;			// 待機のカウンター
	m_nCntSpeedUP = 0;		// スピードアップのカウンター
	m_nCntSpeedDOWN = 0;	// スピードダウンのカウンター
	m_nCntImpactWave = 0;	// 円形波の回数
	m_nCntDirectWave = 0;	// 直線波の回数
	m_bEndStep = false;		// ステップの終了判定
	m_bSetOK = false;		// OKの設定判定

	// 成功
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CTutorialStep::Uninit(void)
{
	
}

//==========================================================================
// 更新処理
//==========================================================================
void CTutorialStep::Update(void)
{
	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	// ゲームパッド情報取得
	CInputGamepad *pInputGamepad = CManager::GetInputGamepad();

	switch (m_step)
	{
	case CTutorialStep::STEP_WAIT:

		// 待機時間加算
		m_nCntWait++;

		if (m_nCntWait >= 120)
		{// 待機が終了していたら

			// ステップの設定処理
			SetStep(STEP_WAIT);
		}
		break;

	case CTutorialStep::STEP_MAX:
		if (m_step >= CTutorialStep::STEP_MAX - 1 &&
			(pInputKeyboard->GetTrigger(DIK_BACKSPACE) || pInputGamepad->GetTrigger(CInputGamepad::BUTTON_START, 0) == true))
		{
			// モード設定
			CManager::GetFade()->SetFade(CScene::MODE_GAME);
		}
		break;
	}

	if (m_step > CTutorialStep::STEP_WAIT &&
		m_bEndStep == true && m_bSetOK == false)
	{// ステップが終了していたら && OKを設定していない

		// OKの生成処理
		m_bSetOK = true;

		// サウンド再生
		CManager::GetSound()->PlaySound(CSound::LABEL_SE_STEPCLEAR);
	}

}

//==========================================================================
// ステップの設定処理
//==========================================================================
void CTutorialStep::SetStep(STEP step)
{
	if (m_step != step)
	{
		return;
	}

	switch (step)
	{
	case CTutorialStep::STEP_WAIT:
		if (m_bEndStep == false)
		{
			CTutorialWindow::Create(m_step);
			m_bEndStep = true;
		}
		break;

	case CTutorialStep::STEP_SPEEDUP:
		// スピードアップのカウンター
		m_nCntSpeedUP++;

		if (m_bEndStep == false && m_nCntSpeedUP >= SPEEDTIME)
		{
			m_bEndStep = true;
		}
		break;

	case CTutorialStep::STEP_SPEEDDOWN:
		// スピードダウンのカウンター
		m_nCntSpeedDOWN++;

		if (m_bEndStep == false && m_nCntSpeedDOWN >= SPEEDTIME)
		{
			m_bEndStep = true;
		}
		break;

	case CTutorialStep::STEP_IMPACTWAVE:
		// 円形波の回数
		m_nCntImpactWave++;

		if (m_bEndStep == false && m_nCntImpactWave >= 2)
		{
			m_bEndStep = true;
		}
		break;

	case CTutorialStep::STEP_DIRECTWAVE:
		// 直線波の回数
		m_nCntDirectWave++;

		if (m_bEndStep == false && m_nCntDirectWave >= 2)
		{
			m_bEndStep = true;
		}
		break;

	case CTutorialStep::STEP_MAX:
		break;

	default:
		break;
	}
}

//==========================================================================
// ステップを進める
//==========================================================================
void CTutorialStep::AddStep(void)
{
	if (m_step < CTutorialStep::STEP_MAX && m_bEndStep == true)
	{// 進められる状態だったら
		m_step = (STEP)(m_step + 1);
		m_bEndStep = false;
	}
}

//==========================================================================
// OKサインのフラグOFFにする
//==========================================================================
void CTutorialStep::SetDisableOKSign(void)
{
	m_bSetOK = false;
}

//==========================================================================
// 現在のステップ取得
//==========================================================================
CTutorialStep::STEP CTutorialStep::GetNowStep(void)
{
	return m_step;
}

//==========================================================================
// ステップ終了の判定取得
//==========================================================================
bool CTutorialStep::IsEndStep(void)
{
	return m_bEndStep;
}
