//=============================================================================
// 
//  弾処理 [bullet.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "bullet.h"
#include "manager.h"
#include "texture.h"
#include "renderer.h"
#include "explosion.h"
#include "3D_effect.h"
#include "sound.h"
#include "particle.h"
#include "enemy.h"
#include "calculation.h"
#include "mapmanager.h"
#include "game.h"
#include "bulletmanager.h"
#include "player.h"
#include "impactwave.h"
#include "effect_thunderring.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define WIDTH			(30.0f)							// 横幅
#define HEIGHT			(30.0f)							// 縦幅
#define MOVE_SPEED		(15.0f)							// 移動速度
#define ANIM_SPEED		(4)								// 読み込み間隔
#define MAX_PATTERN_U	(1)								// Uの分割数
#define MAX_PATTERN_V	(1)								// Vの分割数
#define MAX_PATTERN		(MAX_PATTERN_U)					// アニメーションパターンの最大数
#define MOVE_U			(1.0f / (float)MAX_PATTERN_U)	// U座標移動量
#define MOVE_V			(1.0f / (float)MAX_PATTERN_V)	// V座標移動量

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
const char *CBullet::m_apTextureFile[TYPE_MAX] =	// テクスチャのファイル
{
	"data\\TEXTURE\\sunder_03.png",
	"data\\TEXTURE\\sunder_03.png",
};
int CBullet::m_nNumAll = 0;		// 弾の総数

//==========================================================================
// 関数ポインタ
//==========================================================================
CBullet::STATE_FUNC CBullet::m_FuncList[] =
{
	&CBullet::StateNone,
	&CBullet::StateDamage,
};

CBullet::COLLISION_FUNC CBullet::m_CollisionFuncList[] =	// 当たり判定のリスト
{
	&CBullet::CollisionEnemy,	// 敵との判定
	&CBullet::CollisionPlayer,	// プレイヤーとの判定
};

//==========================================================================
// コンストラクタ
//==========================================================================
CBullet::CBullet(int nPriority) : CMeshSphere(nPriority), m_nLifeMax(1)
{
	// 値のクリア
	m_type = TYPE_PLAYER;
	m_state = STATE_NONE;			// 状態
	m_MoveType = MOVETYPE_NORMAL;	// 移動の種類
	m_nCntState = 0;			// 状態遷移カウンター
	m_nLife = 0;
	m_nIdxBulletManager = 0;			// 弾マネージャのインデックス番号
	m_nCntEmission = 0;	// 発生物のカウンター
	m_pMeshSphereEffect = NULL;		// メッシュスフィアのエフェクト
	m_pEffectThunderRing = NULL;	// 雷のリングのエフェクト

	// テクスチャデータの配列分繰り返す
	m_nTexIdx = 0;		// テクスチャのインデックス番号

	// 総数加算
	m_nNumAll++;
}

//==========================================================================
// デストラクタ
//==========================================================================
CBullet::~CBullet()
{
	
}

