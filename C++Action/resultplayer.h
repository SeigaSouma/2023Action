//=============================================================================
// 
//  リザルトプレイヤーヘッダー [resultplayer.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _RESULTPLAYER_H_
#define _RESULTPLAYER_H_	// 二重インクルード防止

#include "main.h"
#include "player.h"

//==========================================================================
// クラス定義
//==========================================================================
// リザルトプレイヤークラス定義
class CResultPlayer : public CPlayer
{
public:

	CResultPlayer(int nPriority = mylib_const::DEF2D_PRIORITY);
	~CResultPlayer();

	// オーバーライドされた関数
	HRESULT Init(void) override;
	void Update(void) override;

	void UninitByMode(void);
private:

	// オーバーライドされた関数
	void Controll(void) override;	// 操作

	// メンバ関数
	void UpdateByStep(void);	// ステップごとの更新処理
	
};



#endif