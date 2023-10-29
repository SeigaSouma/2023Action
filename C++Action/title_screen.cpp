//=============================================================================
// 
//  タイトル画面処理 [title_screen.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "title_screen.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "object2D.h"
#include "calculation.h"
#include "titleslash.h"
#include "input.h"
#include "titleslash_endress.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define WIDTH		(640.0f)	// 横幅
#define HEIGHT		(360.0f)	// 縦幅
#define ALPHATIME	(60)		// 不透明度更新の時間
#define LOGOMOVE_TIME	(180)	// ロゴ移動の時間
#define SLASH_TIME		(60)	// 斬撃の時間
#define SLASH_MOVETIME	(8)		// 斬撃の移動時間
#define LOGO_POSITION	(D3DXVECTOR3(640.0f, 200.0f, 0.0f))
#define LOGOSIZE_MULTIPLY	(0.55f)
#define CHANGE_TIME		(40)

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
const char *CTitleScreen::m_apTextureFile[VTX_LOGO_MAX] =			// テクスチャのファイル
{
	NULL,
	"data\\TEXTURE\\titlelogo_03.png",
	"data\\TEXTURE\\titlelogo_02.png",
	"data\\TEXTURE\\titlelogo_02.png",
	"data\\TEXTURE\\titlelogo_01.png",
	"data\\TEXTURE\\titlelogo_01.png",
};

const char *CTitleScreen::m_apTextureFile_Select[VTXSELECT_MAX] =	// テクスチャのファイル
{
	"data\\TEXTURE\\titlelogo_08.png",
};

//==========================================================================
// コンストラクタ
//==========================================================================
CTitleScreen::CTitleScreen(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	memset(&m_pObj2D[0], NULL, sizeof(m_pObj2D));				// オブジェクト2Dのオブジェクト
	memset(&m_pSelect2D[0], NULL, sizeof(m_pSelect2D));			// 選択肢のオブジェクト
	memset(&m_nTexIdx[0], 0, sizeof(m_nTexIdx));				// テクスチャのインデックス番号
	memset(&m_nTexIdx_Select[0], 0, sizeof(m_nTexIdx_Select));	// テクスチャのインデックス番号
	m_nCntAlpha = 0;		// 不透明度のカウンター
	m_nCntLogo = 0;		// ロゴ移動のカウント
	m_bStick = false;		// スティックの判定
}

