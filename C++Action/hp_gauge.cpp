//=============================================================================
// 
//  体力ゲージ処理 [hp_gauge.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "hp_gauge.h"
#include "texture.h"
#include "manager.h"
#include "renderer.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define WIDTH			(25.0f)	// 横幅
#define HEIGHT			(5.0f)	// 縦幅

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
const char *CHP_Gauge::m_apTextureFile[] =					// テクスチャのファイル
{
	NULL,
	NULL,
	"data\\TEXTURE\\hypnosis_fram.png",
};
int CHP_Gauge::m_nNumAll = 0;	// 総数

//==========================================================================
// コンストラクタ
//==========================================================================
CHP_Gauge::CHP_Gauge(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		m_HPGauge[nCntGauge].pObjBillboard = NULL;
		m_HPGauge[nCntGauge].fMaxWidth = 0.0f;		// 幅の最大値
		m_HPGauge[nCntGauge].fMaxHeight = 0.0f;		// 高さの最大値
		m_HPGauge[nCntGauge].fWidthDest = 0.0f;		// 幅の差分
		m_nTexIdx[nCntGauge] = 0;					// テクスチャのインデックス番号
	}

	m_nLife = 0;	// 体力
	m_nMaxLife = 0;	// 最大体力

	// 総数加算
	m_nNumAll++;
}

//==========================================================================
// デストラクタ
//==========================================================================
CHP_Gauge::~CHP_Gauge()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CHP_Gauge *CHP_Gauge::Create(float fPosLen, int nMaxLife, float fSizeBuff)
{
	// 生成用のオブジェクト
	CHP_Gauge *pHPGauge = NULL;

	if (pHPGauge == NULL)
	{// NULLだったら

		// メモリの確保
		pHPGauge = DEBUG_NEW CHP_Gauge;

		//if (pHPGauge->GetID() < 0)
		//{// メモリ確保に失敗していたら

		//	delete pHPGauge;
		//	return NULL;
		//}

		if (pHPGauge != NULL)
		{// メモリの確保が出来ていたら

			// 原点からの長さを渡す
			pHPGauge->m_fPosLength = fPosLen;

			// 最大体力
			pHPGauge->m_nMaxLife = nMaxLife;

			for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
			{
				// 生成処理
				pHPGauge->m_HPGauge[nCntGauge].pObjBillboard = 
					pHPGauge->m_HPGauge[nCntGauge].pObjBillboard->Create();

				if (pHPGauge->m_HPGauge[nCntGauge].pObjBillboard == NULL)
				{// NULLだったら
					pHPGauge->Uninit();
					return NULL;
				}

				pHPGauge->m_HPGauge[nCntGauge].pObjBillboard->SetSize(
					D3DXVECTOR2(WIDTH * fSizeBuff, HEIGHT * fSizeBuff));	// サイズ
			}

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
HRESULT CHP_Gauge::Init(void)
{
	// 最大体力
	m_nLife = m_nMaxLife;

	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{

		// 各種変数の初期化
		m_HPGauge[nCntGauge].fMaxWidth = m_HPGauge[nCntGauge].pObjBillboard->GetSize().x;		// 幅の最大値
		m_HPGauge[nCntGauge].fMaxHeight = m_HPGauge[nCntGauge].pObjBillboard->GetSize().y;		// 高さの最大値

		// 頂点カラーの設定
		switch (nCntGauge)
		{
		case VTXTYPE_BLACK:		//下地

			// 色設定
			m_HPGauge[nCntGauge].pObjBillboard->SetColor(D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f));
			break;

		case VTXTYPE_PINK:		//変動するゲージ

			// 色設定
			m_HPGauge[nCntGauge].pObjBillboard->SetColor(D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f));
			break;

		case VTXTYPE_FRAM:		//フレーム

			// 色設定
			m_HPGauge[nCntGauge].pObjBillboard->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
			break;
		}


		// テクスチャの割り当て
		m_nTexIdx[nCntGauge] = CManager::GetTexture()->Regist(m_apTextureFile[nCntGauge]);

		// テクスチャの割り当て
		m_HPGauge[nCntGauge].pObjBillboard->BindTexture(m_nTexIdx[nCntGauge]);
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CHP_Gauge::Uninit(void)
{
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		if (m_HPGauge[nCntGauge].pObjBillboard != NULL)
		{// NULLじゃなかったら

			// 終了処理
			m_HPGauge[nCntGauge].pObjBillboard->Uninit();
			m_HPGauge[nCntGauge].pObjBillboard = NULL;
			m_HPGauge[nCntGauge] = {};
		}
	}

	// 情報削除
	Release();

	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		if (m_HPGauge[nCntGauge].pObjBillboard != NULL)
		{// NULLじゃなかったら

			// 終了処理
			m_HPGauge[nCntGauge].pObjBillboard = NULL;
			m_HPGauge[nCntGauge] = {};
		}
	}

	m_nNumAll--;
}

//==========================================================================
// 更新処理
//==========================================================================
void CHP_Gauge::Update(void)
{

	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		if (m_HPGauge[nCntGauge].pObjBillboard != NULL)
		{// NULLじゃなかったら

			if (nCntGauge == VTXTYPE_PINK)
			{// ゲージ部分だけ　

				// 減少処理
				GaugeDecrement(nCntGauge);
			}

			// 頂点座標設定
			SetVtx(nCntGauge);
		}
	}

}

