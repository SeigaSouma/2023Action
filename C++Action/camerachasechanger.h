//=============================================================================
// 
//  カメラの軸ヘッダー [camerachasechanger.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _CAMERACHASECHANGER_H_
#define _CAMERACHASECHANGER_H_	// 二重インクルード防止

#include "main.h"
#include "constans.h"
#include "object.h"
#include "camera.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CDebugPointNumber;

//==========================================================================
// クラス定義
//==========================================================================
// カメラの軸クラス定義
class CCameraChaseChanger
{
public:

	// 構造体定義
	struct sChaseChangeInfo
	{
		int nByTypeIdx;					// 種類別インデックス番号
		D3DXVECTOR3 pos;				// 位置
		CCamera::CHASETYPE chaseType;	// 追従の種類
	};

	CCameraChaseChanger();
	~CCameraChaseChanger();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);

	void CreatePos(CCamera::CHASETYPE type, D3DXVECTOR3 pos);	// 位置作成
	void DeletePos(int nIdx);			// 位置削除
	HRESULT ReadText(const std::string pFileName);	// 外部ファイル読み込み処理
	void Save(const std::string pFileName);			// 外部ファイル書き出し処理

	static CCameraChaseChanger *Create(const std::string pFileName);
	int GetAxisNum(void);			// 軸数取得
	D3DXVECTOR3 GetAxis(int nIdx);	// 軸取得
	void SetAxis(D3DXVECTOR3 pos, int nIdx);	// 軸設定
	sChaseChangeInfo GetChaseChangeInfo(int nIdx);	// 変更の情報取得
	static int GetNumAll(void) { return m_nNumAll; }	// 総数取得
private:

	std::vector<sChaseChangeInfo> m_ChaseChangeInfo;		// 追従変更の情報
	CDebugPointNumber *m_pMultiNumber[mylib_const::MAX_CAMERAAXIS];
	CObjectX *m_apObjX[mylib_const::MAX_CAMERAAXIS];	// オブジェクトX

	static int m_nNumChaseAxis;		// 軸追従の総数
	static int m_nNumChaseMap;		// マップ追従の総数
	static int m_nNumAll;		// 総数
};



#endif