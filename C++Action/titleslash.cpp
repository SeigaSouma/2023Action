//=============================================================================
// 
// タイトル斬撃処理 [titleslash.cpp]
// Author : 相馬靜雅
// 
//=============================================================================
#include "titleslash.h"
#include "renderer.h"
#include "texture.h"
#include "manager.h"
#include "sound.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define TEXTURE	"data\\TEXTURE\\title_slash.png"
#define LIFE	(8)
#define FADEOUT	(40)

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CTitleSlash::CTitleSlash(int nPriority) : CObject2D(nPriority)
{
	// 値のクリア
	m_nLife = 0;								// 寿命
	m_nMaxLife = 0;								// 最大寿命(固定)
	m_nTexIdx = 0;		// テクスチャのインデックス番号
	m_fMaxWidth = 0.0f;		// 最大の横幅
}

//==========================================================================
// デストラクタ
//==========================================================================
CTitleSlash::~CTitleSlash()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CTitleSlash *CTitleSlash::Create(const D3DXVECTOR3 pos)
{
	// 生成用のオブジェクト
	CTitleSlash *pEffect = NULL;

	if (pEffect == NULL)
	{// NULLだったら

		// メモリの確保
		pEffect = DEBUG_NEW CTitleSlash;

		if (pEffect != NULL)
		{// メモリの確保が出来ていたら

			// タイトル斬撃
			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_TITLESLASH);

			// 初期化処理
			pEffect->Init();

			// サイズ取得
			D3DXVECTOR2 size = CManager::GetInstance()->GetTexture()->GetImageSize(pEffect->m_nTexIdx) * 0.5f;
			pEffect->m_fMaxWidth = size.x;
			pEffect->SetSize(D3DXVECTOR2(0.0f, size.y));	// サイズ設定
			pEffect->SetPosition(D3DXVECTOR3(pos.x - size.x, pos.y, pos.z));
		}

		return pEffect;
	}

	return NULL;
}

//==================================================================================
// 初期化処理
//==================================================================================
HRESULT CTitleSlash::Init(void)
{
	HRESULT hr;

	// 初期化処理
	hr = CObject2D::Init();
	if (FAILED(hr))
	{// 失敗したとき
		return E_FAIL;
	}

	// 種類の設定
	SetType(TYPE_EFFECT2D);

	// テクスチャの割り当て
	m_nTexIdx = CManager::GetInstance()->GetTexture()->Regist(TEXTURE);

	// テクスチャの割り当て
	BindTexture(m_nTexIdx);

	// 各種変数の初期化
	m_nLife = LIFE;		// 寿命
	m_nMaxLife = LIFE;	// 最大寿命

	return S_OK;
}

//==================================================================================
// 終了処理
//==================================================================================
void CTitleSlash::Uninit(void)
{
	// 終了処理
	CObject2D::Uninit();
}

//==================================================================================
// 更新処理
//==================================================================================
void CTitleSlash::Update(void)
{

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// 色取得
	D3DXCOLOR col = GetColor();

	// 寿命減算
	m_nLife--;

	if (m_nLife <= 0)
	{// 寿命が0を下回ったら

		// 不透明度計算
		col.a = ((float)FADEOUT - fabsf((float)m_nLife)) / (float)FADEOUT;

		SetColor(col);

		if (m_nLife <= -FADEOUT)
		{
			Uninit();
			return;
		}
	}

	// 頂点座標の設定
	SetVtx();
}

//==================================================================================
// 描画処理
//==================================================================================
void CTitleSlash::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// 描画処理
	CObject2D::Draw();

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CTitleSlash::SetVtx(void)
{
	// テクスチャ座標取得
	D3DXVECTOR2 *pTex = GetTex();

	int nLife = m_nLife;
	if (nLife <= 0)
	{
		nLife = 0;
	}

	// テクスチャの割合を求める
	float fTex = 1.0f - ((float)nLife / (float)LIFE);

	// テクスチャ座標計算
	pTex[0] = D3DXVECTOR2(0.0f, 0.0f);
	pTex[1] = D3DXVECTOR2(fTex, 0.0f);
	pTex[2] = D3DXVECTOR2(0.0f, 1.0f);
	pTex[3] = D3DXVECTOR2(fTex, 1.0f);

	// 頂点設定
	CObject2D::SetVtx();

	// 頂点情報へのポインタ
	VERTEX_2D *pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR2 size = GetSize();

	// サイズ設定
	size.x = (1.0f - ((float)nLife / (float)LIFE)) * m_fMaxWidth;

	SetSize(size);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(pos.x + 0.0f, pos.y + -size.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(pos.x + size.x * 2.0f, pos.y + -size.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(pos.x + 0.0f, pos.y + size.y, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(pos.x + size.x * 2.0f, pos.y + size.y, 0.0f);

	// 頂点バッファをアンロックロック
	GetVtxBuff()->Unlock();

}

