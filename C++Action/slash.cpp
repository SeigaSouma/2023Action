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
#include "debugproc.h"

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
	collisionRotation = mylib_const::DEFAULT_VECTOR3;	// �����蔻��̌���
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
			pObjMeshCylinder->m_nTexIdx = CManager::GetInstance()->GetTexture()->Regist(GetFileName(nTexType));

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
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// ��ސݒ�
	SetType(TYPE_MESHDONUTS);

	// �I�u�W�F�N�g3D���b�V���̏���������
	hr = CObject3DMesh::Init(CObject3DMesh::TYPE_DONUTS);
	if (FAILED(hr))
	{// ���s���Ă�����
		return E_FAIL;
	}

	// �v�Z�p�}�g���b�N�X�錾
	D3DXMATRIX mtxRot, mtxTrans, mtxRotOrigin, mtxCollision;

#if 0
	D3DXVECTOR3 rotOrigin = GetRotation();
	D3DXVECTOR3 rot = GetOriginRotation();
#else
	D3DXVECTOR3 rotOrigin = GetOriginRotation();
	D3DXVECTOR3 rot = GetRotation();
#endif
	D3DXVECTOR3 pos = GetPosition();
	RotNormalize(rotOrigin);
	RotNormalize(rot);

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&mtxCollision);
	D3DXMatrixIdentity(&mtxRotOrigin);
#if 1
	// ���̌����𔽉f����
	D3DXMatrixRotationYawPitchRoll(&mtxRotOrigin, rotOrigin.y, rotOrigin.x, rotOrigin.z);
	D3DXMatrixMultiply(&mtxCollision, &mtxCollision, &mtxRotOrigin);

	// �����𔽉f����
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);
	D3DXMatrixMultiply(&mtxCollision, &mtxCollision, &mtxRot);
#else

	// �����𔽉f����
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);
	D3DXMatrixMultiply(&mtxCollision, &mtxCollision, &mtxRot);

	// ���̌����𔽉f����
	D3DXMatrixRotationYawPitchRoll(&mtxRotOrigin, rotOrigin.y, rotOrigin.x, rotOrigin.z);
	D3DXMatrixMultiply(&mtxCollision, &mtxCollision, &mtxRotOrigin);

#endif
	// �ʒu�𔽉f����
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(&mtxCollision, &mtxCollision, &mtxTrans);

	// ���[���h�s�񂩂��]�������v�Z
	collisionRotation.x = atan2f(mtxCollision._32, mtxCollision._33);
	collisionRotation.y = atan2f(-mtxCollision._31, sqrtf(mtxCollision._32 * mtxCollision._32 + mtxCollision._33 * mtxCollision._33));
	collisionRotation.z = atan2f(mtxCollision._21, mtxCollision._11);
	RotNormalize(collisionRotation);

	// �O���̕�
	float fOutWidth = GetOutWidth();

	m_pObj3D = CObject3D::Create();
	m_pObj3D->SetSize(D3DXVECTOR3(fOutWidth, 0.0f, fOutWidth));
	m_pObj3D->SetPosition(pos);

#if 0
	m_pObj3D->SetRotation(rot);
	m_pObj3D->SetOriginRotation(rotOrigin);
#else
	m_pObj3D->SetRotation(rotOrigin);
	m_pObj3D->SetOriginRotation(rot);
#endif

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CSlash::Uninit(void)
{
	// �I������
	CImpactWave::Uninit();

	m_pObj3D->Uninit();
	m_pObj3D = NULL;

}

//==========================================================================
// �X�V����
//==========================================================================
void CSlash::Update(void)
{
	if (IsDeath() == true)
	{
		return;
	}

	// �O���̕�
	float fOutWidth = GetOutWidth();

	fOutWidth += (m_fSizeDest - fOutWidth) * 0.15f;

	// �O���̕�
	SetOutWidth(fOutWidth);

	// �X�V����
	CImpactWave::Update();

	// �O���̕�
	SetOutWidth(fOutWidth);

	if (m_pObj3D != NULL)
	{
		m_pObj3D->Update();
		m_pObj3D->SetSize(D3DXVECTOR3(fOutWidth, 0.0f, fOutWidth));
	}

	// �����蔻��
	Collision();
}

