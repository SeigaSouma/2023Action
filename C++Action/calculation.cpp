//==================================================================================
// 
// 便利関数[calculation.cpp]
// Author:相馬靜雅
// 
//==================================================================================
#include "calculation.h"

// グローバル変数宣言
MOVELOG g_aMoveLog[MOVE_LOG];		// ロゴ拡縮の構造体

//==================================================================================
// 初期化処理
//==================================================================================
void InitCalculastion(void)
{
	// ロゴ拡縮
	for (int nCntLog = 0; nCntLog < MOVE_LOG; nCntLog++)
	{
		g_aMoveLog[nCntLog].nCnt = 0;
		g_aMoveLog[nCntLog].nPattern = 0;
		g_aMoveLog[nCntLog].Length = 0.0f;
		g_aMoveLog[nCntLog].bUse = false;
	}
}

//==================================================================================
// ロゴの設定
//==================================================================================
int SetMoveLog(void)
{
	int nIdxMoveLog = -1;

	for (int nCntLog = 0; nCntLog < MOVE_LOG; nCntLog++)
	{
		if (g_aMoveLog[nCntLog].bUse == false)
		{// 使用されていなかったら

		 // インデックス保持して処理抜け
			g_aMoveLog[nCntLog].bUse = true;
			nIdxMoveLog = nCntLog;
			break;
		}
	}

	return nIdxMoveLog;
}

//==================================================================================
// ロゴの相対座標算出
//==================================================================================
float MoveLog(int nIdxMoveLog, float pos, int nCntElapsed)
{

	// カウント加算
	g_aMoveLog[nIdxMoveLog].nCnt = (g_aMoveLog[nIdxMoveLog].nCnt + 1) % nCntElapsed;

	if (g_aMoveLog[nIdxMoveLog].nPattern == MOVELOG_TYPE_ADD)
	{// 拡大

		if ((g_aMoveLog[nIdxMoveLog].nCnt % nCntElapsed) == 0)
		{
			g_aMoveLog[nIdxMoveLog].nPattern = (g_aMoveLog[nIdxMoveLog].nPattern + 1) % MOVELOG_TYPE_MAX;	// パターンNo.を更新
		}

		g_aMoveLog[nIdxMoveLog].Length = pos + g_aMoveLog[nIdxMoveLog].nCnt * 6.0f;
	}
	else if (g_aMoveLog[nIdxMoveLog].nPattern == MOVELOG_TYPE_REMOVE)
	{// 縮小

		if ((g_aMoveLog[nIdxMoveLog].nCnt % nCntElapsed) == 0)
		{
			g_aMoveLog[nIdxMoveLog].nPattern = (g_aMoveLog[nIdxMoveLog].nPattern + 1) % MOVELOG_TYPE_MAX;	// パターンNo.を更新
		}

		g_aMoveLog[nIdxMoveLog].Length = pos - g_aMoveLog[nIdxMoveLog].nCnt * 6.0f;
	}
	else if (g_aMoveLog[nIdxMoveLog].nPattern == MOVELOG_TYPE_SAME)
	{// 戻す

		// 拡大する
		g_aMoveLog[nIdxMoveLog].Length = pos + g_aMoveLog[nIdxMoveLog].nCnt * 6.0f;

		if (g_aMoveLog[nIdxMoveLog].Length >= pos)
		{// 規定値以上

		 // 規定値に戻す
			g_aMoveLog[nIdxMoveLog].Length = pos;
		}
	}

	return g_aMoveLog[nIdxMoveLog].Length;
}

//==================================================================================
// ロゴのリセット
//==================================================================================
void ResetMoveLog(int nIdxMoveLog)
{
	g_aMoveLog[nIdxMoveLog].nCnt = 0;
	g_aMoveLog[nIdxMoveLog].Length = 0.0f;
	g_aMoveLog[nIdxMoveLog].nPattern = MOVELOG_TYPE_ADD;
}

//==================================================================================
// 角度の正規化処理
//==================================================================================
void RotNormalize(float &fRot)
{
	while (1)
	{
		// 1周分越えたら補正する
		if (fRot < -D3DX_PI)
		{
			fRot += D3DX_PI * 2.0f;
		}
		else if (fRot > D3DX_PI)
		{
			fRot -= D3DX_PI * 2.0f;
		}

		if (fRot >= -D3DX_PI && fRot <= D3DX_PI)
		{// 全て正規化済みでOK
			break;
		}
	}
}

//==================================================================================
// 角度の正規化処理
//==================================================================================
void RotNormalize(D3DXVECTOR3 &rot)
{
	// それぞれの正規化
	RotNormalize(rot.x);
	RotNormalize(rot.y);
	RotNormalize(rot.z);
}

