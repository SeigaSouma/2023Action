//=============================================================================
// 
//  メッシュドームヘッダー [meshdome.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _MESHDOME_H_
#define _MESHDOME_H_	// 二重インクルード防止

#include "main.h"
#include "object3DMesh.h"

//==========================================================================
// クラス定義
//==========================================================================
// メッシュドームクラス定義
class CMeshDome : public CObject3DMesh
{
public:
	CMeshDome(int nPriority = 2);
	~CMeshDome();

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtx(void);

	static CMeshDome *Create();
	static CMeshDome *Create(const float fMove, const char *aFileName = NULL, const int nWidth = 16, const int nHeight = 8, int nPriority = mylib_const::DEFAULT_PRIORITY);
	CMeshDome *GetObject3DMesh(void);

private:
	float m_fMove;	// 回転移動量
	int m_nTexIdx;	// テクスチャのインデックス番号
};



#endif