//=============================================================================
// 
//  制御点エディットヘッダー [edit_controlpoint.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _EDIT_CONTROLPOINT_H_
#define _EDIT_CONTROLPOINT_H_	// 二重インクルード防止

#include "main.h"
#include "object.h"


//==========================================================================
// クラス定義
//==========================================================================
// エディットクラス定義
class CEditControlPoint
{
public:

	CEditControlPoint();
	~CEditControlPoint();

	// オーバーライドされた関数
	HRESULT Init(void);
	HRESULT Init(const char *pFileName);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CEditControlPoint *Create();

private:

	void Control(D3DXVECTOR3 &pos);	// 操作
	void Grab(void);
	void Delete(void);

	D3DXVECTOR3 m_pos;	// 位置
};



#endif