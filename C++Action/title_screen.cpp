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

//==========================================================================
// マクロ定義
//==========================================================================
#define WIDTH		(640.0f)	// 横幅
#define HEIGHT		(360.0f)	// 縦幅
#define ALPHATIME	(60)		// 不透明度更新の時間
#define LOGOMOVE_TIME	(180)	// ロゴ移動の時間
#define LOGO_POSITION	(D3DXVECTOR3(640.0f, 160.0f, 0.0f))

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
const char *CTitleScreen::m_apTextureFile[VTX_LOGO_MAX] =				// テクスチャのファイル
{
	"data\\TEXTURE\\titlelogo_01.png",
	"data\\TEXTURE\\titlelogo_02.png",
};

//==========================================================================
// コンストラクタ
//==========================================================================
CTitleScreen::CTitleScreen(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	for (int nCntTex = 0; nCntTex < VTX_LOGO_MAX; nCntTex++)
	{
		m_nTexIdx[nCntTex] = 0;		// テクスチャのインデックス番号
		m_pObj2D[nCntTex] = NULL;
	}
	m_nCntAlpha = 0;		// 不透明度のカウンター
	m_nCntLogoMove = 0;		// ロゴ移動のカウント
	m_nCntLogoRotation = 0;	// ロゴ回転のカウント
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
		m_pObj2D[nCntSelect] = CObject2D::Create(6);

		// 種類の設定
		m_pObj2D[nCntSelect]->SetType(TYPE_TITLESELECT);

		// テクスチャの割り当て
		m_nTexIdx[nCntSelect] = CManager::GetTexture()->Regist(m_apTextureFile[nCntSelect]);

		// テクスチャの割り当て
		m_pObj2D[nCntSelect]->BindTexture(m_nTexIdx[nCntSelect]);

		D3DXVECTOR2 size;
		// 各種変数の初期化
		switch (nCntSelect)
		{
		case VTX_LOGO:

			// サイズ取得
			size = CManager::GetTexture()->GetImageSize(m_nTexIdx[nCntSelect]);
			m_pObj2D[nCntSelect]->SetSize(size * 0.5f);	// サイズ
			m_pObj2D[nCntSelect]->SetPosition(LOGO_POSITION);	// 位置
			break;

		case VTX_SELECTSTART:

			// サイズ取得
			size = CManager::GetTexture()->GetImageSize(m_nTexIdx[nCntSelect]);
			m_pObj2D[nCntSelect]->SetSize(D3DXVECTOR2(size.x * 0.4f, size.y * 0.4f));	// サイズ
			m_pObj2D[nCntSelect]->SetPosition(D3DXVECTOR3(640.0f, 550.0f, 0.0f));	// 位置
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
			//m_pObj2D[nCntSelect]->Uninit();
			m_pObj2D[nCntSelect] = NULL;
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
	// 不透明度のカウンター加算
	//m_nCntAlpha = (m_nCntAlpha + 1) % (ALPHATIME * 2);

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

		case VTX_SELECTSTART:
			// 選択肢の更新処理
			UpdateSelect(nCntSelect);
			break;
		}

		// 頂点情報設定
		SetVtx(nCntSelect);
	}
}

//==========================================================================
// ロゴの更新処理
//==========================================================================
void CTitleScreen::UpdateLogo(int nCntSelect)
{
	// 位置取得
	D3DXVECTOR3 pos = m_pObj2D[nCntSelect]->GetPosition();
	D3DXVECTOR3 rot = m_pObj2D[nCntSelect]->GetRotation();

	// ロゴ移動のカウント加算
	m_nCntLogoMove = (m_nCntLogoMove + 1) % (LOGOMOVE_TIME * 2);
	m_nCntLogoRotation = (m_nCntLogoRotation + 1) % (360 * 2);	// ロゴ回転のカウント

	// 高さを変える
	float fHeight = sinf(D3DX_PI * ((float)m_nCntLogoMove / (float)LOGOMOVE_TIME)) * 10.0f;
	pos.y = LOGO_POSITION.y + fHeight;

	// 向きを変える
	float fRot = sinf(D3DX_PI * ((float)m_nCntLogoRotation / (float)360)) * 0.075f;
	rot.z = fRot;

	// 位置設定
	m_pObj2D[nCntSelect]->SetPosition(pos);
	m_pObj2D[nCntSelect]->SetRotation(rot);
}

//==========================================================================
// 選択肢の更新処理
//==========================================================================
void CTitleScreen::UpdateSelect(int nCntSelect)
{
	// 色取得
	D3DXCOLOR col = m_pObj2D[nCntSelect]->GetColor();

	// 不透明度更新
	//col.a = sinf(D3DX_PI * ((float)m_nCntAlpha / (float)ALPHATIME));
	CuadricCurveComp(col.a, ALPHATIME, 0.1f, 1.0f, m_nCntAlpha);

	// 色設定
	m_pObj2D[nCntSelect]->SetColor(col);
}

//==========================================================================
// 描画処理
//==========================================================================
void CTitleScreen::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

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

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CTitleScreen::SetVtx(void)
{

}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CTitleScreen::SetVtx(int nCntSelect)
{
	// 頂点設定
	m_pObj2D[nCntSelect]->SetVtx();
}
