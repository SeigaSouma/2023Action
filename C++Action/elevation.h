//=============================================================================
// 
//  起伏ヘッダー [elevation.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _ELEVATION_H_
#define _ELEVATION_H_	// 二重インクルード防止

#include "main.h"
#include "object3DMesh.h"

// 前方宣言
class CTargetPoint;

//==========================================================================
// クラス定義
//==========================================================================
// 起伏クラス定義
class CElevation : public CObject3DMesh
{
private:

	struct Info
	{
		std::string TextureFileName;	// テクスチャファイル名
		D3DXVECTOR3 pos;		// 位置
		float fWidthLength;		// 横長さ
		float fHeightLength;	// 縦長さ
		int nWidthBlock;		// 横分割数
		int nHeightBlock;		// 縦分割数
		D3DXVECTOR3 *pVtxPos;	// 頂点座標
	};

public:

	CElevation(int nPriority = 1);
	~CElevation();

	// オーバーライドされた関数
	virtual HRESULT Init(void);
	virtual HRESULT Init(const char *pText);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);
	virtual void SetVtx(void);

	void Release(void);
	bool IsHit(D3DXVECTOR3& pos);	// 当たったかの判定
	bool IsEdit(void);	// エディットの判定
	void Save(void);	// セーブ
	HRESULT Load(const char *pText);	// ロード

	static CElevation *Create(const char *pText);
	CElevation *GetElevation(void);
	float GetHeight(D3DXVECTOR3& pos, bool &bLand);	// 高さ取得

protected:

private:

	void UpdateState(void);				// 状態更新処理
	void UPVtxField(D3DXVECTOR3 pos);	// 頂点上げ下げ(デバッグ)

	Info m_aInfo;	// 起伏の情報
	float m_fBrushStrength;							// ブラシ強さ
	float m_fBrushRange;							// 範囲
	bool m_bEdit;									// エディットON/OFF
	CTargetPoint *m_pTargetP;						// 目標の地点
	D3DXVECTOR3 m_VtxPos[mylib_const::MAX_VTX];		// 頂点座標
	int m_nTexIdx;									// テクスチャのインデックス番号
	static bool m_bLoadInfo;						// 情報読み込み判定
};



#endif