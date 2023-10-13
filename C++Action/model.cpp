//=============================================================================
// 
//  モデル処理 [model.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "model.h"
#include "manager.h"
#include "renderer.h"
#include "object.h"
#include "calculation.h"
#include "texture.h"
#include "Xload.h"

//==========================================================================
// マクロ定義
//==========================================================================

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
int CModel::m_nNumAll = 0;	// 総数

//==========================================================================
// コンストラクタ
//==========================================================================
CModel::CModel(int nPriority)
{
	D3DXMatrixIdentity(&m_mtxWorld);				// ワールドマトリックス
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);			// 位置
	m_posOld = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 前回の位置
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);			// 向き
	m_rotOrigin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 向き
	m_nIdxXFile = 0;								// Xファイルのインデックス番号
	m_pParent = NULL;								// 親モデルのポインタ
	m_nNumAll++;									// 総数加算
}

//==========================================================================
// デストラクタ
//==========================================================================
CModel::~CModel()
{

}

//==========================================================================
// Xファイルのデータ割り当て
//==========================================================================
void CModel::BindXData(int nIdxXFile)
{
	// 情報割り当て
	m_nIdxXFile = nIdxXFile;
}

//==========================================================================
// 親のマトリックス設定
//==========================================================================
void CModel::SetParent(CModel *pModel)
{
	// 親のポインタを渡す
	m_pParent = pModel;
}

//==========================================================================
// 生成処理
//==========================================================================
CModel *CModel::Create(const char *pFileName, D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	// 生成用のオブジェクト
	CModel *pModel = NULL;

	if (pModel == NULL)
	{// NULLだったら

		// メモリの確保
		pModel = DEBUG_NEW CModel;

		if (pModel == NULL)
		{// NULLだったら

			delete pModel;
			pModel = NULL;
			return NULL;
		}

		if (pModel != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			HRESULT hr = pModel->Init(pFileName);

			if (FAILED(hr))
			{// 失敗していたら
				return NULL;
			}

			// 位置・向き
			pModel->SetPosition(pos);
			pModel->SetRotation(rot);
			pModel->SetOriginRotation(rot);
		}

		return pModel;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CModel::Init(const char *pFileName)
{
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
// 終了処理
//==========================================================================
void CModel::Uninit(void)
{
	//// 親モデルのポインタ
	//if (m_pParent != NULL)
	//{
	//	m_pParent = NULL;
	//}
}

//==========================================================================
// 更新処理
//==========================================================================
void CModel::Update(void)
{
	// Xファイルのデータ取得
	CXLoad *pXLoad = CScene::GetXLoad();

	// Xファイルのデータ取得
	CXLoad::SXFile *pXData = CScene::GetXLoad()->GetObjectX(m_nIdxXFile);

	// 全頂点チェック
	CheckVtx(GetRotation().y, &pXData->vtxMax, &pXData->vtxMin, pXData->pMesh, pXData->pVtxBuff);
}

//==========================================================================
// 描画処理
//==========================================================================
void CModel::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス宣言
	D3DXMATRIX mtxRotOrigin;		// 計算用マトリックス宣言
	D3DMATERIAL9 matDef;			// 現在のマテリアル保存用
	D3DXMATERIAL *pMat;				// マテリアルデータへのポインタ
	D3DXMATRIX mtxnParent;			// 親の計算用マトリックス

	// 親マトリックスの初期化
	D3DXMatrixIdentity(&mtxRotOrigin);
	D3DXMatrixIdentity(&mtxnParent);

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

	// 親のマトリックスと掛け合わせる
	if (m_pParent == NULL)
	{// 自分が親だった場合

		// 最新のマトリックスを渡す(DirectXが覚えているもの)
		pDevice->GetTransform(D3DTS_WORLD, &mtxnParent);
	}
	else
	{// 親がいる場合

		// 親のマトリックスを渡す
		mtxnParent = m_pParent->GetWorldMtx();
	}

	// 自分に親のワールドマトリックスを掛ける
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxnParent);

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
		pDevice->SetTexture(0, CManager::GetTexture()->GetAdress(pXData->nIdxTexture[nCntMat]));

		// パーツの描画
		pXData->pMesh->DrawSubset(nCntMat);
	}

	// 保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);
}

