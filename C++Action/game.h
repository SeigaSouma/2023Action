//=============================================================================
// 
//  ゲームヘッダー [game.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _GAME_H_
#define _GAME_H_	// 二重インクルード防止

#include "main.h"
#include "scene.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CPlayer;
class CScore;
class CTimer;
class CPowerGauge;
class CEditControlPoint;
class CBulletManager;
class CEditCameraAxis;
class CEditCameraChaseChanger;
class CStage;

//==========================================================================
// クラス定義
//==========================================================================
// ゲームクラス定義
class CGame : public CScene
{
public:

	CGame();
	~CGame();

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void Reset(void);
	static CScore *GetScore(void);
	static CTimer *GetTimer(void);
	static CPowerGauge *GetPowerGauge(void);
	static CEditControlPoint *GetEditControlPoint(void);
	static CBulletManager *GetBulletManager(void);
	static CStage *GetStage(void);

private:
	
	static CScore *m_pScore;					// スコアのオブジェクト
	static CTimer *m_pTimer;					// タイマーのオブジェクト
	static CPowerGauge *m_pPowerGauge;			// パワーゲージのオブジェクト
	static CEditControlPoint *m_pEditControlPoint;	// 制御点エディターのオブジェクト
	static CBulletManager *m_pBulletManager;		// 弾マネージャのオブジェクト
	static CEditCameraAxis *m_pEditCameraAxis;		// カメラ軸エディターのオブジェクト
	static CEditCameraChaseChanger *m_pEditCmaeraChaseChanger;	// カメラ追従変更者エディターのオブジェクト
	static CStage *m_pStage;	// ステージのオブジェクト
};



#endif