//=============================================================================
// 
//  パーティクル処理 [my_particle.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "particle.h"
#include "calculation.h"
#include "2D_effect.h"
#include "3D_effect.h"
using namespace my_particle;	// 名前空間を指定

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
//==========================================================================
//==========================================================================
D3DXVECTOR3 m_pos;		// 位置
D3DXVECTOR3 m_move;		// 移動量
D3DXCOLOR m_col;		// 色
float m_fRadius;		// 半径
float m_fAngle;			// 向き
float m_fMove;			// 移動量
int m_nLife;			// 寿命
TYPE m_nType;			// 種類

HRESULT Init(void);
void Smoke(void);
void SmokeRed(void);
void SmokeYel(void);
void ManyUI_Mini(void);
void ResultMessage(void);
void OffSetting(void);
void SuperAttack(void);
void IceSpawn(void);
void Check(void);
void Check2D(void);
void SickLine(void);
void MagicExplosion(void);
void ExplosionRemain(void);
void ExplosionRemainFire(void);
void ExplosionStart(void);
void WaveSmoke(void);
void ImpactWaveSmoke(void);
void EnemySpawn(void);
void EnemyFade(void);
void NaaturalWaveSpawn(void);
void WaterIn(void);
void Move(void);
void EnemyKillCombo(void);

//==========================================================================
// パーティクルの初期化処理
//==========================================================================
HRESULT Init(void)
{

	// パーティクルの情報の初期化
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 位置
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 移動量
	m_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_fRadius = 0.0f;
	m_nLife = 0;
	m_nType = TYPE_NONE;

	return S_OK;
}

//==================================================================================
// パーティクルの設定処理
//==================================================================================
void my_particle::Create(D3DXVECTOR3 pos, TYPE nType)
{
	//return;

	Init();

	// 位置
	m_pos = pos;

	// 種類
	m_nType = nType;

	switch (m_nType)
	{
	case TYPE_EXPLOSION:
		m_nLife = 30;
		Smoke();
		SmokeRed();
		SmokeYel();
		break;

	case TYPE_SMOKE:

		m_nLife = 30;
		Smoke();
		break;

	case TYPE_SMOKE_RED:

		m_nLife = 30;
		SmokeRed();
		break;


	case TYPE_SMOKE_YEL:

		m_nLife = 30;
		SmokeYel();
		break;

	case TYPE_OFFSETTING:
		m_nLife = 30;
		OffSetting();
		break;

	case TYPE_MANYTXT_MINI:

		m_nLife = 30;
		ManyUI_Mini();
		break;

	case TYPE_RESULTMESSAGE:
		m_nLife = 30;
		ResultMessage();
		break;

	case TYPE_SUPERATTACK:
		m_nLife = 30;
		SuperAttack();
		break;

	case TYPE_CHECK:
		m_nLife = 10;
		Check();
		break;

	case TYPE_CHECK2D:
		m_nLife = 2;
		Check2D();
		break;

	case TYPE_ICE_SPAWN:
		m_nLife = 30;
		IceSpawn();
		break;

	case TYPE_SICKLELINE:
		m_nLife = 30;
		SickLine();
		break;

	case TYPE_MAGIC_EXPLOSION:
		m_nLife = 50;
		MagicExplosion();
		break;

	case TYPE_EXPLOSION_REMAIN:
		m_nLife = 180;
		ExplosionRemain();
		break;

	case TYPE_EXPLOSION_REMAIN_F:
		m_nLife = 100;
		ExplosionRemainFire();
		break;

	case TYPE_EXPLOSION_START:
		m_nLife = 70;
		ExplosionStart();
		break;

	case TYPE_WAVESMOKE:
		m_nLife = 40;
		WaveSmoke();
		break;

	case TYPE_IMPACTWAVESMOKE:
		m_nLife = 30;
		ImpactWaveSmoke();
		break;

	case TYPE_ENEMYSPAWN:
		m_nLife = 40;
		EnemySpawn();
		break;

	case TYPE_ENEMY_FADE:
		m_nLife = 120;
		EnemyFade();
		break;

	case TYPE_NATURALWAVE_SPAWN:
		m_nLife = 90;
		NaaturalWaveSpawn();
		break;

	case TYPE_WATERIN:
		m_nLife = 30;
		WaterIn();
		break;

	case TYPE_MOVESMOKE:
		m_nLife = 40;
		Move();
		break;

	case TYPE_ENEMYKILL_COMBO:
		m_nLife = 40;
		EnemyKillCombo();
		break;
	}
}

