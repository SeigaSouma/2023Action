//=============================================================================
// 
//  雷のリングエフェクト処理 [effect_thunderring.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "effect_thunderring.h"
#include "manager.h"
#include "texture.h"
#include "renderer.h"
#include "sound.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define TEXTURE			"data\\TEXTURE\\anim_thunderring.png"
#define SIZE			(400.0f)						// 横幅
#define ANIM_SPEED		(1)								// 読み込み間隔
#define MAX_PATTERN_U	(20)							// Uの分割数
#define MAX_PATTERN_V	(1)								// Vの分割数
#define MAX_PATTERN		(MAX_PATTERN_U)					// アニメーションパターンの最大数
#define MOVE_U			(1.0f / (float)MAX_PATTERN_U)	// U座標移動量
#define MOVE_V			(1.0f / (float)MAX_PATTERN_V)	// V座標移動量

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
int CThunderRing::m_nNumAll = 0;		// 弾の総数
int CThunderRing::m_nTexIdx = 0;		// テクスチャのインデックス番号

//==========================================================================
// コンストラクタ
//==========================================================================
CThunderRing::CThunderRing(int nPriority) : CObjectBillboard(nPriority)
{
	// 値のクリア
	m_nCntAnim = 0;			// アニメーションカウンター
	m_nPatternAnim = 0;		// アニメーションパターンNo.
	m_bAutoDeath = false;	// 自動削除のフラグ
	m_bFinish = false;		// アニメーションが終わった判定

	// 総数加算
	m_nNumAll++;
}

//==========================================================================
// デストラクタ
//==========================================================================
CThunderRing::~CThunderRing()
{
	
}

//==========================================================================
// 生成処理
//==========================================================================
CThunderRing *CThunderRing::Create(const D3DXVECTOR3 pos, const D3DXVECTOR2 size)
{
	// 生成用のオブジェクト
	CThunderRing *pExplosion = NULL;

	if (pExplosion == NULL)
	{// NULLだったら

		// メモリの確保
		pExplosion = DEBUG_NEW CThunderRing;

		if (pExplosion != NULL)
		{// メモリの確保が出来ていたら

			// 位置割り当て
			pExplosion->SetPosition(pos);

			// サイズ設定
			pExplosion->SetSize(size);

			// 初期化処理
			pExplosion->Init();

			// 爆発音
			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_EXPLOSION);
		}

		return pExplosion;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CThunderRing::Init(void)
{
	// 各種変数の初期化
	m_nCntAnim = 0;			// アニメーションカウンター
	m_nPatternAnim = 0;		// アニメーションパターンNo.
	m_bAutoDeath = true;	// 自動削除にする
	SetColor(D3DXCOLOR(0.5f, 0.5f, 1.0f, 1.0f));

	// 種類の設定
	SetType(TYPE_EXPLOSION);

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
void CThunderRing::Uninit(void)
{
	// 終了処理
	CObjectBillboard::Uninit();

	// 総数減算
	m_nNumAll--;
}

//==========================================================================
// 更新処理
//==========================================================================
void CThunderRing::Update(void)
{
	// カウントを更新
	m_nCntAnim = (m_nCntAnim + 1) % ANIM_SPEED;

	// パターン更新
	if ((m_nCntAnim % ANIM_SPEED) == 0)
	{
		// パターンNo.を更新
		m_nPatternAnim = (m_nPatternAnim + 1) % MAX_PATTERN;

		if (m_nPatternAnim == 0)
		{// パターンが一周した時

			// 終了状態
			m_bFinish = true;

			if (m_bAutoDeath == true)
			{
				// オブジェクト破棄
				Uninit();
				return;
			}
		}
	}

	// 頂点情報設定
	SetVtx();
}

//==========================================================================
// 描画処理
//==========================================================================
void CThunderRing::Draw(void)
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
void CThunderRing::SetVtx(void)
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
int CThunderRing::GetNumAll(void)
{
	return m_nNumAll;
}
