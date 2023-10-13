//=============================================================================
// 
//  ポーズ処理 [pause.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "pause.h"
#include "manager.h"
#include "renderer.h"
#include "object2D.h"
#include "input.h"
#include "texture.h"
#include "fade.h"
#include "sound.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define DIS_POSY	(120.0f)	// 選択肢の間隔

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
const char *CPause::m_apTextureFile[CPause::VTX_MAX] =		// テクスチャのファイル
{
	NULL,
	"data\\TEXTURE\\pause_menu.png",
	"data\\TEXTURE\\pause_game.png",
	"data\\TEXTURE\\pause_retry.png",
	"data\\TEXTURE\\pause_title.png",
};

//==========================================================================
// コンストラクタ
//==========================================================================
CPause::CPause()
{
	// 値のクリア
	for (int nCntVtx = 0; nCntVtx < VTX_MAX; nCntVtx++)
	{
		m_aObject2D[nCntVtx] = NULL;	// オブジェクト2Dのオブジェクト
		m_nTexIdx[nCntVtx] = 0;			// テクスチャのインデックス番号
	}
	m_bPause = false;	// ポーズの判定
	m_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);		// ポーズのカラー
	m_nSelect = 0;									// 選択肢
}

//==========================================================================
// デストラクタ
//==========================================================================
CPause::~CPause()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CPause *CPause::Create(void)
{
	// 生成用のオブジェクト
	CPause *pFade = NULL;

	if (pFade == NULL)
	{// NULLだったら

		// メモリの確保
		pFade = DEBUG_NEW CPause;

		if (pFade != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			if (FAILED(pFade->Init()))
			{// 失敗していたら
				return NULL;
			}
		}
		else
		{
			delete pFade;
			pFade = NULL;
		}

		return pFade;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CPause::Init(void)
{
	// 生成処理
	for (int nCntVtx = 0; nCntVtx < VTX_MAX; nCntVtx++)
	{
		// 生成処理
		m_aObject2D[nCntVtx] = CObject2D::Create(7);

		if (m_aObject2D[nCntVtx] == NULL)
		{// 失敗していたら
			return E_FAIL;
		}

		// テクスチャの割り当て
		m_nTexIdx[nCntVtx] = CManager::GetTexture()->Regist(m_apTextureFile[nCntVtx]);

		// テクスチャの割り当て
		m_aObject2D[nCntVtx]->BindTexture(m_nTexIdx[nCntVtx]);

		if (nCntVtx == VTX_FADE)
		{// 黒幕の時
			m_aObject2D[nCntVtx]->SetSize(D3DXVECTOR2(640.0f, 360.0f));				// サイズ
			m_aObject2D[nCntVtx]->SetPosition(D3DXVECTOR3(640.0f, 360.0f, 0.0f));	// 位置
			m_aObject2D[nCntVtx]->SetColor(D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.5f));		// 色設定
		}
		else if (nCntVtx == VTX_WINDOW)
		{// ウィンドウの時
			m_aObject2D[nCntVtx]->SetSize(D3DXVECTOR2(640.0f, 360.0f));				// サイズ
			m_aObject2D[nCntVtx]->SetPosition(D3DXVECTOR3(640.0f, 360.0f, 0.0f));	// 位置
			m_aObject2D[nCntVtx]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));		// 色設定
		}
		else
		{// 選択肢
			m_aObject2D[nCntVtx]->SetSize(CManager::GetTexture()->GetImageSize(m_nTexIdx[nCntVtx]) * 0.45f);	// サイズ
			m_aObject2D[nCntVtx]->SetPosition(D3DXVECTOR3(640.0f, 410.0f + ((nCntVtx - VTX_RETRY) * DIS_POSY), 0.0f));	// 位置
			m_aObject2D[nCntVtx]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));	// 色設定
		}
	}

	// 種類の設定
	//m_aObject2D->SetType(CObject::TYPE_FADE);

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CPause::Uninit(void)
{
	for (int nCntVtx = 0; nCntVtx < VTX_MAX; nCntVtx++)
	{
		if (m_aObject2D[nCntVtx] != NULL)
		{// NULLじゃなかったら

			// 終了処理
			m_aObject2D[nCntVtx]->Uninit();
			delete m_aObject2D[nCntVtx];
			m_aObject2D[nCntVtx] = NULL;
		}
	}
}

