//=============================================================================
// 
//  目標地点ヘッダー [targetpoint.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _TARGETPOINT_H_
#define _TARGETPOINT_H_	// 二重インクルード防止

#include "main.h"
#include "object3DMesh.h"

//==========================================================================
// クラス定義
//==========================================================================
// 目標地点クラス定義
class CTargetPoint : public CObject3DMesh
{
public:
	CTargetPoint(int nPriority = 6);
	~CTargetPoint();

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtx(void);

	void Control(void);	// 操作
	static CTargetPoint *Create(D3DXVECTOR3 pos, float fWidthLen, float fHeightLen);
	CTargetPoint *GetObject3DMesh(void);

private:

	int m_nTexIdx;	// テクスチャのインデックス番号
	int m_nAlphaCnt;	// 不透明度のカウント
};



#endif