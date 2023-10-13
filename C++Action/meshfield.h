//=============================================================================
// 
//  メッシュフィールドヘッダー [meshfield.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _MESHFIELD_H_
#define _MESHFIELD_H_	// 二重インクルード防止

#include "main.h"
#include "object3DMesh.h"

//==========================================================================
// クラス定義
//==========================================================================
// メッシュフィールドクラス定義
class CMeshField : public CObject3DMesh
{
public:
	typedef enum
	{
		TYPE_CONCREAT = 0,
		TYPE_LINE,
		TYPE_LONGLINE,
		TYPE_LINE_L,
		TYPE_LINE_R,
		TYPE_ODAN,
		TYPE_WALKLOAD_0,
		TYPE_WALKLOAD_1,
		TYPE_TILE_0,
		TYPE_TILE_1,
		TYPE_WATER,
		TYPE_MAX,
	}TYPE;

	CMeshField(int nPriority = 1);
	~CMeshField();

	// オーバーライドされた関数
	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);
	virtual void SetVtx(void);

	static CMeshField *Create();
	static CMeshField *Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fWidthLen, float fHeightLen, int nWidth = 1, int nHeight = 1, TYPE type = TYPE_CONCREAT, const char *aFileName = NULL, int nPriority = mylib_const::DEFAULT_PRIORITY);
	virtual CMeshField *GetObject3DMesh(void);
protected:

private:
	TYPE m_type;	// 種類
	static const char *m_apFilename[TYPE_MAX];		//ファイル読み込み
	int m_nTexIdx;	// テクスチャのインデックス番号
};



#endif