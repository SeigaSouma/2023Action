//=============================================================================
// 
//  黒フレームヘッダー [blackframe.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _BLACKFRAME_H_
#define _BLACKFRAME_H_	// 二重インクルード防止

#include "main.h"
#include "object.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CObject2D;

//==========================================================================
// マクロ定義
//==========================================================================

//==========================================================================
// クラス定義
//==========================================================================
// 黒フレームクラス定義
class CBlackFrame
{
public:

	// 列挙型定義
	enum STATE
	{
		STATE_NONE = 0,
		STATE_IN,				// 侵入
		STATE_OUT,				// 捌ける
		STATE_INCOMPLETION,		// 侵入完了
		STATE_OUTCOMPLETION,	// 捌ける完了
		STATE_MAX
	};

	CBlackFrame(int nPriority = 8);
	~CBlackFrame();

	static CBlackFrame *Create(void);

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void SetState(STATE state);	// 状態設定
private:
	// 列挙型定義
	enum VTX
	{
		VTX_UP = 0,		// 上
		VTX_DOWN,		// 下
		VTX_MAX
	};

	// メンバ関数
	void UpdateState(int nCntVtx);	// 状態更新処理
	void StateIn(int nCntVtx);	// 侵入
	void StateOut(int nCntVtx);	// 捌け

	// メンバ変数
	STATE m_state;	// 状態
	int m_nCntMove;	// 移動カウント
	CObject2D *m_pObj2D[VTX_MAX];	// オブジェクト2Dのオブジェクト
	static const D3DXVECTOR3 m_DestPosition[VTX_MAX];	// 目標の位置
};



#endif