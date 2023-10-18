//=============================================================================
// 
//  オブジェクト3D処理 [object3DMesh.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "object3DMesh.h"
#include "calculation.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "debugproc.h"
#include "input.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define CAL_VTX		(1280)		// 計算用の頂点数
#define MAX_MOVE	(8.0f)		// 移動量
#define MAX_RANGE	(200.0f)	// 範囲
#define MOVE_LEN	(1.0f)

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
int nInit = 0;
int nUniit = 0;

//==========================================================================
// コンストラクタ
//==========================================================================
CObject3DMesh::CObject3DMesh(int nPriority) : CObject3D(nPriority)
{
	m_pVtxPos = NULL;		// 頂点座標
	m_pVtxNor = NULL;		// 頂点法線
	m_pVtxCol = NULL;		// 頂点カラー
	m_pVtxTex = NULL;		// 頂点テクスチャ座標
	m_nNumIndex = 0;		// インデックス数
	m_nNumVertex = 0;		// 頂点数
	m_nWidth = 0;			// 横分割数
	m_nHeight = 0;			// 縦分割数
	m_fWidthLen = 0.0f;		// 横の長さ
	m_fHeightLen = 0.0f;	// 縦の長さ
	m_pVtxBuff = NULL;		// 頂点バッファへのポインタ
	m_pIdxBuff = NULL;		// インデックスバッファへのポインタ
	m_nTexIdx = 0;			// テクスチャのインデックス番号
	m_type = TYPE_FIELD;	// メッシュのタイプ
}

//==========================================================================
// デストラクタ
//==========================================================================
CObject3DMesh::~CObject3DMesh()
{

}

//==========================================================================
// テクスチャの割り当て
//==========================================================================
void CObject3DMesh::BindTexture(int nIdx)
{
	// 割り当てる
	m_nTexIdx = nIdx;
}

