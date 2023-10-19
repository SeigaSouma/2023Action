//=============================================================================
// 
//  カメラ追従種類変更エディットヘッダー [edit_camerachasechanger.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _EDIT_CAMERACHASECHANGER_H_
#define _EDIT_CAMERACHASECHANGER_H_	// 二重インクルード防止

#include "main.h"
#include "object.h"
#include "camera.h"


//==========================================================================
// クラス定義
//==========================================================================
// カメラ追従種類変更エディットクラス定義
class CEditCameraChaseChanger
{
public:

	CEditCameraChaseChanger();
	~CEditCameraChaseChanger();

	// オーバーライドされた関数
	HRESULT Init(void);
	HRESULT Init(const char *pFileName);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CEditCameraChaseChanger *Create(void);	// 生成処理

private:

	void Control(D3DXVECTOR3 &pos);	// 操作
	void Grab(void);	// 掴み
	void Delete(void);	// 削除

	int m_nIdxMapPoint;			// マップポイントのインデックス番号
	float m_fPointRatio;		// 移動割合
	float m_fMoveValue;			// 移動量
	D3DXVECTOR3 m_pos;	// 位置
	CCamera::CHASETYPE m_ChaseType;			// 追従の種類
};



#endif