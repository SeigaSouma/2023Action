//=============================================================================
// 
//  オブジェクト3D処理 [object3D.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "object3D.h"
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "texture.h"
#include "debugproc.h"
#include "calculation.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define POLYGON_TOP		(4)								// ポリゴンの頂点数
#define WIDTH			(80.0f)							// 横幅
#define HEIGHT			(60.0f)							// 縦幅
#define ANIM_SPEED		(10)							// 読み込み間隔
#define MAX_PATTERN_U	(5)								// Uの分割数
#define MAX_PATTERN_V	(2)								// Vの分割数
#define MAX_PATTERN		(MAX_PATTERN_U * MAX_PATTERN_V)
#define MOVE_U			(1.0f / (float)MAX_PATTERN_U)	// U座標移動量
#define MOVE_V			(1.0f / (float)MAX_PATTERN_V)	// V座標移動量

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CObject3D::CObject3D(int nPriority) : CObject(nPriority)
{
	D3DXMatrixIdentity(&m_mtxWorld);				// ワールドマトリックス
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);			// 位置
	m_posOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 元の位置
	m_posOld = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 前回の位置
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);			// 移動量
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);			// 向き
	m_rotOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 元の向き
	m_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);		// 色
	m_fSize = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// サイズ
	m_pVtxBuff = NULL;								// 頂点バッファ
	m_nTexIdx = 0;									// テクスチャのインデックス番号
}

//==========================================================================
// デストラクタ
//==========================================================================
CObject3D::~CObject3D()
{

}

//==========================================================================
// テクスチャの割り当て
//==========================================================================
void CObject3D::BindTexture(int nIdx)
{
	// 割り当てる
	m_nTexIdx = nIdx;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CObject3D::Init(void)
{
	HRESULT hr;

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// 頂点バッファの生成
	if (m_pVtxBuff != NULL)
	{// 既に情報が入ってる場合

		return E_FAIL;
	}

	hr = pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * POLYGON_TOP,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	if (FAILED(hr))
	{// 失敗したとき

		return E_FAIL;
	}

	// 頂点情報設定
	SetVtx();

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CObject3D::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}

	// オブジェクトの破棄
	Release();
}

//==========================================================================
// 更新処理
//==========================================================================
void CObject3D::Update(void)
{
	// 頂点情報設定
	SetVtx();
}

//==========================================================================
// 描画処理
//==========================================================================
void CObject3D::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//計算用マトリックス宣言
	D3DXMATRIX mtxRot, mtxTrans, mtxRotOrigin;
	D3DXMatrixIdentity(&mtxRotOrigin);

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// 元の向きを反映する
	D3DXMatrixRotationYawPitchRoll(&mtxRotOrigin, m_rotOrigin.y, m_rotOrigin.x, m_rotOrigin.z);

	// 向きを反映する
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRotOrigin);

	// 位置を反映する
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, CManager::GetTexture()->GetAdress(m_nTexIdx));

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

//==========================================================================
// 生成処理
//==========================================================================
CObject3D *CObject3D::Create(void)
{
	// 生成用のオブジェクト
	CObject3D *pObject3D = NULL;

	if (pObject3D == NULL)
	{// NULLだったら

		// メモリの確保
		pObject3D = DEBUG_NEW CObject3D;

		//if (pObject3D->GetID() < 0)
		//{// メモリ確保に失敗していたら

		//	delete pObject3D;
		//	return NULL;
		//}

		if (pObject3D != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			pObject3D->Init();
		}

		return pObject3D;
	}

	return NULL;
}