//==================================================================================
// 慣性補正
//==================================================================================
void InertiaCorrection(float &fMain, float fTarget, float fCntCorrection)
{
	fMain += (fTarget - fMain) * fCntCorrection;
}

//==================================================================================
// 線形補正
//==================================================================================
float Lerp(float start, float end, float time)
{
	// 範囲外補正
	ValueNormalize(time, 1.0f, 0.0f);
	float fValue = start + time * (end - start);

	return fValue;
}

//==================================================================================
// ランダム
//==================================================================================
int Random(int nMinNum, int nMaxNum)
{
	// ランダム数
	int nRand = 0;

	// 範囲
	int nRange = nMaxNum - nMinNum;

	if (nMinNum == 0)
	{// 最小値が0の時

	 // 範囲に1加算
		nRange += 1;
	}

	// ランダム生成
	nRand = rand() % nRange + nMinNum;

	return nRand;
}

//==================================================================================
// 桁数取得
//==================================================================================
int GetDigit(int nNum)
{
	if (nNum <= 0)
	{
		nNum = 0;
	}

	// 整数を文字列に変換
	std::string number;
	sprintf((char*)number.c_str(), "%d", nNum);

	// 文字列の長さ（桁数）を取得
	int nDigit = strlen(number.c_str());

	if (nDigit <= 0)
	{
		nDigit = 1;
	}

	return nDigit;
}

//==================================================================================
// 割合取得
//==================================================================================
float GetRatio(D3DXVECTOR3 Mypos, D3DXVECTOR3 Targetpos)
{
	return sqrtf((Mypos.x - Targetpos.x) * (Mypos.x - Targetpos.x)
			+ (Mypos.z - Targetpos.z) * (Mypos.z - Targetpos.z));
}

//==================================================================================
// 長さ取得
//==================================================================================
float GetPosLength(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2)
{
	return sqrtf((pos1.x - pos2.x) * (pos1.x - pos2.x)
		+ (pos1.z - pos2.z) * (pos1.z - pos2.z));
}

//==================================================================================
// 円の当たり判定
//==================================================================================
bool CircleRange(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2, float fRadius1, float fRadius2)
{
	float fLength = 
		(pos1.x - pos2.x) * (pos1.x - pos2.x) +
		(pos1.z - pos2.z) * (pos1.z - pos2.z);

	if (fLength <= (fRadius1 + fRadius2) * (fRadius1 + fRadius2))
	{// 円の中に入ったら

		// 当たった判定返す
		return true;
	}

	return false;
}

//==================================================================================
// 球の当たり判定
//==================================================================================
bool SphereRange(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2, float fRadius1, float fRadius2)
{
	float fLength = 
		(pos1.x - pos2.x) * (pos1.x - pos2.x) +
		(pos1.y - pos2.y) * (pos1.y - pos2.y) +
		(pos1.z - pos2.z) * (pos1.z - pos2.z);

	if (fLength <= (fRadius1 + fRadius2) * (fRadius1 + fRadius2))
	{// 円の中に入ったら

		// 当たった判定返す
		return true;
	}

	return false;
}

//==================================================================================
// 1と2の当たり判定(筒)
//==================================================================================
bool CylinderRange(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2, float fRadius1, float fRadius2, float fHeight1, float fHeight2)
{
	float fLength = (pos1.x - pos2.x) * (pos1.x - pos2.x)
		+ (pos1.z - pos2.z) * (pos1.z - pos2.z);

	if (fLength <= (fRadius1 * fRadius2) + (fRadius1 * fRadius2))
	{// 円の中に入ったら

		if (pos1.y - fHeight1 <= pos2.y + fHeight2)
		{// 高さもぶつかっていたら

			// 当たった判定返す
			return true;
		}
	}

	return false;
}

