//=============================================================================
// 
//  弾処理 [explosion.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "explosion.h"
#include "manager.h"
#include "texture.h"
#include "renderer.h"
#include "sound.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define TEXTURE			"data\\TEXTURE\\explosion000.png"
#define WIDTH			(30.0f)							// 横幅
#define HEIGHT			(30.0f)							// 縦幅
#define ANIM_SPEED		(10)							// 読み込み間隔
#define MAX_PATTERN_U	(8)								// Uの分割数
#define MAX_PATTERN_V	(1)								// Vの分割数
#define MAX_PATTERN		(MAX_PATTERN_U)					// アニメーションパターンの最大数
#define MOVE_U			(1.0f / (float)MAX_PATTERN_U)	// U座標移動量
#define MOVE_V			(1.0f / (float)MAX_PATTERN_V)	// V座標移動量

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
int CExplosion::m_nNumAll = 0;		// 弾の総数

//==========================================================================
// コンストラクタ
//==========================================================================
CExplosion::CExplosion(int nPriority) : CObjectBillboard(nPriority)
{
	// 値のクリア
	m_nCntAnim = 0;			// アニメーションカウンター
	m_nPatternAnim = 0;		// アニメーションパターンNo.
	m_nTexIdx = 0;			// テクスチャのインデックス番号

	// 総数加算
	m_nNumAll++;
}

//==========================================================================
// デストラクタ
//==========================================================================
CExplosion::~CExplosion()
{
	
}

//==========================================================================
// 生成処理
//==========================================================================
CExplosion *CExplosion::Create(void)
{
	// 生成用のオブジェクト
	CExplosion *pExplosion = NULL;

	if (pExplosion == NULL)
	{// NULLだったら

		// メモリの確保
		pExplosion = DEBUG_NEW CExplosion;

		//if (pExplosion->GetID() < 0)
		//{// メモリ確保に失敗していたら

		//	delete pExplosion;
		//	return NULL;
		//}

		if (pExplosion != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			pExplosion->Init();

			// 爆発音
			CManager::GetSound()->PlaySound(CSound::LABEL_SE_EXPLOSION);
		}

		return pExplosion;
	}

	return NULL;
}

//==========================================================================
// 生成処理(オーバーロード)
//==========================================================================
CExplosion *CExplosion::Create(const D3DXVECTOR3 pos)
{
	// 生成用のオブジェクト
	CExplosion *pExplosion = NULL;

	if (pExplosion == NULL)
	{// NULLだったら

		// メモリの確保
		pExplosion = DEBUG_NEW CExplosion;

		//if (pExplosion->GetID() < 0)
		//{// メモリ確保に失敗していたら

		//	delete pExplosion;
		//	return NULL;
		//}

		if (pExplosion != NULL)
		{// メモリの確保が出来ていたら

			// 位置割り当て
			pExplosion->SetPosition(pos);

			// 初期化処理
			pExplosion->Init();

			// 爆発音
			CManager::GetSound()->PlaySound(CSound::LABEL_SE_EXPLOSION);
		}

		return pExplosion;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CExplosion::Init(void)
{
	// 各種変数の初期化
	SetSize(D3DXVECTOR2(WIDTH, HEIGHT));	// サイズ
	m_nCntAnim = 0;			// アニメーションカウンター
	m_nPatternAnim = 0;		// アニメーションパターンNo.

	// 種類の設定
	SetType(TYPE_EXPLOSION);

	// テクスチャの割り当て
	m_nTexIdx = CManager::GetTexture()->Regist(TEXTURE);

	// テクスチャの割り当て
	BindTexture(m_nTexIdx);

	// 初期化処理
	CObjectBillboard::Init();

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CExplosion::Uninit(void)
{
	// 終了処理
	CObjectBillboard::Uninit();

	// 総数減算
	m_nNumAll--;
}

//==========================================================================
// 更新処理
//==========================================================================
void CExplosion::Update(void)
{
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

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

	// 移動量設定
	SetMove(move);

	// 向き設定
	SetRotation(rot);

	// 頂点情報設定
	SetVtx();
}

//==========================================================================
// 描画処理
//==========================================================================
void CExplosion::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ライティングを無効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 80);

	// ビルボードの描画
	CObjectBillboard::Draw();

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
void CExplosion::SetVtx(void)
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
int CExplosion::GetNumAll(void)
{
	return m_nNumAll;
}