//==========================================================================
// 生成処理
//==========================================================================
CObject3D *CObject3D::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	// 生成用のオブジェクト
	CObject3D *pObject3D = NULL;

	if (pObject3D == NULL)
	{// NULLだったら

		// メモリの確保
		pObject3D = DEBUG_NEW CObject3D;

		//if (pObject3D->GetID() < 0)
		//{// メモリ確保に失敗していたら

		//	delete pObject3D;
		//	return NULL;
		//}

		if (pObject3D != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			pObject3D->Init();

			// 位置・向き
			pObject3D->SetPosition(pos);
			pObject3D->SetOriginPosition(pos);
			pObject3D->SetRotation(rot);
		}

		return pObject3D;
	}

	return NULL;
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CObject3D::SetVtx(void)
{

	// 頂点情報へのポインタ
	VERTEX_3D *pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	D3DXVECTOR3 size = GetSize();
	D3DXCOLOR col = GetColor();

		// 位置を更新
		pVtx[0].pos = D3DXVECTOR3(-size.x, +size.y, +size.z);
		pVtx[1].pos = D3DXVECTOR3(+size.x, +size.y, +size.z);
		pVtx[2].pos = D3DXVECTOR3(-size.x, -size.y, -size.z);
		pVtx[3].pos = D3DXVECTOR3(+size.x, -size.y, -size.z);

	// 境界線のベクトル
	D3DXVECTOR3 vecLine0 = pVtx[1].pos - pVtx[0].pos;
	D3DXVECTOR3 vecLine1 = pVtx[2].pos - pVtx[0].pos;

	// 外積求める
	D3DXVECTOR3 Nor[4];

	// 外積
	D3DXVec3Cross(&Nor[0], &vecLine0, &vecLine1);

	// 正規化
	D3DXVec3Normalize(&Nor[0], &Nor[0]);

	Nor[0] = D3DXVECTOR3(
		((vecLine0.y * vecLine1.z) - (vecLine0.z * vecLine1.y)),
		((vecLine0.z * vecLine1.x) - (vecLine0.x * vecLine1.z)),
		((vecLine0.x * vecLine1.y) - (vecLine0.y * vecLine1.x)));
	
	// 正規化
	D3DXVec3Normalize(&Nor[0], &Nor[0]);


	// 境界線のベクトル
	vecLine0 = pVtx[2].pos - pVtx[3].pos;
	vecLine1 = pVtx[1].pos - pVtx[3].pos;

	// 外積求める
	Nor[3] = D3DXVECTOR3(
		((vecLine0.y * vecLine1.z) - (vecLine0.z * vecLine1.y)),
		((vecLine0.z * vecLine1.x) - (vecLine0.x * vecLine1.z)),
		((vecLine0.x * vecLine1.y) - (vecLine0.y * vecLine1.x)));

	// 外積
	D3DXVec3Cross(&Nor[3], &vecLine0, &vecLine1);

	// 正規化
	D3DXVec3Normalize(&Nor[3], &Nor[3]);

	Nor[1] = (Nor[0] + Nor[3]) / 2.0f;
	Nor[2] = (Nor[0] + Nor[3]) / 2.0f;

	// 法線ベクトルの設定
	pVtx[0].nor = Nor[0];
	pVtx[1].nor = Nor[1];
	pVtx[2].nor = Nor[2];
	pVtx[3].nor = Nor[3];

	// 頂点カラーの設定
	pVtx[0].col = col;
	pVtx[1].col = col;
	pVtx[2].col = col;
	pVtx[3].col = col;

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点バッファをアンロックロック
	m_pVtxBuff->Unlock();
}

//==========================================================================
// マトリックス設定
//==========================================================================
void CObject3D::SetWorldMtx(const D3DXMATRIX mtx)
{
	m_mtxWorld = mtx;
}

//==========================================================================
// マトリックス取得
//==========================================================================
D3DXMATRIX CObject3D::GetWorldMtx(void) const
{
	return m_mtxWorld;
}

//==========================================================================
// 位置設定
//==========================================================================
void CObject3D::SetPosition(const D3DXVECTOR3 pos)
{
	m_pos = pos;
}

//==========================================================================
// 位置取得
//==========================================================================
D3DXVECTOR3 CObject3D::GetPosition(void) const
{
	return m_pos;
}

//==========================================================================
//	元の位置設定
//==========================================================================
void CObject3D::SetOriginPosition(const D3DXVECTOR3 pos)
{
	m_posOrigin = pos;
}

//==========================================================================
//	元の位置取得
//==========================================================================
D3DXVECTOR3 CObject3D::GetOriginPosition(void) const
{
	return m_posOrigin;
}

//==========================================================================
// 位置設定
//==========================================================================
void CObject3D::SetOldPosition(const D3DXVECTOR3 posOld)
{
	m_posOld = posOld;
}

//==========================================================================
// 位置取得
//==========================================================================
D3DXVECTOR3 CObject3D::GetOldPosition(void) const
{
	return m_posOld;
}

//==========================================================================
// 移動量設定
//==========================================================================
void CObject3D::SetMove(const D3DXVECTOR3 move)
{
	m_move = move;
}

//==========================================================================
// 移動量取得
//==========================================================================
D3DXVECTOR3 CObject3D::GetMove(void) const
{
	return m_move;
}

//==========================================================================
// 向き設定
//==========================================================================
void CObject3D::SetRotation(const D3DXVECTOR3 rot)
{
	m_rot = rot;
}

//==========================================================================
// 向き取得
//==========================================================================
D3DXVECTOR3 CObject3D::GetRotation(void) const
{
	return m_rot;
}

//==========================================================================
// 元の向き設定
//==========================================================================
void CObject3D::SetOriginRotation(const D3DXVECTOR3 rot)
{
	m_rotOrigin = rot;
}

//==========================================================================
// 元の向き取得
//==========================================================================
D3DXVECTOR3 CObject3D::GetOriginRotation(void) const
{
	return m_rotOrigin;
}

//==========================================================================
// 色設定
//==========================================================================
void CObject3D::SetColor(const D3DXCOLOR col)
{
	m_col = col;
}

//==========================================================================
// 色取得
//==========================================================================
D3DXCOLOR CObject3D::GetColor(void) const
{
	return m_col;
}

//==========================================================================
// サイズ設定
//==========================================================================
void CObject3D::SetSize(const D3DXVECTOR3 size)
{
	m_fSize = size;
}

//==========================================================================
// サイズ取得
//==========================================================================
D3DXVECTOR3 CObject3D::GetSize(void) const
{
	return m_fSize;
}

//==========================================================================
// オブジェクト3Dオブジェクトの取得
//==========================================================================
CObject3D *CObject3D::GetObject3D(void)
{
	return this;
}

//==========================================================================
// オブジェクト3Dメッシュオブジェクトの取得
//==========================================================================
CObject3DMesh *CObject3D::GetObject3DMesh(void)
{
	return NULL;
}