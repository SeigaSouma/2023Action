//=============================================================================
// 
//  弾爆発ヘッダー [bulletexplosion.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _BULLETEXPLOSION_H_
#define _BULLETEXPLOSION_H_	// 二重インクルード防止

#include "main.h"
#include "meshsphere.h"

class CThunderRing;
class CObjectBillboard;

//==========================================================================
// クラス定義
//==========================================================================
// 弾爆発クラス定義
class CBulletExplosion : public CMeshSphere
{
public:

	CBulletExplosion(int nPriority = 6);
	~CBulletExplosion();

	// オーバーライドされた関数
	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);
	void SetVtx(void);

	static int GetNumAll(void);
	static CBulletExplosion *Create(const D3DXVECTOR3 pos, const float fSize);

private:
	void UpdatePos(void);		// 移動
	void CollisionEnemy(void);	// 敵との判定

	int m_nLife;			// 寿命
	int m_nLifeMax;			// 寿命の最大値
	int m_nTexIdx;			// テクスチャのインデックス番号
	float m_fDestSize;		// 目標のサイズ
	D3DXVECTOR3 rotmove;	// 向きの移動量
	CThunderRing *m_pEffectThunderRing;	// 雷のリングのエフェクト
	CObjectBillboard *m_pBulletAppearance[mylib_const::BULLETAPPEARANCE_NUM];	// 見た目だけの弾
	static int m_nNumAll;	// 弾の総数
};


#endif