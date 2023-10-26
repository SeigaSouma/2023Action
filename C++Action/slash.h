//=============================================================================
// 
//  斬撃ヘッダー [slash.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _SLASH_H_
#define _SLASH_H_	// 二重インクルード防止

#include "main.h"
#include "impactwave.h"

//==========================================================================
// クラス定義
//==========================================================================
// 斬撃クラス定義
class CSlash : public CImpactWave
{
public:
	CSlash(int nPriority = 7);
	~CSlash();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CSlash *Create(D3DXVECTOR3 pos, D3DXVECTOR3 Parentrot, D3DXVECTOR3 rot, D3DXCOLOR col, float fWidth, float fCenterDistance, int nLife, float fAddSizeValue, int nTexType, bool bAddBlend, ANGLE angle);

private:

	void UpdateTex(void);
	void Collision(void);	// 当たり判定
	bool IsHit(D3DXVECTOR3 TargetPos, float fTargetRadius);	// 角度付きの判定

	int m_nCntAnim;			// アニメーションカウンター
	int m_nPatternAnim;		// アニメーションパターンNo.
	int m_nTexIdx;	// テクスチャのインデックス番号
	D3DXCOLOR m_colOrigin;	// 元の色
	float m_fSizeDest;		// 目標のサイズ
	D3DXVECTOR3 collisionRotation;	// 当たり判定の向き
	CObject3D *m_pObj3D;
};



#endif