//==================================================================================
// チェック用
//==================================================================================
void Check(void)
{
	for (int nCntUse = 0; nCntUse < 1; nCntUse++)
	{

		float fMove = 0.0f;		// 移動量
		float fAngle = 0.0f;	// 角度

		// 移動量の設定
		m_move.x = sinf((float)(rand() % 629 - 314) / 100.0f) * fMove;
		m_move.y = cosf((float)(rand() % 629 - 314) / 100.0f) * fMove;
		m_move.z = cosf((float)(rand() % 629 - 314) / 100.0f) * fMove;

		m_col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);

		m_fRadius = 40.0f;

		// エフェクトの設定
		CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect3D::MOVEEFFECT_SUB, CEffect3D::TYPE_NORMAL);
	}
}


//==================================================================================
// チェック用
//==================================================================================
void Check2D(void)
{
	for (int nCntUse = 0; nCntUse < 5; nCntUse++)
	{

		float fMove = 0.0f;		// 移動量
		float fAngle = 0.0f;	// 角度

		// 移動量の設定
		m_move.x = 0.0f;
		m_move.y = 0.0f;
		m_move.z = 0.0f;

		m_col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);

		m_fRadius = 20.0f;

		// エフェクトの設定
		CEffect2D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect2D::MOVEEFFECT_NONE, CEffect2D::TYPE_NORMAL);
	}
}

//==================================================================================
// 煙のパーティクル
//==================================================================================
void Smoke(void)
{
	
	for (int nCntUse = 0; nCntUse < 3; nCntUse++)
	{
		m_fMove = (float)(Random(0, 10)) / 10 + 1.0f;		// 移動量
		m_fAngle = (float)(Random(-50, 50)) / 100.0f;	// 角度

		// 移動量の設定
		m_move.x = sinf((float)(Random(-314, 314)) / 100.0f) * m_fMove;
		m_move.y = cosf((float)(Random(-314, 314)) / 100.0f) * m_fMove;
		m_move.z = cosf((float)(Random(-314, 314)) / 100.0f) * m_fMove;

		m_pos.x += m_move.x * 0.3f;
		m_pos.y += m_move.y * 0.3f;

		m_col = D3DXCOLOR(0.7f, 0.7f, 0.7f, 0.5f);

		m_fRadius = 60.0f;

		if (rand() % 3 == 0)
		{
			m_fRadius *= 0.95f;
		}
		else if (rand() % 3 == 1)
		{
			m_fRadius *= 0.9f;
		}
		else if(rand() % 3 == 2)
		{
			m_fRadius *= 0.8f;
		}

		// エフェクトの設定
		CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect3D::MOVEEFFECT_ADD, CEffect3D::TYPE_SMOKEBLACK);
	}
}

//==================================================================================
// 煙のパーティクル(赤)
//==================================================================================
void SmokeRed(void)
{

	for (int nCntUse = 0; nCntUse < 10; nCntUse++)
	{
		m_fMove = (float)(Random(0, 10)) / 10 + 2.0f;		// 移動量
		m_fAngle = (float)(Random(-314, 314)) / 100.0f;	// 角度

		// 移動量の設定
		m_move.x = sinf((float)(Random(-314, 314)) / 100.0f) * m_fMove;
		m_move.y = cosf((float)(Random(-314, 314)) / 100.0f) * m_fMove;
		m_move.z = cosf((float)(Random(-314, 314)) / 100.0f) * m_fMove;

		m_pos.x += m_move.x * 0.3f;
		m_pos.y += m_move.y * 0.3f;

		m_col = D3DXCOLOR(1.0f, 0.4f, 0.3f, 1.0f);

		m_fRadius = 30.0f;

		if (rand() % 3 == 0)
		{
			m_fRadius *= 0.95f;
		}
		else if (rand() % 3 == 1)
		{
			m_fRadius *= 0.9f;
		}
		else if (rand() % 3 == 2)
		{
			m_fRadius *= 0.8f;
		}

		// エフェクトの設定
		CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect3D::MOVEEFFECT_ADD, CEffect3D::TYPE_SMOKEBLACK);
	}
}

//==================================================================================
// 煙のパーティクル(黄)
//==================================================================================
void SmokeYel(void)
{

	for (int nCntUse = 0; nCntUse < 10; nCntUse++)
	{
		m_fMove = (float)(Random(0, 10)) / 10 + 2.0f;		// 移動量
		m_fAngle = (float)(Random(-314, 314)) / 100.0f;	// 角度

		// 移動量の設定
		m_move.x = sinf((float)(Random(-314, 314)) / 100.0f) * m_fMove;
		m_move.y = cosf((float)(Random(-314, 314)) / 100.0f) * m_fMove;
		m_move.z = cosf((float)(Random(-314, 314)) / 100.0f) * m_fMove;

		m_pos.x += m_move.x * 0.3f;
		m_pos.y += m_move.y * 0.3f;

		m_col = D3DXCOLOR(0.8f, 0.5f, 0.0f, 1.0f);

		m_fRadius = 30.0f;

		if (rand() % 3 == 0)
		{
			m_fRadius *= 0.95f;
		}
		else if (rand() % 3 == 1)
		{
			m_fRadius *= 0.9f;
		}
		else if (rand() % 3 == 2)
		{
			m_fRadius *= 0.8f;
		}

		// エフェクトの設定
		CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect3D::MOVEEFFECT_ADD, CEffect3D::TYPE_SMOKEBLACK);
	}
}