//==========================================================================
// 描画処理
//==========================================================================
void CModel::Draw(D3DXCOLOR col)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス宣言
	D3DXMATRIX mtxRotOrigin;		// 計算用マトリックス宣言
	D3DMATERIAL9 matDef;			// 現在のマテリアル保存用
	D3DXMATERIAL *pMat;				// マテリアルデータへのポインタ
	D3DXMATRIX mtxnParent;			// 親の計算用マトリックス

	D3DXMATERIAL matNow;			// 今回のマテリアル

	// 他の情報クリア
	ZeroMemory(&matNow, sizeof(D3DXMATERIAL));
	matNow.MatD3D.Diffuse = col;
	matNow.MatD3D.Ambient = col;

	// 親マトリックスの初期化
	D3DXMatrixIdentity(&mtxRotOrigin);
	D3DXMatrixIdentity(&mtxnParent);

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

	// 親のマトリックスと掛け合わせる
	if (m_pParent == NULL)
	{// 自分が親だった場合

		// 最新のマトリックスを渡す(DirectXが覚えているもの)
		pDevice->GetTransform(D3DTS_WORLD, &mtxnParent);
	}
	else
	{// 親がいる場合

		// 親のマトリックスを渡す
		mtxnParent = m_pParent->GetWorldMtx();
	}

	// 自分に親のワールドマトリックスを掛ける
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxnParent);

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
		pDevice->SetTexture(0, CManager::GetTexture()->GetAdress(pXData->nIdxTexture[nCntMat]));

		// パーツの描画
		pXData->pMesh->DrawSubset(nCntMat);
	}

	// 保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);
}

//==========================================================================
// マトリックス設定
//==========================================================================
void CModel::SetWorldMtx(const D3DXMATRIX mtx)
{
	m_mtxWorld = mtx;
}

//==========================================================================
// マトリックス取得
//==========================================================================
D3DXMATRIX CModel::GetWorldMtx(void) const
{
	return m_mtxWorld;
}

//==========================================================================
// 位置設定
//==========================================================================
void CModel::SetPosition(const D3DXVECTOR3 pos)
{
	m_pos = pos;
}

//==========================================================================
// 位置取得
//==========================================================================
D3DXVECTOR3 CModel::GetPosition(void) const
{
	return m_pos;
}

//==========================================================================
// 位置設定
//==========================================================================
void CModel::SetOldPosition(const D3DXVECTOR3 posOld)
{
	m_posOld = posOld;
}

//==========================================================================
// 位置取得
//==========================================================================
D3DXVECTOR3 CModel::GetOldPosition(void) const
{
	return m_posOld;
}

//==========================================================================
// 向き設定
//==========================================================================
void CModel::SetRotation(const D3DXVECTOR3 rot)
{
	m_rot = rot;
}

//==========================================================================
// 向き取得
//==========================================================================
D3DXVECTOR3 CModel::GetRotation(void) const
{
	return m_rot;
}

//==========================================================================
// 元の向き設定
//==========================================================================
void CModel::SetOriginRotation(const D3DXVECTOR3 rot)
{
	m_rotOrigin = rot;
}

//==========================================================================
// 元の向き取得
//==========================================================================
D3DXVECTOR3 CModel::GetOriginRotation(void) const
{
	return m_rotOrigin;
}

//==========================================================================
// 頂点の最大値取得
//==========================================================================
D3DXVECTOR3 CModel::GetVtxMax(void) const
{
	// Xファイルのデータ取得
	return CScene::GetXLoad()->GetObjectX(m_nIdxXFile)->vtxMax;
}

//==========================================================================
// 頂点の最小値取得
//==========================================================================
D3DXVECTOR3 CModel::GetVtxMin(void) const
{
	// Xファイルのデータ取得
	return CScene::GetXLoad()->GetObjectX(m_nIdxXFile)->vtxMin;
}

//==========================================================================
// モデルオブジェクトの取得
//==========================================================================
CModel *CModel::GetModel(void)
{
	return this;
}

//==========================================================================
// 総数取得
//==========================================================================
int CModel::GetNumAll(void)
{
	return m_nNumAll;
}
