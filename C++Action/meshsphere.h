//=============================================================================
// 
//  爆発球ヘッダー [meshsphere.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _EXPLOSION_SPHERE_H_
#define _EXPLOSION_SPHERE_H_	// 二重インクルード防止

#include "main.h"
#include "object3DMesh.h"

//==========================================================================
// クラス定義
//==========================================================================
// メッシュスフィアクラス定義
class CMeshSphere : public CObject3DMesh
{
public:
	CMeshSphere(int nPriority = mylib_const::DEF2D_PRIORITY);
	~CMeshSphere();

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void SetSizeDest(float fSize);	// 目標のサイズ設定
	static CMeshSphere *Create(D3DXVECTOR3 pos, float fSize, const char *pFileName, int nPriority = mylib_const::DEF2D_PRIORITY);	// 生成処理
	static CMeshSphere *Create(D3DXVECTOR3 pos, float fSize, int nTexIdx, int nPriority = mylib_const::DEF2D_PRIORITY);	// 生成処理

private:

	// メンバ変数
	int m_nTexIdx;		// テクスチャのインデックス番号
	float m_fSizeDest;	// 目標のサイズ

};



#endif