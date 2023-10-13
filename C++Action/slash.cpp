//=============================================================================
// 
//  �a������ [slash.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "slash.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "game.h"
#include "bulletmanager.h"
#include "calculation.h"
#include "camera.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define TEXTURE		"data\\TEXTURE\\field002.png"
#define POS_IMPACTWAVE	(5000.0f)
#define POS_IMPACTWAVE_Y	(3000.0f)
#define WIDTH	(32)

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CSlash::CSlash(int nPriority) : CImpactWave(nPriority)
{
	m_nTexIdx = 0;										// �e�N�X�`���̃C���f�b�N�X�ԍ�
	m_colOrigin = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// ���̐F
	m_fSizeDest = 0.0f;		// �ڕW�̃T�C�Y
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CSlash::~CSlash()
{

}

//==========================================================================
// ��������
//==========================================================================
CSlash *CSlash::Create(D3DXVECTOR3 pos, D3DXVECTOR3 Parentrot, D3DXVECTOR3 rot, D3DXCOLOR col, float fWidth, float fCenterDistance, int nLife, float fAddSizeValue, int nTexType, bool bAddBlend, ANGLE angle)
{
	// �����p�̃I�u�W�F�N�g
	CSlash *pObjMeshCylinder = NULL;

	if (pObjMeshCylinder == NULL)
	{// NULL��������

		// �������̊m��
		pObjMeshCylinder = DEBUG_NEW CSlash;

		if (pObjMeshCylinder != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// �ʒu�E����
			pObjMeshCylinder->SetPosition(pos);				// �ʒu
			pObjMeshCylinder->SetOriginRotation(Parentrot);	// ���̌����ݒ�
			pObjMeshCylinder->SetRotation(rot);				// ����
			pObjMeshCylinder->SetColor(col);				// �F
			pObjMeshCylinder->m_colOrigin = col;			// ���̐F
			pObjMeshCylinder->SetWidthBlock(WIDTH);			// ������
			pObjMeshCylinder->SetRotWidth((D3DX_PI * 2) / (float)(WIDTH));	// �ꕪ��������̊p�x
			pObjMeshCylinder->SetHeightBlock(1);			// �c����
			pObjMeshCylinder->SetOutWidth(fWidth + fCenterDistance);			// ����(�O)
			pObjMeshCylinder->SetInWidth(fWidth);			// ����(��)
			pObjMeshCylinder->SetLife(nLife);				// ����
			pObjMeshCylinder->SetMaxLife(nLife);			// �ő����
			pObjMeshCylinder->SetMoveSize(fAddSizeValue);	// �L���鑬�x
			pObjMeshCylinder->SetTexDivision(4);			// �e�N�X�`���������ݒ�
			pObjMeshCylinder->SetIsAddBlend(bAddBlend);		// ���Z�������邩
			pObjMeshCylinder->m_fSizeDest = fWidth + fCenterDistance + (fAddSizeValue * nLife);
			pObjMeshCylinder->SetMoveAngle(angle);			// ����

			// �e�N�X�`���̊��蓖��
			pObjMeshCylinder->m_nTexIdx = CManager::GetTexture()->Regist(GetFileName(nTexType));

			// �e�N�X�`���̊��蓖��
			pObjMeshCylinder->BindTexture(pObjMeshCylinder->m_nTexIdx);

			// ����������
			pObjMeshCylinder->Init();
		}

		return pObjMeshCylinder;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CSlash::Init(void)
{
	HRESULT hr;

	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ��ސݒ�
	SetType(TYPE_MESHDONUTS);

	// �I�u�W�F�N�g3D���b�V���̏���������
	hr = CObject3DMesh::Init(CObject3DMesh::TYPE_DONUTS);
	if (FAILED(hr))
	{// ���s���Ă�����
		return E_FAIL;
	}

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CSlash::Uninit(void)
{
	// �I������
	CImpactWave::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CSlash::Update(void)
{

	// �O���̕�
	float fOutWidth = GetOutWidth();

	fOutWidth += (m_fSizeDest - fOutWidth) * 0.15f;

	// �O���̕�
	SetOutWidth(fOutWidth);

	// �X�V����
	CImpactWave::Update();

	// �O���̕�
	SetOutWidth(fOutWidth);

	CBullet **ppBullet = CGame::GetBulletManager()->GetBullet();
	int nNumAll = CGame::GetBulletManager()->GetNumAll();

	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();
	for (int nCntBullet = 0; nCntBullet < nNumAll; nCntBullet++)
	{
		if (ppBullet[nCntBullet] == NULL)
		{// NULL��������
			break;
		}

		// �e�̈ʒu
		D3DXVECTOR3 BulletPosition = ppBullet[nCntBullet]->GetPosition();
		D3DXVECTOR3 BulletMove = ppBullet[nCntBullet]->GetMove();

		// �e�Ƃ̔���
		float fBulletRadius = ppBullet[nCntBullet]->GetRadius();

		if (ppBullet[nCntBullet]->GetState() == CBullet::STATE_NONE && 
			ppBullet[nCntBullet]->GetType() == CBullet::TYPE_ENEMY &&
			SphereRange(pos, BulletPosition, fOutWidth, fBulletRadius) == true)
		{// ���̔���

			float fRot = 0.0f;
			D3DXVECTOR3 SlashMove = mylib_const::DEFAULT_VECTOR3;

			// �ړ��ʐݒ�
			switch (GetMoveAngle())
			{
			case CObject::ANGLE_UP:
				fRot = atan2f((pos.x - 0.0f), (pos.z - 0.0f));

				ppBullet[nCntBullet]->SetMove(D3DXVECTOR3(
					sinf(D3DX_PI + fRot) * 7.5f,
					0.0f,
					cosf(D3DX_PI + fRot) * 7.5f));
				break;

			case CObject::ANGLE_DOWN:
			{
				int nAngle = 1;

				if (ppBullet[nCntBullet]->GetMoveAngle() == ANGLE_LEFT)
				{
					nAngle = -1;
				}

				fRot = ppBullet[nCntBullet]->GetRotation().y - ((D3DX_PI * 0.5f) * nAngle);

				ppBullet[nCntBullet]->SetMove(D3DXVECTOR3(
					sinf(D3DX_PI + fRot) * 5.0f,
					0.0f,
					cosf(D3DX_PI + fRot) * 5.0f));
			}
				break;

			default:

				float fSlashRot = GetOriginRotation().x;
				if (GetMoveAngle() == ANGLE_LEFT)
				{
					fSlashRot += D3DX_PI;
				}

				if (BulletMove.x <= 0.0f)
				{
					BulletMove.x *= -1;
				}
				BulletMove.x *= 2.5f;

				SlashMove = D3DXVECTOR3(BulletMove.x, sinf(D3DX_PI + fSlashRot) * 2.0f, 0.0f);
				ppBullet[nCntBullet]->SetMove(SlashMove);
				break;
			}

			ppBullet[nCntBullet]->SetType(CBullet::TYPE_PLAYER);
			ppBullet[nCntBullet]->SetState(CBullet::STATE_DMG, 3);
			ppBullet[nCntBullet]->SetMoveAngle(GetMoveAngle());

			// �q�b�g�X�g�b�v
			CManager::SetEnableHitStop(6);

			// �U��
			CManager::GetCamera()->SetShake(12, 25.0f, 0.0f);
		}
	}
}

//==========================================================================
// �`�揈��
//==========================================================================
void CSlash::Draw(void)
{
	// �`�揈��
	CImpactWave::Draw();
}
