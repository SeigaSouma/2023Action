//=============================================================================
// 
// エフェクト処理 [2D_effect.cpp]
// Author : 相馬靜雅
// 
//=============================================================================
#include "renderer.h"
#include "texture.h"
#include "manager.h"
#include "2D_effect.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define DEF_RADIUS			(20.0f)
#define EFFECT_2D_LIFE		(30)
#define EFFECT_2DSIZE1		(0.97f)
#define EFFECT_2DSIZE2		(0.98f)
#define EFFECT_2DSIZE3		(0.99f)

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
const char *CEffect2D::m_apTextureFile[] =					// ファイル読み込み
{
	"data\\TEXTURE\\effect\\effect000.jpg",	   // 通常エフェクト
	"data\\TEXTURE\\effect\\smoke_05.tga",	   // 煙エフェクト
	"data\\TEXTURE\\effect\\smoke_05.tga",	   // 黒煙
	"data\\TEXTURE\\effect\\effect000.png",	   // 黒エフェクト
	"data\\TEXTURE\\effect\\effect001.png",	   // 十字エフェクト
};
int CEffect2D::m_nNumAll = 0;	// 総数

//==========================================================================
// コンストラクタ
//==========================================================================
CEffect2D::CEffect2D(int nPriority) : CObject2D(nPriority)
{
	// 値のクリア
	m_colOrigin = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// 色の元
	m_fRadius = 0.0f;							// 半径
	m_fMaxRadius = 0.0f;						// 最大半径
	m_nLife = 0;								// 寿命
	m_nMaxLife = 0;								// 最大寿命(固定)
	m_moveType = MOVEEFFECT_NONE;				// 移動の種類
	m_nType = TYPE_NORMAL;						// 種類
	m_bAddAlpha = true;							// 加算合成の判定
	
	// テクスチャデータの配列分繰り返す
	for (int nCntTex = 0; nCntTex < sizeof(m_apTextureFile) / sizeof(*m_apTextureFile); nCntTex++)
	{
		m_nTexIdx[nCntTex] = 0;		// テクスチャのインデックス番号
	}

	// 総数加算
	m_nNumAll++;
}

//==========================================================================
// デストラクタ
//==========================================================================
CEffect2D::~CEffect2D()
{
	// 総数減算
	m_nNumAll--;
}

//==========================================================================
// 生成処理
//==========================================================================
CEffect2D *CEffect2D::Create(void)
{
	// 生成用のオブジェクト
	CEffect2D *pEffect = NULL;

	if (pEffect == NULL)
	{// NULLだったら

		// メモリの確保
		pEffect = DEBUG_NEW CEffect2D;

		//if (pEffect->GetID() < 0)
		//{// メモリ確保に失敗していたら

		//	delete pEffect;
		//	return NULL;
		//}

		if (pEffect != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			pEffect->Init();
		}

		return pEffect;
	}

	return NULL;
}

//==========================================================================
// 生成処理
//==========================================================================
CEffect2D *CEffect2D::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 move, const D3DXCOLOR col, const float fRadius, const int nLife, const int moveType, const TYPE type)
{
	// 生成用のオブジェクト
	CEffect2D *pEffect = NULL;

	if (pEffect == NULL)
	{// NULLだったら

		// メモリの確保
		pEffect = DEBUG_NEW CEffect2D;

		//if (pEffect->GetID() < 0)
		//{// メモリ確保に失敗していたら

		//	delete pEffect;
		//	return NULL;
		//}

		if (pEffect != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			pEffect->Init(pos, move, col, fRadius, nLife, moveType, type);
		}

		return pEffect;
	}

	return NULL;
}

//==================================================================================
// エフェクトの初期化処理
//==================================================================================
HRESULT CEffect2D::Init(void)
{
	HRESULT hr;

	// 各種変数の初期化
	m_colOrigin = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// 色の元
	m_fRadius = DEF_RADIUS;						// 半径
	m_fMaxRadius = m_fRadius;					// 最大半径
	m_nLife = EFFECT_2D_LIFE;					// 寿命
	m_nMaxLife = m_nLife;						// 最大寿命(固定)
	m_moveType = MOVEEFFECT_NONE;				// 移動の種類
	m_nType = TYPE_NORMAL;						// 種類
	m_bAddAlpha = true;							// 加算合成の判定

	// 種類の設定
	SetType(TYPE_EFFECT2D);

	// テクスチャデータの配列分繰り返す
	for (int nCntTex = 0; nCntTex < sizeof(m_apTextureFile) / sizeof(*m_apTextureFile); nCntTex++)
	{
		// テクスチャの割り当て
		m_nTexIdx[nCntTex] = CManager::GetTexture()->Regist(m_apTextureFile[nCntTex]);

		// テクスチャの割り当て
		BindTexture(m_nTexIdx[nCntTex]);
	}

	// 初期化処理
	hr = CObject2D::Init();

	if (FAILED(hr))
	{// 失敗したとき

		return E_FAIL;
	}

	return S_OK;
}