//==================================================================================
// 全頂点チェック
//==================================================================================
void CheckVtx(float RotY, D3DXVECTOR3 *pVtxMax, D3DXVECTOR3 *pVtxMin, LPD3DXMESH pMesh, BYTE *pVtxBuff)
{
	//リセット
	*pVtxMin = D3DXVECTOR3(10000.0f, 10000.0f, 10000.0f);
	*pVtxMax = D3DXVECTOR3(-10000.0f, -10000.0f, -10000.0f);

	//モデルの頂点数を取得
	int nNumVtx = pMesh->GetNumVertices();

	//頂点フォーマットのサイズを取得
	DWORD dwSizeFVF = D3DXGetFVFVertexSize(pMesh->GetFVF());

	//頂点バッファをロック
	pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

	//全ての頂点をチェックする
	for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
	{
		//今回のデータ
		D3DXVECTOR3 vtx = *(D3DXVECTOR3*)pVtxBuff;	//頂点座標の代入

		//X
		if (pVtxMin->x > vtx.x)
		{//今の最小値よりも今回の値がちいさかったら

			pVtxMin->x = vtx.x;
		}
		if (pVtxMax->x < vtx.x)
		{//今の最大値よりも今回の値が大きかったら

			pVtxMax->x = vtx.x;
		}

		//Y
		if (pVtxMin->y > vtx.y)
		{//今の最小値よりも今回の値がちいさかったら

			pVtxMin->y = vtx.y;
		}
		if (pVtxMax->y < vtx.y)
		{//今の最大値よりも今回の値が大きかったら

			pVtxMax->y = vtx.y;
		}

		//Z
		if (pVtxMin->z > vtx.z)
		{//今の最小値よりも今回の値がちいさかったら

			pVtxMin->z = vtx.z;
		}
		if (pVtxMax->z < vtx.z)
		{//今の最大値よりも今回の値が大きかったら

			pVtxMax->z = vtx.z;
		}

		pVtxBuff += dwSizeFVF;	//サイズ分ポインタ移動
	}

	//頂点バッファをアンロック
	pMesh->UnlockVertexBuffer();

	D3DXVECTOR3 VtxMax = *pVtxMax, VtxMin = *pVtxMin;

	if (RotY <= D3DX_PI * 0.5f + 0.05f && RotY >= D3DX_PI * 0.5f - 0.05f)
	{//1.57fのとき

		pVtxMax->x = VtxMax.z;
		pVtxMin->x = VtxMin.z;

		pVtxMax->z = -VtxMin.x;
		pVtxMin->z = -VtxMax.x;
	}
	else if (RotY <= -D3DX_PI * 0.5f + 0.05f && RotY >= -D3DX_PI * 0.5f - 0.05f)
	{//-1.57fのとき
		
		////////////////////////////////

		pVtxMax->x = -VtxMin.z;
		pVtxMin->x = -VtxMax.z;

		pVtxMax->z = VtxMax.x;
		pVtxMin->z = VtxMin.x;
	}
	else if (RotY <= D3DX_PI + 0.05f && RotY >= D3DX_PI - 0.05f)
	{//D3DX_PIのとき

	 ////////////////////////////////

		pVtxMax->x = -VtxMin.x;
		pVtxMin->x = -VtxMax.x;

		pVtxMax->z = -VtxMin.z;
		pVtxMin->z = -VtxMax.z;
	}
	else if (RotY <= -D3DX_PI + 0.05f && RotY >= -D3DX_PI - 0.05f)
	{//-D3DX_PIのとき

		pVtxMax->x = -VtxMin.x;
		pVtxMin->x = -VtxMax.x;

		pVtxMax->z = -VtxMin.z;
		pVtxMin->z = -VtxMax.z;
	}
}

//==================================================================================
// キャラ同士の当たり判定(補正)
//==================================================================================
void CollisionCharacter(D3DXVECTOR3 &MainPos, D3DXVECTOR3 &MainPosOld, D3DXVECTOR3 MainVtxMax, D3DXVECTOR3 MainVtxMin, D3DXVECTOR3 &TargetPos, D3DXVECTOR3 TargetVtxMax, D3DXVECTOR3 TargetVtxMin)
{

	// 横の判定
	if (MainPos.z + MainVtxMax.z > TargetPos.z + TargetVtxMin.z &&	// 敵の最小値に入ってる
		MainPos.z + MainVtxMin.z < TargetPos.z + TargetVtxMax.z)		// 敵の最大値に入ってる
	{// 敵のZ幅の中にいるとき

		if (MainPos.x + MainVtxMin.x <= TargetPos.x + TargetVtxMax.x &&	// 現在の最小値が敵の最大値よりも小さい
			MainPosOld.x + MainVtxMin.x >= TargetPos.x + TargetVtxMax.x)	// 前回の最小値が敵の最大値よりも大きい
		{// 右から

			MainPos.x = (TargetPos.x + TargetVtxMax.x) - MainVtxMin.x + 0.1f;
		}
		if (MainPos.x + MainVtxMax.x >= TargetPos.x + TargetVtxMin.x &&	// 現在の最大値が敵の最小値よりも大きい
			MainPosOld.x + MainVtxMax.x <= TargetPos.x + TargetVtxMin.x)	// 前回の最大値が敵の最小値よりも小さい
		{// 左から

			MainPos.x = (TargetPos.x + TargetVtxMin.x) - MainVtxMax.x - 0.1f;
		}
	}

	// 縦の判定
	if (MainPos.x + MainVtxMax.x > TargetPos.x + TargetVtxMin.x &&	// 敵の最小値に入ってる
		MainPos.x + MainVtxMin.x < TargetPos.x + TargetVtxMax.x)		// 敵の最大値に入ってる
	{// 敵のX幅の中にいるとき

		if (MainPos.z + MainVtxMin.z <= TargetPos.z + TargetVtxMax.z &&	// 現在の最小値が敵の最大値よりも小さい
			MainPosOld.z + MainVtxMin.z >= TargetPos.z + TargetVtxMax.z)	// 前回の最小値が敵の最大値よりも大きい
		{// 奥から

			MainPos.z = (TargetPos.z + TargetVtxMax.z) - MainVtxMin.z + 0.1f;
		}
		if (MainPos.z + MainVtxMax.z >= TargetPos.z + TargetVtxMin.z &&	// 現在の最大値が敵の最小値よりも大きい
			MainPosOld.z + MainVtxMax.z <= TargetPos.z + TargetVtxMin.z)	// 前回の最大値が敵の最小値よりも小さい
		{// 手前から

			MainPos.z = (TargetPos.z + TargetVtxMin.z) - MainVtxMax.z - 0.1f;
		}
	}
}

