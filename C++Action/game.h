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
class CEditEnemyBase;
class CStage;
class CGameManager;

class CEnemyManager;
class CMapManager;
class CPlayer;
class CCameraAxis;
class CCameraChaseChanger;
class CEnemyBase;

//==========================================================================
// クラス定義
//==========================================================================
// ゲームクラス定義
class CGame : public CScene
{
public:

	// エディットタイプ
	enum EEditType
	{
		EDITTYPE_OFF = 0,			// オフ
		EDITTYPE_MAPCONTROL,		// マップ軌道
		EDITTYPE_CAMERACHASE,		// カメラ追従
		EDITTYPE_CAMERAAXIS,		// カメラ軸
		EDITTYPE_ENEMYBASE,			// 敵拠点
		EDITTYPE_MAX
	};

	CGame();
	~CGame();

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static void Reset(void);
	static EEditType GetEditType(void) { return m_EditType; }
	static CScore *GetScore(void);
	static CTimer *GetTimer(void);
	static CPowerGauge *GetPowerGauge(void);
	static CEditControlPoint *GetEditControlPoint(void);
	static CEditEnemyBase *GetEditEnemyBase(void);
	static CBulletManager *GetBulletManager(void);
	static CStage *GetStage(void);
	static CGameManager *GetGameManager(void);

	static CEnemyManager *GetEnemyManager(void);
	static CMapManager *GetMapManager(void);	// マップマネージャ取得
	static CCameraAxis *GetCameraAxis(void);	// カメラの軸取得
	static CCameraChaseChanger *GetCameraChaseChanger(void);	// カメラ追従種類の変更者取得
	static CEnemyBase *GetEnemyBase(void);		// 敵の拠点

private:

	// メンバ関数
	void EditReset(void);

	// メンバ変数
	static CScore *m_pScore;					// スコアのオブジェクト
	static CTimer *m_pTimer;					// タイマーのオブジェクト
	static CPowerGauge *m_pPowerGauge;			// パワーゲージのオブジェクト
	static CEditControlPoint *m_pEditControlPoint;	// 制御点エディターのオブジェクト
	static CBulletManager *m_pBulletManager;		// 弾マネージャのオブジェクト
	static CEditCameraAxis *m_pEditCameraAxis;		// カメラ軸エディターのオブジェクト
	static CEditCameraChaseChanger *m_pEditCmaeraChaseChanger;	// カメラ追従変更者エディターのオブジェクト
	static CEditEnemyBase *m_pEditEnemyBase;		// 敵の拠点エディター
	static CStage *m_pStage;	// ステージのオブジェクト
	static CGameManager *m_pGameManager;	// ゲームマネージャのオブジェクト
	static EEditType m_EditType;		// エディットの種類

	static CEnemyManager *m_pEnemyManager;		// 敵マネージャのオブジェクト
	static CMapManager *m_pMapManager;			// マップマネージャのオブジェクト
	static CCameraAxis *m_pCameraAxis;	// カメラの軸のオブジェクト
	static CCameraChaseChanger *m_pCameraChaseChanger;	// カメラ追従変更者のオブジェクト
	static CEnemyBase *m_pEnemyBase;	// 敵の拠点
};



#endif