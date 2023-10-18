//=============================================================================
// 
//  チュートリアルウィンドウ処理 [tutorialwindow.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "tutorialwindow.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "calculation.h"
#include "input.h"
#include "fade.h"
#include "tutorial.h"
#include "sound.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define WINDOW_POSITION	(D3DXVECTOR3(1920.0f, 360.0f, 0.0f))
#define WAITTIME	(15)	// 待機時間

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
const char *CTutorialWindow::m_apTextureFile[CTutorialStep::STEP_MAX] = // テクスチャのファイル
{
	"data\\TEXTURE\\tutorialwindow\\window_01.png",
	"data\\TEXTURE\\tutorialwindow\\window_02.png",
	"data\\TEXTURE\\tutorialwindow\\window_03.png",
	"data\\TEXTURE\\tutorialwindow\\window_04.png",
	"data\\TEXTURE\\tutorialwindow\\window_05.png",
};
int CTutorialWindow::m_nTexIdx[CTutorialStep::STEP_MAX] = {};	// テクスチャのインデックス番号

//==========================================================================
// コンストラクタ
//==========================================================================
CTutorialWindow::CTutorialWindow(int nPriority) : CObject2D(nPriority)
{
	// 値のクリア
	m_NowStep = CTutorialStep::STEP_WAIT;	// 今回のステップ
	m_nCntWait = 0;		// 待機時間

}

//==========================================================================
// デストラクタ
//==========================================================================
CTutorialWindow::~CTutorialWindow()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CTutorialWindow *CTutorialWindow::Create(CTutorialStep::STEP step)
{
	// 生成用のオブジェクト
	CTutorialWindow *pWindow = NULL;

	if (pWindow == NULL)
	{// NULLだったら

		// メモリの確保
		pWindow = DEBUG_NEW CTutorialWindow;

		if (pWindow != NULL)
		{// メモリの確保が出来ていたら

			// 引数の情報を渡す
			pWindow->m_NowStep = step;

			// 初期化処理
			pWindow->Init();
		}

		return pWindow;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CTutorialWindow::Init(void)
{

	// オブジェクト2Dの初期化処理
	CObject2D::Init();

	// 種類の設定
	SetType(TYPE_TITLESCREEN);

	// 各種変数の初期化
	m_nCntWait = WAITTIME;		// 待機時間


	// テクスチャの割り当て
	if (m_nTexIdx[m_NowStep] == 0)
	{// まだ読み込んでなかったら
		m_nTexIdx[m_NowStep] = CManager::GetInstance()->GetTexture()->Regist(m_apTextureFile[m_NowStep]);
	}

	// テクスチャの割り当て
	BindTexture(m_nTexIdx[m_NowStep]);

	// サイズ取得
	SetSize(D3DXVECTOR2(640.0f, 360.0f));	// サイズ
	SetPosition(WINDOW_POSITION);			// 位置


	// サウンド再生
	CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_TUTORIALWINDOW);

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CTutorialWindow::Uninit(void)
{
	// オブジェクト2Dの終了処理
	CObject2D::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CTutorialWindow::Update(void)
{
	// 頂点情報更新
	SetVtx();

	// 待機時間減算
	m_nCntWait--;

	if (m_nCntWait >= 0)
	{// 待機時間が終わったら

		D3DXVECTOR3 pos = GetPosition();			// 位置

		pos.x += (640.0f - WINDOW_POSITION.x) / (float)WAITTIME;

		SetPosition(pos);			// 位置
		return;
	}

	// リセット
	m_nCntWait = -1;

	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// ゲームパッド情報取得
	CInputGamepad *pInputGamepad = CManager::GetInstance()->GetInputGamepad();

	if (pInputKeyboard->GetTrigger(DIK_RETURN) || pInputGamepad->GetTrigger(CInputGamepad::BUTTON_A, 0) == true)
	{// 決定が押されていたら

		// 終了処理
		Uninit();
		return;
	}

}