//==================================================================================
// エフェクトの初期化処理
//==================================================================================
HRESULT CEffect2D::Init(const D3DXVECTOR3 pos, const D3DXVECTOR3 move, const D3DXCOLOR col, const float fRadius, const int nLife, const int moveType, const TYPE type)
{
	HRESULT hr;

	// 各種変数の初期化
	SetPosition(pos);
	SetMove(move);
	m_colOrigin = col;							// 色の元
	SetColor(col);								// 色
	m_fRadius = fRadius;						// 半径
	m_fMaxRadius = m_fRadius;					// 最大半径
	SetSize(D3DXVECTOR2(m_fRadius, m_fRadius));	// サイズ設定
	m_nLife = nLife;							// 寿命
	m_nMaxLife = m_nLife;						// 最大寿命(固定)
	m_moveType = moveType;						// 移動の種類
	m_nType = type;								// 種類

	// 加算合成の判定
	switch (m_nType)
	{
	case TYPE_NORMAL:
		m_bAddAlpha = true;
		break;

	case TYPE_SMOKE:
		m_bAddAlpha = true;
		break;

	case TYPE_SMOKEBLACK:
		m_bAddAlpha = false;
		break;

	case TYPE_BLACK:
		m_bAddAlpha = false;
		break;

	case TYPE_JUJI:
		m_bAddAlpha = true;
		break;

	default:
		m_bAddAlpha = true;
		break;
	}

	// 種類の設定
	SetType(TYPE_EFFECT2D);

	// テクスチャデータの配列分繰り返す
	
		// テクスチャの割り当て
		m_nTexIdx[m_nType] = CManager::GetTexture()->Regist(m_apTextureFile[m_nType]);

		// テクスチャの割り当て
		BindTexture(m_nTexIdx[m_nType]);
	

	// 初期化処理
	hr = CObject2D::Init();

	if (FAILED(hr))
	{// 失敗したとき

		return E_FAIL;
	}

	return S_OK;
}


//==================================================================================
// エフェクトの終了処理
//==================================================================================
void CEffect2D::Uninit(void)
{
	// 終了処理
	CObject2D::Uninit();
}

//==================================================================================
// エフェクトの更新処理
//==================================================================================
void CEffect2D::Update(void)
{

	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// 色取得
	D3DXCOLOR col = GetColor();

	// 位置更新
	pos += move;

	// 位置設定
	SetPosition(pos);

	// 移動量設定
	SetMove(move);

	// 小さくしていく処理の場合
	if (m_moveType == MOVEEFFECT_SUB)
	{// エフェクトを小さくしていく

		// 縮小処理
		SubSize();
	}
	else if (m_moveType == MOVEEFFECT_SUPERSUB)
	{// エフェクトを小さくしていく

		// 超縮小処理
		SuperSubSize();
	}
	else if (m_moveType == MOVEEFFECT_ADD)
	{// エフェクトを大きくしていく

		// 拡大処理
		AddSize();
	}

	// 寿命の更新
	m_nLife--;

	// 不透明度の更新
	col.a = m_colOrigin.a * ((float)m_nLife / (float)m_nMaxLife);

	// サイズ設定
	SetSize(D3DXVECTOR2(m_fRadius, m_fRadius));

	// 色設定
	SetColor(col);

	if (m_nLife <= 0)
	{// 寿命が尽きたら

		// エフェクトの削除
		Uninit();
		return;
	}

	// 頂点座標の設定
	SetVtx();

}

//==================================================================================
// エフェクトの縮小処理
//==================================================================================
void CEffect2D::SubSize(void)
{
	int nEffect_2DType = rand() % 3;

	if (nEffect_2DType == 0)
	{
		m_fRadius *= EFFECT_2DSIZE1;
	}
	else if (nEffect_2DType == 1)
	{
		m_fRadius *= EFFECT_2DSIZE2;
	}
	else if (nEffect_2DType == 2)
	{
		m_fRadius *= EFFECT_2DSIZE3;
	}
}

//==================================================================================
// エフェクトの縮小処理
//==================================================================================
void CEffect2D::SuperSubSize(void)
{
	m_fRadius = m_fMaxRadius * (float)m_nLife / (float)m_nMaxLife;
}

//==================================================================================
// エフェクトの拡大処理
//==================================================================================
void CEffect2D::AddSize(void)
{
	int nEffect_2DType = rand() % 3;

	if (nEffect_2DType == 0)
	{
		m_fRadius += 1.8f;
	}
	else if (nEffect_2DType == 1)
	{
		m_fRadius += 0.8f;
	}
	else if (nEffect_2DType == 2)
	{
		m_fRadius += 0.0f;
	}
}

//==================================================================================
// エフェクトの描画処理
//==================================================================================
void CEffect2D::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// αブレンディングを加算合成に設定
	if (m_bAddAlpha == true)
	{
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	}

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// 描画処理
	CObject2D::Draw();

	// αブレンディングを元に戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CEffect2D::SetVtx(void)
{
	// 頂点設定
	CObject2D::SetVtx();
}

//==========================================================================
// 総数取得
//==========================================================================
int CEffect2D::GetNumAll(void)
{
	return m_nNumAll;
}
