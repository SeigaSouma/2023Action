//=============================================================================
// 
//  チュートリアルヘッダー [tutorial.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _TUTORIAL_H_
#define _TUTORIAL_H_	// 二重インクルード防止

#include "main.h"
#include "manager.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CTutorialStep;
class CTutorialText;

//==========================================================================
// クラス定義
//==========================================================================
// チュートリアルクラス定義
class CTutorial : public CScene
{
public:

	CTutorial();
	~CTutorial();

	// オーバーライドされた関数
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

private:

};



#endif