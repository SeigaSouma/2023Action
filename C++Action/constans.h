//=============================================================================
// 
//  定数ヘッダー [constans.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _CONSTANS_H_
#define _CONSTANS_H_	// 二重インクルード防止

#include "main.h"

//==========================================================================
// 名前空間
//==========================================================================
// 定数定義
namespace mylib_const
{
	const int MAX_OBJ = 1600;			// オブジェクトの最大数
	const int PRIORITY_NUM = 9;			// 優先順位の最大数
	const int DEFAULT_PRIORITY = 3;		// 優先順位のデフォルト
	const int DEF2D_PRIORITY = 4;		// 優先順位の2Dデフォルト
	const int MAX_STRING = 512;			// 最大文字数
	const int MAX_MODEL = 64;			// モデル最大数
	const int MAX_PARTS = 32;			// パーツ最大数
	const int MAX_PATTEN_ENEMY = 64;	// 敵の最大パターン
	const int MAX_WAVE_ENEMY = 32;		// 敵の最大パターン
	const int MAX_STAGE = 4;			// ステージの最大数
	const int MAX_BASE_ENEMY = 32;		// 敵の拠点最大数
	const int MAX_ENEMY = 256;			// 敵の最大数
	const int ENEMY_PRIORITY = 4;		// 敵の優先順位
	const int MAX_FIXEDMOVE = 32;		// 一定の動き最大数
	const int MAX_FIXEDMOVE_INFO = 16;	// 一定の動き情報最大数
	const int MAX_CAMERAAXIS = 62;		// カメラ軸の最大数
	const int MAX_BULLET = 256;			// 弾の最大数
	const float GRAVITY = 0.7f;			// 重力
	const int MAX_VTX = 51200;			// 頂点の最大数
	const float KILL_Y = -800.0f;		// 消滅する座標
	const int MAX_DIGIT = 8;			// 数字の最大桁数
	const D3DXCOLOR DEFAULT_COLOR = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// フィールドの色
	const D3DXCOLOR FIELDCOLOR_01 = D3DXCOLOR(0.6f, 0.4f, 0.1f, 1.0f);	// フィールドの色
	const D3DXVECTOR3 DEFAULT_VECTOR3 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// Vector3のデフォルト値
}

#endif