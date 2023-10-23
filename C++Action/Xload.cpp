//=============================================================================
// 
//  Xファイル処理 [Xload.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "Xload.h"
#include "texture.h"
#include "manager.h"
#include "renderer.h"
#include "objectX.h"
#include "calculation.h"

// Xファイルオブジェクトを読み込むファイル
#include "map.h"
#include "ballast.h"

//==========================================================================
// マクロ定義
//==========================================================================

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
CXLoad::SXFile CXLoad::m_pXFile[mylib_const::MAX_OBJ] = {};	// Xファイルの情報
int CXLoad::m_nNumAll = 0;				// 読み込んだ総数

//==========================================================================
// コンストラクタ
//==========================================================================
CXLoad::CXLoad()
{
	for (int nCntData = 0; nCntData < mylib_const::MAX_OBJ; nCntData++)
	{
		m_pXFile[nCntData].vtxMin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// モデルの最小値
		m_pXFile[nCntData].vtxMax = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// モデルの最大値
		m_pXFile[nCntData].pVtxBuff = NULL;								// 頂点バッファのポインタ
		m_pXFile[nCntData].pMesh = NULL;								// メッシュ(頂点情報)へのポインタ
		m_pXFile[nCntData].pBuffMat = NULL;								// マテリアルへのポインタ
		m_pXFile[nCntData].dwNumMat = 0;								// マテリアルの数
		m_pXFile[nCntData].nIdxTexture = NULL;							// テクスチャのインデックス番号
		m_pXFile[nCntData].nFileNameLen = 0;							// ファイル名の文字数

		for (int nCntMat = 0; nCntMat < MAX_MAT; nCntMat++)
		{
			// マテリアルのデータ
			ZeroMemory(&m_pXFile[nCntData].pMatData[nCntMat], sizeof(D3DXMATERIAL));
		}

		for (int nCntFile = 0; nCntFile < mylib_const::MAX_STRING; nCntFile++)
		{
			m_pXFile[nCntData].acFilename[nCntFile] = NULL;	// ファイル名
		}
	}
}