//==================================================================================
//打ち消しのパーティクル
//==================================================================================
void OffSetting(void)
{
	float fAngle, fMove;

	for (int nCntUse = 0; nCntUse < 10; nCntUse++)
	{

		fMove = (float)(rand() % 10) / 1 + 3.0f;		// 移動量
		fAngle = (float)(rand() % 629 - 314) / 100.0f;	// 角度

		// 移動量の設定
		m_move.x = sinf((float)(rand() % 629 - 314) / 100.0f) * fMove;
		m_move.y = cosf((float)(rand() % 629 - 314) / 100.0f) * fMove;
		m_move.z = cosf((float)(rand() % 629 - 314) / 100.0f) * fMove;

		m_col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);

		m_fRadius = 40.0f;

		// エフェクトの設定
		if (CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect3D::MOVEEFFECT_SUB, CEffect3D::TYPE_NORMAL) == NULL)
		{
			return;
		}
	}
}

//==================================================================================
//  大量発生のパーティクル(ミニ)
//==================================================================================
void ManyUI_Mini(void)
{
	float m_fMove;
	m_fMove = (float)(rand() % 30) / 10 + 2.0f;		// 移動量

	for (int nCntCircle = 0; nCntCircle < 30; nCntCircle++)
	{
		float fRot = ((D3DX_PI * 2.0f) / (float)(15 % (nCntCircle + 1))) * nCntCircle;
		fRot -= D3DX_PI;

		for (int nCntUse = 0; nCntUse < 1; nCntUse++)
		{
			float fMooooooooove = (float)(Random(-15, 15) / 10.0f);		// バラバラ移動量

			float fRotRand = (float)(Random(-10, 10) / 100.0f);		// バラバラ向き

			// 移動量の設定
			m_move.x = sinf(fRot + fRotRand) * (m_fMove + fMooooooooove);
			m_move.z = 0.0f;
			m_move.y = cosf(fRot + fRotRand) * (m_fMove + fMooooooooove);

			//m_col = D3DXCOLOR(Random(0, 10) * 0.1f, Random(0, 10) * 0.1f, Random(0, 10) * 0.1f, 1.0f);
			m_col = D3DXCOLOR(0.9f, 0.5f, 0.1f, 1.0f);
																							
			m_fRadius = 40.0f;

			if (rand() % 3 == 0)
			{
				m_fRadius *= 0.95f;
			}
			else if (rand() % 3 == 1)
			{
				m_fRadius *= 0.98f;
			}
			else if (rand() % 3 == 2)
			{
				m_fRadius *= 0.92f;
			}

			// エフェクトの設定
			CEffect2D::Create(m_pos,
				m_move,
				m_col,
				m_fRadius,
				m_nLife,
				CEffect2D::MOVEEFFECT_NONE, CEffect2D::TYPE_NORMAL);
		}
	}
}

//==================================================================================
// 結果のメッセージのパーティクル
//==================================================================================
void ResultMessage(void)
{
	float fMove;
	fMove = (float)Random(0, 30) / 10 + 5.0f;		//移動量

	float fff = (D3DX_PI * 2.0f) / 40;

	for (int nCntCircle = 0; nCntCircle < 40; nCntCircle++)
	{
		float fRot = fff * nCntCircle;

		for (int nCntUse = 0; nCntUse < 10; nCntUse++)
		{
			float fMooooooooove = (float)Random(-15, 15) / 10;		//バラバラ移動量

			float fRotRand = (float)Random(-10, 10) / 100.0f;		//バラバラ向き

			//移動量の設定
			m_move.x = sinf(fRot + fRotRand) * (fMove + fMooooooooove);
			m_move.z = 0.0f;
			m_move.y = cosf(fRot + fRotRand) * (fMove + fMooooooooove);

			m_col = D3DXCOLOR(0.9f, 0.5f, 0.1f, 1.0f);

			m_fRadius = 15.0f;

			if (rand() % 3 == 0)
			{
				m_fRadius *= 0.95f;
			}
			else if (rand() % 3 == 1)
			{
				m_fRadius *= 0.98f;
			}
			else if (rand() % 3 == 2)
			{
				m_fRadius *= 0.92f;
			}

			//エフェクトの設定
			CEffect2D::Create(m_pos,
				m_move,
				m_col,
				m_fRadius,
				m_nLife,
				CEffect2D::MOVEEFFECT_NONE, CEffect2D::TYPE_NORMAL);
		}
	}


	for (int nCntUse = 0; nCntUse < 10; nCntUse++)
	{
		fMove = (float)(rand() % 50) * 0.1f;		//移動量

		//移動量の設定
		m_move.x = sinf((float)(Random(-314, 314)) / 100.0f) * fMove;
		m_move.y = sinf((float)(Random(-314, 314)) / 100.0f) * fMove;
		m_move.z = 0.0f;

		//出現地点のブレ
		m_pos.x += m_move.x * 0.1f;
		m_pos.y += m_move.z * 0.1f;
		m_pos.z = 0;

		m_col = D3DXCOLOR(0.9f, 0.6f, 0.3f, 0.8f);

		m_fRadius = 200.0f;

		if (rand() % 3 == 0)
		{
			m_fRadius *= 0.95f;
		}
		else if (rand() % 3 == 1)
		{
			m_fRadius *= 0.98f;
		}
		else if (rand() % 3 == 2)
		{
			m_fRadius *= 0.92f;
		}

		//エフェクトの設定
		CEffect2D::Create(m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect2D::MOVEEFFECT_ADD, CEffect2D::TYPE_JUJI);
	}
}

