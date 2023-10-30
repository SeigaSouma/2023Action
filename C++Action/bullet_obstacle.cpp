//=============================================================================
// 
//  障害物弾処理 [bullet_obstacle.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "bullet_obstacle.h"
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
#include "bulletexplosion.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define BULLETMODEL		"data\\MODEL\\obstacleballet_01.x"
#define MOVE_SPEED		(15.0f)							// 移動速度

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// 関数ポインタ
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CBulletObstacle::CBulletObstacle(int nPriority) : CObjectX(nPriority)
{
	// 値のクリア
	m_nLife = 0;			// 寿命
	m_nLifeMax = 0;			// 寿命の最大値
	m_nTexIdx = 0;			// テクスチャのインデックス番号
	m_nCntEmission = 0;		// 発生物のカウンター
	m_fRadius = 0.0f;		// 半径
}

//==========================================================================
// デストラクタ
//==========================================================================
CBulletObstacle::~CBulletObstacle()
{
	
}

//==========================================================================
// 生成処理
//==========================================================================
CBulletObstacle *CBulletObstacle::Create(const D3DXVECTOR3 pos, const D3DXVECTOR3 rot, const D3DXVECTOR3 move, const float fSize)
{
	// 生成用のオブジェクト
	CBulletObstacle *pBullet = NULL;

	if (pBullet == NULL)
	{// NULLだったら

		// メモリの確保
		pBullet = DEBUG_NEW CBulletObstacle;

		if (pBullet != NULL)
		{// メモリの確保が出来ていたら

			// 位置割り当て
			pBullet->SetPosition(pos);

			// 位置割り当て
			pBullet->SetRotation(rot);

			// 移動量設定
			pBullet->SetMove(move);

			// サイズ設定
			pBullet->m_fRadius = fSize;

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
HRESULT CBulletObstacle::Init(void)
{
	HRESULT hr;

	// 各種変数の初期化
	m_nLifeMax = 60 * 5;
	m_nLife = m_nLifeMax;	// 寿命

	// 種類の設定
	CObject::SetType(TYPE_BULLET);

	// 初期化処理
	hr = CObjectX::Init(BULLETMODEL);
	if (FAILED(hr))
	{// 失敗したとき
		return E_FAIL;
	}
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CBulletObstacle::Uninit(void)
{
	// 終了処理
	CObjectX::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CBulletObstacle::Update(void)
{
	// 位置更新
	UpdatePos();

	// プレイヤーとの判定
	CollisionPlayer();

	// 寿命減算
	m_nLife--;

	if (m_nLife <= 0)
	{// 寿命が尽きたら

		// 爆発の生成
		//CExplosion::Create(GetPosition());

		// 終了処理
		Uninit();
		return;
	}
}

//==========================================================================
// 移動処理
//==========================================================================
void CBulletObstacle::UpdatePos(void)
{
	// 位置取得
	D3DXVECTOR3 pos = GetPosition();

	// 移動量取得
	D3DXVECTOR3 move = GetMove();

	// 向き取得
	D3DXVECTOR3 rot = GetRotation();

	// マップマネージャの取得
	CMapManager *pMapManager = CGame::GetMapManager();
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

	if (m_nLife % 2 == 0)
	{
		CEffect3D::Create(pos, D3DXVECTOR3(Random(-10, 10) * 0.1f + -move.x, -move.y, Random(-10, 10) * 0.1f + -move.z), D3DXCOLOR(1.0f, 0.4f, 0.1f, 1.0f), (float)Random(80, 120), 12, CEffect3D::MOVEEFFECT_ADD, CEffect3D::TYPE_SMOKE);
		CEffect3D::Create(pos, D3DXVECTOR3(Random(-10, 10) * 0.1f + -move.x, -move.y, Random(-10, 10) * 0.1f + -move.z), D3DXCOLOR(1.0f, 0.1f, 0.1f, 1.0f), (float)Random(60, 100), 12, CEffect3D::MOVEEFFECT_ADD, CEffect3D::TYPE_SMOKE);
	}

	// マップのインデックス番号設定
	SetMapIndex(nIdxMapPoint);

	// マップポイント間の割合設定
	SetMapPointRatio(fPointRatio);

	// マップポイント間の移動量設定
	SetMapMoveValue(fMoveValue);

	// 位置設定
	SetPosition(pos);

	// 移動量設定
	SetMove(move);

	// 向き設定
	SetRotation(rot);
}

//==========================================================================
// プレイヤーとの判定
//==========================================================================
void CBulletObstacle::CollisionPlayer(void)
{
	// プレイヤー情報取得
	CPlayer *pPlayer = CManager::GetInstance()->GetScene()->GetPlayer();
	if (pPlayer == NULL)
	{// NULLだったら
		return;
	}

	// プレイヤーの情報取得
	D3DXVECTOR3 PlayerPosition = pPlayer->GetCenterPosition();
	D3DXVECTOR3 PlayerRotation = pPlayer->GetRotation();
	float fPlayerRadius = pPlayer->GetRadius();

	// 情報取得
	D3DXVECTOR3 pos = GetPosition();

	if (SphereRange(pos, PlayerPosition, m_fRadius, fPlayerRadius))
	{// 当たっていたら

		// ヒット処理
		pPlayer->Hit(1);

		// 終了処理
		Uninit();
	}

}

//==========================================================================
// 描画処理
//==========================================================================
void CBulletObstacle::Draw(void)
{
	// 描画
	CObjectX::Draw();
}

