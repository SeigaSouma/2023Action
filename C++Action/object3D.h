//=============================================================================
// 
//  オブジェクト3Dヘッダー [object3D.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _OBJECT3D_H_
#define _OBJECT3D_H_	// 二重インクルード防止

#include "main.h"
#include "object.h"

//==========================================================================
// クラス定義
//==========================================================================
// オブジェクト3Dクラス定義
class CObject3D : public CObject
{
public:

	CObject3D(int nPriority = 5);
	virtual ~CObject3D();

	// オーバーライドされた関数
	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);
	virtual void SetVtx(void);
	virtual void BindTexture(int nIdx);

	void SetWorldMtx(const D3DXMATRIX mtx);		// マトリックス設定
	D3DXMATRIX GetWorldMtx(void) const;			// マトリックス取得
	void SetPosition(const D3DXVECTOR3 pos);	// 位置設定
	D3DXVECTOR3 GetPosition(void) const;		// 位置取得
	void SetOriginPosition(const D3DXVECTOR3 pos);	//	元の位置設定
	D3DXVECTOR3 GetOriginPosition(void) const;		//	元の位置取得
	void SetOldPosition(const D3DXVECTOR3 posOld);	// 前回の位置設定
	D3DXVECTOR3 GetOldPosition(void) const;		// 前回の位置取得
	void SetRotation(const D3DXVECTOR3 rot);	// 向き設定
	D3DXVECTOR3 GetRotation(void) const;		// 向き取得
	void SetOriginRotation(const D3DXVECTOR3 rot);	// 元の向き設定
	D3DXVECTOR3 GetOriginRotation(void) const;		// 元の向き取得
	void SetMove(const D3DXVECTOR3 move);		// 移動量設定
	D3DXVECTOR3 GetMove(void) const;			// 移動量取得
	void SetColor(const D3DXCOLOR col);			// 色設定
	D3DXCOLOR GetColor(void) const;				// 色取得
	void SetSize(const D3DXVECTOR3 size);		// サイズの設定
	D3DXVECTOR3 GetSize(void) const;			// サイズの取得

	static CObject3D *Create();
	static CObject3D *Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot);
	CObject3D *GetObject3D(void);
	virtual CObject3DMesh *GetObject3DMesh(void);
protected:

private:
	D3DXMATRIX	m_mtxWorld;		// ワールドマトリックス
	D3DXVECTOR3 m_posOrigin;	// 元の位置
	D3DXVECTOR3 m_posOld;		// 前回の位置
	D3DXVECTOR3 m_move;			// 移動量
	D3DXVECTOR3 m_rot;			// 向き
	D3DXVECTOR3 m_rotOrigin;	// 元の向き
	D3DXCOLOR m_col;			// 色
	D3DXVECTOR3 m_fSize;		// サイズ
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;	// 頂点バッファ
	int m_nTexIdx;					// テクスチャのインデックス番号
};



#endif