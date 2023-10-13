//=============================================================================
// 
//  メッシュシリンダー処理 [meshcylinder.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "meshcylinder.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define TEXTURE		"data\\TEXTURE\\field002.png"
#define POS_MESHCYLINDER	(5000.0f)
#define POS_MESHCYLINDER_Y	(3500.0f)

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CMeshCylinder::CMeshCylinder(int nPriority) : CObject3DMesh(nPriority)
{
	m_nTexIdx = 0;	// テクスチャのインデックス番号
}

//==========================================================================
// デストラクタ
//==========================================================================
CMeshCylinder::~CMeshCylinder()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CMeshCylinder *CMeshCylinder::Create(void)
{
	// 生成用のオブジェクト
	CMeshCylinder *pObjMeshCylinder = NULL;

	if (pObjMeshCylinder == NULL)
	{// NULLだったら

		// メモリの確保
		pObjMeshCylinder = DEBUG_NEW CMeshCylinder;

		//if (pObjMeshCylinder->GetID() < 0)
		//{// メモリ確保に失敗していたら

		//	delete pObjMeshCylinder;
		//	return NULL;
		//}

		if (pObjMeshCylinder != NULL)
		{// メモリの確保が出来ていたら

			// 位置・向き
			pObjMeshCylinder->SetWidthBlock(16);
			pObjMeshCylinder->SetHeightBlock(1);
			pObjMeshCylinder->SetWidthLen(POS_MESHCYLINDER);
			pObjMeshCylinder->SetHeightLen(POS_MESHCYLINDER_Y);

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
CMeshCylinder *CMeshCylinder::Create(const char *aFileName)
{
	// 生成用のオブジェクト
	CMeshCylinder *pObjMeshCylinder = NULL;

	if (pObjMeshCylinder == NULL)
	{// NULLだったら

		// メモリの確保
		pObjMeshCylinder = DEBUG_NEW CMeshCylinder;

		//if (pObjMeshCylinder->GetID() < 0)
		//{// メモリ確保に失敗していたら

		//	delete pObjMeshCylinder;
		//	return NULL;
		//}

		if (pObjMeshCylinder != NULL)
		{// メモリの確保が出来ていたら

			// 位置・向き
			pObjMeshCylinder->SetPosition(D3DXVECTOR3(0.0f, -800.0f, 0.0f));
			pObjMeshCylinder->SetWidthBlock(32);
			pObjMeshCylinder->SetHeightBlock(1);
			pObjMeshCylinder->SetWidthLen(POS_MESHCYLINDER);
			pObjMeshCylinder->SetHeightLen(POS_MESHCYLINDER_Y);

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
HRESULT CMeshCylinder::Init(void)
{
	HRESULT hr;

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// 種類設定
	SetType(TYPE_MESHCYLINDER);

	// オブジェクト3Dメッシュの初期化処理
	hr = CObject3DMesh::Init(CObject3DMesh::TYPE_CYLINDER);

	if (FAILED(hr))
	{// 失敗していたら
		return E_FAIL;
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CMeshCylinder::Uninit(void)
{
	// 終了処理
	CObject3DMesh::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CMeshCylinder::Update(void)
{
	// 頂点情報設定
	SetVtx();
}

//==========================================================================
// 描画処理
//==========================================================================
void CMeshCylinder::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//// テクスチャの繰り返しを元に戻す
	//pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
	//pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);

	// ライティングを無効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// 描画処理
	CObject3DMesh::Draw();

	// アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// ライティングを有効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	//// テクスチャの繰り返しを元に戻す
	//pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	//pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CMeshCylinder::SetVtx(void)
{
	// 頂点情報更新
	CObject3DMesh::SetVtx();
}

//==========================================================================
// メッシュシリンダーオブジェクトの取得
//==========================================================================
CMeshCylinder *CMeshCylinder::GetObject3DMesh(void)
{
	return this;
}
