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
// �p�x�t���̔���
//==========================================================================
bool CSlash::IsHit(D3DXVECTOR3 TargetPos, float fTargetRadius)
{
	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �O���̕�
	float fOutWidth = GetOutWidth();

#if 0

	if (SphereRange(pos, TargetPos, fOutWidth, fTargetRadius) == false)
	{// �傫�����̔���
		return false;
	}

	// �����擾
	D3DXVECTOR3 rot = GetRotation();
	float RotX = GetOriginRotation().x;

	if (GetMoveAngle() == ANGLE_LEFT)
	{
		RotX += D3DX_PI;
	}

	float fLength = sqrtf(fOutWidth * fOutWidth + fOutWidth * fOutWidth);	// �Ίp���̒���
	float fAngle = atan2f(fOutWidth, fOutWidth);									// �Ίp���̌���

	float fPosY = sinf(GetOriginRotation().x) * fOutWidth;

	// ���肷��l�p��4���_
	bool bLine1 = false, bLine2 = false, bLine3 = false, bLine4 = false;
	
	D3DXVECTOR3 LeftUp = D3DXVECTOR3(
		pos.x + cosf(RotX) * sinf(rot.y - fAngle) * fLength,
		pos.y + fPosY,
		pos.z + cosf(RotX) * cosf(rot.y - fAngle) * fLength);

	D3DXVECTOR3 RightUp = D3DXVECTOR3(
		pos.x + cosf(RotX) * sinf(rot.y + fAngle) * fLength,
		pos.y + fPosY,
		pos.z + cosf(RotX) * cosf(rot.y + fAngle) * fLength);

	D3DXVECTOR3 LeftDown = D3DXVECTOR3(
		pos.x + cosf(RotX) * sinf(rot.y - D3DX_PI + fAngle) * fLength,
		pos.y - fPosY,
		pos.z + cosf(RotX) * cosf(rot.y - D3DX_PI + fAngle) * fLength);

	D3DXVECTOR3 RightDown = D3DXVECTOR3(
		pos.x + cosf(RotX) * sinf(rot.y + D3DX_PI - fAngle) * fLength,
		pos.y - fPosY,
		pos.z + cosf(RotX) * cosf(rot.y + D3DX_PI - fAngle) * fLength);

	if (CollisionCircleSquare2D(TargetPos, D3DXVECTOR3(pos.x, TargetPos.y, pos.z), D3DXVECTOR3(RotX, rot.y, rot.z), fTargetRadius, D3DXVECTOR2(fOutWidth, fOutWidth)))
	{
		return true;
	}


#else

	// �����擾
	D3DXVECTOR3 rot = GetRotation();

	{

		D3DXVECTOR3 rectCenter = pos; // ��]������`�̒��S���W
		D3DXVECTOR3 rectRotation = D3DXVECTOR3(D3DX_PI - GetOriginRotation().x, rot.y, rot.z);
		D3DXVECTOR3 rectSize(fOutWidth, 1.0f, fOutWidth); // ��]������`�̕��A�����A���s��
		D3DXMATRIX rectWorldMatrix = GetWorldMtx(); // ��]������`�̃��[���h�ϊ��s��
		D3DXVECTOR3 sphereCenter = TargetPos; // ���̒��S���W
		float sphereRadius = fTargetRadius; // ���̔��a

		// ��]������`�̔������v�Z
		float halfWidth = rectSize.x;
		float halfHeight = rectSize.y;
		float halfDepth = rectSize.z;

		// ���̒��S����]�������W�n�ɕϊ�
		D3DXMATRIX rotationMatrix;
		D3DXMatrixRotationYawPitchRoll(&rotationMatrix, rectRotation.y, rectRotation.x, rectRotation.z);
		D3DXVECTOR3 transformedSphereCenter = sphereCenter - rectCenter;
		D3DXVec3TransformCoord(&transformedSphereCenter, &transformedSphereCenter, &rotationMatrix);

		// �ړ����鋅�̐V�����ʒu���v�Z
		D3DXVECTOR3 newSphereCenter = sphereCenter;

		// ���̒��S�Ƌ�`�̓����̍ł��߂��_�����߂�
		float closestX = max(-halfWidth, min(transformedSphereCenter.x, halfWidth));
		float closestY = max(-halfHeight, min(transformedSphereCenter.y, halfHeight));
		float closestZ = max(-halfDepth, min(transformedSphereCenter.z, halfDepth));

		// �ł��߂��_�Ƌ��̒��S�Ƃ̋������v�Z
		float distanceSquared = 
			(closestX - transformedSphereCenter.x) * (closestX - transformedSphereCenter.x) +
			(closestY - transformedSphereCenter.y) * (closestY - transformedSphereCenter.y) +
			(closestZ - transformedSphereCenter.z) * (closestZ - transformedSphereCenter.z);

		float sphereRadiusWithMargin = sphereRadius;  // ���̔��a�ɒǉ��̃}�[�W�����l������ꍇ�A�K�؂Ȓl�ɕύX

		// �����V�����ʒu�ŋ�`�ƌ������Ă���ꍇ�A�����蔻��
		if (distanceSquared <= (sphereRadiusWithMargin * sphereRadiusWithMargin)) {
			return true;
		}

		return false;
	}

#endif

	

	return false;
}

//==========================================================================
// �`�揈��
//==========================================================================
void CSlash::Draw(void)
{
	// �`�揈��
	CImpactWave::Draw();
}
