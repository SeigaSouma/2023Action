//=============================================================================
// 
//  カメラの軸ヘッダー [cameraaxis.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _CAMERAAXIS_H_
#define _CAMERAAXIS_H_	// 二重インクルード防止

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
class CCameraAxis
{
public:
	CCameraAxis();
	~CCameraAxis();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);

	void CreatePos(D3DXVECTOR3 pos);	// 位置作成
	void DeletePos(int nIdx);			// 位置削除
	HRESULT ReadText(const std::string pFileName);	// 外部ファイル読み込み処理
	void Save(const std::string pFileName);			// 外部ファイル書き出し処理

	static CCameraAxis *Create(const std::string pFileName);
	int GetAxisNum(void);			// 軸数取得
	D3DXVECTOR3 GetAxis(int nIdx);	// 軸取得
	void SetAxis(D3DXVECTOR3 pos, int nIdx);	// 軸設定
	static int GetNumAll(void) { return m_nNumAll; }	// 総数取得
private:

	std::vector<D3DXVECTOR3> m_posAll;		// 位置
	CDebugPointNumber *m_pMultiNumber[mylib_const::MAX_CAMERAAXIS];
	CObjectX *m_apObjX[mylib_const::MAX_CAMERAAXIS];	// オブジェクトX

	static int m_nNumAll;		// 総数
};



#endif