//==================================================================================
// キャラ同士の当たり判定(当たったかの判定返す)
//==================================================================================
bool bHitCharacter(D3DXVECTOR3 &MainPos, D3DXVECTOR3 &MainPosOld, D3DXVECTOR3 MainVtxMax, D3DXVECTOR3 MainVtxMin, D3DXVECTOR3 &TargetPos, D3DXVECTOR3 TargetVtxMax, D3DXVECTOR3 TargetVtxMin)
{
	bool bHit = false;

	// 横の判定
	if (MainPos.z + MainVtxMax.z > TargetPos.z + TargetVtxMin.z &&	// 敵の最小値に入ってる
		MainPos.z + MainVtxMin.z < TargetPos.z + TargetVtxMax.z)		// 敵の最大値に入ってる
	{// 敵のZ幅の中にいるとき

		if (MainPos.x + MainVtxMin.x <= TargetPos.x + TargetVtxMax.x &&	// 現在の最小値が敵の最大値よりも小さい
			MainPosOld.x + MainVtxMin.x >= TargetPos.x + TargetVtxMax.x)	// 前回の最小値が敵の最大値よりも大きい
		{// 右から

			bHit = true;
		}
		if (MainPos.x + MainVtxMax.x >= TargetPos.x + TargetVtxMin.x &&	// 現在の最大値が敵の最小値よりも大きい
			MainPosOld.x + MainVtxMax.x <= TargetPos.x + TargetVtxMin.x)	// 前回の最大値が敵の最小値よりも小さい
		{// 左から

			bHit = true;
		}
	}

	// 縦の判定
	if (MainPos.x + MainVtxMax.x > TargetPos.x + TargetVtxMin.x &&	// 敵の最小値に入ってる
		MainPos.x + MainVtxMin.x < TargetPos.x + TargetVtxMax.x)		// 敵の最大値に入ってる
	{// 敵のX幅の中にいるとき

		if (MainPos.z + MainVtxMin.z <= TargetPos.z + TargetVtxMax.z &&	// 現在の最小値が敵の最大値よりも小さい
			MainPosOld.z + MainVtxMin.z >= TargetPos.z + TargetVtxMax.z)	// 前回の最小値が敵の最大値よりも大きい
		{// 奥から

			bHit = true;
		}
		if (MainPos.z + MainVtxMax.z >= TargetPos.z + TargetVtxMin.z &&	// 現在の最大値が敵の最小値よりも大きい
			MainPosOld.z + MainVtxMax.z <= TargetPos.z + TargetVtxMin.z)	// 前回の最大値が敵の最小値よりも小さい
		{// 手前から

			bHit = true;
		}
	}

	return bHit;
}

