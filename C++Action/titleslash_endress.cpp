//=============================================================================
// 
//  タイトルの斬撃処理 [titleslash_endress.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "titleslash_endress.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "object2D.h"
#include "calculation.h"
#include "titleslash.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define LOGO_POSITION		(D3DXVECTOR3(640.0f, 200.0f, 0.0f))		// 初期位置
#define LOGOSIZE_MULTIPLY	(0.55f)									// ロゴサイズの倍率
#define MAX_LIFE			(40)									// 寿命
#define DEST_LOGOALPHA		(D3DXVECTOR3(250.0f, -120.0f, 0.0f))	// 目標の位置

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
const char *CTitleSlashEndress::m_apTextureFile[VTX_LOGO_MAX] =			// テクスチャのファイル
{
	"data\\TEXTURE\\titlelogo_02.png",
	"data\\TEXTURE\\titlelogo_02.png",
	"data\\TEXTURE\\titlelogo_01.png",
};

//==========================================================================
// コンストラクタ
//==========================================================================
CTitleSlashEndress::CTitleSlashEndress(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	memset(&m_pObj2D[0], NULL, sizeof(m_pObj2D));	// オブジェクト2Dのオブジェクト
	memset(&m_nTexIdx[0], 0, sizeof(m_nTexIdx));	// テクスチャのインデックス番号
	m_nLife = 0;									// 寿命
	m_DestSize = D3DXVECTOR2(0.0f, 0.0f);			// 目標のサイズ
}

//==========================================================================
// デストラクタ
//==========================================================================
CTitleSlashEndress::~CTitleSlashEndress()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CTitleSlashEndress *CTitleSlashEndress::Create(void)
{
	// 生成用のオブジェクト
	CTitleSlashEndress *pTitleScreen = NULL;

	if (pTitleScreen == NULL)
	{// NULLだったら

		// メモリの確保
		pTitleScreen = DEBUG_NEW CTitleSlashEndress;

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
HRESULT CTitleSlashEndress::Init(void)
{
	// 種類の設定
	SetType(TYPE_TITLESCREEN);

	// 寿命
	m_nLife = MAX_LIFE;

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
		case VTX_LOGOALPHA1:
			// サイズ取得
			size = CManager::GetInstance()->GetTexture()->GetImageSize(m_nTexIdx[nCntSelect]);
			m_pObj2D[nCntSelect]->SetSize(size * LOGOSIZE_MULTIPLY);	// サイズ
			m_pObj2D[nCntSelect]->SetPosition(LOGO_POSITION);	// 位置
			m_pObj2D[nCntSelect]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.3f));	// 色
			break;

		case VTX_LOGOALPHA2:
			// サイズ取得
			size = CManager::GetInstance()->GetTexture()->GetImageSize(m_nTexIdx[nCntSelect]);
			m_pObj2D[nCntSelect]->SetSize(size * LOGOSIZE_MULTIPLY);	// サイズ
			m_pObj2D[nCntSelect]->SetPosition(LOGO_POSITION);	// 位置
			m_pObj2D[nCntSelect]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.3f));	// 色
			break;

		case VTX_LOGOPERFECTALPHA:
			// サイズ取得
			size = CManager::GetInstance()->GetTexture()->GetImageSize(m_nTexIdx[nCntSelect]);
			m_pObj2D[nCntSelect]->SetSize(size * LOGOSIZE_MULTIPLY);	// サイズ
			m_DestSize = size * 1.2f;									// 目標のサイズ
			m_pObj2D[nCntSelect]->SetPosition(LOGO_POSITION);	// 位置
			m_pObj2D[nCntSelect]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.7f));	// 色
			break;
		}
	}

	// テクスチャ情報取得
	CTexture *pTexture = CManager::GetInstance()->GetTexture();
	int nTex = pTexture->Regist("data\\TEXTURE\\title_slash.png");

	// 斬撃生成
	CTitleSlash::Create(LOGO_POSITION);

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CTitleSlashEndress::Uninit(void)
{
	for (int nCntSelect = 0; nCntSelect < VTX_LOGO_MAX; nCntSelect++)
	{
		if (m_pObj2D[nCntSelect] != NULL)
		{// NULLじゃなかったら

			// 終了処理
			m_pObj2D[nCntSelect] = NULL;
		}
	}

	// 情報削除
	Release();
}

//==========================================================================
// 更新処理
//==========================================================================
void CTitleSlashEndress::Update(void)
{

	// 斬撃更新
	UpdateSlash();

	for (int nCntSelect = 0; nCntSelect < VTX_LOGO_MAX; nCntSelect++)
	{
		if (m_pObj2D[nCntSelect] == NULL)
		{// NULLだったら
			continue;
		}

		// 頂点情報設定
		m_pObj2D[nCntSelect]->SetVtx(nCntSelect);
	}

	if (m_nLife < 0)
	{
		for (int nCntSelect = 0; nCntSelect < VTX_LOGO_MAX; nCntSelect++)
		{
			if (m_pObj2D[nCntSelect] != NULL)
			{// NULLじゃなかったら

				// 終了処理
				m_pObj2D[nCntSelect]->Uninit();
				m_pObj2D[nCntSelect] = NULL;
			}
		}

		// 終了処理
		Uninit();
	}
}

//==========================================================================
// 斬撃更新
//==========================================================================
void CTitleSlashEndress::UpdateSlash()
{
	// 寿命減算
	m_nLife--;

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
		else if (nCntSelect == VTX_LOGOPERFECTALPHA)
		{
			// サイズ加算
			size += (m_DestSize - size) * 0.1f;

			// 色
			col.a = 0.15f * ((float)m_nLife / (float)MAX_LIFE);
		}

		// 色設定
		m_pObj2D[nCntSelect]->SetColor(col);
		m_pObj2D[nCntSelect]->SetSize(size);
	}
}

//==========================================================================
// 透明の斬撃更新
//==========================================================================
void CTitleSlashEndress::UpdateSlashAlpha(int nCntSelect)
{
	// 情報取得
	D3DXCOLOR col = m_pObj2D[nCntSelect]->GetColor();
	D3DXVECTOR3 pos = m_pObj2D[nCntSelect]->GetPosition();

	// 目標の位置
	D3DXVECTOR3 posDiff = DEST_LOGOALPHA;
	if (nCntSelect != 0)
	{
		posDiff.x *= -1;
		posDiff.y *= -1;
	}
	D3DXVECTOR3 posDest = LOGO_POSITION + posDiff;

	// 色
	col.a = 0.3f * ((float)m_nLife / (float)MAX_LIFE);

	// 位置更新
	pos += (posDest - pos) * 0.1f;

	// 情報設定
	m_pObj2D[nCntSelect]->SetColor(col);
	m_pObj2D[nCntSelect]->SetPosition(pos);

}

//==========================================================================
// ロゴの更新処理
//==========================================================================
void CTitleSlashEndress::UpdateLogo(int nCntSelect)
{
	// 位置取得
	D3DXVECTOR3 pos = m_pObj2D[nCntSelect]->GetPosition();


	// 位置設定
	m_pObj2D[nCntSelect]->SetPosition(pos);
}

//==========================================================================
// 描画処理
//==========================================================================
void CTitleSlashEndress::Draw(void)
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

