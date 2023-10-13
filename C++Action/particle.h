//=============================================================================
//
// パーティクル処理 [particle.h]
// Author : 相馬靜雅
//
//=============================================================================

#ifndef _PARTICLE_H_		//このマクロ定義がされていなかったら
#define _PARTICLE_H_		//二重インクルード防止のマクロを定義する

//==========================================================================
// 名前空間
//==========================================================================
// 定数定義
namespace my_particle
{
	// 列挙型定義
	typedef enum
	{
		TYPE_NONE = 0,		// 煙のパーティクル
		TYPE_SMOKE,			// 煙のパーティクル
		TYPE_SMOKE_RED,		// 煙のパーティクル赤
		TYPE_SMOKE_YEL,		// 煙のパーティクル黄
		TYPE_EXPLOSION,		// 爆発のパーティクル
		TYPE_MANYTXT_MINI,	// 大量発生のUI(ミニ)
		TYPE_RESULTMESSAGE,	// リザルトメッセージ
		TYPE_OFFSETTING,	// 弾打ち消し
		TYPE_SUPERATTACK,	// 超攻撃
		TYPE_ICE_SPAWN,		// 氷出現
		TYPE_CHECK,			// 確認用
		TYPE_CHECK2D,			// 確認用
		TYPE_SICKLELINE,	// 大鎌のライン
		TYPE_MAGIC_EXPLOSION,	// 魔法の爆発
		TYPE_EXPLOSION_REMAIN,	// 魔法の爆発の残り
		TYPE_EXPLOSION_REMAIN_F,	// 魔法の爆発の残り,火
		TYPE_EXPLOSION_START,	// 魔法の爆発開始
		TYPE_WAVESMOKE,		// 波の煙
		TYPE_IMPACTWAVESMOKE,		// 円形波の煙
		TYPE_ENEMYSPAWN,	// 敵スポーン
		TYPE_NATURALWAVE_SPAWN,	// 自然の波
		TYPE_ENEMY_FADE,	// 敵のフェード
		TYPE_WATERIN,	// 水入った時
		TYPE_MOVESMOKE,	// 移動の煙
		TYPE_ENEMYKILL_COMBO,	// コンボ時の敵倒した
		TYPE_MAX
	}TYPE;

	void Create(D3DXVECTOR3 pos, my_particle::TYPE nType);
}

#endif