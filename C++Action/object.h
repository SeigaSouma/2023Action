//=============================================================================
// 
//  オブジェクトヘッダー [object.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _OBJECT_H_
#define _OBJECT_H_	// 二重インクルード防止

#include "main.h"
#include "constans.h"

class CObject2D;
class CObject3D;
class CObject3DMesh;
class CObjectX;
class CObjectChara;
class CEffect3D;

//==========================================================================
// クラス定義
//==========================================================================
// オブジェクトクラス定義
class CObject
{
public:

	// 列挙型定義
	enum TYPE
	{
		TYPE_NONE = 0,		// 無し
		TYPE_PLAYER,		// プレイヤー
		TYPE_ENEMY,			// 敵
		TYPE_BOSS,			// ボス
		TYPE_BULLET,		// 弾
		TYPE_EXPLOSION,		// 爆発
		TYPE_BG,			// 背景
		TYPE_EFFECT2D,		// 2Dエフェクト
		TYPE_EFFECT3D,		// 3Dエフェクト
		TYPE_SCORE,			// スコア
		TYPE_TIMER,			// タイマー
		TYPE_BLOCK,			// ブロック
		TYPE_ITEM,			// アイテム
		TYPE_DELUSION_WINDOW,			// 妄想のウィンドウ
		TYPE_DELUSION_METER,			// 妄想のウィンドウ
		TYPE_DELUSION_METER_MAIN,			// 妄想のウィンドウ
		TYPE_DELUSION_POINT,			// 妄想のポイント
		TYPE_FIELD,			// フィールド
		TYPE_HPGAUGE,		// HPゲージ
		TYPE_CONFUSIONEFFECT,		// 混乱エフェクト
		TYPE_MAGIC_CIRCLE,	// 魔法陣
		TYPE_CAR,			// 車
		TYPE_SHADOW,		// 影
		TYPE_MAP,			// マップ
		TYPE_XFILE,			// Xファイル
		TYPE_MODEL,			// モデル
		TYPE_EDIT,			// エディット
		TYPE_MESHFIELD,		// メッシュフィールド
		TYPE_ELEVATION,		// 起伏フィールド
		TYPE_ELEVATION_OUTSIDE,		// 起伏フィールド
		TYPE_WATERFIELD,	// 水のフィールド
		TYPE_MAGMA,		// マグマフィールド
		TYPE_MESHWALL,		// メッシュウォール
		TYPE_MESHCYLINDER,	// メッシュシリンダー
		TYPE_MESHDOME,		// メッシュドーム
		TYPE_MESHDONUTS,	// メッシュドーナツ
		TYPE_MESHSPHERE,	// メッシュスフィア
		TYPE_BALLAST,		// 瓦礫
		TYPE_MAGIC_ICE,		// 氷魔法
		TYPE_TARGETPOINT,	// 目標の地点
		TYPE_SPEED_RUSH,	// 高速ラッシュ
		TYPE_GRASSBLADES,	// 草
		TYPE_TITLESCREEN,	// タイトル画面
		TYPE_TITLESELECT,	// タイトル選択肢
		TYPE_RESULTSCREEN,	// リザルト画面
		TYPE_RESULTSELECT,	// リザルト選択肢
		TYPE_STAGESWITCH,	// ステージ切り替え
		TYPE_OKSIGN,		// OKサイン
		TYPE_FADE,			// フェード
		TYPE_NUMBER,		// 数字
	};

	enum ANGLE
	{
		ANGLE_UP = 0,	// 上
		ANGLE_RIGHT,	// 右
		ANGLE_DOWN,		// 下
		ANGLE_LEFT,		// 左
		ANGLE_MAX
	};

	CObject(int nPriority = mylib_const::DEFAULT_PRIORITY);
	virtual ~CObject();