//==================================================================================
// 強力攻撃
//==================================================================================
void SuperAttack(void)
{
	for (int nCntUse = 0; nCntUse < 40; nCntUse++)
	{
		float fMove = (float)Random(0, 30) * 0.1f;	// 移動量
		float fMoveY = (float)Random(0, 50) * 0.1f;	// 移動量

		// 移動量の設定
		m_move.x = sinf((float)Random(-314, 314) / 100.0f) * fMove;
		m_move.y = cosf((float)Random(-20, 20) / 100.0f) * fMoveY;
		m_move.z = cosf((float)Random(-314, 314) / 100.0f) * fMove;

		m_col = D3DXCOLOR(1.0f, 0.6f, 0.3f, 1.0f);

		m_pos.x += m_move.x * 0.1f;
		m_pos.z += m_move.z * 0.1f;

		m_fRadius = 60.0f + (float)Random(-50, 100) * 0.1f;

		// エフェクトの設定
		CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect3D::MOVEEFFECT_SUB, CEffect3D::TYPE_SMOKE);
	}

	for (int nCntUse = 0; nCntUse < 20; nCntUse++)
	{
		float fMove = (float)Random(0, 25) * 0.1f;	// 移動量
		float fMoveY = (float)Random(0, 40) * 0.1f;	// 移動量

		// 移動量の設定
		m_move.x = sinf((float)Random(-314, 314) / 100.0f) * fMove;
		m_move.y = cosf((float)Random(-20, 20) / 100.0f) * fMoveY;
		m_move.z = cosf((float)Random(-314, 314) / 100.0f) * fMove;

		m_col = D3DXCOLOR(0.4f, 0.1f, 0.0f, 0.6f);

		m_pos.x += m_move.x * 0.5f;
		m_pos.z += m_move.z * 0.5f;

		m_fRadius = 40.0f + (float)Random(-50, 100) * 0.1f;

		// エフェクトの設定
		CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect3D::MOVEEFFECT_SUB, CEffect3D::TYPE_SMOKEBLACK);
	}
}

//==========================================================================
// 氷出現
//==========================================================================
void IceSpawn(void)
{
	for (int nCntUse = 0; nCntUse < 2; nCntUse++)
	{
		float fMove = 0.0f;		// 移動量

		// 移動量の設定
		m_move.x = sinf((float)Random(-314, 314) / 100.0f) * fMove;
		m_move.y = cosf((float)Random(-314, 314) / 100.0f) * fMove;
		m_move.z = cosf((float)Random(-314, 314) / 100.0f) * fMove;

		// 色設定
		m_col = D3DXCOLOR(0.2f, 0.2f, 1.0f, 1.0f);

		// 半径設定
		m_fRadius = 120.0f;

		// エフェクトの設定
		CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect3D::MOVEEFFECT_SUB, CEffect3D::TYPE_JUJI);
	}
}

//==========================================================================
// 大鎌のライン
//==========================================================================
void SickLine(void)
{
	for (int nCntUse = 0; nCntUse < 2; nCntUse++)
	{
		float fMove = 0.0f;		// 移動量

		// 移動量の設定
		m_move.x = 0.0f;
		m_move.y = 0.0f;
		m_move.z = 0.0f;

		// 色設定
		m_col = D3DXCOLOR(1.0f, 0.3f, 0.1f, 0.4f);

		// 半径設定
		m_fRadius = 30.0f;

		// エフェクトの設定
		CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect3D::MOVEEFFECT_SUB, CEffect3D::TYPE_JUJI);
	}
}

