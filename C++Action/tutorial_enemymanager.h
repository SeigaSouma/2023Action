//=============================================================================
// 
//  敵のマネージャヘッダー [tutorial_enemymanager.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _TUTORIAL_ENEMYMANAGER_H_
#define _TUTORIAL_ENEMYMANAGER_H_	// 二重インクルード防止

#include "enemymanager.h"

// 前方宣言

//==========================================================================
// クラス定義
//==========================================================================
// 敵のマネージャクラス定義
class CTutorialEnemyManager : public CEnemyManager
{

public:

	CTutorialEnemyManager();
	~CTutorialEnemyManager();

	void Update(void) override;
	void SetEnemy(int nIdx, int nPattern, int nCntEnemy, int nParent, CWaveManager::SetInfo setinfo, D3DXVECTOR3 pos, D3DXVECTOR3 rot, int movetype = 0);	// 敵配置
	void SetSpawnWaveNormal(int nStage, int nWave, int nCntPattern, int *pParent);		// 通常湧き
	void SetSpawnWave(int nStage, int nWave);		// スポーンの起伏設定


private:

};



#endif