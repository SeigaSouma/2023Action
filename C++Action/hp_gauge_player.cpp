//=============================================================================
// 
//  プレイヤーの体力ゲージ処理 [hp_gauge_player.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "hp_gauge_player.h"
#include "texture.h"
#include "manager.h"
#include "renderer.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define WIDTH			(100.0f)	// 横幅
#define HEIGHT			(25.0f)	// 縦幅

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
const char *CHP_GaugePlayer::m_apTextureFile[] =					// テクスチャのファイル
{
	"data\\TEXTURE\\player_HPgaugeMoto.png",
	"data\\TEXTURE\\player_HPgauge.png",
	"data\\TEXTURE\\player_HPFrampng.png",
};

//==========================================================================
// コンストラクタ
//==========================================================================
CHP_GaugePlayer::CHP_GaugePlayer(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		m_HPGauge[nCntGauge].pObj2D = NULL;
		m_HPGauge[nCntGauge].fMaxWidth = 0.0f;		// 幅の最大値
		m_HPGauge[nCntGauge].fMaxHeight = 0.0f;		// 高さの最大値
		m_HPGauge[nCntGauge].fWidthDest = 0.0f;		// 幅の差分
		m_nTexIdx[nCntGauge] = 0;					// テクスチャのインデックス番号
	}

	m_nLife = 0;	// 体力
	m_nMaxLife = 0;	// 最大体力
}

//==========================================================================
// デストラクタ
//==========================================================================
CHP_GaugePlayer::~CHP_GaugePlayer()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CHP_GaugePlayer *CHP_GaugePlayer::Create(D3DXVECTOR3 pos, int nMaxLife)
{
	// 生成用のオブジェクト
	CHP_GaugePlayer *pHPGauge = NULL;

	if (pHPGauge == NULL)
	{// NULLだったら

		// メモリの確保
		pHPGauge = DEBUG_NEW CHP_GaugePlayer;

		if (pHPGauge != NULL)
		{// メモリの確保が出来ていたら

			// 最大体力
			pHPGauge->m_nMaxLife = nMaxLife;

			// 位置設定
			pHPGauge->SetPosition(pos);

			// 初期化処理
			pHPGauge->Init();

			// 種類の設定
			pHPGauge->SetType(TYPE_HPGAUGE);
		}

		return pHPGauge;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CHP_GaugePlayer::Init(void)
{
	// 最大体力
	m_nLife = m_nMaxLife;

	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		// 生成処理
		m_HPGauge[nCntGauge].pObj2D = CObject2D::Create();
		if (m_HPGauge[nCntGauge].pObj2D == NULL)
		{// NULLだったら
			Uninit();
			return NULL;
		}

		// テクスチャの割り当て
		m_nTexIdx[nCntGauge] = CManager::GetInstance()->GetTexture()->Regist(m_apTextureFile[nCntGauge]);

		// テクスチャの割り当て
		m_HPGauge[nCntGauge].pObj2D->BindTexture(m_nTexIdx[nCntGauge]);

		// 頂点カラーの設定
		switch (nCntGauge)
		{
		case VTXTYPE_BLACK:		//下地

			// 色設定
			m_HPGauge[nCntGauge].pObj2D->SetColor(D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f));
			break;

		case VTXTYPE_PINK:		//変動するゲージ

			// 色設定
			m_HPGauge[nCntGauge].pObj2D->SetColor(D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f));
			break;

		case VTXTYPE_FRAM:		//フレーム

			// 色設定
			m_HPGauge[nCntGauge].pObj2D->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
			break;
		}
	}

	D3DXVECTOR2 texsize = CManager::GetInstance()->GetTexture()->GetImageSize(m_nTexIdx[VTXTYPE_FRAM]);
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		if (m_HPGauge[nCntGauge].pObj2D == NULL)
		{// NULLだったら
			continue;
		}

		switch (nCntGauge)
		{
		case VTXTYPE_BLACK:	//下地

			// サイズ
			m_HPGauge[nCntGauge].pObj2D->SetSize(texsize * 0.5f);
			break;

		case VTXTYPE_PINK:	//変動するゲージ

			// サイズ
			m_HPGauge[nCntGauge].pObj2D->SetSize(texsize * 0.5f);
			break;

		case VTXTYPE_FRAM:	//フレーム

			// サイズ
			m_HPGauge[nCntGauge].pObj2D->SetSize(texsize * 0.5f);
			break;
		}

		// 各種変数の初期化
		m_HPGauge[nCntGauge].fMaxWidth = m_HPGauge[nCntGauge].pObj2D->GetSize().x;		// 幅の最大値
		m_HPGauge[nCntGauge].fMaxHeight = m_HPGauge[nCntGauge].pObj2D->GetSize().y;		// 高さの最大値
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CHP_GaugePlayer::Uninit(void)
{
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		if (m_HPGauge[nCntGauge].pObj2D != NULL)
		{// NULLじゃなかったら

			// 終了処理
			m_HPGauge[nCntGauge].pObj2D->Uninit();
			m_HPGauge[nCntGauge].pObj2D = NULL;
		}
	}

	// 情報削除
	Release();
}

//==========================================================================
// 終了処理
//==========================================================================
void CHP_GaugePlayer::Kill(void)
{
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		if (m_HPGauge[nCntGauge].pObj2D != NULL)
		{// NULLじゃなかったら

			// 終了処理
			m_HPGauge[nCntGauge].pObj2D->Uninit();
			m_HPGauge[nCntGauge].pObj2D = NULL;
		}
	}


	// 情報削除
	Release();
}

