//=============================================================================
// 
//  メッシュシリンダーヘッダー [meshcylinder.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _MESHCYLINDER_H_
#define _MESHCYLINDER_H_	// 二重インクルード防止

#include "main.h"
#include "object3DMesh.h"

//==========================================================================
// クラス定義
//==========================================================================
// メッシュシリンダークラス定義
class CMeshCylinder : public CObject3DMesh
{
public:
	CMeshCylinder(int nPriority = 2);
	~CMeshCylinder();

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtx(void);

	static CMeshCylinder *Create();
	static CMeshCylinder *Create(const char *aFileName = NULL);
	CMeshCylinder *GetObject3DMesh(void);

private:
	int m_nTexIdx;	// テクスチャのインデックス番号
};



#endif