//==================================================================================
// 線分の当たり判定
//==================================================================================
void CollisionLimitLine(D3DXVECTOR3 pos0, D3DXVECTOR3 pos1, D3DXVECTOR3 &pPos, D3DXVECTOR3 posOld)
{
	// 境界線のベクトル
	D3DXVECTOR3 vecLine;
	vecLine.x = pos1.x - pos0.x;
	vecLine.z = pos1.z - pos0.z;

	// プレイヤーの境界線のベクトル
	D3DXVECTOR3 vecLinePlayer;
	vecLinePlayer.x = pPos.x - posOld.x;
	vecLinePlayer.z = pPos.z - posOld.z;

	// プレイヤーと壁のベクトル
	D3DXVECTOR3 vecToPosPlayer;
	vecToPosPlayer.x = pos1.x - posOld.x;
	vecToPosPlayer.z = pos1.z - posOld.z;

	// 面積の最大値
	float fMaxAreaPlayer = (vecLinePlayer.z * vecLine.x) - (vecLinePlayer.x * vecLine.z);

	// 今回の面積
	float fNowAreaPlayer = (vecToPosPlayer.z * vecLine.x) - (vecToPosPlayer.x * vecLine.z);

	// 割合
	float fRatePlayer = fNowAreaPlayer / fMaxAreaPlayer;

	if (fRatePlayer >= 0.0f && fRatePlayer <= 1.0f)
	{// 面積の範囲内にいたら判定

		if ((vecLinePlayer.z * vecToPosPlayer.x) - (vecLinePlayer.x * vecToPosPlayer.z) > 0)
		{// 壁に当たったら

			// 交点からはみ出た分
			D3DXVECTOR3 CollisionPointPlayer = D3DXVECTOR3(0.0f, pPos.y, 0.0f);
			CollisionPointPlayer.x = (pPos.x + (vecLinePlayer.x * (fRatePlayer - 1.0f)));
			CollisionPointPlayer.z = (pPos.z + (vecLinePlayer.z * (fRatePlayer - 1.0f)));

			// 法線ベクトル(境界線ベクトルのXとZ反転)
			D3DXVECTOR3 vecNor = D3DXVECTOR3(vecLine.z, 0.0f, -vecLine.x);

			// ベクトルの正規化
			D3DXVec3Normalize(&vecNor, &vecNor);

			// プレイヤーの逆移動量
			D3DXVECTOR3 PlayerInverceMove;
			PlayerInverceMove.z = vecLinePlayer.z * (fRatePlayer - 1.0f);
			PlayerInverceMove.x = vecLinePlayer.x * (fRatePlayer - 1.0f);

			// 内積(壁の法線とプレイヤーの逆移動量)
			float fDot = (PlayerInverceMove.x * vecNor.x) + (PlayerInverceMove.z * vecNor.z);

			// 壁ずり移動量
			D3DXVECTOR3 MoveWall = vecNor * fDot;

			D3DXVECTOR3 ppp = (CollisionPointPlayer - pPos);
			D3DXVECTOR3 pppp = ppp + MoveWall;

			// ぶつかった点に補正
			pPos += MoveWall + (vecNor * 0.1f);

		}
	}
}

//==================================================================================
//  扇の当たり判定
//==================================================================================
bool CollisionFan(D3DXVECTOR3 posChar, D3DXVECTOR3 posLeft, D3DXVECTOR3 posRight, D3DXVECTOR3 TargetPos, float fTargetRadius)
{
	// 当たったかの判定
	bool bHit = false;
	bool bLine1 = false, bLine2 = false, bCircle = false;

	// 左の点との線分
	bLine1 = CollisionLine(posLeft, posChar, TargetPos, TargetPos);

	// 右の点との線分
	bLine2 = CollisionLine(posChar, posRight, TargetPos, TargetPos);

	// 扇の半径
	float fLen = 
		sqrtf((posLeft.x - posChar.x) * (posLeft.x - posChar.x)
		+ (posLeft.z - posChar.z) * (posLeft.z - posChar.z));

	// 円の当たり判定
	if (CircleRange(posChar, TargetPos, fLen, fTargetRadius) == true)
	{// 当たっていたら
		bCircle = true;
	}

	// 全ての範囲に入っていたら
	if (bLine1 == true && bLine2 == true && bCircle == true)
	{
		bHit = true;
	}

	return bHit;
}

//==================================================================================
// 三角の内側にいるか判定
//==================================================================================
bool CollisionTriangle(D3DXVECTOR3 posCenter, D3DXVECTOR3 posLeft, D3DXVECTOR3 posRight, D3DXVECTOR3 MainPos, D3DXVECTOR3 MainPosOld)
{
	// 当たったかの判定
	bool bHit = false;
	bool bLine1 = false, bLine2 = false, bLine3 = false;

	// 真ん中と左の線
	bLine1 = CollisionLine(posCenter, posLeft, MainPos, MainPosOld);

	// 左と右の線
	bLine2 = CollisionLine(posLeft, posRight, MainPos, MainPosOld);

	// 右と真ん中の線
	bLine3 = CollisionLine(posRight, posCenter, MainPos, MainPosOld);

	// 全ての範囲に入っていたら
	if (bLine1 == true && bLine2 == true && bLine3 == true)
	{
		bHit = true;
	}

	return bHit;
}

