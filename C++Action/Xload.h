//=============================================================================
// 
//  Xファイルヘッダー [Xload.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _XLOAD_H_
#define _XLOAD_H_	// 二重インクルード防止

#include "main.h"
#include "object.h"

class CObjectX;

//==========================================================================
// クラス定義
//==========================================================================
// Xファイルクラス定義
class CXLoad
{
public:

	// マクロ定義
#define MAX_MAT				(512)		// マテリアルサイズ

	struct SXFile
	{
		D3DXVECTOR3 vtxMin;						// モデルの最小値
		D3DXVECTOR3	vtxMax;						// モデルの最大値
		BYTE *pVtxBuff;							// 頂点バッファのポインタ
		LPD3DXMESH pMesh;						// メッシュ(頂点情報)へのポインタ
		LPD3DXBUFFER pBuffMat;					// マテリアルへのポインタ
		DWORD dwNumMat;							// マテリアルの数
		D3DXMATERIAL pMatData[MAX_MAT];			// マテリアルのデータ
		int *nIdxTexture;						// テクスチャのインデックス番号
		char acFilename[mylib_const::MAX_STRING];				// ファイル名
		int nFileNameLen;						// ファイル名の文字数
	};

	CXLoad();
	~CXLoad();

	HRESULT Init(void);
	void Uninit(void);
	int XLoad(const char *pFileName);

	SXFile *GetObjectX(int nIdx);
	static int GetNumAll(void);		// 総数
protected:

private:
	static HRESULT Load(const char *pFileName);
	static void Unload(void);

	static int m_nNumAll;				// 読み込んだ総数
	static SXFile m_pXFile[mylib_const::MAX_OBJ];	// Xファイルの情報
};



#endif