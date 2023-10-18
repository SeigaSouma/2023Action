//=============================================================================
// 
//  ランキング処理 [ranking.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "ranking.h"
#include "input.h"
#include "fade.h"
#include "renderer.h"
#include "calculation.h"
#include "debugproc.h"
#include "result_screen.h"
#include "rankingscore.h"
#include "enemymanager.h"

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
int CRanking::m_nRandStage = 0;	// ステージのランダムインデックス番号
CRankingScore *CRanking::m_pRankingScore = NULL;	// ランキングスコアのオブジェクト
bool CRanking::m_bAllArrival = false;		// 全て到着した判定

//==========================================================================
// コンストラクタ
//==========================================================================
CRanking::CRanking()
{
	m_pRankingScore = NULL;	// ランキングスコアのオブジェクト
	m_nCntSwitch = 0;		// 切り替えのカウンター
	m_bAllArrival = false;	// 全て到着した判定
}

//==========================================================================
// デストラクタ
//==========================================================================
CRanking::~CRanking()
{

}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CRanking::Init(void)
{
	// ステージのランダムインデックス番号
	m_nRandStage = Random(1, 3);

	// 初期化処理
	if (FAILED(CScene::Init()))
	{// 失敗した場合
		return E_FAIL;
	}

	// ランキングのスコア生成
	m_pRankingScore = CRankingScore::Create();

	// 成功
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CRanking::Uninit(void)
{
	// ランキングスコアのオブジェクト
	m_pRankingScore = NULL;

	// 終了処理
	CScene::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CRanking::Update(void)
{
	CManager::GetInstance()->GetDebugProc()->Print(
		"現在のモード：【ランキング】\n"
		"切り替え：【 F 】\n\n");

	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// ゲームパッド情報取得
	CInputGamepad *pInputGamepad = CManager::GetInstance()->GetInputGamepad();

	// 切り替えのカウンター加算
	m_nCntSwitch++;

	if (m_nCntSwitch >= 60 * 20)
	{
		// モード設定
		CManager::GetInstance()->GetFade()->SetFade(CScene::MODE_TITLE);
	}

	if (pInputKeyboard->GetTrigger(DIK_RETURN) || pInputGamepad->GetTrigger(CInputGamepad::BUTTON_A, 0) == true)
	{
		if (m_bAllArrival == true)
		{
			// モード設定
			CManager::GetInstance()->GetFade()->SetFade(CScene::MODE_TITLE);
		}
		else
		{
			// 全ての到着処理
			m_pRankingScore->SetAllArrival();
			m_bAllArrival = true;
		}
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CRanking::Draw(void)
{

}

//==========================================================================
// 到着設定ON
//==========================================================================
void CRanking::SetEnableArrival(void)
{
	m_bAllArrival = true;
}
