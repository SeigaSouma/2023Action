//=============================================================================
// 
//  敵の拠点エディターヘッダー [edit_enemybase.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _EDIT_ENEMYBASE_H_
#define _EDIT_ENEMYBASE_H_	// 二重インクルード防止

#include "main.h"
#include "object.h"
#include "camera.h"

class CEnemy;

//==========================================================================
// クラス定義
//==========================================================================
// カメラ追従種類変更エディットクラス定義
class CEditEnemyBase
{
public:

	CEditEnemyBase();
	~CEditEnemyBase();

	// オーバーライドされた関数
	HRESULT Init(void);
	HRESULT Init(const char *pFileName);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void Release(void);
	static CEditEnemyBase *Create(void);	// 生成処理

private:

	void Control(D3DXVECTOR3 &pos);	// 操作
	void Grab(void);	// 掴み
	void Delete(void);	// 削除
	void ChangeType(void);

	int m_nIdxMapPoint;			// マップポイントのインデックス番号
	float m_fPointRatio;		// 移動割合
	float m_fMoveValue;			// 移動量
	CObject::ANGLE m_MoveAngle;	// 移動方向
	D3DXVECTOR3 m_pos;			// 位置
	D3DXVECTOR3 m_rot;			// 位置
	int m_nEnemyType;			// 敵の種類
	
	CEnemy *m_pEnemy[mylib_const::MAX_PATTEN_ENEMY];	// 敵へのポインタ
	CObjectX *m_apObjX;	// オブジェクトX
};



#endif