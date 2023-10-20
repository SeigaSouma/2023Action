//=============================================================================
// 
//  オブジェクトX処理 [objectX.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "objectX.h"
#include "manager.h"
#include "renderer.h"
#include "calculation.h"
#include "texture.h"
#include "shadow.h"
#include "elevation.h"

//==========================================================================
// マクロ定義
//==========================================================================

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
int CObjectX::m_nNumAll = 0;	// 総数

//==========================================================================
// コンストラクタ
//==========================================================================
CObjectX::CObjectX(int nPriority) : CObject(nPriority)
{
	D3DXMatrixIdentity(&m_mtxWorld);				// ワールドマトリックス
	SetScale(D3DXVECTOR3(1.0f, 1.0f, 1.0f));		// スケール
	SetPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));		// 位置
	SetOldPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));	// 前回の位置
	SetMove(D3DXVECTOR3(0.0f, 0.0f, 0.0f));			// 移動量
	SetRotation(D3DXVECTOR3(0.0f, 0.0f, 0.0f));		// 向き
	SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));	// 色
	SetSize(D3DXVECTOR3(0.0f, 0.0f, 0.0f));			// サイズ
	m_bShadow = false;								// 影を使っているかどうか
	m_nIdxTexure = NULL;							// テクスチャのインデックス番号
	m_nIdxXFile = 0;								// Xファイルのインデックス番号
	m_pShadow = NULL;								// 影の情報
	m_nNumAll++;									// 総数加算
}

//==========================================================================
// デストラクタ
//==========================================================================
CObjectX::~CObjectX()
{

}

//==========================================================================
// テクスチャの割り当て
//==========================================================================
void CObjectX::BindTexture(int *nIdx)
{
	// 割り当てる
	m_nIdxTexure = *nIdx;
}

//==========================================================================
// Xファイルのデータ割り当て
//==========================================================================
void CObjectX::BindXData(int nIdxXFile)
{
	// 情報割り当て
	m_nIdxXFile = nIdxXFile;
}

//==========================================================================
// 生成処理
//==========================================================================
CObjectX *CObjectX::Create(void)
{
	// 生成用のオブジェクト
	CObjectX *pObjectX = NULL;

	if (pObjectX == NULL)
	{// NULLだったら

		// メモリの確保
		pObjectX = DEBUG_NEW CObjectX;

		if (pObjectX != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			pObjectX->Init();
		}

		return pObjectX;
	}

	return NULL;
}

//==========================================================================
// 生成処理
//==========================================================================
CObjectX *CObjectX::Create(const char *pFileName)
{
	// 生成用のオブジェクト
	CObjectX *pObjectX = NULL;

	if (pObjectX == NULL)
	{// NULLだったら

		// メモリの確保
		pObjectX = DEBUG_NEW CObjectX;

		if (pObjectX != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			HRESULT hr = pObjectX->Init(pFileName);

			if (FAILED(hr))
			{// 失敗していたら
				return NULL;
			}

			// 種類設定
			pObjectX->SetType(TYPE_XFILE);
		}

		return pObjectX;
	}

	return NULL;
}

//==========================================================================
// 生成処理
//==========================================================================
CObjectX *CObjectX::Create(const char *pFileName, D3DXVECTOR3 pos, D3DXVECTOR3 rot, bool bShadow)
{
	// 生成用のオブジェクト
	CObjectX *pObjectX = NULL;

	if (pObjectX == NULL)
	{// NULLだったら

		// メモリの確保
		pObjectX = DEBUG_NEW CObjectX;

		if (pObjectX != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			HRESULT hr = pObjectX->Init(pFileName);

			if (FAILED(hr))
			{// 失敗していたら
				return NULL;
			}

			// 種類設定
			//pObjectX->SetType(TYPE_XFILE);

			// 位置・向き
			pObjectX->SetPosition(pos);
			pObjectX->SetRotation(rot);
			pObjectX->m_bShadow = bShadow;

			if (bShadow == true)
			{
				// 影設定
				float f = pObjectX->GetVtxMax().x * 0.5f;
				pObjectX->m_pShadow = CShadow::Create(pObjectX->GetPosition(), f);
			}
		}

		return pObjectX;
	}

	return NULL;
}

