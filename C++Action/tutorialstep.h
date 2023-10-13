//=============================================================================
// 
//  チュートリアルステップヘッダー [tutorialstep.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _TUTORIALSTEP_H_
#define _TUTORIALSTEP_H_	// 二重インクルード防止

//==========================================================================
// 前方宣言
//==========================================================================

//==========================================================================
// クラス定義
//==========================================================================
// チュートリアルステップクラス定義
class CTutorialStep
{
public:
	// 列挙型定義
	enum STEP
	{
		STEP_WAIT = 0,
		STEP_SPEEDUP,
		STEP_SPEEDDOWN,
		STEP_IMPACTWAVE,
		STEP_DIRECTWAVE,
		STEP_MAX
	};

	enum STEPFRAG
	{
		STEPFRAG_SPEEDUP = 0,	// 加速
		STEPFRAG_SPEEDDOWN,		// 減速
		STEPFRAG_IMPACTWAVE,	// 円形波
		STEPFRAG_DIRECTWAVE,	// 直線波
		STEPFRAG_MAX
	};

	CTutorialStep();
	~CTutorialStep();

	static CTutorialStep *Create(void);	// 生成処理

	// メンバ関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);

	void AddStep(void);	// ステップを進める
	void SetStep(STEP step);	// ステップの設定処理
	STEP GetNowStep(void);	// 現在のステップ取得
	bool IsEndStep(void);	// ステップ終了の判定取得
	void SetDisableOKSign(void);	// OKサインのフラグOFFにする
private:

	int m_nCntWait;			// 待機のカウンター
	int m_nCntSpeedUP;		// スピードアップのカウンター
	int m_nCntSpeedDOWN;	// スピードダウンのカウンター
	int m_nCntImpactWave;	// 円形波の回数
	int m_nCntDirectWave;	// 直線波の回数
	bool m_bEndStep;		// ステップの終了判定
	bool m_bSetOK;			// OKの設定判定
	STEP m_step;			// 現在のステップ
};



#endif