//==========================================================================
// デストラクタ
//==========================================================================
CTitleScreen::~CTitleScreen()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CTitleScreen *CTitleScreen::Create(void)
{
	// 生成用のオブジェクト
	CTitleScreen *pTitleScreen = NULL;

	if (pTitleScreen == NULL)
	{// NULLだったら

		// メモリの確保
		pTitleScreen = DEBUG_NEW CTitleScreen;

		if (pTitleScreen != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			pTitleScreen->Init();
		}

		return pTitleScreen;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CTitleScreen::Init(void)
{
	// 種類の設定
	SetType(TYPE_TITLESCREEN);

	for (int nCntSelect = 0; nCntSelect < VTX_LOGO_MAX; nCntSelect++)
	{
		// 生成処理
		m_pObj2D[nCntSelect] = CObject2D::Create(8);

		// 種類の設定
		m_pObj2D[nCntSelect]->SetType(TYPE_TITLESELECT);

		// テクスチャの割り当て
		m_nTexIdx[nCntSelect] = CManager::GetInstance()->GetTexture()->Regist(m_apTextureFile[nCntSelect]);

		// テクスチャの割り当て
		m_pObj2D[nCntSelect]->BindTexture(m_nTexIdx[nCntSelect]);

		D3DXVECTOR2 size;
		// 各種変数の初期化
		switch (nCntSelect)
		{
		case VTX_BLACK:
			// サイズ取得
			m_pObj2D[nCntSelect]->SetSize(D3DXVECTOR2(640.0f, 360.0f));	// サイズ
			m_pObj2D[nCntSelect]->SetPosition(D3DXVECTOR3(640.0f, 360.0f, 0.0f));	// 位置
			m_pObj2D[nCntSelect]->SetColor(D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));	// 色
			break;

		case VTX_LOGO:
			// サイズ取得
			size = CManager::GetInstance()->GetTexture()->GetImageSize(m_nTexIdx[nCntSelect]);
			m_pObj2D[nCntSelect]->SetSize(size * LOGOSIZE_MULTIPLY);	// サイズ
			m_pObj2D[nCntSelect]->SetPosition(LOGO_POSITION);	// 位置
			break;

		case VTX_LOGOALPHA1:
			// サイズ取得
			size = CManager::GetInstance()->GetTexture()->GetImageSize(m_nTexIdx[nCntSelect]);
			m_pObj2D[nCntSelect]->SetSize(size * LOGOSIZE_MULTIPLY);	// サイズ
			m_pObj2D[nCntSelect]->SetPosition(LOGO_POSITION);	// 位置
			m_pObj2D[nCntSelect]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));	// 色
			break;

		case VTX_LOGOALPHA2:
			// サイズ取得
			size = CManager::GetInstance()->GetTexture()->GetImageSize(m_nTexIdx[nCntSelect]);
			m_pObj2D[nCntSelect]->SetSize(size * LOGOSIZE_MULTIPLY);	// サイズ
			m_pObj2D[nCntSelect]->SetPosition(LOGO_POSITION);	// 位置
			m_pObj2D[nCntSelect]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));	// 色
			break;

		case VTX_LOGOPERFECT:
			// サイズ取得
			size = CManager::GetInstance()->GetTexture()->GetImageSize(m_nTexIdx[nCntSelect]);
			m_pObj2D[nCntSelect]->SetSize(size * LOGOSIZE_MULTIPLY);	// サイズ
			m_pObj2D[nCntSelect]->SetPosition(LOGO_POSITION);	// 位置
			m_pObj2D[nCntSelect]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));	// 色
			break;

		case VTX_LOGOPERFECTALPHA:
			// サイズ取得
			size = CManager::GetInstance()->GetTexture()->GetImageSize(m_nTexIdx[nCntSelect]);
			m_pObj2D[nCntSelect]->SetSize(size * LOGOSIZE_MULTIPLY);	// サイズ
			m_pObj2D[nCntSelect]->SetPosition(LOGO_POSITION);	// 位置
			m_pObj2D[nCntSelect]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));	// 色
			break;
		}
	}

	for (int nCntSelect = 0; nCntSelect < VTXSELECT_MAX; nCntSelect++)
	{
		// 生成処理
		m_pSelect2D[nCntSelect] = CObject2D::Create(7);

		// 種類の設定
		m_pSelect2D[nCntSelect]->SetType(TYPE_TITLESELECT);

		// テクスチャの割り当て
		m_nTexIdx_Select[nCntSelect] = CManager::GetInstance()->GetTexture()->Regist(m_apTextureFile_Select[nCntSelect]);

		// テクスチャの割り当て
		m_pSelect2D[nCntSelect]->BindTexture(m_nTexIdx_Select[nCntSelect]);

		// 各種変数の初期化
		D3DXVECTOR2 size;
		switch (nCntSelect)
		{
		case VTXSELECT_SELECTSTART:

			// サイズ取得
			size = CManager::GetInstance()->GetTexture()->GetImageSize(m_nTexIdx_Select[nCntSelect]);
			m_pSelect2D[nCntSelect]->SetSize(D3DXVECTOR2(size.x * 0.4f, size.y * 0.4f));	// サイズ
			m_pSelect2D[nCntSelect]->SetPosition(D3DXVECTOR3(640.0f, 550.0f, 0.0f));	// 位置
			break;
		}
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CTitleScreen::Uninit(void)
{
	for (int nCntSelect = 0; nCntSelect < VTX_LOGO_MAX; nCntSelect++)
	{
		if (m_pObj2D[nCntSelect] != NULL)
		{// NULLじゃなかったら

			// 終了処理
			m_pObj2D[nCntSelect] = NULL;
		}
	}

	for (int nCntSelect = 0; nCntSelect < VTXSELECT_MAX; nCntSelect++)
	{
		if (m_pSelect2D[nCntSelect] != NULL)
		{// NULLじゃなかったら

			// 終了処理
			m_pSelect2D[nCntSelect] = NULL;
		}
	}

	// 情報削除
	Release();
}

//==========================================================================
// 更新処理
//==========================================================================
void CTitleScreen::Update(void)
{

	// 斬撃更新
	UpdateSlash();

	for (int nCntSelect = 0; nCntSelect < VTX_LOGO_MAX; nCntSelect++)
	{
		if (m_pObj2D[nCntSelect] == NULL)
		{// NULLだったら
			continue;
		}

		switch (nCntSelect)
		{
		case VTX_LOGO:
			// ロゴの更新処理
			UpdateLogo(nCntSelect);
			break;
		}

		// 頂点情報設定
		m_pObj2D[nCntSelect]->SetVtx();
	}

	for (int nCntSelect = 0; nCntSelect < VTXSELECT_MAX; nCntSelect++)
	{
		if (m_pSelect2D[nCntSelect] == NULL)
		{// NULLだったら
			continue;
		}

		switch (nCntSelect)
		{
		case VTXSELECT_SELECTSTART:
			// 選択肢の更新処理
			UpdateSelect(nCntSelect);
			break;
		}

		// 頂点情報設定
		m_pSelect2D[nCntSelect]->SetVtx();
	}
}

//==========================================================================
// 斬撃更新
//==========================================================================
void CTitleScreen::UpdateSlash()
{
	// ロゴのカウンター加算
	m_nCntLogo++;

	if (m_nCntLogo >= SLASH_TIME && m_nCntLogo <= SLASH_TIME + CHANGE_TIME)
	{// 斬撃の時間超えたら

		if (m_nCntLogo == SLASH_TIME)
		{
			// テクスチャ情報取得
			CTexture *pTexture = CManager::GetInstance()->GetTexture();
			int nTex = pTexture->Regist("data\\TEXTURE\\title_slash.png");

			// 斬撃生成
			CTitleSlash::Create(LOGO_POSITION);

			for (int nCntSelect = 0; nCntSelect < VTX_LOGO_MAX; nCntSelect++)
			{
				if (m_pObj2D[nCntSelect] == NULL)
				{// NULLだったら
					continue;
				}

				if (nCntSelect == VTX_LOGOALPHA1 ||
					nCntSelect == VTX_LOGOALPHA2)
				{
					m_pObj2D[nCntSelect]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f));
				}
			}
		}

		for (int nCntSelect = 0; nCntSelect < VTX_LOGO_MAX; nCntSelect++)
		{
			if (m_pObj2D[nCntSelect] == NULL)
			{// NULLだったら
				continue;
			}

			// 色取得
			D3DXCOLOR col = m_pObj2D[nCntSelect]->GetColor();
			D3DXVECTOR2 size = m_pObj2D[nCntSelect]->GetSize();

			if (nCntSelect == VTX_LOGOALPHA1 ||
				nCntSelect == VTX_LOGOALPHA2)
			{
				// 透明の処理
				UpdateSlashAlpha(nCntSelect);
				continue;
			}
			else if (nCntSelect == VTX_BLACK)
			{
				// 差分
				int nCntTime = m_nCntLogo - SLASH_TIME;

				// 色
				col.a = (1.0f - ((float)nCntTime / (float)CHANGE_TIME));
			}
			else if (nCntSelect == VTX_LOGO)
			{
				// 差分
				int nCntTime = m_nCntLogo - SLASH_TIME;

				// 色
				col.a = (1.0f - ((float)nCntTime / (float)CHANGE_TIME));
			}
			else if (nCntSelect == VTX_LOGOPERFECT)
			{
				// 差分
				int nCntTime = m_nCntLogo - SLASH_TIME;

				// 色
				col.a = (float)nCntTime / (float)CHANGE_TIME;
			}
			else if (nCntSelect == VTX_LOGOPERFECTALPHA)
			{
				// 差分
				int nCntTime = m_nCntLogo - SLASH_TIME;
				D3DXVECTOR2 DestSize = CManager::GetInstance()->GetTexture()->GetImageSize(m_nTexIdx[nCntSelect]) * 1.2f;

				size += (DestSize - size) * 0.1f;

				// 色
				col.a = 0.5f * (1.0f - ((float)nCntTime / (float)CHANGE_TIME));
			}

			// 色設定
			m_pObj2D[nCntSelect]->SetColor(col);
			m_pObj2D[nCntSelect]->SetSize(size);
		}
	}

	if (m_nCntLogo > SLASH_TIME + CHANGE_TIME)
	{// 斬撃の時間超えたら

		// ゲームパッド情報取得
		CInputGamepad *pInputGamepad = CManager::GetInstance()->GetInputGamepad();

		if (pInputGamepad->GetStickPositionRatioR(0).y <= 0.2f && pInputGamepad->GetStickPositionRatioR(0).y >= -0.2f &&
			pInputGamepad->GetStickPositionRatioR(0).x <= 0.2f && pInputGamepad->GetStickPositionRatioR(0).x >= -0.2f)
		{// 中央あたり
			// スティック倒せる状態にする
			m_bStick = false;
		}

		if (m_bStick == false &&
			(pInputGamepad->GetStickPositionRatioR(0).y >= 0.5f || pInputGamepad->GetStickPositionRatioR(0).y <= -0.5f ||
				pInputGamepad->GetStickPositionRatioR(0).x >= 0.5f || pInputGamepad->GetStickPositionRatioR(0).x <= -0.5f))
		{// 攻撃

			// スティック倒した判定
			m_bStick = true;
			CTitleSlashEndress::Create();
		}
	}
}