//==========================================================================
// 更新処理
//==========================================================================
void CPause::Update(void)
{
	if (m_bPause == false)
	{// ポーズ中じゃなかったら
		return;
	}

	if (CManager::GetFade()->GetState() != CFade::STATE_NONE)
	{// フェード中じゃない時
		return;
	}

	// 選択肢の更新処理
	UpdateSelect(m_nSelect);

	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	// ゲームパッド情報取得
	CInputGamepad *pInputGamepad = CManager::GetInputGamepad();

	if (pInputKeyboard->GetTrigger(DIK_W) == true || pInputGamepad->GetTrigger(CInputGamepad::BUTTON_UP, 0))
	{// 上系が押された

		// パターンNo.を更新
		m_nSelect = (m_nSelect + (MENU_MAX - 1)) % MENU_MAX;

		// サウンド再生
		CManager::GetSound()->PlaySound(CSound::LABEL_SE_CURSOR);
	}
	else if (pInputKeyboard->GetTrigger(DIK_S) == true || pInputGamepad->GetTrigger(CInputGamepad::BUTTON_DOWN, 0))
	{// 下系が押された

		// パターンNo.を更新
		m_nSelect = (m_nSelect + 1) % MENU_MAX;

		// サウンド再生
		CManager::GetSound()->PlaySound(CSound::LABEL_SE_CURSOR);
	}


	if (pInputGamepad->GetStickSelect(CInputGamepad::STICK_Y) == false && pInputGamepad->GetStickMoveL(0).y > 0)
	{// 上に倒された, スティックの判定がOFFの場合

		// 左スティックの判定を渡す
		pInputGamepad->SetEnableStickSelect(true, CInputGamepad::STICK_Y);

		// パターンNo.を更新
		m_nSelect = (m_nSelect + (MENU_MAX - 1)) % MENU_MAX;

		// サウンド再生
		CManager::GetSound()->PlaySound(CSound::LABEL_SE_CURSOR);
	}
	else if (pInputGamepad->GetStickSelect(CInputGamepad::STICK_Y) == false && pInputGamepad->GetStickMoveL(0).y < 0)
	{// 下に倒された, スティックの判定がOFFの場合

		// 左スティックの判定を渡す
		pInputGamepad->SetEnableStickSelect(true, CInputGamepad::STICK_Y);

		// パターンNo.を更新
		m_nSelect = (m_nSelect + 1) % MENU_MAX;

		// サウンド再生
		CManager::GetSound()->PlaySound(CSound::LABEL_SE_CURSOR);
	}


	if (pInputKeyboard->GetTrigger(DIK_RETURN) == true || pInputGamepad->GetTrigger(CInputGamepad::BUTTON_A, 0))
	{// 決定が押された

		switch (m_nSelect)
		{
		case MENU_RETURNGAME:
			SetPause();
			break;

		case MENU_RETRY:

			// サウンド再生
			CManager::GetSound()->PlaySound(CSound::LABEL_SE_DONE);

			// ゲームをリトライする
			CManager::GetFade()->SetFade(CScene::MODE_GAME);
			break;

		case MENU_RETURNTITLE:

			// サウンド再生
			CManager::GetSound()->PlaySound(CSound::LABEL_SE_DONE);

			// タイトルに戻る
			CManager::GetFade()->SetFade(CScene::MODE_TITLE);
			break;
		}
	}



	// 更新処理
	for (int nCntVtx = 0; nCntVtx < VTX_MAX; nCntVtx++)
	{
		if (m_aObject2D[nCntVtx] != NULL)
		{// NULLじゃなかったら

			// 更新処理
			m_aObject2D[nCntVtx]->Update();

			// 頂点座標更新
			m_aObject2D[nCntVtx]->SetVtx();
		}
	}
}


//==================================================================================
//ポーズ中の選択肢更新
//==================================================================================
void CPause::UpdateSelect(int nSelect)
{
	static float fMoveAlpha = 0.008f;

	if (m_col.a > 1.0f)
	{//不透明度100%
		m_col.a = 1.0f;
		fMoveAlpha *= -1;
	}
	else if (m_col.a < 0.4f)
	{//不透明度20%
		m_col.a = 0.4f;
		fMoveAlpha *= -1;
	}

	//不透明度の更新
	m_col.a += fMoveAlpha;

	//選択肢から始める
	for (int nCount = VTX_CONTINUE; nCount < VTX_MAX; nCount++)
	{
		// 色取得
		D3DXCOLOR col = m_aObject2D[nCount]->GetColor();

		if (nCount != nSelect + VTX_CONTINUE)
		{// 今の選択肢じゃないとき

			// 頂点カラーの設定
			col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		}
		else
		{
			// 頂点カラーの設定
			col = m_col;
		}

		// 色設定
		m_aObject2D[nCount]->SetColor(col);
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CPause::Draw(void)
{
	if (m_bPause == false)
	{// ポーズ中じゃなかったら
		return;
	}

	// 描画処理
	for (int nCntVtx = 0; nCntVtx < VTX_MAX; nCntVtx++)
	{
		if (m_aObject2D[nCntVtx] != NULL)
		{// NULLじゃなかったら

			// 描画処理
			m_aObject2D[nCntVtx]->Draw();
		}
	}
}

//==========================================================================
// ポーズの設定
//==========================================================================
void CPause::SetPause(void)
{
	// 使用状況変更
	m_bPause = m_bPause ? false : true;
	m_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);		// ポーズのカラー
	m_nSelect = 0;									// 選択肢
}

//==========================================================================
// 使用状況取得
//==========================================================================
bool CPause::IsPause(void)
{
	return m_bPause;
}

//==========================================================================
// オブジェクト2Dオブジェクトの取得
//==========================================================================
CObject2D **CPause::GetObject2D(void)
{
	return &m_aObject2D[0];
}