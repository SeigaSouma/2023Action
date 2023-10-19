//=============================================================================
// 
//  マップマネージャヘッダー [mapmanager.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _MAPMANAGER_H_
#define _MAPMANAGER_H_	// 二重インクルード防止

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
// マップマネージャクラス定義
class CMapManager
{
public:
	CMapManager();
	~CMapManager();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);

	void CreatePos(D3DXVECTOR3 pos);	// 位置作成
	void DeletePos(int nIdx);			// 位置削除
	HRESULT ReadText(const std::string pFileName);	// 外部ファイル読み込み処理
	void Save(const std::string pFileName);			// 外部ファイル書き出し処理

	static CMapManager *Create(const std::string pFileName);
	int GetControlPointNum(void);			// 制御点数取得
	D3DXVECTOR3 GetControlPoint(void);		// 制御点取得
	D3DXVECTOR3 GetControlPoint(int nIdx);	// 制御点取得
	void SetControlPoint(D3DXVECTOR3 pos, int nIdx);	// 制御点設定
	D3DXVECTOR3 GetTargetPosition(int nIdx, float fMoveValue);	// 目標の位置取得
	D3DXVECTOR3 UpdateNowPosition(int& nIdx, float& fRatio, float& fMoveValue, float PosY);
	D3DXVECTOR3 UpdateNowPosition(int& nIdx, float& fRatio, float& fMoveValue, float PosY, CObject::ANGLE MoveAngle, float fMove);	// 現在地更新
	CObject::ANGLE GetTargetAngle(int myIdx, int TargetIdx, float myMoveValue, float TargetMoveValue);	// 目標が自分のどっちにいるかの判定
	CObject::ANGLE GetTargetAngle(CObject *pMyObj, CObject *pTargetObj);	// 目標が自分のどっちにいるかの判定
	int ConversionToIndex(D3DXVECTOR3 pos);			// 位置をマップのインデックス変換
	float GetThusFarMoveValue(int nIdx);		// これまでのマップ移動量取得
	static int GetNumAll(void) { return m_nNumAll; }	// 総数取得
private:

	std::vector<D3DXVECTOR3> m_posAll;		// 位置

	D3DXVECTOR3 m_pos;			// 位置
	static CMapManager *m_pTop;	// 先頭のポインタ
	static CMapManager *m_pCur;	// 最後尾のポインタ
	CMapManager *m_pPrev;		// 前のオブジェクトへのポインタ
	CMapManager *m_pNext;		// 次のオブジェクトへのポインタ
	static int m_nNumAll;		// 総数
	CDebugPointNumber *m_pMultiNumber[mylib_const::MAX_CAMERAAXIS];
};



#endif