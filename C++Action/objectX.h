//=============================================================================
// 
//  オブジェクトXヘッダー [objectX.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _OBJECTX_H_
#define _OBJECTX_H_	// 二重インクルード防止

#include "main.h"
#include "object.h"
#include "Xload.h"

// 前方宣言
class CShadow;

//==========================================================================
// クラス定義
//==========================================================================
// オブジェクトXクラス定義
class CObjectX : public CObject
{
public:

	// マクロ定義
#define MAX_MAT				(512)		// マテリアルサイズ
#define MAX_TX				(96)		// テクスチャサイズ

	CObjectX(int nPriority = mylib_const::DEFAULT_PRIORITY);
	~CObjectX();

	// オーバーライドされた関数
	HRESULT Init(void);
	HRESULT Init(const char *pFileName);
	HRESULT Init(int nIdxXFile);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void Draw(D3DXCOLOR col);
	void Draw(float fAlpha);
	void BindTexture(int *nIdx);
	void BindXData(int nIdxXFile);

	void SetWorldMtx(const D3DXMATRIX mtx);		// マトリックス設定
	D3DXMATRIX GetWorldMtx(void) const;			// マトリックス取得
	void SetPosition(const D3DXVECTOR3 pos);	// 位置設定
	D3DXVECTOR3 GetPosition(void) const;		// 位置取得
	void SetOldPosition(const D3DXVECTOR3 posOld);	// 前回の位置設定
	D3DXVECTOR3 GetOldPosition(void) const;		// 前回の位置取得
	void SetRotation(const D3DXVECTOR3 rot);	// 向き設定
	D3DXVECTOR3 GetRotation(void) const;		// 向き取得
	void SetMove(const D3DXVECTOR3 move);		// 移動量設定
	D3DXVECTOR3 GetMove(void) const;			// 移動量取得
	void SetScale(const D3DXVECTOR3 scale);		// スケール設定
	D3DXVECTOR3 GetScale(void) const;			// スケール取得
	void SetColor(const D3DXCOLOR col);			// 色設定
	D3DXCOLOR GetColor(void) const;				// 色取得
	void SetSize(const D3DXVECTOR3 size);		// サイズの設定
	D3DXVECTOR3 GetSize(void) const;			// サイズの取得
	D3DXVECTOR3 GetVtxMax(void) const;			// 頂点の最大値取得
	D3DXVECTOR3 GetVtxMin(void) const;			// 頂点の最小値取得
	int GetIdxXFile(void) const;				// Xファイルのインデックス取得
	bool GetUseShadow(void) const;				// 影を使っているかどうか

	void Kill(void);
	static CObjectX *Create();
	static CObjectX *Create(const char *pFileName);
	static CObjectX *Create(const char *pFileName, D3DXVECTOR3 pos, D3DXVECTOR3 rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f), bool bShadow = false);
	static CObjectX *Create(int nIdxXFile, D3DXVECTOR3 pos, D3DXVECTOR3 rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f), bool bShadow = false);
	CObjectX *GetObjectX(void);
	static int GetNumAll(void);
protected:

private:
	D3DXMATRIX	m_mtxWorld;	// ワールドマトリックス
	D3DXVECTOR3 m_scale;	// スケール
	D3DXVECTOR3 m_pos;		// 位置
	D3DXVECTOR3 m_posOld;	// 前回の位置
	D3DXVECTOR3 m_move;		// 移動量
	D3DXVECTOR3 m_rot;		// 向き
	D3DXCOLOR m_col;		// 色
	D3DXVECTOR3 m_fSize;	// サイズ
	bool m_bShadow;			// 影を使うかどうか
	int m_nIdxTexure;		// テクスチャのインデックス番号
	int m_nIdxXFile;		// Xファイルのインデックス番号
	static int m_nNumAll;	// 総数
	CShadow *m_pShadow;		// 影の情報
};



#endif