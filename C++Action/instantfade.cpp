//=============================================================================
// 
//  遷移なしフェード処理 [instantfade.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "instantfade.h"
#include "manager.h"
#include "renderer.h"
#include "object2D.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define ALPHAMOVE	(0.025f)

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CInstantFade::CInstantFade()
{
	// 値のクリア
	m_nDuration = 0;		// フェードまでの時間
	m_aObject2D = NULL;		// オブジェクト2Dのオブジェクト
	m_state = STATE_NONE;	// 状態
}

//==========================================================================
// デストラクタ
//==========================================================================
CInstantFade::~CInstantFade()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CInstantFade *CInstantFade::Create(void)
{
	// 生成用のオブジェクト
	CInstantFade *pFade = NULL;

	if (pFade == NULL)
	{// NULLだったら

		// メモリの確保
		pFade = DEBUG_NEW CInstantFade;

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
HRESULT CInstantFade::Init(void)
{
	// 生成処理
	m_aObject2D = CObject2D::Create(8);

	if (m_aObject2D == NULL)
	{// 失敗していたら
		return E_FAIL;
	}

	m_state = STATE_FADEIN;			// 状態
	m_aObject2D->SetSize(D3DXVECTOR2(640.0f, 360.0f));	// サイズ
	m_aObject2D->SetPosition(D3DXVECTOR3(640.0f, 360.0f, 0.0f));	// 位置
	m_aObject2D->SetColor(D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));	// 色設定

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CInstantFade::Uninit(void)
{
	if (m_aObject2D != NULL)
	{// NULLじゃなかったら

		// 終了処理
		m_aObject2D->Uninit();
		delete m_aObject2D;
		m_aObject2D = NULL;
	}
}

//==========================================================================
// 更新処理
//==========================================================================
void CInstantFade::Update(void)
{
	// 色取得
	D3DXCOLOR col = m_aObject2D->GetColor();

	switch (m_state)
	{
	case STATE_NONE:
		break;

	case STATE_FADEOUT:

		// 不透明度増加
		col.a += 1.0f / m_nDuration;

		if (col.a >= 1.0f)
		{// 目標まで行ったら
			col.a = 1.0f;
			m_state = STATE_FADECOMPLETION;
		}
		break;

	case STATE_FADEIN:

		// 不透明度減少
		col.a -= 1.0f / m_nDuration;

		if (col.a <= 0.0f)
		{// 透明になったら
			col.a = 0.0f;
			m_state = STATE_NONE;
		}
		break;

	case STATE_FADECOMPLETION:
		m_state = STATE_FADEIN;
		break;
	}

	// 色設定
	m_aObject2D->SetColor(col);

	// 頂点座標更新
	m_aObject2D->SetVtx();
}

//==========================================================================
// 描画処理
//==========================================================================
void CInstantFade::Draw(void)
{
	// 描画処理
	m_aObject2D->Draw();
}

//==========================================================================
// フェード設定
//==========================================================================
void CInstantFade::SetFade(D3DXCOLOR FadeColor, int nDuration)
{
	if (m_state == STATE_NONE)
	{// 何もしていないとき

		m_nDuration = nDuration;	// フェードまでの時間
		m_state = STATE_FADEOUT;	// フェードアウト状態に設定

		// 色設定
		m_aObject2D->SetColor(D3DXCOLOR(FadeColor.r, FadeColor.g, FadeColor.b, 0.0f));
	}
}

//==========================================================================
// 状態取得
//==========================================================================
CInstantFade::STATE CInstantFade::GetState(void)
{
	return m_state;
}

//==========================================================================
// オブジェクト2Dオブジェクトの取得
//==========================================================================
CObject2D *CInstantFade::GetObject2D(void)
{
	return m_aObject2D;
}