//==========================================================================
// 透明の斬撃更新
//==========================================================================
void CTitleScreen::UpdateSlashAlpha(int nCntSelect)
{
	// 情報取得
	D3DXCOLOR col = m_pObj2D[nCntSelect]->GetColor();
	D3DXVECTOR3 pos = m_pObj2D[nCntSelect]->GetPosition();

	D3DXVECTOR3 posDest[VTX_LOGO_MAX];
	posDest[VTX_LOGOALPHA1] = D3DXVECTOR3(640.0f + 250.0f, LOGO_POSITION.y - 120.0f, 0.0f);
	posDest[VTX_LOGOALPHA2] = D3DXVECTOR3(640.0f - 250.0f, LOGO_POSITION.y + 120.0f, 0.0f);

	// 差分
	int nCntTime = m_nCntLogo - SLASH_TIME;

	// 色
	col.a = 0.3f * (1.0f - ((float)nCntTime / (float)CHANGE_TIME));

	// 位置更新
	pos += (posDest[nCntSelect] - pos) * 0.1f;

	// 情報設定
	m_pObj2D[nCntSelect]->SetColor(col);
	m_pObj2D[nCntSelect]->SetPosition(pos);

}

//==========================================================================
// ロゴの更新処理
//==========================================================================
void CTitleScreen::UpdateLogo(int nCntSelect)
{
	// 位置取得
	D3DXVECTOR3 pos = m_pObj2D[nCntSelect]->GetPosition();


	// 位置設定
	m_pObj2D[nCntSelect]->SetPosition(pos);
}

//==========================================================================
// 選択肢の更新処理
//==========================================================================
void CTitleScreen::UpdateSelect(int nCntSelect)
{
	// 色取得
	D3DXCOLOR col = m_pSelect2D[nCntSelect]->GetColor();

	// 不透明度更新
	CuadricCurveComp(col.a, ALPHATIME, 0.1f, 1.0f, m_nCntAlpha);

	// 色設定
	m_pSelect2D[nCntSelect]->SetColor(col);
}

//==========================================================================
// 描画処理
//==========================================================================
void CTitleScreen::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	for (int nCntSelect = 0; nCntSelect < VTX_LOGO_MAX; nCntSelect++)
	{
		// 描画処理
		m_pObj2D[nCntSelect]->Draw();
	}

	// アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}

