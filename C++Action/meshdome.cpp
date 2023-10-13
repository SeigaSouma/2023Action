//=============================================================================
// 
//  メッシュドーム処理 [meshdome.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "meshdome.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "calculation.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define TEXTURE		"data\\TEXTURE\\field002.png"
#define POS_MESHDOME	(6000.0f)
#define POS_MESHDOME_Y	(POS_MESHDOME)

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CMeshDome::CMeshDome(int nPriority) : CObject3DMesh(nPriority)
{
	m_fMove = 0.0f;	// 回転移動量
	m_nTexIdx = 0;	// テクスチャのインデックス番号
}

//==========================================================================
// デストラクタ
//==========================================================================
CMeshDome::~CMeshDome()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CMeshDome *CMeshDome::Create(void)
{
	// 生成用のオブジェクト
	CMeshDome *pObjMeshCylinder = NULL;

	if (pObjMeshCylinder == NULL)
	{// NULLだったら

		// メモリの確保
		pObjMeshCylinder = DEBUG_NEW CMeshDome;

		//if (pObjMeshCylinder->GetID() < 0)
		//{// メモリ確保に失敗していたら

		//	delete pObjMeshCylinder;
		//	return NULL;
		//}

		if (pObjMeshCylinder != NULL)
		{// メモリの確保が出来ていたら

			// 位置・向き
			pObjMeshCylinder->SetWidthBlock(32);
			pObjMeshCylinder->SetHeightBlock(8);
			pObjMeshCylinder->SetWidthLen(POS_MESHDOME);
			pObjMeshCylinder->SetHeightLen(POS_MESHDOME_Y);

			// テクスチャの割り当て
			pObjMeshCylinder->m_nTexIdx = CManager::GetTexture()->Regist(TEXTURE);

			// テクスチャの割り当て
			pObjMeshCylinder->BindTexture(pObjMeshCylinder->m_nTexIdx);

			// 初期化処理
			pObjMeshCylinder->Init();
		}

		return pObjMeshCylinder;
	}

	return NULL;
}

//==========================================================================
// 生成処理(オーバーロード)
//==========================================================================
CMeshDome *CMeshDome::Create(const float fMove, const char *aFileName, const int nWidth, const int nHeight, int nPriority)
{
	// 生成用のオブジェクト
	CMeshDome *pObjMeshCylinder = NULL;

	if (pObjMeshCylinder == NULL)
	{// NULLだったら

		// メモリの確保
		pObjMeshCylinder = DEBUG_NEW CMeshDome(nPriority);

		//if (pObjMeshCylinder->GetID() < 0)
		//{// メモリ確保に失敗していたら

		//	delete pObjMeshCylinder;
		//	return NULL;
		//}

		if (pObjMeshCylinder != NULL)
		{// メモリの確保が出来ていたら

			// 位置・向き
			pObjMeshCylinder->m_fMove = fMove;
			pObjMeshCylinder->SetWidthBlock(nWidth);
			pObjMeshCylinder->SetHeightBlock(nHeight);
			pObjMeshCylinder->SetWidthLen(POS_MESHDOME);
			pObjMeshCylinder->SetHeightLen(POS_MESHDOME_Y);

			if (aFileName == NULL)
			{// NULLだったら

				// テクスチャの割り当て
				pObjMeshCylinder->m_nTexIdx = CManager::GetTexture()->Regist(TEXTURE);
			}
			else
			{// ファイル名が入っていたら

				// テクスチャの割り当て
				pObjMeshCylinder->m_nTexIdx = CManager::GetTexture()->Regist(aFileName);
			}

			// テクスチャの割り当て
			pObjMeshCylinder->BindTexture(pObjMeshCylinder->m_nTexIdx);

			// 初期化処理
			pObjMeshCylinder->Init();
		}

		return pObjMeshCylinder;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CMeshDome::Init(void)
{
	HRESULT hr;

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// 種類設定
	SetType(TYPE_MESHDOME);

	// オブジェクト3Dメッシュの初期化処理
	hr = CObject3DMesh::Init(CObject3DMesh::TYPE_DOME);

	if (FAILED(hr))
	{// 失敗していたら
		return E_FAIL;
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CMeshDome::Uninit(void)
{
	// 終了処理
	CObject3DMesh::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CMeshDome::Update(void)
{
	D3DXVECTOR3 rot = GetRotation();	// 向き

	// 回転
	rot.y += m_fMove;

	// 角度の正規化
	RotNormalize(rot.y);

	// 向き設定
	SetRotation(rot);

	// 頂点情報設定
	SetVtx();

}

//==========================================================================
// 描画処理
//==========================================================================
void CMeshDome::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ライティングを無効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// 描画処理
	CObject3DMesh::Draw();

	// ライティングを有効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CMeshDome::SetVtx(void)
{
	// 頂点情報更新
	CObject3DMesh::SetVtx();
}

//==========================================================================
// メッシュドームオブジェクトの取得
//==========================================================================
CMeshDome *CMeshDome::GetObject3DMesh(void)
{
	return this;
}