//==================================================================================
// 四角の内側にいるか判定
//==================================================================================
bool CollisionSquare(D3DXVECTOR3 posLeftUP, D3DXVECTOR3 posRightUP, D3DXVECTOR3 posLeftDW, D3DXVECTOR3 posRightDW, D3DXVECTOR3 MainPos)
{
	// 当たったかの判定
	bool bHit = false;
	bool bLine1 = false, bLine2 = false, bLine3 = false, bLine4 = false;

	// 左上と右上
	bLine1 = CollisionLine(posRightUP, posLeftUP, MainPos, MainPos);

	// 右上と右下
	bLine2 = CollisionLine(posRightDW, posRightUP, MainPos, MainPos);

	// 右下と左下
	bLine3 = CollisionLine(posLeftDW, posRightDW, MainPos, MainPos);

	// 左下と左上
	bLine4 = CollisionLine(posLeftUP, posLeftDW, MainPos, MainPos);

	// 全ての範囲に入っていたら
	if (bLine1 == true && bLine2 == true && bLine3 == true && bLine4 == true)
	{
		// 当たった
		bHit = true;
	}

	return bHit;
}

//==================================================================================
// 四角の内側にいるか判定
//==================================================================================
bool CollisionSquare(D3DXVECTOR3 &posSquare, D3DXVECTOR2 SquareSize, float fSquareRot, D3DXVECTOR3 MainPos)
{
	// 当たったかの判定
	bool bHit = false;
	bool bLine1 = false, bLine2 = false, bLine3 = false, bLine4 = false;

	float fLength = sqrtf(SquareSize.x * SquareSize.x + SquareSize.y * SquareSize.y);	// 対角線の長さ
	float fAngle = atan2f(SquareSize.x, SquareSize.y);									// 対角線の向き

	// 判定する四角の4頂点
	D3DXVECTOR3 LeftUp = D3DXVECTOR3(posSquare.x + sinf(fSquareRot - fAngle) * fLength, posSquare.y, posSquare.z + cosf(fSquareRot - fAngle) * fLength);
	D3DXVECTOR3 RightUp = D3DXVECTOR3(posSquare.x + sinf(fSquareRot + fAngle) * fLength, posSquare.y, posSquare.z + cosf(fSquareRot + fAngle) * fLength);
	D3DXVECTOR3 LeftDown = D3DXVECTOR3(posSquare.x + sinf(fSquareRot - D3DX_PI + fAngle) * fLength, posSquare.y, posSquare.z + cosf(fSquareRot - D3DX_PI + fAngle) * fLength);
	D3DXVECTOR3 RightDown = D3DXVECTOR3(posSquare.x + sinf(fSquareRot + D3DX_PI - fAngle) * fLength, posSquare.y, posSquare.z + cosf(fSquareRot + D3DX_PI - fAngle) * fLength);

	// 左上と右上
	bLine1 = CollisionLine(RightUp, LeftUp, MainPos, MainPos);

	// 右上と右下
	bLine2 = CollisionLine(RightDown, RightUp, MainPos, MainPos);

	// 右下と左下
	bLine3 = CollisionLine(LeftDown, RightDown, MainPos, MainPos);

	// 左下と左上
	bLine4 = CollisionLine(LeftUp, LeftDown, MainPos, MainPos);

	// 全ての範囲に入っていたら
	if (bLine1 == true && bLine2 == true && bLine3 == true && bLine4 == true)
	{
		// 当たった
		bHit = true;
	}

	return bHit;
}

//==================================================================================
// 四角と四角の当たり判定
//==================================================================================
bool CollisionSquareSquare2D(D3DXVECTOR3 &MainPos, D3DXVECTOR3 &TargetPos, D3DXVECTOR2 MainSize, D3DXVECTOR2 TargetSize, float fMainRot, float fTargetRot)
{
	float fTargetLength = sqrtf(
		TargetSize.x * TargetSize.x +
		TargetSize.y * TargetSize.y);							// 対角線の長さ
	float fTargetAngle = atan2f(TargetSize.x, TargetSize.y);	// 対角線の向き
	
	// 判定する四角の4頂点
	D3DXVECTOR3 LeftUp = D3DXVECTOR3(TargetPos.x + sinf(fTargetRot - fTargetAngle) * fTargetLength, TargetPos.y, TargetPos.z + cosf(fTargetRot - fTargetAngle) * fTargetLength);
	D3DXVECTOR3 RightUp = D3DXVECTOR3(TargetPos.x + sinf(fTargetRot + fTargetAngle) * fTargetLength, TargetPos.y, TargetPos.z + cosf(fTargetRot + fTargetAngle) * fTargetLength);
	D3DXVECTOR3 LeftDown = D3DXVECTOR3(TargetPos.x + sinf(fTargetRot - D3DX_PI + fTargetAngle) * fTargetLength, TargetPos.y, TargetPos.z + cosf(fTargetRot - D3DX_PI + fTargetAngle) * fTargetLength);
	D3DXVECTOR3 RightDown = D3DXVECTOR3(TargetPos.x + sinf(fTargetRot + D3DX_PI - fTargetAngle) * fTargetLength, TargetPos.y, TargetPos.z + cosf(fTargetRot + D3DX_PI - fTargetAngle) * fTargetLength);

	//***********************
	// 矩形の判定
	//***********************
	if (CollisionSquare(MainPos, MainSize, fMainRot, LeftUp) == true)
	{// 左上
		return true;
	}
	if (CollisionSquare(MainPos, MainSize, fMainRot, RightUp) == true)
	{// 右上
		return true;
	}
	if (CollisionSquare(MainPos, MainSize, fMainRot, LeftDown) == true)
	{// 左下
		return true;
	}
	if (CollisionSquare(MainPos, MainSize, fMainRot, RightDown) == true)
	{// 右下
		return true;
	}

	return false;
}