//==========================================================================
// 魔法の爆発
//==========================================================================
void MagicExplosion(void)
{

	float fff = (D3DX_PI * 2.0f) / 16;

	// 煙
	for (int nCntUse = 0; nCntUse < 16; nCntUse++)
	{
		float fRot = fff * nCntUse;

		float fMove = (float)Random(0, 100) * 0.1f;		// 移動量
		float fMoveY = (float)Random(0, 100) * 0.01f;		// 移動量

		// 移動量の設定
		m_move.x = sinf(fRot) * fMove;
		m_move.y = cosf((float)Random(-314, 314) / 100.0f) * fMoveY;
		m_move.z = cosf(fRot) * fMove;

		// 色設定
		m_col = D3DXCOLOR(0.4f, 0.4f, 0.4f, 0.8f);

		// 半径設定
		m_fRadius = 80.0f;

		// エフェクトの設定
		CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect3D::MOVEEFFECT_ADD, CEffect3D::TYPE_SMOKEBLACK);
	}
}

//==========================================================================
// 爆発の残り
//==========================================================================
void ExplosionRemain(void)
{
	for (int nCntUse = 0; nCntUse < 2; nCntUse++)
	{
		float fMove = (float)Random(0, 150) * 0.01f;			// 移動量
		float fMoveY = (float)Random(0, 120) * 0.01f + 2.0f;	// 移動量

		// 移動量の設定
		m_move.x = sinf((float)Random(-314, 314) * 0.01f) * fMove;
		m_move.y = sinf((float)Random(0, 314) * 0.01f) * fMoveY;
		m_move.z = cosf((float)Random(-314, 314) * 0.01f) * fMove;

		D3DXVECTOR3 pos = m_pos;
		pos.x += Random(-150, 150);
		pos.z += Random(-150, 150);

		// 色設定
		m_col = D3DXCOLOR(1.0f, 0.4f, 0.1f, 1.0f);

		// 半径設定
		m_fRadius = 10.0f;

		// エフェクトの設定
		CEffect3D::Create(
			pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect3D::MOVEEFFECT_SUB, CEffect3D::TYPE_NORMAL);
	}
}

//==========================================================================
// 魔法の爆発の残り,火
//==========================================================================
void ExplosionRemainFire(void)
{
	// 火
	for (int nCntUse = 0; nCntUse < 2; nCntUse++)
	{
		float fMove = (float)Random(0, 50) * 0.05f;		// 移動量
		float fMoveY = (float)Random(0, 200) * 0.01f;		// 移動量

		// 移動量の設定
		m_move.x = sinf((float)Random(-314, 314) * 0.01f) * fMove;
		m_move.y = cosf((float)Random(0, 314) / 100.0f) * fMoveY;
		m_move.z = cosf((float)Random(-314, 314) * 0.01f) * fMove;

		// 色設定
		m_col = D3DXCOLOR(1.0f, 0.1f, 0.1f, 1.0f);

		// 半径設定
		m_fRadius = 10.0f;

		// エフェクトの設定
		CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect3D::MOVEEFFECT_NONE, CEffect3D::TYPE_SMOKE);
	}
}

//==========================================================================
// 
//==========================================================================
void ExplosionStart(void)
{
	for (int nCntUse = 0; nCntUse < 36; nCntUse++)
	{
		float fMove = (float)Random(0, 150) * 0.01f;			// 移動量
		float fMoveY = (float)Random(70, 180) * 0.1f + 5.0f;	// 移動量

		// 移動量の設定
		m_move.x = sinf((float)Random(-314, 314) * 0.01f) * fMove;
		m_move.y = sinf((float)Random(0, 314) * 0.01f) * fMoveY;
		m_move.z = cosf((float)Random(-314, 314) * 0.01f) * fMove;

		D3DXVECTOR3 pos = m_pos;
		pos.x += Random(-50, 50);
		pos.z += Random(-50, 50);

		// 色設定
		m_col = D3DXCOLOR(1.0f, Random(0, 40) * 0.01f, 0.1f, 1.0f);

		// 半径設定
		m_fRadius = 200.0f;

		// エフェクトの設定
		CEffect3D::Create(
			pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect3D::MOVEEFFECT_ADD, CEffect3D::TYPE_SMOKE);
	}
}

//==========================================================================
// 波の煙
//==========================================================================
void WaveSmoke(void)
{
#if MODE_SCREENSHOT
	for (int nCntUse = 0; nCntUse < 1; nCntUse++)
	{
		float fMove = (float)Random(0, 50) * 0.15f;		// 移動量
		float fMoveY = (float)Random(10, 50) * 0.2f;		// 移動量

		// 移動量の設定
		m_move.x = sinf((float)Random(-314, 314) * 0.01f) * fMove;
		m_move.y = sinf((float)Random(0, 314) / 100.0f) * fMoveY;
		m_move.z = cosf((float)Random(-314, 314) * 0.01f) * fMove;

		// 色設定
		m_col = D3DXCOLOR(0.6f, 0.6f, 0.3f, 0.6f);

		// 半径設定
		m_fRadius = 80.0f;

		// エフェクトの設定
		CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect3D::MOVEEFFECT_ADD, CEffect3D::TYPE_SMOKEBLACK);
	}

	for (int nCntUse = 0; nCntUse < 1; nCntUse++)
	{
		float fMove = (float)Random(0, 50) * 0.15f;		// 移動量
		float fMoveY = (float)Random(10, 50) * 0.1f;		// 移動量

		// 移動量の設定
		m_move.x = sinf((float)Random(-314, 314) * 0.01f) * fMove;
		m_move.y = sinf((float)Random(0, 314) / 100.0f) * fMoveY;
		m_move.z = cosf((float)Random(-314, 314) * 0.01f) * fMove;

		// 色設定
		m_col = D3DXCOLOR(0.6f, 0.4f, 0.0f, 0.7f);

		// 半径設定
		m_fRadius = 20.0f;

		// エフェクトの設定
		CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect3D::MOVEEFFECT_ADD, CEffect3D::TYPE_SANDBLACK);
	}
