//=============================================================================
// 
//  オブジェクト3Dヘッダー [object3DMesh.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _OBJECT3DMESH_H_
#define _OBJECT3DMESH_H_	// 二重インクルード防止

#include "main.h"
#include "object3D.h"

//==========================================================================
// クラス定義
//==========================================================================
// オブジェクト3Dクラス定義
class CObject3DMesh : public CObject3D
{
public:

	typedef enum
	{
		TYPE_FIELD = 0,	// 地面
		TYPE_WALL,		// 壁
		TYPE_CYLINDER,	// シリンダー
		TYPE_DOME,		// ドーム
		TYPE_DONUTS,	// ドーナツ
		TYPE_SPHERE,	// 球
		TYPE_MAX
	}TYPE;

	CObject3DMesh(int nPriority = mylib_const::DEFAULT_PRIORITY);
	~CObject3DMesh();

	// オーバーライドされた関数
	HRESULT Init(void);
	HRESULT Init(TYPE type);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtx(void);
	void BindTexture(int nIdx);

	int GetNumIndex(void);		// インデックス数
	void SetNumIndex(int nWidth, int nHeight);
	int GetNumVertex(void);		// 頂点数
	void SetNumVertex(int nWidth, int nHeight);
	D3DXVECTOR3 *GetVtxPos(void);		// 頂点座標取得
	void SetVtxPos(D3DXVECTOR3 *pos);	// 頂点座標取得
	D3DXVECTOR3 *GetVtxNor(void);		// 頂点ベクトル取得
	void SetVtxNor(D3DXVECTOR3 *nor);	// 頂点ベクトル
	D3DXCOLOR *GetVtxCol(void);			// 頂点カラー取得
	void SetVtxCol(D3DXCOLOR *col);		// 頂点カラー設定
	D3DXVECTOR2 *GetVtxTex(void);		// 頂点テクスチャ取得
	void SetVtxTex(D3DXVECTOR2 *tex);	// 頂点テクスチャ設定
	int GetWidthBlock(void);			// 横分割数
	void SetWidthBlock(int nWidth);		// 横分割数
	int GetHeightBlock(void);			// 縦分割数
	void SetHeightBlock(int nHeight);	// 縦分割数
	float GetWidthLen(void);			// 横の長さ
	void SetWidthLen(float fLen);		// 横の長さ
	float GetHeightLen(void);			// 縦の長さ
	void SetHeightLen(float fLen);		// 縦の長さ
	int GetIdxTex(void);				// テクスチャのインデックス番号取得

	static CObject3DMesh *Create();
	static CObject3DMesh *Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nPriority = mylib_const::DEFAULT_PRIORITY);
	static CObject3DMesh *Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fWidthLen, float fHeightLen, int nWidth, int nHeight, TYPE type, const char *pFileName, int nPriority = mylib_const::DEFAULT_PRIORITY);
	CObject3DMesh *GetObject3DMesh(void);
protected:

	LPDIRECT3DVERTEXBUFFER9 GetVtxBuff(void);
private:
	HRESULT CreateVertex(void);
	HRESULT CreateIndex(void);
	void SetInitVtxField(void);
	void SetVtxField(void);
	void SetVtxWall(void);
	void SetVtxCylinder(void);
	void SetVtxDome(void);
	void SetVtxDonuts(void);
	void SetVtxSphere(void);
	void UPVtxField(D3DXVECTOR3 pos);

	D3DXVECTOR3 *m_pVtxPos;	// 頂点座標
	D3DXVECTOR3 *m_pVtxNor;	// 頂点法線
	D3DXCOLOR *m_pVtxCol;	// 頂点カラー
	D3DXVECTOR2 *m_pVtxTex;	// 頂点テクスチャ座標
	int m_nNumIndex;		// インデックス数
	int m_nNumVertex;		// 頂点数
	int m_nWidth;			// 横分割数
	int m_nHeight;			// 縦分割数
	float m_fWidthLen;		// 横の長さ
	float m_fHeightLen;		// 縦の長さ
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;	// 頂点バッファのポインタ
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuff;	// インデックスバッファへのポインタ
	int m_nTexIdx;						// テクスチャのインデックス番号
	TYPE m_type;						// メッシュのタイプ
};



#endif