//==========================================================================
// デストラクタ
//==========================================================================
CXLoad::~CXLoad()
{

}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CXLoad::Init(void)
{
	// モデルの読み込み
	if (FAILED(map::ReadXFile()))
	{// 失敗した場合
		return E_FAIL;
	}

	//// 瓦礫の読み込み
	//if (FAILED(CBallast::Load()))
	//{// 失敗した場合
	//	return E_FAIL;
	//}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CXLoad::Uninit(void)
{
	for (int nCntData = 0; nCntData < mylib_const::MAX_OBJ; nCntData++)
	{
		if (m_pXFile[nCntData].nIdxTexture != NULL)
		{// NULLじゃなかったら

			// 開放処理
			delete[] m_pXFile[nCntData].nIdxTexture;
			m_pXFile[nCntData].nIdxTexture = NULL;
		}

		// メッシュの破棄
		if (m_pXFile[nCntData].pMesh != NULL)
		{
			m_pXFile[nCntData].pMesh->Release();
			m_pXFile[nCntData].pMesh = NULL;
		}

		// マテリアルの破棄
		if (m_pXFile[nCntData].pBuffMat != NULL)
		{
			m_pXFile[nCntData].pBuffMat->Release();
			m_pXFile[nCntData].pBuffMat = NULL;
		}

		// 頂点座標の破棄
		if (m_pXFile[nCntData].pVtxPos != NULL)
		{
			delete[] m_pXFile[nCntData].pVtxPos;
			m_pXFile[nCntData].pVtxPos = NULL;
		}
	}

	m_nNumAll = 0;
}

//==========================================================================
// Xファイルの破棄
//==========================================================================
void CXLoad::Unload(void)
{

}


//==========================================================================
// Xファイルの読み込み
//==========================================================================
int CXLoad::XLoad(const char *pFileName)
{
	// 最大数取得
	int nIdx = 0;
	int nNumAll = GetNumAll() + 1;
	int nNowLen = 0;

	if (pFileName != NULL)
	{
		nNowLen = strlen(pFileName);
	}

	if (pFileName != NULL)
	{// NULLじゃなかったら
		for (int nCntData = 0; nCntData < nNumAll; nCntData++)
		{
			if (m_pXFile[nCntData].nFileNameLen == nNowLen)
			{// ファイル名の長さが同じだったら

				// 既にテクスチャが読み込まれてないかの最終確認
				if (strcmp(m_pXFile[nCntData].acFilename, pFileName) == 0)
				{// ファイル名が一致している

					// インデックス番号保存
					nIdx = nCntData;
					return nIdx;
				}
			}
		}
	}

	if (pFileName != NULL)
	{// NULLじゃなかったら

		// 読み込み
		HRESULT hr = Load(pFileName);

		if (FAILED(hr))
		{// 失敗していたら
			return E_FAIL;
		}

		// インデックス番号保存
		nIdx = nNumAll - 1;
		return nIdx;
	}

	return -1;
}

//==========================================================================
// ロード処理
//==========================================================================
HRESULT CXLoad::Load(const char *pFileName)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();
	int nIdx = m_nNumAll;

	//Xファイルの読み込み
	HRESULT hr = D3DXLoadMeshFromX(pFileName,
		D3DXMESH_SYSTEMMEM,
		pDevice,
		NULL,
		&m_pXFile[nIdx].pBuffMat,
		NULL,
		&m_pXFile[nIdx].dwNumMat,
		&m_pXFile[nIdx].pMesh);

	if (FAILED(hr))
	{// 失敗していたら
		return E_FAIL;
	}

	// ファイル名と長さ保存
	strcpy(m_pXFile[nIdx].acFilename, pFileName);
	m_pXFile[nIdx].nFileNameLen = strlen(&m_pXFile[nIdx].acFilename[0]);

	if (m_pXFile[nIdx].nIdxTexture == NULL)
	{
		// テクスチャのインデックス番号
		m_pXFile[nIdx].nIdxTexture = DEBUG_NEW int[(int)m_pXFile[nIdx].dwNumMat];
	}

	if (m_pXFile[nIdx].nIdxTexture == NULL)
	{// 確保できていない場合
		return E_FAIL;
	}

	// 頂点数取得
	m_pXFile[nIdx].nVtxNum = m_pXFile[nIdx].pMesh->GetNumVertices();

	// 頂点数分でメモリ確保
	m_pXFile[nIdx].pVtxPos = DEBUG_NEW D3DXVECTOR3[m_pXFile[nIdx].nVtxNum];

	// 面の数取得
	m_pXFile[nIdx].nFaceNum = m_pXFile[nIdx].pMesh->GetNumFaces();

	D3DXMATERIAL *pMat;		// マテリアルデータへのポインタ

	// マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)m_pXFile[nIdx].pBuffMat->GetBufferPointer();

	// 頂点数分繰り返し
	for (int nCntMat = 0; nCntMat < (int)m_pXFile[nIdx].dwNumMat; nCntMat++)
	{
		m_pXFile[nIdx].nIdxTexture[nCntMat] = 0;

		if (pMat[nCntMat].pTextureFilename != NULL)
		{// ファイルが存在する

			// テクスチャの読み込み
			m_pXFile[nIdx].nIdxTexture[nCntMat] = CManager::GetInstance()->GetTexture()->Regist(pMat[nCntMat].pTextureFilename);

			if (FAILED(hr))
			{// 失敗していたら
				return E_FAIL;
			}
		}
	}

	BYTE* pVtxBuff;

	// 頂点バッファをロック
	m_pXFile[nIdx].pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

	// 頂点フォーマットのサイズを取得
	DWORD dwSizeFVF = D3DXGetFVFVertexSize(m_pXFile[nIdx].pMesh->GetFVF());

	// 頂点座標
	for (int nCntVtx = 0; nCntVtx < m_pXFile[nIdx].nVtxNum; nCntVtx++)
	{
		// 頂点座標代入
		m_pXFile[nIdx].pVtxPos[nCntVtx] = *(D3DXVECTOR3*)pVtxBuff;

		// サイズ分ポインタ移動
		pVtxBuff += dwSizeFVF;
	}

	// 頂点バッファをアンロック
	m_pXFile[nIdx].pMesh->UnlockVertexBuffer();

	// 全頂点チェック
	CheckVtx(0.0f, &m_pXFile[nIdx].vtxMax, &m_pXFile[nIdx].vtxMin, m_pXFile[nIdx].pMesh, m_pXFile[nIdx].pVtxBuff);



	// インデックスバッファをロック
	WORD* pIndexBuff;
	m_pXFile[nIdx].pMesh->LockIndexBuffer(D3DLOCK_READONLY, (void**)&pIndexBuff);

	for (int nCntIdx = 0; nCntIdx < m_pXFile[nIdx].nFaceNum; nCntIdx++)
	{
		// 三角形を形成するインデックス, 面毎だから3つずつ見る
		int nIdx1 = (int)pIndexBuff[nCntIdx * 3];
		int nIdx2 = (int)pIndexBuff[nCntIdx * 3 + 1];
		int nIdx3 = (int)pIndexBuff[nCntIdx * 3 + 2];

		// 一時代入
		D3DXVECTOR3 pos1 = m_pXFile[nIdx].pVtxPos[nIdx1];
		D3DXVECTOR3 pos2 = m_pXFile[nIdx].pVtxPos[nIdx2];
		D3DXVECTOR3 pos3 = m_pXFile[nIdx].pVtxPos[nIdx3];

		// 頂点間の最大距離
		float fVtxDistance = 0.0f;

		// 2点の距離
		fVtxDistance = sqrtf(
			((pos1.x - pos2.x) * (pos1.x - pos2.x)) +
			((pos1.z - pos2.z) * (pos1.z - pos2.z)));

		if (fVtxDistance > m_pXFile[nIdx].fMaxVtxDistance)
		{
			// 最大距離保存
			m_pXFile[nIdx].fMaxVtxDistance = fVtxDistance;
		}

		// 2点の距離
		fVtxDistance = sqrtf(
			((pos2.x - pos3.x) * (pos2.x - pos3.x)) +
			((pos2.z - pos3.z) * (pos2.z - pos3.z)));
		if (fVtxDistance > m_pXFile[nIdx].fMaxVtxDistance)
		{
			// 最大距離保存
			m_pXFile[nIdx].fMaxVtxDistance = fVtxDistance;
		}

		// 2点の距離
		fVtxDistance = sqrtf(
			((pos3.x - pos1.x) * (pos3.x - pos1.x)) +
			((pos3.z - pos1.z) * (pos3.z - pos1.z)));
		if (fVtxDistance > m_pXFile[nIdx].fMaxVtxDistance)
		{
			// 最大距離保存
			m_pXFile[nIdx].fMaxVtxDistance = fVtxDistance;
		}
	}

	// インデックスバッファをアンロック
	m_pXFile[nIdx].pMesh->UnlockIndexBuffer();


	// 総数加算
	m_nNumAll++;

	return S_OK;
}

//==========================================================================
// Xファイルのデータ取得
//==========================================================================
CXLoad::SXFile *CXLoad::GetObjectX(int nIdx)
{
	return &m_pXFile[nIdx];
}

//==========================================================================
// Xファイルの総数取得
//==========================================================================
int CXLoad::GetNumAll(void)
{
	return m_nNumAll;
}