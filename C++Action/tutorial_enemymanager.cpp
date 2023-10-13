//=============================================================================
// 
//  敵のマネージャ処理 [tutorial_enemymanager.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "debugproc.h"
#include "manager.h"
#include "scene.h"
#include "tutorial.h"
#include "tutorial_enemymanager.h"
#include "tutorialstep.h"
#include "enemy.h"

//==========================================================================
// マクロ定義
//==========================================================================

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CTutorialEnemyManager::CTutorialEnemyManager()
{
	// 値のクリア
	
}

//==========================================================================
// デストラクタ
//==========================================================================
CTutorialEnemyManager::~CTutorialEnemyManager()
{

}

//==========================================================================
// 更新処理
//==========================================================================
void CTutorialEnemyManager::Update(void)
{
	if (CTutorial::GetStep()->GetNowStep() < CTutorialStep::STEP_IMPACTWAVE)
	{
		return;
	}

	if (m_nNumAll <= 0)
	{// 敵がいなくなったら

		int nWave = 0;
		if (CTutorial::GetStep()->GetNowStep() == CTutorialStep::STEP_MAX)
		{
			nWave = 1;
		}
		// 敵設置しなおし
		SetSpawnWave(0, nWave);
	}

	// インターバル減算
	m_nIntervalKill--;

	if (m_nIntervalKill <= 0 && m_state != STATE_COMBOANIM)
	{// 最低値で固定

		if (m_bHitStop == true)
		{// ヒットストップまでいっていたら
			m_state = STATE_COMBOANIM;
		}

		m_nNumTotalKill = 0;
		m_nIntervalKill = 0;
		m_bHitStop = false;
	}

	if (m_state == STATE_COMBOANIM)
	{// コンボ時のアニメーション状態の時
		UpdateComboAnimation();
	}
}

//==========================================================================
// スポーンの起伏設定
//==========================================================================
void CTutorialEnemyManager::SetSpawnWave(int nStage, int nWave)
{
	// スポーン時の起伏設定
	CElevation *pElevation = CManager::GetScene()->GetObject3DMesh();


	int aParent[mylib_const::MAX_PATTEN_ENEMY][mylib_const::MAX_ENEMY] = {};
	int nToatalCount = 0;

	for (int nCntPattern = 0; nCntPattern < m_aSetEnemyInfo[nStage].aWaveInfo[nWave].nSetNum; nCntPattern++)
	{
		// 今回の情報一時保存
		CWaveManager::SetInfo info = m_aSetEnemyInfo[nStage].aWaveInfo[nWave].Info[nCntPattern];
		int nPattern = info.nType;

		switch (info.nSpawnType)
		{
		case SPAWNTYPE_NORMAL:
			SetSpawnWaveNormal(nStage, nWave, nCntPattern, &aParent[nCntPattern][0]);
			break;

		default:
			SetSpawnWaveNormal(nStage, nWave, nCntPattern, &aParent[nCntPattern][0]);
			break;
		}

		nToatalCount += m_aPattern[nPattern].nNumEnemy;
	}
}