//==========================================================================
// 生成処理
//==========================================================================
CBullet *CBullet::Create(TYPE type, MOVETYPE movetype, const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR3 move, const float fSize)
{
	// 生成用のオブジェクト
	CBullet *pBullet = NULL;

	if (pBullet == NULL)
	{// NULLだったら

		// メモリの確保
		pBullet = DEBUG_NEW CBullet;

		if (pBullet != NULL)
		{// メモリの確保が出来ていたら

			// 種類設定
			pBullet->m_type = type;

			// 移動種類
			pBullet->m_MoveType = movetype;

			// 位置割り当て
			pBullet->SetPosition(pos);

			// 位置割り当て
			pBullet->SetRotation(rot);

			// 移動量設定
			pBullet->SetMove(move);

			// サイズ設定
			pBullet->m_fRadius = fSize;
			pBullet->SetWidthLen(fSize);
			pBullet->SetHeightLen(fSize);
			pBullet->SetWidthBlock(16);
			pBullet->SetHeightBlock(16);

			// 初期化処理
			pBullet->Init();
		}

		return pBullet;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CBullet::Init(void)
{
	HRESULT hr;

	// 各種変数の初期化
	m_nLifeMax = 60 * 5;
	m_nLife = m_nLifeMax;	// 寿命
	SetColor(D3DXCOLOR(0.3f, 0.3f, 1.0f, 1.0f));

	// テクスチャの割り当て
	m_nTexIdx = CManager::GetInstance()->GetTexture()->Regist(m_apTextureFile[m_type]);

	// テクスチャの割り当て
	BindTexture(m_nTexIdx);

	// 種類の設定
	CObject::SetType(TYPE_BULLET);

	// 初期化処理
	hr = CMeshSphere::Init();
	if (FAILED(hr))
	{// 失敗したとき
		return E_FAIL;
	}

	// 割り当て
	m_nIdxBulletManager = CGame::GetBulletManager()->Regist(this);

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CBullet::Uninit(void)
{
	// 削除
	if (CManager::GetInstance()->GetMode() == CScene::MODE_GAME && CGame::GetBulletManager() != NULL)
	{// 弾マネージャの削除
		CGame::GetBulletManager()->Delete(m_nIdxBulletManager);
	}

	if (m_pMeshSphereEffect != NULL)
	{// メッシュスフィアのエフェクト
		m_pMeshSphereEffect = NULL;
	}

	if (m_pEffectThunderRing != NULL)
	{// 雷のリングのエフェクト
		m_pEffectThunderRing = NULL;
	}

	// 終了処理
	CMeshSphere::Uninit();

	// 総数減算
	m_nNumAll--;
}

//==========================================================================
// 更新処理
//==========================================================================
void CBullet::Update(void)
{
	// 位置更新
	UpdatePos();

	// 状態別処理
	(this->*(m_FuncList[m_state]))();

	// 当たり判定処理
	(this->*(m_CollisionFuncList[m_type]))();

	if (IsDeath() == true)
	{// 死亡フラグが立っていたら
		return;
	}

	if (m_type == TYPE_ENEMY)
	{
		m_nCntEmission = (m_nCntEmission + 1) % 100;	// 発生物のカウンター

		//if (m_nCntEmission % 3 == 0)
		//{
		//	// 位置取得
		//	D3DXVECTOR3 pos = GetPosition();

		//	// 移動量取得
		//	D3DXVECTOR3 move = GetMove();

		//	CEffect3D *pEffect = CEffect3D::Create(
		//		pos,
		//		-move,
		//		D3DXCOLOR(0.2f, 0.2f, 0.8f, 0.8f),
		//		GetRadius() * 1.5f,
		//		6,
		//		CEffect3D::MOVEEFFECT_SUB,
		//		CEffect3D::TYPE_THUNDER);

		//	// セットアップ位置設定
		//	pEffect->SetUp(D3DXVECTOR3(0.0f, 0.0f, 0.0f), CObject::GetObject(), SetEffectParent(pEffect));
		//}

		if (m_nCntEmission == 10 && m_pMeshSphereEffect != NULL)
		{
			float fSize = GetRadius();
			m_pMeshSphereEffect->SetSizeDest(fSize + 10.0f);
		}
		if (m_nCntEmission == 15 && m_pMeshSphereEffect != NULL)
		{
			m_pMeshSphereEffect->Uninit();
			m_pMeshSphereEffect = NULL;
		}

		if (m_nCntEmission == 0)
		{
			if (m_pMeshSphereEffect != NULL)
			{
				m_pMeshSphereEffect->Uninit();
				m_pMeshSphereEffect = NULL;
			}

			float fSize = GetRadius();

			// メッシュスフィア生成
			m_pMeshSphereEffect = CMeshSphere::Create(GetPosition(), GetRadius(), m_nTexIdx, 7);

			// 情報設定
			m_pMeshSphereEffect->SetColor(D3DXCOLOR(0.2f, 0.2f, 1.0f, 0.7f));
			m_pMeshSphereEffect->SetWidthLen(fSize);
			m_pMeshSphereEffect->SetHeightLen(fSize);
			m_pMeshSphereEffect->SetSizeDest(fSize + 100.0f);

			if (m_pEffectThunderRing != NULL)
			{// 雷のリングのエフェクト
				m_pEffectThunderRing->Uninit();
				m_pEffectThunderRing = NULL;
			}

			// 雷のリング生成
			m_pEffectThunderRing = CThunderRing::Create(GetPosition(), D3DXVECTOR2(300.0f, 300.0f));

			// 自動で消えないようにする
			m_pEffectThunderRing->SetDesableAutoDeath();
		}
	}
	else if (m_type == TYPE_PLAYER)
	{
		// 半径
		float fSize = GetRadius();

		if (m_pMeshSphereEffect == NULL)
		{
			// メッシュスフィア生成
			m_pMeshSphereEffect = CMeshSphere::Create(GetPosition(), GetRadius(), m_nTexIdx, 7);

			// 情報設定
			m_pMeshSphereEffect->SetColor(D3DXCOLOR(1.0f, 0.2f, 1.0f, 0.7f));
			m_pMeshSphereEffect->SetWidthLen(fSize);
			m_pMeshSphereEffect->SetHeightLen(fSize);
			m_pMeshSphereEffect->SetSizeDest(fSize + 100.0f);
		}

		if (m_pEffectThunderRing == NULL)
		{
			// 雷のリング生成
			m_pEffectThunderRing = CThunderRing::Create(GetPosition(), D3DXVECTOR2(400.0f, 400.0f));

			// 色設定
			m_pEffectThunderRing->SetColor(D3DXCOLOR(1.0f, 0.3f, 1.0f, 1.0f));

			// 自動で消えないようにする
			m_pEffectThunderRing->SetDesableAutoDeath();
		}
	}

	if (m_pMeshSphereEffect != NULL)
	{
		m_pMeshSphereEffect->SetPosition(GetPosition());
	}

	if (m_pEffectThunderRing != NULL)
	{// 雷のリングのエフェクト

		if (m_pEffectThunderRing->IsFinish() == true)
		{
			m_pEffectThunderRing->Uninit();
			m_pEffectThunderRing = NULL;
		}
		else
		{
			m_pEffectThunderRing->SetPosition(GetPosition());
		}
	}

	// 寿命減算
	m_nLife--;

	if (m_nLife <= 0)
	{// 寿命が尽きたら

		// 爆発の生成
		CExplosion::Create(GetPosition());

		// 弾の削除
		if (m_pMeshSphereEffect != NULL)
		{
			m_pMeshSphereEffect->Uninit();
			m_pMeshSphereEffect = NULL;
		}

		if (m_pEffectThunderRing != NULL)
		{// 雷のリングのエフェクト
			m_pEffectThunderRing->Uninit();
			m_pEffectThunderRing = NULL;
		}
		Uninit();
		return;
	}

	// 頂点情報設定
	SetVtx();
}

//==========================================================================
// 移動処理
//==========================================================================
void CBullet::UpdatePos(void)
{
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// マップマネージャの取得
	CMapManager *pMapManager = CManager::GetInstance()->GetScene()->GetMapManager();
	if (pMapManager == NULL)
	{// NULLだったら
		return;
	}

	// 位置更新
	ANGLE MoveAngle = GetMoveAngle();

	// マップ間の移動量
	float fMoveValue = GetMapMoveValue();

	// 曲線作る為の4点
	int nIdxMapPoint = GetMapIndex();

	// 位置の割合取得
	float fPointRatio = GetMapPointRatio();

	// 回転
	rot.y += D3DX_PI * 0.025f;
	rot.x += D3DX_PI * (Random(5, 25) * 0.001f);

	// 位置更新
	if (m_state == STATE_NONE)
	{
		switch (MoveAngle)
		{
		case CObject::ANGLE_UP:
			pos += move;
			break;

		case CObject::ANGLE_RIGHT:
		case CObject::ANGLE_LEFT:
		{
			// 移動
			pos.y += move.y;
			pos = pMapManager->UpdateNowPosition(nIdxMapPoint, fPointRatio, fMoveValue, pos.y, MoveAngle, move.x);
		}
			break;

		case CObject::ANGLE_DOWN:
			pos += move;
			break;

		default:
			break;
		}
	}

	// マップのインデックス番号設定
	SetMapIndex(nIdxMapPoint);

	// マップポイント間の割合設定
	SetMapPointRatio(fPointRatio);

	// マップポイント間の移動量設定
	SetMapMoveValue(fMoveValue);

	switch (m_MoveType)
	{
	case CBullet::MOVETYPE_NORMAL:
		break;

	case CBullet::MOVETYPE_CHASE:
		break;

	case CBullet::MOVETYPE_PARABOLA:

		// 二次曲線移動
		CuadricCurveComp(pos.y, 120, 0.0f, 200.0f, m_nLife);
		break;
	}

	// 位置設定
	SetPosition(pos);

	// 移動量設定
	SetMove(move);

	// 向き設定
	SetRotation(rot);
}

//==========================================================================
// 通常状態処理
//==========================================================================
void CBullet::StateNone(void)
{
	// 状態遷移カウンター減算
	m_nCntState--;
	if (m_nCntState <= 0)
	{
		m_nCntState = 0;
		m_state = STATE_NONE;
	}
}

//==========================================================================
// ダメージ状態処理
//==========================================================================
void CBullet::StateDamage(void)
{
	// 状態遷移カウンター減算
	m_nCntState--;
	if (m_nCntState <= 0)
	{
		m_nCntState = 0;
		m_state = STATE_NONE;

		if (m_pMeshSphereEffect != NULL)
		{
			m_pMeshSphereEffect->Uninit();
			m_pMeshSphereEffect = NULL;
		}

		if (m_pEffectThunderRing != NULL)
		{// 雷のリングのエフェクト
			m_pEffectThunderRing->Uninit();
			m_pEffectThunderRing = NULL;
		}
	}
}

//==========================================================================
// プレイヤーとの判定
//==========================================================================
void CBullet::CollisionPlayer(void)
{
	// プレイヤー情報取得
	CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();
	if (pPlayer == NULL)
	{// NULLだったら
		return;
	}

	// プレイヤーの情報取得
	D3DXVECTOR3 PlayerPosition = pPlayer->GetPosition();
	D3DXVECTOR3 PlayerRotation = pPlayer->GetRotation();
	float fPlayerRadius = pPlayer->GetRadius();

	// 情報取得
	D3DXVECTOR3 pos = GetPosition();
	float fRadius = GetRadius();

	if (SphereRange(pos, PlayerPosition, fRadius, fPlayerRadius))
	{// 当たっていたら

		// ヒット処理
		pPlayer->Hit(1);

		if (m_pMeshSphereEffect != NULL)
		{
			m_pMeshSphereEffect->Uninit();
			m_pMeshSphereEffect = NULL;
		}

		if (m_pEffectThunderRing != NULL)
		{// 雷のリングのエフェクト
			m_pEffectThunderRing->Uninit();
			m_pEffectThunderRing = NULL;
		}
		// 終了処理
		Uninit();
	}

}

//==========================================================================
// 敵との判定
//==========================================================================
void CBullet::CollisionEnemy(void)
{
	// 敵マネージャ取得
	CEnemyManager *pEnemyManager = CManager::GetInstance()->GetScene()->GetEnemyManager();
	if (pEnemyManager == NULL)
	{// NULLだったら
		return;
	}

	// 敵情報取得
	CEnemy **ppEnemy = pEnemyManager->GetEnemy();
	int nNumEnemy = pEnemyManager->GetNumAll();

	// 情報取得
	D3DXVECTOR3 pos = GetPosition();
	float fRadius = GetRadius();
	bool bHit = false;

	int nUse = 0;
	for (int nCntEnemy = 0; nUse < nNumEnemy; nCntEnemy++)
	{
		if (ppEnemy[nCntEnemy] == NULL)
		{// NULLだったら
			continue;
		}
		nUse++;

		// 敵の情報取得
		D3DXVECTOR3 EnemyPosition = ppEnemy[nCntEnemy]->GetPosition();
		float fEnemyRadius = ppEnemy[nCntEnemy]->GetRadius();

		if (SphereRange(pos, EnemyPosition, fRadius, fEnemyRadius))
		{// 当たっていたら

			// ヒット処理
			ppEnemy[nCntEnemy]->Hit(mylib_const::DMG_BOUNCE);
			bHit = true;
		}
	}

	if (bHit == true)
	{// 当たってたら

		if (m_pMeshSphereEffect != NULL)
		{
			m_pMeshSphereEffect->Uninit();
			m_pMeshSphereEffect = NULL;
		}

		if (m_pEffectThunderRing != NULL)
		{// 雷のリングのエフェクト
			m_pEffectThunderRing->Uninit();
			m_pEffectThunderRing = NULL;
		}
		Uninit();
		return;
	}

}

//==========================================================================
// プレイヤー弾の更新
//==========================================================================
void CBullet::UpdateTypePlayer(void)
{

}

//==========================================================================
// 描画処理
//==========================================================================
void CBullet::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// ビルボードの描画
	CMeshSphere::Draw();

	// アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CBullet::SetVtx(void)
{
	// 頂点設定
	CMeshSphere::SetVtx();
}

//==========================================================================
// 状態設定
//==========================================================================
void CBullet::SetState(STATE state, int nCntState)
{
	m_state = state;			// 状態
	m_nCntState = nCntState;	// 状態遷移カウンター
}

//==========================================================================
// 状態取得
//==========================================================================
CBullet::STATE CBullet::GetState(void)
{
	return m_state;
}

//==========================================================================
// 半径取得
//==========================================================================
float CBullet::GetRadius(void)
{
	return m_fRadius;
}

//==========================================================================
// 種類設定
//==========================================================================
void CBullet::SetType(TYPE type)
{
	m_type = type;

	// テクスチャの割り当て
	m_nTexIdx = CManager::GetInstance()->GetTexture()->Regist(m_apTextureFile[m_type]);

	// テクスチャの割り当て
	BindTexture(m_nTexIdx);
}

//==========================================================================
// 種類取得
//==========================================================================
CBullet::TYPE CBullet::GetType(void)
{
	return m_type;
}

//==========================================================================
// 総数取得
//==========================================================================
int CBullet::GetNumAll(void)
{
	return m_nNumAll;
}