	// 派生クラスでオーバーライドされた関数が絶対必要(純粋仮想関数)
	virtual HRESULT Init(void) = 0;
	virtual void Uninit(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;

	virtual void SetVtx(void);
	virtual void SetPosition(const D3DXVECTOR3 pos);	// 位置設定
	virtual D3DXVECTOR3 GetPosition(void) const;		// 位置取得
	virtual void SetOldPosition(const D3DXVECTOR3 posOld);	// 過去の位置設定
	virtual D3DXVECTOR3 GetOldPosition(void) const;		// 過去の位置取得
	virtual void SetMove(const D3DXVECTOR3 rot);	// 移動量設定
	virtual D3DXVECTOR3 GetMove(void) const;		// 移動量取得
	virtual void SetRotation(const D3DXVECTOR3 rot);	// 向き設定
	virtual D3DXVECTOR3 GetRotation(void) const;		// 向き取得
	virtual bool Hit(const int nValue);					// 嘆きのバーチャルヒット処理
	virtual bool Hit(const int nValue, D3DXVECTOR3 move);

	void SetMapIndex(int nIdx);	// マップインデックス番号の設定
	int GetMapIndex(void);		// マップインデックス番号の取得
	void SetMapPointRatio(float fRatio);	// マップポイント間の割合設定
	float GetMapPointRatio(void);			// マップポイント間の割合設定
	void SetMapMoveValue(float fValue);		// マップポイント間の移動量設定
	float GetMapMoveValue(void);			// マップポイント間の移動量設定
	void SetMoveAngle(ANGLE angle);			// 移動方向設定
	ANGLE GetMoveAngle(void);				// 移動方向取得
	void SetOldMoveAngle(ANGLE angle);		// 過去の移動方向設定
	ANGLE GetOldMoveAngle(void);			// 過去の移動方向取得

	int SetEffectParent(CEffect3D *pEffect3D);	// エフェクト設定
	CEffect3D *GetEffectParent(int nIdx);	// 親設定したエフェクト取得
	int GetEffectParentNum(void);			// 親設定したエフェクトの数取得
	void ReleaseEffect(int nIdx);	// エフェクトの解放
	void UninitEffect(void);	// エフェクトの解放

	static void ReleaseAll(void);
	static void UpdateAll(void);
	static void DrawAll(void);
	static const int GetNumAll(void) { return m_nNumAll; }
	static const int GetNumPriorityAll(int nPriority) { return m_nNumPriorityAll[nPriority]; }

	static CObject *GetTop(int nPriority);	// 先頭のオブジェクト取得
	CObject *GetNext(void);					// 次のオブジェクト取得
	bool IsDeath(void);						// 死亡の判定
	void SetType(const TYPE type);
	TYPE GetType(void) const;
	CObject *GetObject(void);
	virtual CObject2D *GetObject2D(void);
	virtual CObject3D *GetObject3D(void);
	virtual CObject3DMesh *GetObject3DMesh(void);
	virtual CObjectX *GetObjectX(void);
	virtual CObjectChara *GetObjectChara(void);

protected:

	void Release(void);	// 開放処理
	void Death(void);	// 死亡処理

	D3DXVECTOR3 m_pos;		// 位置
private:

	int m_nPriority;		// 優先順位
	static int m_nNumAll;	// オブジェクトの総数
	static int m_nNumPriorityAll[mylib_const::PRIORITY_NUM];
	TYPE m_type;			// 種類
	ANGLE m_angle;			// 向き
	ANGLE m_angleOld;		// 過去の向き
	static CObject *m_pTop[mylib_const::PRIORITY_NUM];	// 先頭のオブジェクトへのポインタ
	static CObject *m_pCur[mylib_const::PRIORITY_NUM];	// 最後尾のオブジェクトへのポインタ
	CObject *m_pPrev;	// 前のオブジェクトへのポインタ
	CObject *m_pNext;	// 次のオブジェクトへのポインタ
	bool m_bDeath;		// 死亡フラグ
	CEffect3D *m_pEffect[mylib_const::MAX_OBJ];	// エフェクトのポインタ
	int m_nNumEffectParent;	// エフェクトの親設定した数
	int m_nIdxMapPoint;			// マップポイントのインデックス番号
	float m_fPointRatio;		// 移動割合
	float m_fMoveValue;			// 移動量

};

#endif