//==========================================================================
// 生成処理
//==========================================================================
CObjectX *CObjectX::Create(int nIdxXFile, D3DXVECTOR3 pos, D3DXVECTOR3 rot, bool bShadow)
{
	// 生成用のオブジェクト
	CObjectX *pObjectX = NULL;

	if (pObjectX == NULL)
	{// NULLだったら

		// メモリの確保
		pObjectX = DEBUG_NEW CObjectX;

		if (pObjectX != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			HRESULT hr = pObjectX->Init(nIdxXFile);

			if (FAILED(hr))
			{// 失敗していたら
				return NULL;
			}

			// 種類設定
			//pObjectX->SetType(TYPE_XFILE);

			// 位置・向き
			pObjectX->SetPosition(pos);
			pObjectX->SetRotation(rot);
			pObjectX->m_bShadow = bShadow;

			if (bShadow == true)
			{
				// 影設定
				float f = pObjectX->GetVtxMax().x * 0.5f;
				pObjectX->m_pShadow = CShadow::Create(pObjectX->GetPosition(), f);
			}
		}

		return pObjectX;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CObjectX::Init(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// Xファイルのデータ取得
	CXLoad *pXLoad = CScene::GetXLoad();

	// Xファイルのロード
	m_nIdxXFile = pXLoad->XLoad("data\\MODEL\\radiokaikan_01.x");

	// Xファイルのデータ割り当て
	BindXData(m_nIdxXFile);

	// Xファイルのデータ取得
	CXLoad::SXFile *pXData = CScene::GetXLoad()->GetObjectX(m_nIdxXFile);

	// 全頂点チェック
	CheckVtx(GetRotation().y, &pXData->vtxMax, &pXData->vtxMin, pXData->pMesh, pXData->pVtxBuff);

	return S_OK;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CObjectX::Init(const char *pFileName)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// Xファイルのデータ取得
	CXLoad *pXLoad = CScene::GetXLoad();

	// Xファイルのロード
	m_nIdxXFile = pXLoad->XLoad(pFileName);

	// Xファイルのデータ割り当て
	BindXData(m_nIdxXFile);

	// Xファイルのデータ取得
	CXLoad::SXFile *pXData = CScene::GetXLoad()->GetObjectX(m_nIdxXFile);

	// 全頂点チェック
	CheckVtx(GetRotation().y, &pXData->vtxMax, &pXData->vtxMin, pXData->pMesh, pXData->pVtxBuff);

	return S_OK;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CObjectX::Init(int nIdxXFile)
{
	// Xファイルのデータ割り当て
	BindXData(nIdxXFile);

	// Xファイルのデータ取得
	CXLoad::SXFile *pXData = CScene::GetXLoad()->GetObjectX(m_nIdxXFile);

	// 全頂点チェック
	CheckVtx(GetRotation().y, &pXData->vtxMax, &pXData->vtxMin, pXData->pMesh, pXData->pVtxBuff);

	return S_OK;
}


//==========================================================================
// 終了処理
//==========================================================================
void CObjectX::Uninit(void)
{
	// 影を消す
	if (m_pShadow != NULL)
	{
		//m_pShadow->Uninit();
		m_pShadow = NULL;
	}

	m_nIdxTexure = NULL;

	// オブジェクトの破棄
	Release();

	m_nNumAll--;	// 総数減算
}

//==========================================================================
// 死亡処理
//==========================================================================
void CObjectX::Kill(void)
{
	// 影を消す
	if (m_pShadow != NULL)
	{
		m_pShadow->Uninit();
		m_pShadow = NULL;
	}

	// 終了処理
	Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CObjectX::Update(void)
{
#if 1
	// Xファイルのデータ取得
	CXLoad *pXLoad = CScene::GetXLoad();

	// Xファイルのデータ取得
	CXLoad::SXFile *pXData = CScene::GetXLoad()->GetObjectX(m_nIdxXFile);

	D3DXVECTOR3 pos = GetPosition();
	bool bLand = false;
	float fHeight = CScene::GetElevation()->GetHeight(pos, bLand);
	if (bLand == true)
	{
		pos.y = fHeight;
	}

	SetPosition(pos);
#endif

	// 全頂点チェック
	//CheckVtx(GetRotation().y, &pXData->vtxMax, &pXData->vtxMin, pXData->pMesh, pXData->pVtxBuff);
}

//==========================================================================
// 高さ取得
//==========================================================================
float CObjectX::GetHeight(D3DXVECTOR3 pos, bool &bLand)
{
	// Xファイルのデータ取得
	CXLoad::SXFile *pXData = CScene::GetXLoad()->GetObjectX(m_nIdxXFile);

	if (pXData == NULL)
	{// NULLだったら
		return 0.0f;
	}

	// 最大の高さ
	float fHeightMax = 0.0f;

	// 自分の位置
	D3DXVECTOR3 MyPosition = GetPosition();

	WORD* pIndexBuff;

	// インデックスバッファをロック
	pXData->pMesh->LockIndexBuffer(D3DLOCK_READONLY, (void**)&pIndexBuff);

	for (int nCntTri = 0; nCntTri < pXData->nFaceNum; nCntTri++) 
	{
		
		int nIdx1 = (int)pIndexBuff[nCntTri * 3];
		int nIdx2 = (int)pIndexBuff[nCntTri * 3 + 1];
		int nIdx3 = (int)pIndexBuff[nCntTri * 3 + 2];

		// 頂点位置代入
		D3DXVECTOR3 pos1 = pXData->pVtxPos[nIdx1] + MyPosition;
		D3DXVECTOR3 pos2 = pXData->pVtxPos[nIdx2] + MyPosition;
		D3DXVECTOR3 pos3 = pXData->pVtxPos[nIdx3] + MyPosition;

		// 絶対値(マイナスいかない)と最大の距離比較
		if (fabsf(pos1.x - pos.x) > pXData->fMaxVtxDistance ||
			fabsf(pos1.z - pos.z) > pXData->fMaxVtxDistance)
		{// 頂点距離の方が長い
			continue;
		}

		// 三角で高さを求める
		
		bool bNowLand = false;
		float fNowHeight = GetVtxHeight(pos, pos1, pos3, pos2, bNowLand);

		if (bNowLand == true && fNowHeight > fHeightMax)
		{// 着地してたら

			// 最大高さ更新
			fHeightMax = fNowHeight;
			bLand = true;
		}
	}

	// インデックスバッファをアンロック
	pXData->pMesh->UnlockIndexBuffer();

	return fHeightMax;
}

//==========================================================================
// 描画処理
//==========================================================================
void CObjectX::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	D3DXMATRIX mtxRot, mtxTrans, mtxScale;	// 計算用マトリックス宣言
	D3DMATERIAL9 matDef;			// 現在のマテリアル保存用
	D3DXMATERIAL *pMat;				// マテリアルデータへのポインタ

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// スケールを反映する
	D3DXMatrixScaling(&mtxScale, m_scale.x, m_scale.y, m_scale.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxScale);

	// 向きを反映する
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映する
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	// Xファイルのデータ取得
	CXLoad::SXFile *pXData = CScene::GetXLoad()->GetObjectX(m_nIdxXFile);

	// マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)pXData->pBuffMat->GetBufferPointer();

	// 頂点数分繰り返し
	for (int nCntMat = 0; nCntMat < (int)pXData->dwNumMat; nCntMat++)
	{
		// マテリアルの設定
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		// テクスチャの設定
		pDevice->SetTexture(0, CManager::GetInstance()->GetTexture()->GetAdress(pXData->nIdxTexture[nCntMat]));

		if (m_scale != D3DXVECTOR3(1.0f, 1.0f, 1.0f))
		{// 少しでも違う場合

			// 自動正規化をONにする
			pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
		}

		// パーツの描画
		pXData->pMesh->DrawSubset(nCntMat);

		if (m_scale != D3DXVECTOR3(1.0f, 1.0f, 1.0f))
		{// 少しでも違う場合

			// 自動正規化をデフォルトにする
			pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
		}
	}

	// 保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);
}


//==========================================================================
// 描画処理
//==========================================================================
void CObjectX::Draw(D3DXCOLOR col)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	D3DXMATRIX mtxRot, mtxTrans, mtxScale;	// 計算用マトリックス宣言
	D3DMATERIAL9 matDef;			// 現在のマテリアル保存用
	D3DXMATERIAL *pMat;				// マテリアルデータへのポインタ

	D3DXMATERIAL matNow;			// 今回のマテリアル

	// 他の情報クリア
	ZeroMemory(&matNow, sizeof(D3DXMATERIAL));
	matNow.MatD3D.Diffuse = col;
	matNow.MatD3D.Ambient = col;

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// スケールを反映する
	D3DXMatrixScaling(&mtxScale, m_scale.x, m_scale.y, m_scale.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxScale);

	// 向きを反映する
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映する
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	// Xファイルのデータ取得
	CXLoad::SXFile *pXData = CScene::GetXLoad()->GetObjectX(m_nIdxXFile);

	// マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)pXData->pBuffMat->GetBufferPointer();

	// 頂点数分繰り返し
	for (int nCntMat = 0; nCntMat < (int)pXData->dwNumMat; nCntMat++)
	{
		// マテリアルの設定
		pDevice->SetMaterial(&matNow.MatD3D);

		// テクスチャの設定
		pDevice->SetTexture(0, CManager::GetInstance()->GetTexture()->GetAdress(pXData->nIdxTexture[nCntMat]));

		if (m_scale != D3DXVECTOR3(1.0f, 1.0f, 1.0f))
		{// 少しでも違う場合

			// 自動正規化をONにする
			pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
		}

		// パーツの描画
		pXData->pMesh->DrawSubset(nCntMat);

		if (m_scale != D3DXVECTOR3(1.0f, 1.0f, 1.0f))
		{// 少しでも違う場合

			// 自動正規化をデフォルトにする
			pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
		}
	}

	// 保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);
}


//==========================================================================
// 描画処理
//==========================================================================
void CObjectX::Draw(float fAlpha)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	D3DXMATRIX mtxRot, mtxTrans, mtxScale;	// 計算用マトリックス宣言
	D3DMATERIAL9 matDef;			// 現在のマテリアル保存用
	D3DXMATERIAL *pMat;				// マテリアルデータへのポインタ

	D3DXMATERIAL matNow;			// 今回のマテリアル

	// 他の情報クリア
	ZeroMemory(&matNow, sizeof(D3DXMATERIAL));

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// スケールを反映する
	D3DXMatrixScaling(&mtxScale, m_scale.x, m_scale.y, m_scale.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxScale);

	// 向きを反映する
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映する
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	// Xファイルのデータ取得
	CXLoad::SXFile *pXData = CScene::GetXLoad()->GetObjectX(m_nIdxXFile);

	// マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)pXData->pBuffMat->GetBufferPointer();

	// 頂点数分繰り返し
	for (int nCntMat = 0; nCntMat < (int)pXData->dwNumMat; nCntMat++)
	{
		// 不透明度設定
		matNow.MatD3D.Diffuse = D3DXCOLOR(pMat[nCntMat].MatD3D.Diffuse.r, pMat[nCntMat].MatD3D.Diffuse.g, pMat[nCntMat].MatD3D.Diffuse.b, fAlpha);
		matNow.MatD3D.Ambient = D3DXCOLOR(pMat[nCntMat].MatD3D.Ambient.r, pMat[nCntMat].MatD3D.Ambient.g, pMat[nCntMat].MatD3D.Ambient.b, fAlpha);
		matNow.MatD3D.Emissive = pMat[nCntMat].MatD3D.Emissive;
		matNow.MatD3D.Power = pMat[nCntMat].MatD3D.Power;
		matNow.MatD3D.Specular = pMat[nCntMat].MatD3D.Specular;

		//マテリアルの設定
		pDevice->SetMaterial(&matNow.MatD3D);

		// テクスチャの設定
		pDevice->SetTexture(0, CManager::GetInstance()->GetTexture()->GetAdress(pXData->nIdxTexture[nCntMat]));

		if (m_scale != D3DXVECTOR3(1.0f, 1.0f, 1.0f))
		{// 少しでも違う場合

			// 自動正規化をONにする
			pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
		}

		// パーツの描画
		pXData->pMesh->DrawSubset(nCntMat);

		if (m_scale != D3DXVECTOR3(1.0f, 1.0f, 1.0f))
		{// 少しでも違う場合

			// 自動正規化をデフォルトにする
			pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
		}
	}

	// 保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);
}

//==========================================================================
// マトリックス設定
//==========================================================================
void CObjectX::SetWorldMtx(const D3DXMATRIX mtx)
{
	m_mtxWorld = mtx;
}

//==========================================================================
// マトリックス取得
//==========================================================================
D3DXMATRIX CObjectX::GetWorldMtx(void) const
{
	return m_mtxWorld;
}

//==========================================================================
// 位置設定
//==========================================================================
void CObjectX::SetPosition(const D3DXVECTOR3 pos)
{
	m_pos = pos;
}

//==========================================================================
// 位置取得
//==========================================================================
D3DXVECTOR3 CObjectX::GetPosition(void) const
{
	return m_pos;
}

//==========================================================================
// 位置設定
//==========================================================================
void CObjectX::SetOldPosition(const D3DXVECTOR3 posOld)
{
	m_posOld = posOld;
}

//==========================================================================
// 位置取得
//==========================================================================
D3DXVECTOR3 CObjectX::GetOldPosition(void) const
{
	return m_posOld;
}

//==========================================================================
// 移動量設定
//==========================================================================
void CObjectX::SetMove(const D3DXVECTOR3 move)
{
	m_move = move;
}

//==========================================================================
// 移動量取得
//==========================================================================
D3DXVECTOR3 CObjectX::GetMove(void) const
{
	return m_move;
}

//==========================================================================
// スケール設定
//==========================================================================
void CObjectX::SetScale(const D3DXVECTOR3 scale)
{
	m_scale = scale;
}

//==========================================================================
// スケール取得
//==========================================================================
D3DXVECTOR3 CObjectX::GetScale(void) const
{
	return m_scale;
}

//==========================================================================
// 向き設定
//==========================================================================
void CObjectX::SetRotation(const D3DXVECTOR3 rot)
{
	m_rot = rot;
}

//==========================================================================
// 向き取得
//==========================================================================
D3DXVECTOR3 CObjectX::GetRotation(void) const
{
	return m_rot;
}

//==========================================================================
// 色設定
//==========================================================================
void CObjectX::SetColor(const D3DXCOLOR col)
{
	m_col = col;
}

//==========================================================================
// 色取得
//==========================================================================
D3DXCOLOR CObjectX::GetColor(void) const
{
	return m_col;
}

//==========================================================================
// サイズ設定
//==========================================================================
void CObjectX::SetSize(const D3DXVECTOR3 size)
{
	m_fSize = size;
}

//==========================================================================
// サイズ取得
//==========================================================================
D3DXVECTOR3 CObjectX::GetSize(void) const
{
	return m_fSize;
}

//==========================================================================
// 頂点の最大値取得
//==========================================================================
D3DXVECTOR3 CObjectX::GetVtxMax(void) const
{
	// Xファイルのデータ取得
	return CScene::GetXLoad()->GetObjectX(m_nIdxXFile)->vtxMax;
}

//==========================================================================
// 頂点の最小値取得
//==========================================================================
D3DXVECTOR3 CObjectX::GetVtxMin(void) const
{
	// Xファイルのデータ取得
	return CScene::GetXLoad()->GetObjectX(m_nIdxXFile)->vtxMin;
}

//==========================================================================
// Xファイルのインデックス取得
//==========================================================================
int CObjectX::GetIdxXFile(void) const
{
	return m_nIdxXFile;
}

//==========================================================================
// 影を使っているかどうか
//==========================================================================
bool CObjectX::GetUseShadow(void) const
{
	return m_bShadow;
}

//==========================================================================
// オブジェクトXオブジェクトの取得
//==========================================================================
CObjectX *CObjectX::GetObjectX(void)
{
	return this;
}

//==========================================================================
// 総数取得
//==========================================================================
int CObjectX::GetNumAll(void)
{
	return m_nNumAll;
}