//==================================================================================
// 円と矩形の当たり判定(2D)
//==================================================================================
bool CollisionCircleSquare2D(D3DXVECTOR3 &posCircle, D3DXVECTOR3 &posSquare, D3DXVECTOR3 rotSquare, float fCircleRadius, D3DXVECTOR2 SquareSize)
{

	float fSquareSizeX = SquareSize.x;	// 矩形のサイズX
	float fSquareSizeY = SquareSize.y;	// 矩形のサイズY

	float fLength = 0.0f;	// 対角線の長さ
	float fAngle = 0.0f;	// 対角線の向き

	D3DXVECTOR3 LeftUp, RightUp, LeftDown, RightDown;

	//****************************************************
	// Xのサイズに円の半径を足した矩形の判定
	//****************************************************
	fSquareSizeX += fCircleRadius;	// 矩形のサイズX
	fSquareSizeY = SquareSize.y;	// 矩形のサイズY

	fLength = sqrtf(fSquareSizeX * fSquareSizeX + fSquareSizeY * fSquareSizeY);	// 対角線の長さ
	fAngle = atan2f(fSquareSizeX, fSquareSizeY);								// 対角線の向き

	// 判定する四角の4頂点
	LeftUp = D3DXVECTOR3(posSquare.x + cosf(rotSquare.x) * sinf(rotSquare.y - fAngle) * fLength, posSquare.y, posSquare.z + cosf(rotSquare.x) * cosf(rotSquare.y - fAngle) * fLength);
	RightUp = D3DXVECTOR3(posSquare.x + cosf(rotSquare.x) * sinf(rotSquare.y + fAngle) * fLength, posSquare.y, posSquare.z + cosf(rotSquare.x) * cosf(rotSquare.y + fAngle) * fLength);
	LeftDown = D3DXVECTOR3(posSquare.x + cosf(rotSquare.x) * sinf(rotSquare.y - D3DX_PI + fAngle) * fLength, posSquare.y, posSquare.z + cosf(rotSquare.x) * cosf(rotSquare.y - D3DX_PI + fAngle) * fLength);
	RightDown = D3DXVECTOR3(posSquare.x + cosf(rotSquare.x) * sinf(rotSquare.y + D3DX_PI - fAngle) * fLength, posSquare.y, posSquare.z + cosf(rotSquare.x) * cosf(rotSquare.y + D3DX_PI - fAngle) * fLength);

	// 矩形の判定
	if (CollisionSquare(LeftUp, RightUp, LeftDown, RightDown, posCircle) == true)
	{// 矩形に当たった場合
		return true;
	}

	//****************************************************
	// Yのサイズに円の半径を足した矩形の判定
	//****************************************************
	fSquareSizeX = SquareSize.x;	// 矩形のサイズX
	fSquareSizeY += fCircleRadius;	// 矩形のサイズY

	fLength = sqrtf(fSquareSizeX * fSquareSizeX + fSquareSizeY * fSquareSizeY);	// 対角線の長さ
	fAngle = atan2f(fSquareSizeX, fSquareSizeY);								// 対角線の向き

	// 判定する四角の4頂点
	LeftUp = D3DXVECTOR3(posSquare.x + cosf(rotSquare.x) * sinf(rotSquare.y - fAngle) * fLength, posSquare.y, posSquare.z + cosf(rotSquare.x) * cosf(rotSquare.y - fAngle) * fLength);
	RightUp = D3DXVECTOR3(posSquare.x + cosf(rotSquare.x) * sinf(rotSquare.y + fAngle) * fLength, posSquare.y, posSquare.z + cosf(rotSquare.x) * cosf(rotSquare.y + fAngle) * fLength);
	LeftDown = D3DXVECTOR3(posSquare.x + cosf(rotSquare.x) * sinf(rotSquare.y - D3DX_PI + fAngle) * fLength, posSquare.y, posSquare.z + cosf(rotSquare.x) * cosf(rotSquare.y - D3DX_PI + fAngle) * fLength);
	RightDown = D3DXVECTOR3(posSquare.x + cosf(rotSquare.x) * sinf(rotSquare.y + D3DX_PI - fAngle) * fLength, posSquare.y, posSquare.z + cosf(rotSquare.x) * cosf(rotSquare.y + D3DX_PI - fAngle) * fLength);

	// 矩形の判定
	if (CollisionSquare(LeftUp, RightUp, LeftDown, RightDown, posCircle) == true)
	{// 矩形に当たった場合
		return true;
	}


	//***********************
	// 各頂点毎の円の判定
	//***********************
	if (CircleRange(posCircle, LeftUp, fCircleRadius, fCircleRadius) == true)
	{// 左上
		return true;
	}
	if (CircleRange(posCircle, RightUp, fCircleRadius, fCircleRadius) == true)
	{// 右上
		return true;
	}
	if (CircleRange(posCircle, LeftDown, fCircleRadius, fCircleRadius) == true)
	{// 左下
		return true;
	}
	if (CircleRange(posCircle, RightDown, fCircleRadius, fCircleRadius) == true)
	{// 右下
		return true;
	}

	// 当たってない判定を返す
	return false;
}