#else
	for (int nCntUse = 0; nCntUse < 1; nCntUse++)
	{
		float fMove = (float)Random(0, 50) * 0.5f;		// 移動量
		float fMoveY = (float)Random(10, 50) * 0.5f;		// 移動量

															// 移動量の設定
		m_move.x = sinf((float)Random(-314, 314) * 0.01f) * fMove;
		m_move.y = sinf((float)Random(0, 314) / 100.0f) * fMoveY;
		m_move.z = cosf((float)Random(-314, 314) * 0.01f) * fMove;

		// 色設定
		m_col = D3DXCOLOR(0.6f, 0.6f, 0.3f, 0.6f);

		// 半径設定
		m_fRadius = 40.0f;

		// エフェクトの設定
		CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect3D::MOVEEFFECT_ADD, CEffect3D::TYPE_SMOKEBLACK);
	}

	for (int nCntUse = 0; nCntUse < 1; nCntUse++)
	{
		float fMove = (float)Random(0, 50) * 0.5f;		// 移動量
		float fMoveY = (float)Random(10, 50) * 0.5f;		// 移動量

															// 移動量の設定
		m_move.x = sinf((float)Random(-314, 314) * 0.01f) * fMove;
		m_move.y = sinf((float)Random(0, 314) / 100.0f) * fMoveY;
		m_move.z = cosf((float)Random(-314, 314) * 0.01f) * fMove;

		// 色設定
		m_col = D3DXCOLOR(0.4f, 0.4f, 0.0f, 0.7f);

		// 半径設定
		m_fRadius = 10.0f;

		// エフェクトの設定
		CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect3D::MOVEEFFECT_ADD, CEffect3D::TYPE_SANDBLACK);
	}
#endif
}


//==========================================================================
// 円形波の煙
//==========================================================================
void ImpactWaveSmoke(void)
{
	for (int nCntUse = 0; nCntUse < 1; nCntUse++)
	{
		float fMove = (float)Random(0, 50) * 0.35f;		// 移動量
		float fMoveY = (float)Random(10, 50) * 0.1f;		// 移動量

		// 移動量の設定
		m_move.x = sinf((float)Random(-314, 314) * 0.01f) * fMove;
		m_move.y = sinf((float)Random(0, 314) / 100.0f) * fMoveY;
		m_move.z = cosf((float)Random(-314, 314) * 0.01f) * fMove;

		// 色設定
		m_col = D3DXCOLOR(0.6f, 0.6f, 0.3f, 0.6f);

		// 半径設定
		m_fRadius = 40.0f;

		// エフェクトの設定
		CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect3D::MOVEEFFECT_ADD, CEffect3D::TYPE_SMOKEBLACK);
	}

	for (int nCntUse = 0; nCntUse < 1; nCntUse++)
	{
		float fMove = (float)Random(0, 50) * 0.35f;		// 移動量
		float fMoveY = (float)Random(10, 50) * 0.1f;		// 移動量

		// 移動量の設定
		m_move.x = sinf((float)Random(-314, 314) * 0.01f) * fMove;
		m_move.y = sinf((float)Random(0, 314) / 100.0f) * fMoveY;
		m_move.z = cosf((float)Random(-314, 314) * 0.01f) * fMove;

		// 色設定
		m_col = D3DXCOLOR(0.6f, 0.4f, 0.0f, 0.7f);

		// 半径設定
		m_fRadius = 10.0f;

		// エフェクトの設定
		CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect3D::MOVEEFFECT_ADD, CEffect3D::TYPE_SANDBLACK);
	}
}