//==========================================================================
// �����蔻��
//==========================================================================
void  CSlash::Collision(void)
{

	// �O���̕�
	float fOutWidth = GetOutWidth();

	// �e���擾
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
			IsHit(BulletPosition, fBulletRadius) == true)
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

			default:	// ���E

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

				SlashMove = D3DXVECTOR3(cosf(fSlashRot) * BulletMove.x, sinf(D3DX_PI + fSlashRot) * 5.0f, 0.0f);
				ppBullet[nCntBullet]->SetMove(SlashMove);
				break;
			}

			ppBullet[nCntBullet]->SetType(CBullet::TYPE_PLAYER);
			ppBullet[nCntBullet]->SetState(CBullet::STATE_DMG, 3);
			ppBullet[nCntBullet]->SetMoveAngle(GetMoveAngle());

			// �q�b�g�X�g�b�v
			CManager::GetInstance()->SetEnableHitStop(6);

			// �U��
			CManager::GetInstance()->GetCamera()->SetShake(12, 25.0f, 0.0f);
		}
	}
}

//==========================================================================
// �p�x�t���̔���
//==========================================================================
bool CSlash::IsHit(D3DXVECTOR3 TargetPos, float fTargetRadius)
{
	// �O���̕�
	float fOutWidth = GetOutWidth();

	D3DXVECTOR3 rectCenter = GetPosition();							// ��`�̒��S���W
	D3DXVECTOR3 rectSize = D3DXVECTOR3(fOutWidth, 10.0f, fOutWidth);	// ��`�̃T�C�Y
	D3DXVECTOR3 sphereCenter = TargetPos;							// ���̒��S���W
	float sphereRadius = fTargetRadius;								// ���̔��a

	// ��`�̕����ꎞ���
	float halfWidth = rectSize.x;
	float halfHeight = rectSize.y;
	float halfDepth = rectSize.z;

	// ���̒��S����`�Ɉړ�
	D3DXMATRIX rotationMatrix;
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, collisionRotation.y, collisionRotation.x, collisionRotation.z);
	D3DXVECTOR3 transformedSphereCenter = sphereCenter - rectCenter;
	D3DXVec3TransformCoord(&transformedSphereCenter, &transformedSphereCenter, &rotationMatrix);

	// ���̒��S�Ƌ�`�̓����̍ł��߂��_�����߂�
	float closestX = max(-halfWidth, min(transformedSphereCenter.x, halfWidth));
	float closestY = max(-halfHeight, min(transformedSphereCenter.y, halfHeight));
	float closestZ = max(-halfDepth, min(transformedSphereCenter.z, halfDepth));

	// ��̍ł��߂��_�Ƌ��Ƃ̋����v�Z
	float distanceSquared =
		(closestX - transformedSphereCenter.x) * (closestX - transformedSphereCenter.x) +
		(closestY - transformedSphereCenter.y) * (closestY - transformedSphereCenter.y) +
		(closestZ - transformedSphereCenter.z) * (closestZ - transformedSphereCenter.z);

	// ���̔���
	if (distanceSquared <= (sphereRadius * sphereRadius))
	{
		return true;
	}

	return false;

}

//==========================================================================
// �`�揈��
//==========================================================================
void CSlash::Draw(void)
{
	// �`�揈��
	CImpactWave::Draw();


	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();


	// �w�ʂ̃J�����O�Ȃ�
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// ���C�e�B���O�𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pObj3D->Draw();

	// ���C�e�B���O��L���ɂ���
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// �J�����O�̃f�t�H���g
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}
