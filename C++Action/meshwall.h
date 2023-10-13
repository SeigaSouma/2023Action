//=============================================================================
// 
//  メッシュウォールヘッダー [meshwall.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _MESHWALL_H_
#define _MESHWALL_H_	// 二重インクルード防止

#include "main.h"
#include "object3DMesh.h"

//==========================================================================
// クラス定義
//==========================================================================
// メッシュフィールドクラス定義
class CMeshWall : public CObject3DMesh
{
public:
	typedef enum
	{
		TYPE_NONE = 0,
		TYPE_GRASS,
		TYPE_MAX,
	}TYPE;

	CMeshWall(int nPriority = 1);
	~CMeshWall();

	// オーバーライドされた関数
	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);
	virtual void SetVtx(void);

	static CMeshWall *Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fWidthLen, float fHeightLen, int nWidth = 1, int nHeight = 1, TYPE type = TYPE_NONE, const char *aFileName = NULL);
	virtual CMeshWall *GetObject3DMesh(void);
protected:

private:
	TYPE m_type;	// 種類
	static const char *m_apFilename[TYPE_MAX];		//ファイル読み込み
	int m_nTexIdx;	// テクスチャのインデックス番号
};



#endif