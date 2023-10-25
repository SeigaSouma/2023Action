//=============================================================================
//
// ライト処理 [light.h]
// Author : 相馬靜雅
//
//=============================================================================
#ifndef _LIGHT_H_		// このマクロ定義がされていなかったら
#define _LIGHT_H_		// 二重インクルード防止のマクロを定義する

#include "main.h"

//==========================================================================
// クラス定義
//==========================================================================
// ライトクラス定義
class CLight
{
public:

	CLight();
	~CLight();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);

	void UpdateSpotLightDirection(D3DXVECTOR3 vec);	// スポットライトの向き更新
private:

	enum TYPE
	{
		TYPE_DIRECTIONAL_01 = 0,
		TYPE_DIRECTIONAL_02,
		TYPE_DIRECTIONAL_03,
		TYPE_SPOT_01,
		TYPE_MAX,
	};
	
	D3DLIGHT9 m_aLight[TYPE_MAX];		// ライト構造体
};

#endif