//==========================================================================
// 更新処理
//==========================================================================
void CHP_GaugePlayer::Update(void)
{
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		if (m_HPGauge[nCntGauge].pObj2D != NULL)
		{// NULLじゃなかったら

			if (nCntGauge == VTXTYPE_PINK)
			{// ゲージ部分だけ　

				// 減少処理
				GaugeDecrement(nCntGauge);
			}

			// 位置設定
			m_HPGauge[nCntGauge].pObj2D->SetPosition(pos);

			// サイズ設定
			//m_HPGauge[nCntGauge].pObj2D->SetSize(D3DXVECTOR2(m_sizeGauge.x - (m_HPGauge[nCntGauge].fMaxWidth - m_sizeGauge.x), size.y));

			// 頂点座標設定
			SetVtx(nCntGauge);
		}
	}

}

//==================================================================================
// 位置更新
//==================================================================================
void CHP_GaugePlayer::SetLife(int nLife)
{
	// 現在の体力設定
	m_nLife = nLife;

	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		if (m_HPGauge[nCntGauge].pObj2D != NULL)
		{// NULLじゃなかったら

			//目標の幅設定
			m_HPGauge[nCntGauge].fWidthDest = m_HPGauge[nCntGauge].fMaxWidth * ((float)m_nLife / (float)m_nMaxLife);
		}
	}
}

//==================================================================================
// 減少処理
//==================================================================================
void CHP_GaugePlayer::GaugeDecrement(int nCntGauge)
{

	// サイズ取得
	D3DXVECTOR2 size = m_HPGauge[nCntGauge].pObj2D->GetSize();

	// 差分で徐々に減らしていく
	size.x += (m_HPGauge[nCntGauge].fWidthDest - size.x) * 0.15f;

	// サイズ設定
	m_HPGauge[nCntGauge].pObj2D->SetSize(size);

	// 頂点座標設定
	SetVtx(nCntGauge);
}

//==================================================================================
// 色更新
//==================================================================================
void CHP_GaugePlayer::ChangeColor(int nCntGauge)
{
	// サイズ取得
	D3DXVECTOR2 size = m_HPGauge[nCntGauge].pObj2D->GetSize();

	// 色取得
	D3DXCOLOR col = m_HPGauge[nCntGauge].pObj2D->GetColor();

	if (
		size.x / m_HPGauge[nCntGauge].fMaxWidth <= 0.95f &&
		size.x / m_HPGauge[nCntGauge].fMaxWidth > 0.6f)
	{//HPゲージ8割

		col = D3DXCOLOR(0.2f, 0.6f, 0.2f, 1.0f);
	}
	else if (
		size.x / m_HPGauge[nCntGauge].fMaxWidth <= 0.6f &&
		size.x / m_HPGauge[nCntGauge].fMaxWidth > 0.4f)
	{//HPゲージ6割

		col = D3DXCOLOR(0.8f, 0.7f, 0.2f, 1.0f);
	}
	else if (
		size.x / m_HPGauge[nCntGauge].fMaxWidth <= 0.4f &&
		size.x / m_HPGauge[nCntGauge].fMaxWidth > 0.2f)
	{//HPゲージ4割

		col = D3DXCOLOR(0.8f, 0.5f, 0.2f, 1.0f);
	}
	else if (size.x / m_HPGauge[nCntGauge].fMaxWidth <= 0.2f)
	{//危険エリア

		col = D3DXCOLOR(0.8f, 0.2f, 0.2f, 1.0f);
	}
	else
	{//満タン

		col = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
	}

	// 色設定
	m_HPGauge[nCntGauge].pObj2D->SetColor(col);

	// サイズ取得
	m_HPGauge[nCntGauge].pObj2D->SetSize(size);

}

//==========================================================================
// 描画処理
//==========================================================================
void CHP_GaugePlayer::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		// 描画処理
		m_HPGauge[nCntGauge].pObj2D->Draw();
	}
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CHP_GaugePlayer::SetVtx(void)
{

}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CHP_GaugePlayer::SetVtx(int nCntGauge)
{
	// 頂点設定
	m_HPGauge[nCntGauge].pObj2D->SetVtx();

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	D3DXVECTOR2 *pTex = m_HPGauge[nCntGauge].pObj2D->GetTex();

	// 頂点情報へのポインタ
	VERTEX_2D *pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_HPGauge[nCntGauge].pObj2D->GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// サイズ取得
	D3DXVECTOR2 size = m_HPGauge[nCntGauge].pObj2D->GetSize();

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(pos.x + -size.x - (m_HPGauge[nCntGauge].fMaxWidth - size.x),	pos.y + -size.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(pos.x + size.x - (m_HPGauge[nCntGauge].fMaxWidth - size.x),	pos.y + -size.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(pos.x + -size.x - (m_HPGauge[nCntGauge].fMaxWidth - size.x),	pos.y + size.y, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(pos.x + size.x - (m_HPGauge[nCntGauge].fMaxWidth - size.x),	pos.y + size.y, 0.0f);

	pTex[0] = D3DXVECTOR2(0.0f, 0.0f);
	pTex[1] = D3DXVECTOR2(size.x / m_HPGauge[nCntGauge].fMaxWidth, 0.0f);
	pTex[2] = D3DXVECTOR2(0.0f, 1.0f);
	pTex[3] = D3DXVECTOR2(size.x / m_HPGauge[nCntGauge].fMaxWidth, 1.0f);

	// 頂点バッファをアンロックロック
	m_HPGauge[nCntGauge].pObj2D->GetVtxBuff()->Unlock();

}
