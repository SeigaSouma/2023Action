//=============================================================================
// 
//  斬撃ヒットエフェクト処理 [effect_slashhit.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "effect_slashhit.h"
#include "manager.h"
#include "texture.h"
#include "renderer.h"
#include "sound.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define TEXTURE			"data\\TEXTURE\\anim_SlashHit.png"
#define SIZE			(600.0f)						// 横幅
#define ANIM_SPEED		(1)								// 読み込み間隔
#define MAX_PATTERN_U	(18)							// Uの分割数
#define MAX_PATTERN_V	(1)								// Vの分割数
#define MAX_PATTERN		(MAX_PATTERN_U)					// アニメーションパターンの最大数
#define MOVE_U			(1.0f / (float)MAX_PATTERN_U)	// U座標移動量
#define MOVE_V			(1.0f / (float)MAX_PATTERN_V)	// V座標移動量

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
int CEffectSlashHit::m_nNumAll = 0;		// 弾の総数
int CEffectSlashHit::m_nTexIdx = 0;		// テクスチャのインデックス番号

//==========================================================================
// コンストラクタ
//==========================================================================
CEffectSlashHit::CEffectSlashHit(int nPriority) : CObjectBillboard(nPriority)
{
	// 値のクリア
	m_nCntAnim = 0;			// アニメーションカウンター
	m_nPatternAnim = 0;		// アニメーションパターンNo.

	// 総数加算
	m_nNumAll++;
}

//==========================================================================
// デストラクタ
//==========================================================================
CEffectSlashHit::~CEffectSlashHit()
{
	
}

//==========================================================================
// テクスチャ読み込み
//==========================================================================
void CEffectSlashHit::LoadTexture(void)
{
	// テクスチャの読み込み
	CManager::GetInstance()->GetTexture()->Regist(TEXTURE);
}

//==========================================================================
// 生成処理
//==========================================================================
CEffectSlashHit *CEffectSlashHit::Create(const D3DXVECTOR3 pos)
{
	// 生成用のオブジェクト
	CEffectSlashHit *pExplosion = NULL;

	if (pExplosion == NULL)
	{// NULLだったら

		// メモリの確保
		pExplosion = DEBUG_NEW CEffectSlashHit;

		if (pExplosion != NULL)
		{// メモリの確保が出来ていたら

			// 位置割り当て
			pExplosion->SetPosition(pos);

			// 初期化処理
			pExplosion->Init();
		}

		return pExplosion;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CEffectSlashHit::Init(void)
{
	// 各種変数の初期化
	SetSize(D3DXVECTOR2(0.0f, 0.0f));	// サイズ
	m_nCntAnim = 0;			// アニメーションカウンター
	m_nPatternAnim = 0;		// アニメーションパターンNo.
	SetColor(D3DXCOLOR(0.5f, 0.5f, 1.0f, 1.0f));

	// 種類の設定
	SetType(TYPE_EXPLOSION);

	// ヒットストップ中も動くオブジェクトとする
	SetEnableHitstopMove();

	// テクスチャの割り当て
	if (m_nTexIdx == 0)
	{
		m_nTexIdx = CManager::GetInstance()->GetTexture()->Regist(TEXTURE);
	}

	// テクスチャの割り当て
	BindTexture(m_nTexIdx);

	// 初期化処理
	CObjectBillboard::Init();

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CEffectSlashHit::Uninit(void)
{
	// 終了処理
	CObjectBillboard::Uninit();

	// 総数減算
	m_nNumAll--;
}

//==========================================================================
// 更新処理
//==========================================================================
void CEffectSlashHit::Update(void)
{
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// サイズ
	D3DXVECTOR2 size = GetSize();

	// 拡大
	size.x += (SIZE - size.x) * 0.1f;
	size.y += (SIZE - size.y) * 0.1f;

	// カウントを更新
	m_nCntAnim = (m_nCntAnim + 1) % ANIM_SPEED;

	// パターン更新
	if ((m_nCntAnim % ANIM_SPEED) == 0)
	{
		// パターンNo.を更新
		m_nPatternAnim = (m_nPatternAnim + 1) % MAX_PATTERN;

		if (m_nPatternAnim == 0)
		{// パターンが一周した時

			// オブジェクト破棄
			Uninit();
			return;
		}
	}

	// 位置設定
	SetPosition(pos);

	// 位置設定
	SetSize(size);

	// 頂点情報設定
	SetVtx();
}

//==========================================================================
// 描画処理
//==========================================================================
void CEffectSlashHit::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// ライティングを無効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// αブレンディングを加算合成に設定
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// ビルボードの描画
	CObjectBillboard::Draw();

	// αブレンディングを元に戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// ライティングを有効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CEffectSlashHit::SetVtx(void)
{
	// 頂点設定
	CObjectBillboard::SetVtx();

	// 頂点情報へのポインタ
	VERTEX_3D *pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(m_nPatternAnim * MOVE_U,			0.0f);
	pVtx[1].tex = D3DXVECTOR2((m_nPatternAnim + 1) * MOVE_U,	0.0f);
	pVtx[2].tex = D3DXVECTOR2(m_nPatternAnim * MOVE_U,			1.0f);
	pVtx[3].tex = D3DXVECTOR2((m_nPatternAnim + 1) * MOVE_U,	1.0f);

	// 頂点バッファをアンロックロック
	GetVtxBuff()->Unlock();
}

//==========================================================================
// 総数取得
//==========================================================================
int CEffectSlashHit::GetNumAll(void)
{
	return m_nNumAll;
}
