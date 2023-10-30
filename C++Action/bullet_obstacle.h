//=============================================================================
// 
//  障害物弾ヘッダー [bullet_obstacle.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _BULLET_OBSTACLE_H_
#define _BULLET_OBSTACLE_H_	// 二重インクルード防止

#include "main.h"
#include "objectX.h"

//==========================================================================
// クラス定義
//==========================================================================
// 障害物弾クラス定義
class CBulletObstacle : public CObjectX
{
public:

	CBulletObstacle(int nPriority = 6);
	~CBulletObstacle();

	// オーバーライドされた関数
	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);

	static CBulletObstacle *Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR3 move, const float fSize);

private:
	void UpdatePos(void);		// 移動
	void CollisionPlayer(void);		// プレイヤーとの判定

	int m_nLife;			// 寿命
	int m_nLifeMax;			// 寿命の最大値
	int m_nTexIdx;			// テクスチャのインデックス番号
	int m_nCntEmission;	// 発生物のカウンター
	float m_fRadius;		// 半径

};


#endif