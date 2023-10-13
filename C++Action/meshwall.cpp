//=============================================================================
// 
//  メッシュウォール処理 [meshwall.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "meshwall.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"

//==========================================================================
// マクロ定義
//==========================================================================

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
const char *CMeshWall::m_apFilename[] =		//ファイル読み込み
{
	"data\\TEXTURE\\glass.jpg",
	"data\\TEXTURE\\FIELD\\senter.png",
};

//==========================================================================
// コンストラクタ
//==========================================================================
CMeshWall::CMeshWall(int nPriority) : CObject3DMesh(nPriority)
{
	m_type = TYPE_NONE;	// 種類
	m_nTexIdx = 0;			// テクスチャのインデックス番号
}

//==========================================================================
// デストラクタ
//==========================================================================
CMeshWall::~CMeshWall()
{

}

//==========================================================================
// 生成処理(オーバーロード)
//==========================================================================
CMeshWall *CMeshWall::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fWidthLen, float fHeightLen, int nWidth, int nHeight, TYPE type, const char *aFileName)
{
	// 生成用のオブジェクト
	CMeshWall *pObjMeshField = NULL;

	if (pObjMeshField == NULL)
	{// NULLだったら

		// メモリの確保
		switch (type)
		{
		case TYPE_GRASS:
			pObjMeshField = DEBUG_NEW CMeshWall(7);
			break;

		default:
			pObjMeshField = DEBUG_NEW CMeshWall;
			break;
		}

		if (pObjMeshField != NULL)
		{// メモリの確保が出来ていたら

			// 位置・向き
			pObjMeshField->SetPosition(pos);
			//pObjMeshField->SetRotation(rot);
			pObjMeshField->SetOriginRotation(rot);
			pObjMeshField->SetWidthBlock(nWidth);
			pObjMeshField->SetHeightBlock(nHeight);
			pObjMeshField->SetWidthLen(fWidthLen);
			pObjMeshField->SetHeightLen(fHeightLen);
			pObjMeshField->m_type = type;	// 種類

			if (aFileName == NULL)
			{// NULLだったら

				// テクスチャの割り当て
				pObjMeshField->m_nTexIdx = CManager::GetTexture()->Regist(m_apFilename[pObjMeshField->m_type]);
			}
			else
			{// ファイル名が入っていたら

				// テクスチャの割り当て
				pObjMeshField->m_nTexIdx = CManager::GetTexture()->Regist(aFileName);
			}

			// テクスチャの割り当て
			pObjMeshField->BindTexture(pObjMeshField->m_nTexIdx);

			// 初期化処理
			pObjMeshField->Init();
		}

		return pObjMeshField;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CMeshWall::Init(void)
{
	HRESULT hr;

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// 種類設定
	SetType(TYPE_MESHWALL);

	// オブジェクト3Dメッシュの初期化処理
	hr = CObject3DMesh::Init(CObject3DMesh::TYPE_WALL);

	if (FAILED(hr))
	{// 失敗していたら
		return E_FAIL;
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CMeshWall::Uninit(void)
{
	// 終了処理
	CObject3DMesh::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CMeshWall::Update(void)
{
	// 頂点情報設定
	SetVtx();
}

//==========================================================================
// 描画処理
//==========================================================================
void CMeshWall::Draw(void)
{
	//  デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	if (CManager::IsWireframe() == true)
	{
		pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);	// ワイヤーフレームモード
	}

	// 描画処理
	CObject3DMesh::Draw();

	pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);	// 埋めるモード
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CMeshWall::SetVtx(void)
{
	// 頂点情報更新
	CObject3DMesh::SetVtx();
}

//==========================================================================
// メッシュフィールドオブジェクトの取得
//==========================================================================
CMeshWall *CMeshWall::GetObject3DMesh(void)
{
	return this;
}
