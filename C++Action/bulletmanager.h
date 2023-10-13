//=============================================================================
// 
//  弾マネージャヘッダー [bulletmanager.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _BULLETMANAGER_H_
#define _BULLETMANAGER_H_	// 二重インクルード防止

#include "main.h"
#include "constans.h"
#include "bullet.h"

// 前方宣言
class CBullet;

//==========================================================================
// クラス定義
//==========================================================================
// 弾のマネージャクラス定義
class CBulletManager
{
public:

	CBulletManager();
	~CBulletManager();

	HRESULT Init(void);
	void Uninit(void);

	static CBulletManager *Create(void);

	int Regist(CBullet *pBullet);	// 割り当て
	void Delete(int nIdx);			// 削除
	int GetNumAll(void);			// 弾の総数取得
	CBullet **GetBullet(void);		// 弾のオブジェクト取得
protected:


private:

	CBullet *m_apBullet[mylib_const::MAX_BULLET];	// 弾のオブジェクト
	int m_nNumAll;		// 弾の総数

};



#endif