//==========================================================================
// 生成処理
//==========================================================================
CObject3DMesh *CObject3DMesh::Create(void)
{
	// 生成用のオブジェクト
	CObject3DMesh *pObject3D = NULL;

	if (pObject3D == NULL)
	{// NULLだったら

		// メモリの確保
		pObject3D = DEBUG_NEW CObject3DMesh;

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
// 生成処理(オーバーロード)
//==========================================================================
CObject3DMesh *CObject3DMesh::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nPriority)
{
	// 生成用のオブジェクト
	CObject3DMesh *pObject3D = NULL;

	if (pObject3D == NULL)
	{// NULLだったら

		// メモリの確保
		pObject3D = DEBUG_NEW CObject3DMesh(nPriority);

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
// 生成処理(オーバーロード)
//==========================================================================
CObject3DMesh *CObject3DMesh::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fWidthLen, float fHeightLen, int nWidth, int nHeight, TYPE type, const char *pFileName, int nPriority)
{
	// 生成用のオブジェクト
	CObject3DMesh *pObject3D = NULL;

	if (pObject3D == NULL)
	{// NULLだったら

		// メモリの確保
		pObject3D = DEBUG_NEW CObject3DMesh(nPriority);

		if (pObject3D != NULL)
		{// メモリの確保が出来ていたら

			// 引数の情報を渡す
			pObject3D->m_fWidthLen = fWidthLen;
			pObject3D->m_fHeightLen = fHeightLen;
			pObject3D->m_nWidth = nWidth;
			pObject3D->m_nHeight = nHeight;
			pObject3D->m_type = type;

			// テクスチャの割り当て
			if (pFileName != NULL)
			{// NULLじゃなかったら
				pObject3D->m_nTexIdx = CManager::GetInstance()->GetTexture()->Regist(pFileName);
			}

			// 位置・向き
			pObject3D->SetPosition(pos);
			pObject3D->SetOriginPosition(pos);
			pObject3D->SetRotation(rot);

			// 初期化処理
			pObject3D->Init(type);
		}

		return pObject3D;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CObject3DMesh::Init(void)
{
	HRESULT hr;

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	m_nNumIndex = (m_nHeight * ((m_nWidth + 1) * 2)) + (2 * (m_nHeight - 1));	// インデックス数
	m_nNumVertex = (m_nHeight + 1) * (m_nWidth + 1);							// 頂点数
	m_type = TYPE_FIELD;	// 種類設定

	m_pVtxPos = DEBUG_NEW D3DXVECTOR3[m_nNumVertex];	// 頂点座標生成
	memset(m_pVtxPos, 0, sizeof(D3DXVECTOR3) * m_nNumVertex);

	m_pVtxNor = DEBUG_NEW D3DXVECTOR3[m_nNumVertex];	// 頂点法線
	memset(m_pVtxNor, 0, sizeof(D3DXVECTOR3) * m_nNumVertex);

	m_pVtxCol = DEBUG_NEW D3DXCOLOR[m_nNumVertex];	// 頂点カラー
	memset(m_pVtxCol, 0, sizeof(D3DXCOLOR) * m_nNumVertex);

	m_pVtxTex = DEBUG_NEW D3DXVECTOR2[m_nNumVertex];	// 頂点テクスチャ座標
	memset(m_pVtxCol, 0, sizeof(D3DXVECTOR2) * m_nNumVertex);

	nInit++;

	 // 頂点座標設定
	hr = CreateVertex();
	if (FAILED(hr))
	{// 失敗していたら
		return E_FAIL;
	}

	// インデックス生成
	hr = CreateIndex();
	if (FAILED(hr))
	{// 失敗していたら
		return E_FAIL;
	}

	// 頂点情報設定
	SetVtx();

	return S_OK;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CObject3DMesh::Init(TYPE type)
{
	HRESULT hr;

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	m_nNumIndex = (m_nHeight * ((m_nWidth + 1) * 2)) + (2 * (m_nHeight - 1));	// インデックス数
	m_nNumVertex = (m_nHeight + 1) * (m_nWidth + 1);							// 頂点数
	m_type = type;	// 種類設定

	m_pVtxPos = DEBUG_NEW D3DXVECTOR3[m_nNumVertex];	// 頂点座標
	memset(m_pVtxPos, 0, sizeof(D3DXVECTOR3) * m_nNumVertex);

	m_pVtxNor = DEBUG_NEW D3DXVECTOR3[m_nNumVertex];	// 頂点法線
	memset(m_pVtxNor, 0, sizeof(D3DXVECTOR3) * m_nNumVertex);

	m_pVtxCol = DEBUG_NEW D3DXCOLOR[m_nNumVertex];		// 頂点カラー
	memset(m_pVtxCol, 0, sizeof(D3DXCOLOR) * m_nNumVertex);

	m_pVtxTex = DEBUG_NEW D3DXVECTOR2[m_nNumVertex];	// 頂点テクスチャ座標
	memset(m_pVtxCol, 0, sizeof(D3DXVECTOR2) * m_nNumVertex);

	nInit++;

	// 頂点座標設定
	hr = CreateVertex();
	if (FAILED(hr))
	{// 失敗していたら
		return E_FAIL;
	}

	// インデックス生成
	hr = CreateIndex();
	if (FAILED(hr))
	{// 失敗していたら
		return E_FAIL;
	}

	VERTEX_3D *pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点情報の設定
	for (int nCntHeight = 0; nCntHeight < m_nHeight + 1; nCntHeight++)
	{//縦の頂点数分繰り返す

		for (int nCntWidth = 0; nCntWidth < m_nWidth + 1; nCntWidth++)
		{// 横の頂点数分繰り返す

			int nNowVtx = nCntWidth + (nCntHeight * (m_nWidth + 1));

			// 頂点座標割り当て
			m_pVtxPos[nNowVtx] = pVtx[0].pos;

			// 種類毎に頂点情報設定
			switch (m_type)
			{
			case TYPE_FIELD:
				m_pVtxNor[nNowVtx] = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
				m_pVtxTex[nNowVtx] = D3DXVECTOR2
				(
					((float)nCntWidth / (1.0f / (float)(m_nWidth + 1))) * (1.0f / (float)(m_nWidth + 1)),
					((float)nCntHeight / (1.0f / (float)(m_nHeight + 1))) * (1.0f / (float)(m_nHeight + 1))
				);
				break;

			case TYPE_WALL:
				m_pVtxNor[nNowVtx] = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
				m_pVtxTex[nNowVtx] = D3DXVECTOR2
				(
					((float)nCntWidth / (1.0f / (float)(m_nWidth + 1))) * (1.0f / (float)(m_nWidth + 1)),
					((float)nCntHeight / (1.0f / (float)(m_nHeight + 1))) * (1.0f / (float)(m_nHeight + 1))
				);
				break;

			case TYPE_CYLINDER:
				m_pVtxNor[nNowVtx] = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
				m_pVtxTex[nNowVtx] = D3DXVECTOR2
				(
					nCntWidth * (1.0f / (float)(m_nWidth / 2)),
					nCntHeight * (1.0f / (float)(m_nHeight))
				);
				break;

			case TYPE_DOME:
				m_pVtxNor[nNowVtx] = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
				m_pVtxTex[nNowVtx] = -D3DXVECTOR2
				(
					nCntWidth * (1.0f / (float)(m_nWidth)),
					nCntHeight * (1.0f / (float)(m_nHeight))
				);
				break;

			case TYPE_DONUTS:
				m_pVtxNor[nNowVtx] = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
				m_pVtxTex[nNowVtx] = D3DXVECTOR2
				(
					nCntWidth * (1.0f / (float)(m_nWidth / 4)),
					nCntHeight * (1.0f / (float)(m_nHeight))
				);
				break;

			case TYPE_SPHERE:
				m_pVtxNor[nNowVtx] = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
				m_pVtxTex[nNowVtx] = -D3DXVECTOR2
				(
					nCntWidth * (1.0f / (float)(m_nWidth / 4)),
					nCntHeight * (1.0f / (float)(m_nHeight / 4))
				);
				break;
			}

			m_pVtxCol[nNowVtx] = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx += 1;
		}
	}

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();

	// ワールドマトリックスの計算処理
	CalWorldMtx();

	return S_OK;
}

//==========================================================================
// 頂点バッファ生成
//==========================================================================
HRESULT CObject3DMesh::CreateVertex(void)
{
	HRESULT hr;

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// 頂点バッファの生成
	if (m_pVtxBuff != NULL)
	{// 既に情報が入ってる場合
		return E_FAIL;
	}

	// 頂点バッファの生成
	hr = pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * m_nNumVertex,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	if (FAILED(hr))
	{// 失敗していたら
		return E_FAIL;
	}

	if (m_pVtxBuff == NULL)
	{// 既に情報が入ってる場合
		return E_FAIL;
	}

	// 種類に頂点座標設定
	switch (m_type)
	{
	case TYPE_FIELD:
	{
		VERTEX_3D *pVtx;	// 頂点情報へのポインタ

		// 頂点バッファをロックし、頂点情報へのポインタを取得
		m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		// 頂点情報の設定
		for (int nCntHeight = 0; nCntHeight < m_nHeight + 1; nCntHeight++)
		{//縦の頂点数分繰り返す

			for (int nCntWidth = 0; nCntWidth < m_nWidth + 1; nCntWidth++)
			{// 横の頂点数分繰り返す

				// 頂点座標の設定
				pVtx[0].pos = D3DXVECTOR3(
					(m_fWidthLen * nCntWidth) - ((m_fWidthLen * m_nWidth) * 0.5f),
					m_pVtxPos[nCntWidth + (nCntHeight * (m_nWidth + 1))].y,
					-((m_fHeightLen * nCntHeight) - ((m_fHeightLen * m_nHeight) * 0.5f)));

				// 頂点座標割り当て
				m_pVtxPos[nCntWidth + (nCntHeight * (m_nWidth + 1))] = pVtx[0].pos;
				pVtx += 1;
			}
		}

		// 頂点バッファをアンロックする
		m_pVtxBuff->Unlock();

		SetVtxField();
	}
		break;

	case TYPE_WALL:
	{
		VERTEX_3D *pVtx;	// 頂点情報へのポインタ

		// 頂点バッファをロックし、頂点情報へのポインタを取得
		m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		// 頂点情報の設定
		for (int nCntHeight = 0; nCntHeight < m_nHeight + 1; nCntHeight++)
		{//縦の頂点数分繰り返す

			for (int nCntWidth = 0; nCntWidth < m_nWidth + 1; nCntWidth++)
			{// 横の頂点数分繰り返す

				// 頂点座標の設定
				pVtx[0].pos = D3DXVECTOR3
				(
					(m_fWidthLen * nCntWidth) - ((m_fWidthLen * m_nWidth) * 0.5f),
					(m_fHeightLen * m_nHeight) - ((m_fHeightLen * nCntHeight)),
					0.0f
				);

				// 頂点座標割り当て
				m_pVtxPos[nCntWidth + (nCntHeight * (m_nWidth + 1))] = pVtx[0].pos;

				pVtx += 1;
			}
		}

		// 頂点バッファをアンロックする
		m_pVtxBuff->Unlock();
	}
		break;

	case TYPE_CYLINDER:
		SetVtxCylinder();
		break;

	case TYPE_DOME:
		SetVtxDome();
		break;

	case TYPE_DONUTS:
		SetVtxDonuts();
		break;

	case TYPE_SPHERE:
		SetVtxSphere();
		break;
	}

	return S_OK;
}

//==========================================================================
// インデックス生成
//==========================================================================
HRESULT CObject3DMesh::CreateIndex(void)
{
	HRESULT hr;

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// インデックスバッファの生成
	hr = pDevice->CreateIndexBuffer(sizeof(WORD) * m_nNumIndex,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIdxBuff,
		NULL);

	if (FAILED(hr))
	{// 失敗していたら
		return E_FAIL;
	}

	WORD *pIdx;	// インデックス情報へのポインタ

	// インデックスバッファをロックし、頂点番号データへのポインタを取得
	m_pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

	// インデックスバッファの要素番号
	int nCntIdx = 0;

	// 頂点番号データの設定
	for (int nCntHeight = 0; nCntHeight < m_nHeight; nCntHeight++)
	{// 高さの分割数分繰り返す

		for (int nCntWidth = 0; nCntWidth < m_nWidth + 1; nCntWidth++)
		{// 横の頂点数分繰り返す

			pIdx[nCntIdx + 0] = nCntWidth + (m_nWidth + 1) * (nCntHeight + 1);
			pIdx[nCntIdx + 1] = nCntWidth + ((m_nWidth + 1) * nCntHeight);
			nCntIdx += 2;	// 2つずつ見てるから
		}

		if (nCntHeight + 1 < m_nHeight)
		{// 最後のちょんは打たない

			// 空打ち2つ分
			pIdx[nCntIdx + 0] = ((m_nWidth + 1) * (nCntHeight + 1)) - 1;
			pIdx[nCntIdx + 1] = (m_nWidth + 1) * (nCntHeight + 2);
			nCntIdx += 2;	// 2つずつ見てるから
		}
	}

	// インデックスバッファをアンロックする
	m_pIdxBuff->Unlock();

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CObject3DMesh::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}

	// インデックスバッファの破棄
	if (m_pIdxBuff != NULL)
	{
		m_pIdxBuff->Release();
		m_pIdxBuff = NULL;
	}
	nUniit++;
	// 頂点座標の破棄
	if (m_pVtxPos != NULL)
	{
		delete[] m_pVtxPos;
		m_pVtxPos = NULL;
	}

	// 頂点法線の破棄
	if (m_pVtxNor != NULL)
	{
		delete[] m_pVtxNor;
		m_pVtxNor = NULL;
	}

	// 頂点法線の破棄
	if (m_pVtxCol != NULL)
	{
		delete[] m_pVtxCol;
		m_pVtxCol = NULL;
	}

	// 頂点テクスチャ座標の破棄
	if (m_pVtxTex != NULL)
	{
		delete[] m_pVtxTex;
		m_pVtxTex = NULL;
	}

	// 終了処理
	CObject3D::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CObject3DMesh::Update(void)
{
	// 頂点情報設定
	SetVtx();
}

//==========================================================================
// 頂点上げ下げ
//==========================================================================
void CObject3DMesh::UPVtxField(D3DXVECTOR3 pos)
{
	if (m_type != TYPE_FIELD)
	{
		return;
	}

	// デバッグ表示
	CManager::GetInstance()->GetDebugProc()->Print(
		"\n"
		"頂点上げ下げ：[5, 6]\n"
		"幅拡縮：[7, 8] 【%f, %f】\n", m_fWidthLen, m_fHeightLen);

	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// 頂点情報の設定
	for (int nCntHeight = 0; nCntHeight < m_nHeight + 1; nCntHeight++)
	{//縦の頂点数分繰り返す

		for (int nCntWidth = 0; nCntWidth < m_nWidth + 1; nCntWidth++)
		{// 横の頂点数分繰り返す

			float fNowLength = 
				sqrtf((pos.x - m_pVtxPos[nCntWidth + (nCntHeight * (m_nWidth + 1))].x) * (pos.x - m_pVtxPos[nCntWidth + (nCntHeight * (m_nWidth + 1))].x)
				+ (pos.z - m_pVtxPos[nCntWidth + (nCntHeight * (m_nWidth + 1))].z) * (pos.z - m_pVtxPos[nCntWidth + (nCntHeight * (m_nWidth + 1))].z));

			float Wariai = fNowLength / MAX_RANGE;

			if (Wariai >= 1.0f)
			{// 割合が1.0f以下だけ
				continue;
			}

			D3DXVECTOR3 move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

			if (pInputKeyboard->GetPress(DIK_6))
			{// 6で上げる
				move.y = MAX_MOVE - (MAX_MOVE * Wariai);
			}

			if (pInputKeyboard->GetPress(DIK_5))
			{// 5で下げる
				move.y = -MAX_MOVE - (-MAX_MOVE * Wariai);
			}

			// 各頂点座標確認
			m_pVtxPos[nCntWidth + (nCntHeight * (m_nWidth + 1))].y += move.y;
		}
	}

	if (pInputKeyboard->GetPress(DIK_7))
	{// 7で上げる

		m_fWidthLen -= MOVE_LEN;
		m_fHeightLen -= MOVE_LEN;
	}

	if (pInputKeyboard->GetPress(DIK_8))
	{// 8で下げる
		m_fWidthLen += MOVE_LEN;
		m_fHeightLen += MOVE_LEN;
	}

	ValueNormalize(m_fWidthLen, 99999999.0f, 10.0f);
	ValueNormalize(m_fHeightLen, 99999999.0f, 10.0f);
}


//==========================================================================
// ワールドマトリックスの計算処理
//==========================================================================
void CObject3DMesh::CalWorldMtx(void)
{
	D3DXMATRIX m_mtxWorld = GetWorldMtx();			// マトリックス取得
	D3DXVECTOR3 m_rot = GetRotation();				// 向き取得
	D3DXVECTOR3 m_rotOrigin = GetOriginRotation();	// 元の向き取得
	D3DXVECTOR3 m_pos = GetPosition();				// 位置取得

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// 計算用マトリックス宣言
	D3DXMATRIX mtxRot, mtxTrans, mtxRotOrigin;
	D3DXMatrixIdentity(&mtxRotOrigin);

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// 元の向きを反映する
	D3DXMatrixRotationYawPitchRoll(&mtxRotOrigin, m_rotOrigin.y, m_rotOrigin.x, m_rotOrigin.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRotOrigin);

	// 向きを反映する
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映する
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);
	SetWorldMtx(m_mtxWorld);
}

//==========================================================================
// 描画処理
//==========================================================================
void CObject3DMesh::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// ワールドマトリックスの計算処理
	CalWorldMtx();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// インデックスバッファをデータストリームに設定
	pDevice->SetIndices(m_pIdxBuff);

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, CManager::GetInstance()->GetTexture()->GetAdress(m_nTexIdx));

	// ポリゴンの描画
	pDevice->DrawIndexedPrimitive(
		D3DPT_TRIANGLESTRIP,
		0,
		0,
		m_nNumVertex,
		0,
		m_nNumIndex - 2);

}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CObject3DMesh::SetVtx(void)
{
	// 種類に頂点座標設定
	switch (m_type)
	{
	case TYPE_FIELD:
		SetVtxField();
		break;

	case TYPE_WALL:
		SetVtxWall();
		break;

	case TYPE_CYLINDER:
		SetVtxCylinder();
		break;

	case TYPE_DOME:
		SetVtxDome();
		break;

	case TYPE_DONUTS:
		SetVtxDonuts();
		break;

	case TYPE_SPHERE:
		SetVtxSphere();
		break;
	}
}

//==========================================================================
// フィールドの初期化時頂点座標設定
//==========================================================================
void CObject3DMesh::SetInitVtxField(void)
{
	VERTEX_3D *pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点情報の設定
	for (int nCntHeight = 0; nCntHeight < m_nHeight + 1; nCntHeight++)
	{//縦の頂点数分繰り返す

		for (int nCntWidth = 0; nCntWidth < m_nWidth + 1; nCntWidth++)
		{// 横の頂点数分繰り返す

			// 頂点座標の設定
			pVtx[0].pos = D3DXVECTOR3(
				(m_fWidthLen * nCntWidth) - ((m_fWidthLen * m_nWidth) * 0.5f),
				m_pVtxPos[nCntWidth + (nCntHeight * (m_nWidth + 1))].y,
				-((m_fHeightLen * nCntHeight) - ((m_fHeightLen * m_nHeight) * 0.5f)));

			// 頂点座標割り当て
			m_pVtxPos[nCntWidth + (nCntHeight * (m_nWidth + 1))] = pVtx[0].pos;

			// 法線ベクトルの設定
			pVtx[0].nor = m_pVtxNor[nCntWidth + (nCntHeight * (m_nWidth + 1))];

			// 頂点カラーの設定
			pVtx[0].col = m_pVtxCol[nCntWidth + (nCntHeight * (m_nWidth + 1))];

			// テクスチャ座標の設定
			pVtx[0].tex = m_pVtxTex[nCntWidth + (nCntHeight * (m_nWidth + 1))];

			pVtx += 1;
		}
	}

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//==========================================================================
// フィールドの頂点座標設定
//==========================================================================
void CObject3DMesh::SetVtxField(void)
{

	VERTEX_3D *pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点情報の設定
	for (int nCntHeight = 0; nCntHeight < m_nHeight + 1; nCntHeight++)
	{//縦の頂点数分繰り返す

		for (int nCntWidth = 0; nCntWidth < m_nWidth + 1; nCntWidth++)
		{// 横の頂点数分繰り返す

			if (nCntWidth + (nCntHeight * (m_nWidth + 1)) >= (m_nWidth + 1) * (m_nHeight + 1))
			{// 頂点数超えたら

				int n = 0;
			}

			// 頂点座標割り当て
			pVtx[0].pos = m_pVtxPos[nCntWidth + (nCntHeight * (m_nWidth + 1))];

			// 法線ベクトルの設定
			pVtx[0].nor = m_pVtxNor[nCntWidth + (nCntHeight * (m_nWidth + 1))];

			// 頂点カラーの設定
			pVtx[0].col = m_pVtxCol[nCntWidth + (nCntHeight * (m_nWidth + 1))];

			// テクスチャ座標の設定
			pVtx[0].tex = m_pVtxTex[nCntWidth + (nCntHeight * (m_nWidth + 1))];

			pVtx += 1;
		}
	}

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//==========================================================================
// ウォールの頂点座標設定
//==========================================================================
void CObject3DMesh::SetVtxWall(void)
{

	VERTEX_3D *pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点情報の設定
	for (int nCntHeight = 0; nCntHeight < m_nHeight + 1; nCntHeight++)
	{//縦の頂点数分繰り返す

		for (int nCntWidth = 0; nCntWidth < m_nWidth + 1; nCntWidth++)
		{// 横の頂点数分繰り返す

			// 頂点座標の設定
			pVtx[0].pos = m_pVtxPos[nCntWidth + (nCntHeight * (m_nWidth + 1))];

			// 頂点座標割り当て
			m_pVtxPos[nCntWidth + (nCntHeight * (m_nWidth + 1))] = pVtx[0].pos;

			// 法線ベクトルの設定
			pVtx[0].nor = m_pVtxNor[nCntWidth + (nCntHeight * (m_nWidth + 1))];

			// 頂点カラーの設定
			pVtx[0].col = m_pVtxCol[nCntWidth + (nCntHeight * (m_nWidth + 1))];

			// テクスチャ座標の設定
			pVtx[0].tex = m_pVtxTex[nCntWidth + (nCntHeight * (m_nWidth + 1))];

			pVtx += 1;
		}
	}

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//==========================================================================
// シリンダーの頂点座標設定
//==========================================================================
void CObject3DMesh::SetVtxCylinder(void)
{
	D3DXCOLOR col = GetColor();			// 色
	D3DXVECTOR3 pos = GetPosition();	// 位置
	D3DXVECTOR3 posVtx[CAL_VTX];		// 計算用の座標
	float fRot = (D3DX_PI * 2) / (float)(m_nWidth);	//1つごとの角度を求める
	D3DXVECTOR3 NormalizeNor[CAL_VTX];	// 正規化用

	for (int nCntHeight = 0; nCntHeight < m_nHeight; nCntHeight++)
	{//縦の頂点数分繰り返す

		for (int nCntWidth = 0; nCntWidth < m_nWidth + 1; nCntWidth++)
		{//横の頂点数分繰り返す

			// 頂点座標求める
			posVtx[nCntWidth + (nCntHeight * (m_nWidth + 1))] = D3DXVECTOR3
			(
				sinf(nCntWidth % m_nWidth * fRot) * m_fWidthLen,
				(m_fHeightLen * m_nHeight) - ((m_fHeightLen * nCntHeight)),
				cosf(nCntWidth % m_nWidth * fRot) * m_fWidthLen
			);

			// 頂点座標求める
			posVtx[nCntWidth + (m_nWidth + 1) + (nCntHeight * (m_nWidth + 1))] = D3DXVECTOR3
			(
				sinf(nCntWidth % m_nWidth * fRot) * m_fWidthLen,
				(m_fHeightLen * m_nHeight) - ((m_fHeightLen * (nCntHeight + 1))),
				cosf(nCntWidth % m_nWidth * fRot) * m_fWidthLen
			);

			// 各頂点から原点を引く
			NormalizeNor[nCntWidth + (nCntHeight * (m_nWidth + 1))] = posVtx[nCntWidth + (nCntHeight * (m_nWidth + 1))] - pos;
			NormalizeNor[nCntWidth + (m_nWidth + 1) + (nCntHeight * (m_nWidth + 1))] = posVtx[nCntWidth + (m_nWidth + 1) + (nCntHeight * (m_nWidth + 1))] - pos;

			// 出た向きの値を正規化する
			D3DXVec3Normalize(&NormalizeNor[nCntWidth + (nCntHeight * (m_nWidth + 1))],				&NormalizeNor[nCntWidth + (nCntHeight * (m_nWidth + 1))]);
			D3DXVec3Normalize(&NormalizeNor[nCntWidth + (m_nWidth + 1) + (nCntHeight * (m_nWidth + 1))],	&NormalizeNor[nCntWidth + (m_nWidth + 1) + (nCntHeight * (m_nWidth + 1))]);
		}
	}

	VERTEX_3D *pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点情報の設定
	for (int nCntHeight = 0; nCntHeight < m_nHeight + 1; nCntHeight++)
	{// 縦の頂点数分繰り返す

		for (int nCntWidth = 0; nCntWidth < m_nWidth + 1; nCntWidth++)
		{// 横の頂点数分繰り返す

			// 頂点座標の設定
			pVtx[0].pos = posVtx[nCntWidth + (nCntHeight * (m_nWidth + 1))];

			// 法線ベクトルの設定
			pVtx[0].nor = NormalizeNor[nCntWidth + (nCntHeight * (m_nWidth + 1))];

			// 頂点カラーの設定
			pVtx[0].col = col;

			// テクスチャ座標の設定
			pVtx[0].tex = m_pVtxTex[nCntWidth + (nCntHeight * (m_nWidth + 1))];

			pVtx += 1;
		}
	}

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//==========================================================================
// ドームの頂点座標設定
//==========================================================================
void CObject3DMesh::SetVtxDome(void)
{
	D3DXCOLOR col = GetColor();			// 色
	D3DXVECTOR3 pos = GetPosition();	// 位置
	D3DXVECTOR3 posVtx[CAL_VTX];		// 計算用の座標
	D3DXVECTOR3 NormalizeNor[CAL_VTX];	// 計算用のベクトル
	float fRotWidth = (D3DX_PI * 2) / (float)(m_nWidth);	// 1つごとの角度を求める, 周囲
	float fRotHeight = (D3DX_PI * 0.5f) / (float)(m_nHeight);	// 1つごとの角度を求める, 高さ
	float fRotCalW = 0.0f;	// 横の今回の角度
	float fRotCalH = 0.0f;	// 縦の今回の角度

	// 最後の点
	posVtx[m_nWidth + (m_nHeight * (m_nWidth + 1))] = D3DXVECTOR3(0.0f, m_fHeightLen, 0.0f);

	for (int nCntHeight = 0; nCntHeight < m_nHeight + 1; nCntHeight++)
	{// 縦の頂点数分繰り返す

		for (int nCntWidth = 0; nCntWidth < m_nWidth + 1; nCntWidth++)
		{// 横の頂点数分繰り返す

			// 今回の角度を割り出す
			fRotCalH = (float)(nCntHeight % (m_nHeight +1 )) * fRotHeight;
			fRotCalW = (fRotWidth * m_nWidth) - ((float)(nCntWidth % (m_nWidth + 1)) * fRotWidth);

			// 角度の正規化
			RotNormalize(fRotCalW);
			RotNormalize(fRotCalH);

			// 座標割り出し
			posVtx[nCntWidth + (nCntHeight * (m_nWidth + 1))].x = cosf(fRotCalH) * sinf(fRotCalW) * m_fWidthLen;
			posVtx[nCntWidth + (nCntHeight * (m_nWidth + 1))].z = cosf(fRotCalH) * cosf(fRotCalW) * m_fWidthLen;
			posVtx[nCntWidth + (nCntHeight * (m_nWidth + 1))].y = sinf(fRotCalH) * m_fHeightLen;

			// 各頂点から原点を引く
			NormalizeNor[nCntWidth + (nCntHeight * (m_nWidth + 1))] = posVtx[nCntWidth + (nCntHeight * (m_nWidth + 1))] - pos;

			// 出た向きの値を正規化する
			D3DXVec3Normalize(&NormalizeNor[nCntWidth + (nCntHeight * (m_nWidth + 1))], &NormalizeNor[nCntWidth + (nCntHeight * (m_nWidth + 1))]);
		}
	}
	
	VERTEX_3D *pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点情報の設定
	for (int nCntHeight = 0; nCntHeight < m_nHeight + 1; nCntHeight++)
	{// 縦の頂点数分繰り返す

		for (int nCntWidth = 0; nCntWidth < m_nWidth + 1; nCntWidth++)
		{// 横の頂点数分繰り返す

			// 頂点座標の設定
			pVtx[0].pos = posVtx[nCntWidth + (nCntHeight * (m_nWidth + 1))];

			// 法線ベクトルの設定
			pVtx[0].nor = NormalizeNor[nCntWidth + (nCntHeight * (m_nWidth + 1))];

			// 頂点カラーの設定
			pVtx[0].col = col;

			// テクスチャ座標の設定
			pVtx[0].tex = m_pVtxTex[nCntWidth + (nCntHeight * (m_nWidth + 1))];

			pVtx += 1;
		}
	}

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//==========================================================================
// ドーナツの頂点座標設定
//==========================================================================
void CObject3DMesh::SetVtxDonuts(void)
{
	D3DXCOLOR col = GetColor();			// 色
	D3DXVECTOR3 pos = GetPosition();	// 位置
	D3DXVECTOR3 posVtx[CAL_VTX] = {};		// 計算用の座標
	D3DXVECTOR3 NormalizeNor[CAL_VTX] = {};	// 計算用のベクトル

	VERTEX_3D *pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点情報の設定
	for (int nCntHeight = 0; nCntHeight < m_nHeight + 1; nCntHeight++)
	{// 縦の頂点数分繰り返す

		for (int nCntWidth = 0; nCntWidth < m_nWidth + 1; nCntWidth++)
		{// 横の頂点数分繰り返す

			// 頂点座標設定
			pVtx[0].pos = m_pVtxPos[nCntWidth + (nCntHeight * (m_nWidth + 1))];

			// 各頂点から原点を引く
			NormalizeNor[nCntWidth + (nCntHeight * (m_nWidth + 1))] = posVtx[nCntWidth + (nCntHeight * (m_nWidth + 1))] - pos;

			// 出た向きの値を正規化する
			D3DXVec3Normalize(&NormalizeNor[nCntWidth + (nCntHeight * (m_nWidth + 1))], &NormalizeNor[nCntWidth + (nCntHeight * (m_nWidth + 1))]);

			// 法線ベクトルの設定
			pVtx[0].nor = NormalizeNor[nCntWidth + (nCntHeight * (m_nWidth + 1))];

			// 頂点カラーの設定
			pVtx[0].col = col;

			// テクスチャ座標の設定
			pVtx[0].tex = m_pVtxTex[nCntWidth + (nCntHeight * (m_nWidth + 1))];

			pVtx += 1;
		}
	}

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//==========================================================================
// 球の頂点座標設定
//==========================================================================
void CObject3DMesh::SetVtxSphere(void)
{
	D3DXCOLOR col = GetColor();			// 色
	D3DXVECTOR3 pos = GetPosition();	// 位置
	D3DXVECTOR3 posVtx[CAL_VTX];		// 計算用の座標
	D3DXVECTOR3 NormalizeNor[CAL_VTX];	// 計算用のベクトル
	float fRotWidth = (D3DX_PI * 2) / (float)(m_nWidth);	// 1つごとの角度を求める, 周囲
	float fRotHeight = (D3DX_PI * 2) / (float)(m_nHeight);	// 1つごとの角度を求める, 高さ
	float fRotCalW = 0.0f;	// 横の今回の角度
	float fRotCalH = 0.0f;	// 縦の今回の角度

	VERTEX_3D *pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 最後の点
	pVtx[m_nWidth + (m_nHeight * (m_nWidth + 1))].pos = D3DXVECTOR3(0.0f, m_fHeightLen, 0.0f);

	for (int nCntHeight = 0; nCntHeight < m_nHeight + 1; nCntHeight++)
	{// 縦の頂点数分繰り返す

		// リセット
		fRotCalW = 0.0f;

		for (int nCntWidth = 0; nCntWidth < m_nWidth + 1; nCntWidth++)
		{// 横の頂点数分繰り返す

			// 今回の角度を割り出す
			fRotCalH = (float)nCntHeight * fRotHeight;
			fRotCalW = (float)nCntWidth * fRotWidth;

			// 角度の正規化
			/*RotNormalize(fRotCalW);
			RotNormalize(fRotCalH);*/

			// 座標割り出し
			posVtx[nCntWidth + (nCntHeight * (m_nWidth + 1))].x = cosf(fRotCalH) * sinf(fRotCalW) * m_fWidthLen;
			posVtx[nCntWidth + (nCntHeight * (m_nWidth + 1))].z = cosf(fRotCalH) * cosf(fRotCalW) * m_fWidthLen;
			posVtx[nCntWidth + (nCntHeight * (m_nWidth + 1))].y = sinf(fRotCalH) * m_fHeightLen;

			// 頂点座標の設定
			pVtx[0].pos = posVtx[nCntWidth + (nCntHeight * (m_nWidth + 1))];

			// 各頂点から原点を引く
			NormalizeNor[nCntWidth + (nCntHeight * (m_nWidth + 1))] = posVtx[nCntWidth + (nCntHeight * (m_nWidth + 1))] - pos;

			// 出た向きの値を正規化する
			D3DXVec3Normalize(&NormalizeNor[nCntWidth + (nCntHeight * (m_nWidth + 1))], &NormalizeNor[nCntWidth + (nCntHeight * (m_nWidth + 1))]);

			// 法線ベクトルの設定
			pVtx[0].nor = NormalizeNor[nCntWidth + (nCntHeight * (m_nWidth + 1))];

			// 頂点カラーの設定
			pVtx[0].col = col;

			// テクスチャ座標の設定
			pVtx[0].tex = m_pVtxTex[nCntWidth + (nCntHeight * (m_nWidth + 1))];

			pVtx += 1;
		}
	}

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}

//==========================================================================
// インデックス数取得
//==========================================================================
int CObject3DMesh::GetNumIndex(void)
{
	return m_nNumIndex;
}

//==========================================================================
// インデックス数設定
//==========================================================================
void CObject3DMesh::SetNumIndex(int nWidth, int nHeight)
{
	m_nNumIndex = (nHeight * ((nWidth + 1) * 2)) + (2 * (nHeight - 1));	// インデックス数
}

//==========================================================================
// 頂点数設定
//==========================================================================
int CObject3DMesh::GetNumVertex(void)
{
	return m_nNumVertex;
}

//==========================================================================
// 頂点数取得
//==========================================================================
void CObject3DMesh::SetNumVertex(int nWidth, int nHeight)
{
	m_nNumVertex = (nHeight + 1) * (nWidth + 1);							// 頂点数
}

//==========================================================================
// 頂点座標取得
//==========================================================================
D3DXVECTOR3 *CObject3DMesh::GetVtxPos(void)
{
	return m_pVtxPos;
}

//==========================================================================
// 頂点座標設定
//==========================================================================
void CObject3DMesh::SetVtxPos(D3DXVECTOR3 *pos)
{
	m_pVtxPos = pos;
}

//==========================================================================
// 頂点ベクトル取得
//==========================================================================
D3DXVECTOR3 *CObject3DMesh::GetVtxNor(void)
{
	return m_pVtxNor;
}

//==========================================================================
// 頂点ベクトル取得
//==========================================================================
void CObject3DMesh::SetVtxNor(D3DXVECTOR3 *nor)
{
	m_pVtxNor = nor;
}

//==========================================================================
// 頂点カラー取得
//==========================================================================
D3DXCOLOR *CObject3DMesh::GetVtxCol(void)
{
	return m_pVtxCol;
}

//==========================================================================
// 頂点カラー設定
//==========================================================================
void CObject3DMesh::SetVtxCol(D3DXCOLOR *col)
{
	m_pVtxCol = col;
}

//==========================================================================
// テクスチャ座標取得
//==========================================================================
D3DXVECTOR2 *CObject3DMesh::GetVtxTex(void)
{
	return m_pVtxTex;
}

//==========================================================================
// テクスチャ座標設定
//==========================================================================
void CObject3DMesh::SetVtxTex(D3DXVECTOR2 *tex)
{
	m_pVtxTex = tex;
}

//==========================================================================
// 幅の分割数取得
//==========================================================================
int CObject3DMesh::GetWidthBlock(void)
{
	return m_nWidth;
}

//==========================================================================
// 幅の分割数設定
//==========================================================================
void CObject3DMesh::SetWidthBlock(int nWidth)
{
	m_nWidth = nWidth;
}

//==========================================================================
// 高さの分割数取得
//==========================================================================
int CObject3DMesh::GetHeightBlock(void)
{
	return m_nHeight;
}

//==========================================================================
// 高さの分割数設定
//==========================================================================
void CObject3DMesh::SetHeightBlock(int nHeight)
{
	m_nHeight = nHeight;
}

//==========================================================================
// 幅の長さ取得
//==========================================================================
float CObject3DMesh::GetWidthLen(void)
{
	return m_fWidthLen;
}

//==========================================================================
// 幅の長さ設定
//==========================================================================
void CObject3DMesh::SetWidthLen(float fLen)
{
	m_fWidthLen = fLen;
}

//==========================================================================
// 高さの長さ取得
//==========================================================================
float CObject3DMesh::GetHeightLen(void)
{
	return m_fHeightLen;
}

//==========================================================================
// 高さの長さ設定
//==========================================================================
void CObject3DMesh::SetHeightLen(float fLen)
{
	m_fHeightLen = fLen;
}

//==========================================================================
// テクスチャのインデックス番号取得
//==========================================================================
int CObject3DMesh::GetIdxTex(void)
{
	return m_nTexIdx;
}

//==========================================================================
// オブジェクト3Dオブジェクトの取得
//==========================================================================
CObject3DMesh *CObject3DMesh::GetObject3DMesh(void)
{
	return this;
}

LPDIRECT3DVERTEXBUFFER9 CObject3DMesh::GetVtxBuff(void)
{
	return m_pVtxBuff;
}
