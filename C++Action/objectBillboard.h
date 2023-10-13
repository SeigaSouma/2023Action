//=============================================================================
// 
//  ビルボードヘッダー [objectBillboard.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _OBJECTBILLBOARD_H_
#define _OBJECTBILLBOARD_H_	// 二重インクルード防止

#include "main.h"
#include "object.h"
#include "constans.h"

//==========================================================================
// クラス定義
//==========================================================================
// ビルボードクラス定義
class CObjectBillboard : public CObject
{
public:

	CObjectBillboard(int nPriority = 7);
	~CObjectBillboard();

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	virtual void SetVtx(void);
	void BindTexture(int nIdx);

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
	void SetColor(const D3DXCOLOR col);			// 色設定
	D3DXCOLOR GetColor(void) const;				// 色取得
	void SetSize(const D3DXVECTOR2 size);		// サイズの設定
	D3DXVECTOR2 GetSize(void) const;			// サイズの取得
	void SetTex(D3DXVECTOR2 *tex);				// テクスチャ座標の設定
	D3DXVECTOR2 *GetTex(void);					// テクスチャ座標の取得

	static CObjectBillboard *Create();
	static CObjectBillboard *Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot);
	CObjectBillboard *GetObjectBillBoard(void);
	LPDIRECT3DVERTEXBUFFER9 GetVtxBuff(void) const;
protected:

private:
	D3DXMATRIX	m_mtxWorld;	// ワールドマトリックス
	D3DXVECTOR3 m_posOld;	// 前回の位置
	D3DXVECTOR3 m_move;		// 移動量
	D3DXVECTOR3 m_rot;		// 向き
	D3DXCOLOR m_col;		// 色
	D3DXVECTOR2 m_fSize;	// サイズ
	D3DXVECTOR2 m_fTex[32];	// テクスチャ座標
	int m_nTexIdx;			// テクスチャのインデックス番号
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;	// 頂点バッファ
};



#endif