//==========================================================================
// 敵スポーン
//==========================================================================
void EnemySpawn(void)
{
#if 0
	for (int nCntUse = 0; nCntUse < 15; nCntUse++)
	{
		float fMoveXZ = (float)Random(0, 200) * 0.01f;		// 移動量
		float fMove = (float)Random(0, 60) * 0.1f + 1.0f;	// 移動量

		// 移動量の設定
		m_move.x = sinf((float)Random(-314, 314) / 100.0f) * fMoveXZ;
		m_move.y = sinf((float)Random(0, 314) / 100.0f) * fMove;
		m_move.z = cosf((float)Random(-314, 314) / 100.0f) * fMoveXZ;

		// 出現地点のブレ
		m_pos.y += m_move.y * 0.1f;

		m_fRadius = 80.0f;
		m_col = D3DXCOLOR(0.8f, 0.2f, 0.8f, 1.0f);

		// エフェクトの設定
		CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect3D::MOVEEFFECT_SUB, CEffect3D::TYPE_NORMAL);
	}

	for (int nCntUse = 0; nCntUse < 10; nCntUse++)
	{
		float fMoveXZ = (float)Random(0, 200) * 0.01f;	// 移動量
		float fMove = (float)Random(0, 60) * 0.1f + 1.0f;	// 移動量

		// 移動量の設定
		m_move.x = sinf((float)Random(-314, 314) / 100.0f) * fMoveXZ;
		m_move.y = sinf((float)Random(0, 314) / 100.0f) * fMove;
		m_move.z = cosf((float)Random(-314, 314) / 100.0f) * fMoveXZ;

		// 出現地点のブレ
		m_pos.y += m_move.y * 0.1f;

		m_fRadius = 50.0f;
		m_col = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);

		// エフェクトの設定
		CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect3D::MOVEEFFECT_SUB, CEffect3D::TYPE_BLACK);
	}
#else
	for (int nCntUse = 0; nCntUse < 12; nCntUse++)
	{
		float fMoveXZ = (float)Random(0, 200) * 0.01f;		// 移動量
		float fMove = (float)Random(0, 120) * 0.1f + 5.0f;	// 移動量

		// 移動量の設定
		m_move.x = sinf((float)Random(-314, 314) * 0.01f) * fMoveXZ;
		m_move.y = sinf((float)Random(0, 314) * 0.01f) * fMove;
		m_move.z = cosf((float)Random(-314, 314) * 0.01f) * fMoveXZ;

		// 出現地点のブレ
		m_pos.y += m_move.y * 0.1f;

		m_fRadius = 120.0f;
		m_col = D3DXCOLOR(0.6f, 0.6f, 0.3f, 0.6f);

		// エフェクトの設定
		CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect3D::MOVEEFFECT_ADD, CEffect3D::TYPE_SMOKEBLACK);
	}
#endif
}

//==========================================================================
// 敵フェード
//==========================================================================
void EnemyFade(void)
{
	for (int nCntUse = 0; nCntUse < 15; nCntUse++)
	{
		float fMoveXZ = (float)Random(0, 100) * 0.01f;		// 移動量
		float fMove = (float)Random(0, 60) * 0.1f + 1.0f;	// 移動量

		// 移動量の設定
		m_move.x = sinf((float)Random(-314, 314) / 100.0f) * fMoveXZ;
		m_move.y = sinf((float)Random(0, 314) / 100.0f) * fMove;
		m_move.z = cosf((float)Random(-314, 314) / 100.0f) * fMoveXZ;

		// 出現地点のブレ
		m_pos.y += Random(-100, 100) * 0.1f;
		m_pos.x += Random(-100, 100) * 0.1f;
		m_pos.z += Random(-100, 100) * 0.1f;

		m_fRadius = 20.0f + Random(-50, 50) * 0.1f;

		// 色決め
		int nRand = 0;
		//int nRand = Random(0, 1);
		switch (nRand)
		{
		case 0:
			m_col = D3DXCOLOR(
				0.2f + Random(-100, 100) * 0.001f,
				0.8f + Random(-200, 200) * 0.001f,
				0.2f + Random(-100, 100) * 0.001f + Random(0, 20) * 0.01f, 1.0f);
			break;

		case 1:
			m_col = D3DXCOLOR(
				0.8f + Random(-50, 50) * 0.001f,
				0.2f + Random(-50, 50) * 0.001f,
				0.8f + Random(-50, 50) * 0.001f, 1.0f);
			break;

		default:
			break;
		}

		// エフェクトの設定
		CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect3D::MOVEEFFECT_SUB, CEffect3D::TYPE_NORMAL);
	}
}

//==========================================================================
// 自然波出現
//==========================================================================
void NaaturalWaveSpawn(void)
{
	for (int nCntUse = 0; nCntUse < 4; nCntUse++)
	{
		float fMoveXZ = (float)Random(0, 200) * 0.01f;		// 移動量
		float fMove = (float)Random(0, 60) * 0.1f + 1.0f;	// 移動量

		// 移動量の設定
		m_move.x = sinf((float)Random(-314, 314) / 100.0f) * fMoveXZ;
		m_move.y = sinf((float)Random(0, 314) / 100.0f) * fMove;
		m_move.z = cosf((float)Random(-314, 314) / 100.0f) * fMoveXZ;

		// 出現地点のブレ
		m_pos.y += m_move.y * 0.1f;

		m_fRadius = 60.0f;
		m_col = D3DXCOLOR(0.8f, 0.6f, 0.1f, 1.0f);

		// エフェクトの設定
		CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect3D::MOVEEFFECT_ADD, CEffect3D::TYPE_SMOKE);
	}
}