//==================================================================================
// 高さを求める
//==================================================================================
float GetVtxHeight(D3DXVECTOR3 pos, D3DXVECTOR3 NowPos, D3DXVECTOR3 posRight, D3DXVECTOR3 posLeft, bool &bLand)
{
	// 高さ
	float fHeight = pos.y;

	// 直角部分と位置のベクトル
	D3DXVECTOR3 calvec = pos - NowPos;

	// ベクトルと法線
	D3DXVECTOR3 
		vec1 = posRight - NowPos,
		vec2 = posLeft - posRight,
		nor = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	if (CollisionTriangle(NowPos, posRight, posLeft, pos, pos) == true)
	{// 三角に入っていたら

		// 外積を求める
		D3DXVec3Cross(&nor, &vec1, &vec2);

		// 外積の正規化をして法線にする
		D3DXVec3Normalize(&nor, &nor);

		if (nor.y != 0.0f)
		{// 法線が0.0fじゃなかったら

			// 高さを求める
			fHeight = -((calvec.x * nor.x) + (-NowPos.y * nor.y) + (calvec.z * nor.z)) / nor.y;
			bLand = true;
		}
		else
		{
			// 高さを求める
			fHeight = -((calvec.x * nor.x) + (-NowPos.y * nor.y) + (calvec.z * nor.z)) / nor.y;
			bLand = true;
		}
	}

	return fHeight;
}

//==================================================================================
//  線の当たり判定  
//==================================================================================
bool CollisionLine(D3DXVECTOR3 pos0, D3DXVECTOR3 pos1, D3DXVECTOR3 MainPos, D3DXVECTOR3 MainPosOld)
{
	// 当たったかの判定
	bool bHit = false;

	// 境界線のベクトル
	D3DXVECTOR3 vecLine;
	vecLine.x = pos1.x - pos0.x;
	vecLine.z = pos1.z - pos0.z;

	// 弾と壁のベクトル
	D3DXVECTOR3 vecToPos;
	vecToPos.x = MainPos.x - pos0.x;
	vecToPos.z = MainPos.z - pos0.z;

	if ((vecLine.z * vecToPos.x) - (vecLine.x * vecToPos.z) <= 0)
	{// 線を超えた

		// 当たった
		bHit = true;
	}

	return bHit;
}

//==================================================================================
//  スプライト曲線補正
//==================================================================================
D3DXVECTOR3 CatmullRomSplineInterp(D3DXVECTOR3 PointMili1, D3DXVECTOR3 Point0, D3DXVECTOR3 Point1, D3DXVECTOR3 Point2, float fTime)
{
	// 4点で補正
	D3DXVECTOR3 a4 = Point0;
	D3DXVECTOR3 a3 = (Point1 - PointMili1) / 2.0f;
	D3DXVECTOR3 a1 = (Point2 - Point0) / 2.0f - 2.0f * Point1 + a3 + 2.0f * a4;
	D3DXVECTOR3 a2 = 3.0f * Point1 - (Point2 - Point0) / 2.0f - 2.0f * a3 - 3.0f * a4;

	return (a1 * fTime * fTime * fTime + a2 * fTime * fTime + a3 * fTime + a4);
}
