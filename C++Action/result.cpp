//=============================================================================
// 
//  リザルト処理 [result.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "result.h"
#include "input.h"
#include "fade.h"
#include "renderer.h"
#include "calculation.h"
#include "debugproc.h"
#include "result_screen.h"
#include "player.h"
#include "sound.h"

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
CResultScreen *CResult::m_pResultScreen = NULL;	// リザルトスクリーンのオブジェクト
bool CResult::m_bAllArrival = false;		// 全て到着した判定

//==========================================================================
// コンストラクタ
//==========================================================================
CResult::CResult()
{
	// 値のクリア
	m_bAllArrival = false;	// 全て到着した判定
}

//==========================================================================
// デストラクタ
//==========================================================================
CResult::~CResult()
{

}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CResult::Init(void)
{

	// 初期化処理
	if (FAILED(CScene::Init()))
	{// 失敗した場合
		return E_FAIL;
	}

	// BGM再生
	CManager::GetSound()->PlaySound(CSound::LABEL_BGM_RESULT);

	// リザルト画面
	m_pResultScreen = CResultScreen::Create();

	// 成功
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CResult::Uninit(void)
{
	m_pResultScreen = NULL;

	// 終了処理
	CScene::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CResult::Update(void)
{
	CManager::GetDebugProc()->Print(
		"現在のモード：【リザルト】\n"
		"切り替え：【 F 】\n\n");

	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	// ゲームパッド情報取得
	CInputGamepad *pInputGamepad = CManager::GetInputGamepad();

	static int n = 0;
	n = (n + 1) % 5;
	if (n == 0)
	{
		// モード設定
		//CManager::GetFade()->SetFade(CScene::MODE_TITLE);
	}

	if (pInputKeyboard->GetTrigger(DIK_RETURN) || pInputGamepad->GetTrigger(CInputGamepad::BUTTON_A, 0) == true)
	{
		if (m_bAllArrival == true)
		{
			// モード設定
			CManager::GetFade()->SetFade(CScene::MODE_RANKING);
		}

		if (CManager::GetFade()->GetState() == CFade::STATE_NONE)
		{
			// 全ての到着処理
			m_pResultScreen->SetAllArrival();
			m_bAllArrival = true;
		}
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CResult::Draw(void)
{

}

//==========================================================================
// リザルトスクリーンの情報取得
//==========================================================================
CResultScreen *CResult::GetResultScreen(void)
{
	return m_pResultScreen;
}

//==========================================================================
// 到着設定ON
//==========================================================================
void CResult::SetEnableArrival(void)
{
	m_bAllArrival = true;
}