//==================================================================================
// 位置更新
//==================================================================================
void CHP_Gauge::UpdatePosition(D3DXVECTOR3 pos, int nLife)
{
	// 高さ分上にあげる
	pos.y += m_fPosLength;

	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		if (m_HPGauge[nCntGauge].pObjBillboard != NULL)
		{// NULLじゃなかったら

			// 位置設定
			m_HPGauge[nCntGauge].pObjBillboard->SetPosition(pos);

			// 現在の体力設定
			m_nLife = nLife;

			//目標の幅設定
			m_HPGauge[nCntGauge].fWidthDest =
				m_HPGauge[nCntGauge].fMaxWidth * ((float)m_nLife / (float)m_nMaxLife);
		}
	}
}

//==================================================================================
// 減少処理
//==================================================================================
void CHP_Gauge::GaugeDecrement(int nCntGauge)
{
	// サイズ取得
	D3DXVECTOR2 size = m_HPGauge[nCntGauge].pObjBillboard->GetSize();

	// 差分で徐々に減らしていく
	size.x +=
		(m_HPGauge[nCntGauge].fWidthDest - size.x) * 0.025f;

	// サイズ設定
	m_HPGauge[nCntGauge].pObjBillboard->SetSize(size);

	// 頂点座標設定
	SetVtx(nCntGauge);

}

//==================================================================================
// 色更新
//==================================================================================
void CHP_Gauge::ChangeColor(int nCntGauge)
{
	// サイズ取得
	D3DXVECTOR2 size = m_HPGauge[nCntGauge].pObjBillboard->GetSize();

	// 色取得
	D3DXCOLOR col = m_HPGauge[nCntGauge].pObjBillboard->GetColor();

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
	m_HPGauge[nCntGauge].pObjBillboard->SetColor(col);

	// サイズ取得
	m_HPGauge[nCntGauge].pObjBillboard->SetSize(size);

}

//==========================================================================
// 描画処理
//==========================================================================
void CHP_Gauge::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		// ライティングを無効にする
		pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

		// 描画処理
		m_HPGauge[nCntGauge].pObjBillboard->Draw();

		// ライティングを有効にする
		pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	}
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CHP_Gauge::SetVtx(void)
{

}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CHP_Gauge::SetVtx(int nCntGauge)
{
	// 頂点設定
	m_HPGauge[nCntGauge].pObjBillboard->SetVtx();

	// 頂点情報へのポインタ
	VERTEX_3D *pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_HPGauge[nCntGauge].pObjBillboard->GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// サイズ取得
	D3DXVECTOR2 size = m_HPGauge[nCntGauge].pObjBillboard->GetSize();

	//頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(-size.x - (m_HPGauge[nCntGauge].fMaxWidth - size.x), size.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(size.x - (m_HPGauge[nCntGauge].fMaxWidth - size.x), size.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(-size.x - (m_HPGauge[nCntGauge].fMaxWidth - size.x), -size.y, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(size.x - (m_HPGauge[nCntGauge].fMaxWidth - size.x), -size.y, 0.0f);

	// 頂点バッファをアンロックロック
	m_HPGauge[nCntGauge].pObjBillboard->GetVtxBuff()->Unlock();

}
