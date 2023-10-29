//=============================================================================
// 
//  ヒットスコアヘッダー [hitscore.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _HITSCORE_H_
#define _HITSCORE_H_	// 二重インクルード防止

//==========================================================================
// 前方宣言
//==========================================================================
class CMultiNumber;
class CObject2D;

//==========================================================================
// クラス定義
//==========================================================================
// ヒットスコアクラス定義
class CHitScore
{
public:

	CHitScore(int nPriority = 7);
	~CHitScore();

	// オーバーライドされた関数
	HRESULT Init(void);
	HRESULT Init(D3DXVECTOR3 pos);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void Add(int nValue);
	void Reset(void);	// リセット
	static CHitScore *Create(void);
	static CHitScore *Create(D3DXVECTOR3 pos);

private:

	// メンバ関数
	void UpdateMove(void);
	void CalScore(void);	// スコア計算

	// メンバ変数
	CMultiNumber *m_apNumber;	// 数字のオブジェクト
	CObject2D *m_pObj2D;		// オブジェクト2Dのオブジェクト
	D3DXVECTOR3 m_posOrigin;	// 元の位置
	int m_nTexIdx;				// テクスチャのインデックス番号
	int m_nCntCooltime;			// クールタイムのカウンター
};



#endif