//==========================================================================
// 水は行ったとき
//==========================================================================
void WaterIn(void)
{
	for (int nCntUse = 0; nCntUse < 10; nCntUse++)
	{
		float fMoveXZ = (float)Random(0, 100) * 0.01f;		// 移動量
		float fMove = (float)Random(10, 30) * 0.1f;	// 移動量

		// 移動量の設定
		m_move.x = sinf((float)Random(-314, 314) / 100.0f) * fMoveXZ;
		m_move.y = sinf((float)Random(0, 314) / 100.0f) * fMove;
		m_move.z = cosf((float)Random(-314, 314) / 100.0f) * fMoveXZ;

		// 出現地点のブレ
		m_pos.y += m_move.y * 0.1f;

		m_fRadius = 5.0f;
		m_col = D3DXCOLOR(
			0.2f + Random(-100, 100) * 0.001f,
			0.2f + Random(-100, 100) * 0.001f,
			0.8f + Random(-200, 200) * 0.001f, 0.3f);

		// エフェクトの設定
		CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect3D::MOVEEFFECT_SUB, CEffect3D::TYPE_NORMAL);
	}
}

//==========================================================================
// 移動の煙
//==========================================================================
void Move(void)
{
	for (int nCntUse = 0; nCntUse < 4; nCntUse++)
	{
		float fMoveXZ = (float)Random(0, 20) * 0.1f;		// 移動量
		float fMove = (float)Random(0, 20) * 0.1f;	// 移動量

		// 移動量の設定
		m_move.x = sinf((float)Random(-314, 314) / 100.0f) * fMoveXZ;
		m_move.y = sinf((float)Random(0, 314) / 100.0f) * fMove;
		m_move.z = cosf((float)Random(-314, 314) / 100.0f) * fMoveXZ;

		// 出現地点のブレ
		m_pos.y += m_move.y * 0.1f;

		m_fRadius = 20.0f;
		m_col = D3DXCOLOR(
			0.6f + Random(-50, 50) * 0.001f,
			0.6f + Random(-50, 50) * 0.001f,
			0.3f + Random(-50, 50) * 0.001f, 0.5f);

		// エフェクトの設定
		CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect3D::MOVEEFFECT_SUB, CEffect3D::TYPE_SMOKEBLACK);
	}
}

//==========================================================================
// コンボで敵を倒した時
//==========================================================================
void EnemyKillCombo(void)
{
	int nMax = 8;
	float fff = (D3DX_PI * 2.0f) / nMax;

	// 煙
	for (int nCntUse = 0; nCntUse < nMax; nCntUse++)
	{
		float fRot = fff * nCntUse;

		float fMove = (float)Random(150, 250) * 0.1f;		// 移動量
		float fMoveY = (float)Random(150, 250) * 0.01f;		// 移動量

		// 移動量の設定
		m_move.x = sinf(fRot) * fMove;
		m_move.y = cosf((float)Random(-314, 314) / 100.0f) * fMoveY;
		m_move.z = cosf(fRot) * fMove;

		// 色設定
		//m_col = D3DXCOLOR(0.6f, 0.3f, 0.6f, 0.8f);

		m_col = D3DXCOLOR(
			0.6f + Random(-100, 100) * 0.001f,
			0.3f + Random(-100, 100) * 0.001f,
			0.6f + Random(-100, 100) * 0.001f, 0.8f);

		// 半径設定
		m_fRadius = 20.0f;

		// エフェクトの設定
		CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect3D::MOVEEFFECT_ADD, CEffect3D::TYPE_SMOKEBLACK,
			m_fRadius * 0.01f);
	}

	for (int nCntUse = 0; nCntUse < nMax; nCntUse++)
	{
		float fRot = fff * nCntUse;

		float fMove = (float)Random(150, 250) * 0.1f;		// 移動量
		float fMoveY = (float)Random(150, 250) * 0.01f;		// 移動量

		// 移動量の設定
		m_move.x = sinf(fRot) * fMove;
		m_move.y = cosf((float)Random(-314, 314) / 100.0f) * fMoveY;
		m_move.z = cosf(fRot) * fMove;

		m_col = D3DXCOLOR(
			0.2f + Random(-100, 100) * 0.001f,
			0.8f + Random(-200, 200) * 0.001f,
			0.2f + Random(-100, 100) * 0.001f + Random(0, 20) * 0.01f, 1.0f);

		// 半径設定
		m_fRadius = 20.0f;

		// エフェクトの設定
		CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect3D::MOVEEFFECT_ADD, CEffect3D::TYPE_JUJI,
			m_fRadius * 0.1f);
	}
}
