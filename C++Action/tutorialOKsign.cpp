//=============================================================================
// 
//  チュートリアルウィンドウ処理 [tutorialOKsign.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "tutorialOKsign.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "calculation.h"
#include "input.h"
#include "fade.h"
#include "tutorial.h"
#include "tutorialwindow.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define MOVETIME	(100)	// 移動時間
#define TEXTURE		"data\\TEXTURE\\tutorialwindow\\tutorial_OK.png"

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
int CTutorialOKSign::m_nTexIdx = 0;	// テクスチャのインデックス番号

//==========================================================================
// コンストラクタ
//==========================================================================
CTutorialOKSign::CTutorialOKSign(int nPriority) : CObject2D(nPriority)
{
	// 値のクリア
	m_nCntMove = 0;		// 移動時間

}

//==========================================================================
// デストラクタ
//==========================================================================
CTutorialOKSign::~CTutorialOKSign()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CTutorialOKSign *CTutorialOKSign::Create(void)
{
	// 生成用のオブジェクト
	CTutorialOKSign *pWindow = NULL;

	if (pWindow == NULL)
	{// NULLだったら

		// メモリの確保
		pWindow = DEBUG_NEW CTutorialOKSign;

		if (pWindow != NULL)
		{// メモリの確保が出来ていたら

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
HRESULT CTutorialOKSign::Init(void)
{

	// オブジェクト2Dの初期化処理
	CObject2D::Init();

	// 種類の設定
	SetType(TYPE_OKSIGN);

	// 各種変数の初期化
	m_nCntMove = MOVETIME;		// 移動時間

	// テクスチャの割り当て
	if (m_nTexIdx == 0)
	{// まだ読み込んでなかったら
		m_nTexIdx = CManager::GetTexture()->Regist(TEXTURE);
	}

	// テクスチャの割り当て
	BindTexture(m_nTexIdx);

	// 設定
	SetSize(CManager::GetTexture()->GetImageSize(m_nTexIdx) * 0.5f);		// サイズ
	SetPosition(D3DXVECTOR3(640.0f, SCREEN_HEIGHT + GetSize().y, 0.0f));	// 位置

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CTutorialOKSign::Uninit(void)
{
	// オブジェクト2Dの終了処理
	CObject2D::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CTutorialOKSign::Update(void)
{
	// 頂点情報更新
	SetVtx();

	// 移動時間減算
	m_nCntMove--;

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 移動
	pos.y += (360.0f - pos.y) * 0.15f;

	// 位置設定
	SetPosition(pos);

	if (m_nCntMove > 0)
	{// 移動時間が終わってない
		return;
	}

	// リセット
	m_nCntMove = 0;

	// ウィンドウ生成
	CTutorialWindow::Create(CTutorial::GetStep()->GetNowStep());

	// 終了処理
	Uninit();
}