//==========================================================================
// 通常湧き
//==========================================================================
void CTutorialEnemyManager::SetSpawnWaveNormal(int nStage, int nWave, int nCntPattern, int *pParent)
{
	// スポーン時の起伏設定
	CElevation *pElevation = CManager::GetScene()->GetObject3DMesh();

	// 今回の情報一時保存
	CWaveManager::SetInfo info = m_aSetEnemyInfo[nStage].aWaveInfo[nWave].Info[nCntPattern];
	int nPattern = info.nType;
	int nNumSpawn = m_aPattern[nPattern].nNumEnemy;	// スポーンする数

	int nCntNULL = 0;
	for (int nCntEnemy = 0; nCntEnemy < nNumSpawn; nCntEnemy++)
	{// 1体ずつセット

		for (nCntNULL = nCntNULL; nCntNULL < mylib_const::MAX_OBJ; nCntNULL++)
		{
			if (m_nParent[nCntNULL] != -1)
			{// 情報が入ってたら
				continue;
			}

			// 親の番号保存
			m_nParent[nCntNULL] = nCntNULL;

			// 親番号保存
			pParent[nCntEnemy] = nCntNULL;

			// 敵の情報
			EnemyData enemyData = m_aPattern[nPattern].EnemyData[nCntEnemy];

			// 待機時間
			int nWaitTime = enemyData.nWaitTime;

			// 計算用マトリックス
			D3DXMATRIX mtxRot, mtxTrans, mtxWorld;
			D3DXMATRIX mtxRotOrigin;

			// マトリックスの初期化
			D3DXMatrixIdentity(&mtxWorld);
			D3DXMatrixIdentity(&mtxRotOrigin);

			
			// 向きを反映する
			D3DXMatrixRotationYawPitchRoll(&mtxRotOrigin,info.rot.y, 0.0f, 0.0f);

			// 向きを反映する
			D3DXMatrixRotationYawPitchRoll(&mtxRot, info.rot.y, info.rot.x, info.rot.z);
			D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

			// パターン内の位置を反映する
			D3DXMatrixTranslation(&mtxTrans, enemyData.pos.x, enemyData.pos.y, enemyData.pos.z);
			D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);
			D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRotOrigin);

			// スポーン時の向きを掛け合わせる
			D3DXVECTOR3 spawnPos = D3DXVECTOR3(mtxWorld._41, mtxWorld._42, mtxWorld._43);
			D3DXVECTOR3 spawnRot = D3DXVECTOR3(0.0f, info.rot.y, 0.0f);

			// 位置設定
			spawnPos = D3DXVECTOR3(spawnPos.x + info.pos.x, 0.0f, spawnPos.z + info.pos.z);

			// タイプ別の範囲
			D3DXVECTOR2 range = D3DXVECTOR2(80.0f, 80.0f);
			float fStrength = 70.0f;

			// 親のID計算
			int nParent = -1;
			if (enemyData.nParent >= 0)
			{
				nParent = pParent[enemyData.nParent];
			}
			else
			{
				nParent = -1;
			}

			// 波発生
			pElevation->SetSpawnWave(
				nCntNULL,				// 自分のインデックス番号
				nParent,				// 親のインデックス番号
				nCntEnemy,				// 何番目の敵か
				nWaitTime,				// 待機時間
				info,					// ウェーブマネージャの情報
				spawnPos,				// 発生地点
				spawnRot,				// 向き
				range,					// 範囲
				fStrength,				// 高さ
				60						// 持続時間
			);

			// 総数加算
			m_nNumAll++;

			// 親番号保存
			pParent[nCntEnemy] = nCntNULL;
			break;
		}
	}
}

//==========================================================================
// 敵配置
//==========================================================================
void CTutorialEnemyManager::SetEnemy(int nIdx, int nPattern, int nCntEnemy, int nParent, CWaveManager::SetInfo setinfo, D3DXVECTOR3 pos, D3DXVECTOR3 rot, int movetype)
{
	if (m_pEnemy[nIdx] != NULL)
	{// 既に情報が入ってたら
		return;
	}

	int nType = m_aPattern[nPattern].EnemyData[nCntEnemy].nType;

	if (setinfo.nChara != -1)
	{// 何かしら設定されてたら
		nType = setinfo.nChara;
	}

	// 敵の生成
	m_pEnemy[nIdx] = CEnemy::Create(
		nIdx,							// インデックス番号
		sMotionFileName[nType].c_str(),	// ファイル名
		pos,							// 位置
		(CEnemy::TYPE)m_aPattern[nPattern].EnemyData[nCntEnemy].nType,	// 種類
		nType							// キャラの種類
	);


	if (m_pEnemy[nIdx] == NULL)
	{// 失敗していたら

		delete m_pEnemy[nIdx];
		m_pEnemy[nIdx] = NULL;
		return;
	}

	// 向き設定
	m_pEnemy[nIdx]->SetRotation(rot);
	m_pEnemy[nIdx]->SetRotDest(rot.y);
	m_pEnemy[nIdx]->SetOriginRotation(rot);

	// 行動の種類設定
	m_pEnemy[nIdx]->SetMoveType((CEnemy::MOVETYPE)movetype);

	// 行動の種類設定
	m_pEnemy[nIdx]->SetSpawnType((SPAWNTYPE)setinfo.nSpawnType);

	// 隊列設定
	if (setinfo.nSpawnType == 0)
	{
		int n = 0;
	}

	// 親モデルの設定
	if (nParent >= 0)
	{
		m_pEnemy[nIdx]->SetParent(m_pEnemy[nParent]);
	}
	else
	{// 親が自分の場合

		// NULL設定
		m_pEnemy[nIdx]->SetParent(NULL);
	}

	// 敵の情報
	EnemyData enemyData = m_aPattern[nPattern].EnemyData[nCntEnemy];
	D3DXVECTOR3 posParent = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	if (enemyData.nParent != -1)
	{// 親がいる場合
		posParent = m_aPattern[nPattern].EnemyData[enemyData.nParent].pos;
	}

	// 隊列の位置設定
	m_pEnemy[nIdx]->SetFormationPos(m_aPattern[nPattern].EnemyData[nCntEnemy].pos, posParent);
}
