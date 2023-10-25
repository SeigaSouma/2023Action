//=============================================================================
// 
//  敵の拠点ヘッダー [enemybase.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _ENEMYBASE_H_
#define _ENEMYBASE_H_	// 二重インクルード防止

#include "main.h"
#include "constans.h"
#include "object.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CDebugPointNumber;

//==========================================================================
// クラス定義
//==========================================================================
// カメラの軸クラス定義
class CEnemyBase
{
public:

	// 構造体定義
	struct sInfo
	{
		int nPattern;			// 種類
		int nMapIdx;			// マップインデックス
		float fMapMoveValue;	// マップの移動量
		float fSpawnPosY;		// 出現の高さ
		int nRush;				// ラッシュ用かどうか
	};

	CEnemyBase();
	~CEnemyBase();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);

	void CreatePos(int nPattern, int nMapIdx, float fMapMoveValue, int nRush, float PosY);	// 位置作成
	void DeletePos(int nIdx);					// 位置削除
	HRESULT ReadText(void);	// 外部ファイル読み込み処理
	void Save(void);			// 外部ファイル書き出し処理

	static CEnemyBase *Create(void);
	int GetAxisNum(void);			// 軸数取得
	D3DXVECTOR3 GetAxis(int nIdx);	// 軸取得
	void SetSpawnPoint(int nIdx, int nMapIdx, float fMapMoveValue, float PosY);	// 軸設定
	sInfo GetChaseChangeInfo(int nIdx);	// 変更の情報取得
	static int GetNumAll(void) { return m_nNumAll; }	// 総数取得
private:

	std::vector<sInfo> m_ChaseChangeInfo;		// 追従変更の情報
	CObjectX *m_apObjX[mylib_const::MAX_CAMERAAXIS];	// オブジェクトX
	CDebugPointNumber *m_pMultiNumber[mylib_const::MAX_CAMERAAXIS];

	static int m_nNumAll;		// 総数
};



#endif