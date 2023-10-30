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
#include "enemymanager.h"
#include "enemy.h"
#include "effect_slashhit.h"
#include "hitscore.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define TEXTURE			"data\\TEXTURE\\anim_slash.png"
#define ANIM_SPEED		(1)								// �ǂݍ��݊Ԋu
#define MAX_PATTERN_U	(28)							// U�̕�����
#define MAX_PATTERN_V	(1)								// V�̕�����
#define MAX_PATTERN		(MAX_PATTERN_U)					// �A�j���[�V�����p�^�[���̍ő吔
#define MOVE_U			(1.0f / (float)MAX_PATTERN_U)	// U���W�ړ���
#define MOVE_V			(1.0f / (float)MAX_PATTERN_V)	// V���W�ړ���
#define WIDTH	(32)

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CSlash::CSlash(int nPriority) : CImpactWave(nPriority)
{
	m_nCntAnim = 0;			// �A�j���[�V�����J�E���^�[
	m_nPatternAnim = 0;		// �A�j���[�V�����p�^�[��No.
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
// �e�N�X�`���ǂݍ���
//==========================================================================
void CSlash::LoadTexture(void)
{
	// �e�N�X�`���̓ǂݍ���
	CManager::GetInstance()->GetTexture()->Regist(TEXTURE);
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
			pObjMeshCylinder->SetTexDivision(1);			// �e�N�X�`���������ݒ�
			pObjMeshCylinder->SetIsAddBlend(bAddBlend);		// ���Z�������邩
			pObjMeshCylinder->m_fSizeDest = fWidth + fCenterDistance + (fAddSizeValue * nLife);
			pObjMeshCylinder->SetMoveAngle(angle);			// ����

			// �e�N�X�`���̊��蓖��
			pObjMeshCylinder->m_nTexIdx = CManager::GetInstance()->GetTexture()->Regist(TEXTURE);

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

	float sy = sqrtf(mtxCollision._11 * mtxCollision._11 + mtxCollision._21 * mtxCollision._21);
	if (sy > 1e-6) 
	{
		collisionRotation.x = atan2f(mtxCollision._32, mtxCollision._33);
		collisionRotation.y = atan2f(-mtxCollision._31, sy);
		collisionRotation.z = atan2f(mtxCollision._21, mtxCollision._11);
	}
	else 
	{
		collisionRotation.x = atan2f(-mtxCollision._23, mtxCollision._22);
		collisionRotation.y = atan2f(-mtxCollision._31, sy);
		collisionRotation.z = 0;
	}

	// ���[���h�s�񂩂��]�������v�Z
	/*collisionRotation.x = atan2f(mtxCollision._32, mtxCollision._33);
	collisionRotation.y = atan2f(-mtxCollision._31, sqrtf(mtxCollision._32 * mtxCollision._32 + mtxCollision._33 * mtxCollision._33));
	collisionRotation.z = atan2f(mtxCollision._21, mtxCollision._11);*/

	collisionRotation = rot;
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

	SetRotation(rotOrigin);
	SetOriginRotation(rot);

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

	// �F�擾
	D3DXCOLOR col = GetColor();

	// �s�����x�X�V
	col.a = m_colOrigin.a * (1.0f - (float)m_nPatternAnim / (float)(MAX_PATTERN_U + 1));

	// �F�ݒ�
	SetColor(col);

	// �O���̕�
	SetOutWidth(fOutWidth);

	// ���_�X�V
	CImpactWave::SetVtx();

	if (m_pObj3D != NULL)
	{
		m_pObj3D->Update();
		m_pObj3D->SetSize(D3DXVECTOR3(fOutWidth, 0.0f, fOutWidth));
	}

	// �����蔻��
	Collision();

	// �e�N�X�`�����W�X�V
	UpdateTex();
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
	int nUse = 0;
	for (int nCntBullet = 0; nUse < nNumAll; nCntBullet++)
	{
		if (ppBullet[nCntBullet] == NULL)
		{// NULL��������
			continue;
		}
		nUse++;

		// �e�̈ʒu
		D3DXVECTOR3 BulletPosition = ppBullet[nCntBullet]->GetPosition();
		D3DXVECTOR3 BulletMove = ppBullet[nCntBullet]->GetMove();

		// �e�Ƃ̔���
		float fBulletRadius = ppBullet[nCntBullet]->GetWidthLen();

		if (ppBullet[nCntBullet]->GetState() == CBullet::STATE_NONE &&
			ppBullet[nCntBullet]->GetType() == CBullet::TYPE_ENEMY &&
			IsHit(BulletPosition, fBulletRadius) == true)
		{// ���̔���

			float fRot = 0.0f;
			D3DXVECTOR3 SlashMove = mylib_const::DEFAULT_VECTOR3;

			float fFabsMove = fabsf(BulletMove.x * 2.5f);
			fFabsMove = 20.0f;

			// �ړ��ʐݒ�
			switch (GetMoveAngle())
			{
			case CObject::ANGLE_UP:
				fRot = D3DX_PI + GetOriginRotation().y;

				ppBullet[nCntBullet]->SetMove(D3DXVECTOR3(
					sinf(D3DX_PI + fRot) * fFabsMove,
					0.0f,
					cosf(D3DX_PI + fRot) * fFabsMove));
				break;

			case CObject::ANGLE_DOWN:
			{
				int nAngle = 1;

				if (ppBullet[nCntBullet]->GetMoveAngle() == ANGLE_LEFT)
				{
					nAngle = -1;
				}

				fRot = D3DX_PI + GetOriginRotation().y/* - ((D3DX_PI * 0.5f) * nAngle)*/;

				ppBullet[nCntBullet]->SetMove(D3DXVECTOR3(
					sinf(D3DX_PI + fRot) * fFabsMove,
					0.0f,
					cosf(D3DX_PI + fRot) * fFabsMove));
			}
			break;

			default:	// ���E

				float fRotX = GetOriginRotation().x;
				float fRotY = GetOriginRotation().x;
				if (GetMoveAngle() == ANGLE_LEFT)
				{
					fRotX += D3DX_PI;
				}

				if (ppBullet[nCntBullet]->GetMoveAngle() == ANGLE_UP ||
					ppBullet[nCntBullet]->GetMoveAngle() == ANGLE_DOWN)
				{// �e�̌�����������O
					ppBullet[nCntBullet]->SetMapIndex(GetMapIndex());
					ppBullet[nCntBullet]->SetMapMoveValue(GetMapMoveValue());
				}

				SlashMove = D3DXVECTOR3(cosf(fRotX) * fFabsMove, sinf(D3DX_PI + fRotY) * fFabsMove, 0.0f);
				ppBullet[nCntBullet]->SetMove(SlashMove);
				break;
			}

			ppBullet[nCntBullet]->SetType(CBullet::TYPE_PLAYER);
			ppBullet[nCntBullet]->SetState(CBullet::STATE_DMG, 3);
			ppBullet[nCntBullet]->SetMoveAngle(GetMoveAngle());

			// �q�b�g�X�g�b�v
			CManager::GetInstance()->SetEnableHitStop(8);

			// �U��
			CManager::GetInstance()->GetCamera()->SetShake(15, 25.0f, 0.0f);

			// �a���̃q�b�g�G�t�F�N�g����
			CEffectSlashHit::Create(BulletPosition);

			// �q�b�g�X�R�A���Z
			CGame::GetHitScore()->Add(1);
		}
	}



	// �G�}�l�[�W���擾
	CEnemyManager *pEnemyManager = CGame::GetEnemyManager();
	if (pEnemyManager == NULL)
	{// NULL��������
		return;
	}

	// �G���擾
	CEnemy **ppEnemy = pEnemyManager->GetEnemy();
	int nNumEnemy = pEnemyManager->GetNumAll();

	// ���擾
	bool bHit = false;

	nUse = 0;
	for (int nCntEnemy = 0; nUse < nNumEnemy; nCntEnemy++)
	{
		if (ppEnemy[nCntEnemy] == NULL)
		{// NULL��������
			continue;
		}
		nUse++;

		// �G�̏��擾
		D3DXVECTOR3 EnemyPosition = ppEnemy[nCntEnemy]->GetCenterPosition();
		D3DXVECTOR3 EnemyRotation = ppEnemy[nCntEnemy]->GetRotation();
		float fEnemyRadius = ppEnemy[nCntEnemy]->GetRadius();
		CEnemy::STATE EnemyState = (CEnemy::STATE)ppEnemy[nCntEnemy]->GetState();

		if (IsHit(EnemyPosition, fEnemyRadius) == true)
		{// �������Ă�����

			if (EnemyState != CEnemy::STATE_DMG && EnemyState != CEnemy::STATE_DEAD)
			{
				CImpactWave *pImpactWave = CImpactWave::Create
				(
					D3DXVECTOR3(EnemyPosition.x, EnemyPosition.y + 50.0f, EnemyPosition.z),	// �ʒu
					D3DXVECTOR3((float)(Random(-31, 31) * 0.1f), EnemyRotation.y, 0.0f),			// ����
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.4f),			// �F
					60.0f,										// ��
					0.0f,										// ����
					10,											// ����
					15.0f,										// ���̈ړ���
					CImpactWave::TYPE_PURPLE4,				// �e�N�X�`���^�C�v
					true										// ���Z�������邩
				);
				pImpactWave->SetEnableHitstopMove();
			}

			if (ppEnemy[nCntEnemy]->GetState() != CEnemy::STATE_DMG && ppEnemy[nCntEnemy]->GetState() != CEnemy::STATE_DEAD)
			{
				// �q�b�g�X�R�A���Z
				CGame::GetHitScore()->Add(1);
			}

			// �q�b�g����
			ppEnemy[nCntEnemy]->Hit(mylib_const::DMG_SLASH);

			bHit = true;
		}
	}

}

//==========================================================================
// �p�x�t���̔���
//==========================================================================
bool CSlash::IsHit(D3DXVECTOR3 TargetPos, float fTargetRadius)
{
	// ���擾
	D3DXVECTOR3 pos = GetPosition();
	D3DXVECTOR3 rot = GetOriginRotation();
	float fOutWidth = GetOutWidth();

	if (SphereRange(pos, TargetPos, 50.0f, fTargetRadius) == true)
	{// ���S����̏��������œ�����
		return true;
	}

	float fLength = sqrtf(fOutWidth * fOutWidth + fOutWidth * fOutWidth);	// �Ίp���̒���
	float fAngle = atan2f(fOutWidth, fOutWidth);							// �Ίp���̌���

	if (GetMoveAngle() == ANGLE_LEFT)
	{
		rot.x += D3DX_PI;
	}
	rot.y += D3DX_PI;

	float fPosY = sinf(rot.x) * fOutWidth;

	// ���肷��l�p��4���_
	bool bLine1 = false, bLine2 = false, bLine3 = false, bLine4 = false;

	D3DXVECTOR3 LeftUp = D3DXVECTOR3(
		pos.x + cosf(rot.x) * sinf(rot.y - fAngle) * fLength,
		pos.y + fPosY,
		pos.z + cosf(rot.x) * cosf(rot.y - fAngle) * fLength);

	D3DXVECTOR3 RightUp = D3DXVECTOR3(
		pos.x + cosf(rot.x) * sinf(rot.y + fAngle) * fLength,
		pos.y + fPosY,
		pos.z + cosf(rot.x) * cosf(rot.y + fAngle) * fLength);

	D3DXVECTOR3 LeftDown = D3DXVECTOR3(
		pos.x + cosf(rot.x) * sinf(rot.y - D3DX_PI + fAngle) * fLength,
		pos.y - fPosY,
		pos.z + cosf(rot.x) * cosf(rot.y - D3DX_PI + fAngle) * fLength);

	D3DXVECTOR3 RightDown = D3DXVECTOR3(
		pos.x + cosf(rot.x) * sinf(rot.y + D3DX_PI - fAngle) * fLength,
		pos.y - fPosY,
		pos.z + cosf(rot.x) * cosf(rot.y + D3DX_PI - fAngle) * fLength);

	float fWidth = sqrtf(
		((LeftUp.x - RightUp.x) * (LeftUp.x - RightUp.x)) +
		((LeftUp.z - RightUp.z) * (LeftUp.z - RightUp.z))) * 0.5f;

	float fHeight = sqrtf(
		((RightUp.x - RightDown.x) * (RightUp.x - RightDown.x)) +
		((RightUp.z - RightDown.z) * (RightUp.z - RightDown.z))) * 0.5f;

	if (fWidth == 0.0f)
	{// 0�̏ꍇ�������݂���������
		fWidth = 5.0f;
	}
	if (fHeight == 0.0f)
	{// 0�̏ꍇ�������݂���������
		fHeight = 5.0f;
	}

	D3DXVECTOR3 collisionRot = D3DXVECTOR3(0.0f, rot.y, 0.0f);
	bool bHitY = CollisionCircleSquare2D(TargetPos, pos, D3DXVECTOR3(0.0f, rot.y, 0.0f), fTargetRadius * 0.5f, D3DXVECTOR2(fWidth, fHeight));

	if (bHitY == false)
	{// Y�ł��瓖�����ĂȂ�������
		return false;
	}

	bool bTri1 = false, bTri2 = false;
	float fTriHeight1 = 0.0f, fTriHeight2 = 0.0f;
	fTriHeight1 = GetVtxHeight(TargetPos, LeftUp, LeftDown, RightUp, bTri1);
	fTriHeight2 = GetVtxHeight(TargetPos, RightDown, RightUp, LeftDown, bTri2);

	if (bTri1 == false && bTri2 == false)
	{// �ǂ����̎O�p�ɂ��������ĂȂ�������
		return false;
	}

	if (std::isnan(fTriHeight1) == true ||
		std::isnan(fTriHeight2) == true)
	{// �i���C���h��������
		
		if (pos.y + fOutWidth >= TargetPos.y - fTargetRadius &&
			pos.y - fOutWidth <= TargetPos.y + fTargetRadius)
		{
			return true;
		}
	}

	if (bTri1 == true &&
		fTriHeight1 >= TargetPos.y - fTargetRadius &&
		fTriHeight1 <= TargetPos.y + fTargetRadius)
	{// 1�ڂ̎O�p�ɂ������Ă��� && ���a�ȓ��ɓ����Ă���
		return true;
	}
	
	if (bTri2 == true &&
		fTriHeight2 >= TargetPos.y - fTargetRadius &&
		fTriHeight2 <= TargetPos.y + fTargetRadius)
	{// 2�ڂ̎O�p�ɂ������Ă��� && ���a�ȓ��ɓ����Ă���
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

	//m_pObj3D->Draw();

	// ���C�e�B���O��L���ɂ���
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// �J�����O�̃f�t�H���g
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}

//==========================================================================
// �e�N�X�`�����W�X�V
//==========================================================================
void CSlash::UpdateTex(void)
{
	D3DXVECTOR2 *pVtxTex = GetVtxTex();	// �e�N�X�`�����W�擾

	if (pVtxTex== NULL)
	{
		return;
	}

	// �e�N�X�`��������
	int nTexDivision = GetTexDivision();

	// �������擾
	int nHeightBlock = GetHeightBlock();
	int nWidthBlock = GetWidthBlock();



	// �J�E���g���X�V
	m_nCntAnim = (m_nCntAnim + 1) % ANIM_SPEED;

	// �p�^�[���X�V
	if ((m_nCntAnim % ANIM_SPEED) == 0)
	{
		// �p�^�[��No.���X�V
		m_nPatternAnim = (m_nPatternAnim + 1) % MAX_PATTERN;

		if (m_nPatternAnim == 0)
		{// �p�^�[�������������

			// �I�u�W�F�N�g�j��
			Uninit();
			return;
		}
	}



	// ���_���̐ݒ�
	for (int nCntHeight = 0; nCntHeight < nHeightBlock + 1; nCntHeight++)
	{// �c�̒��_�����J��Ԃ�

		for (int nCntWidth = 0; nCntWidth < nWidthBlock + 1; nCntWidth++)
		{// ���̒��_�����J��Ԃ�

			float fTex = m_nPatternAnim * MOVE_U;
			float fTexDest = (m_nPatternAnim + 1) * MOVE_U;

			if (nCntHeight == 1)
			{
				fTex = (m_nPatternAnim + 1) * MOVE_U;
			}

			// �X�^�[�g + 
			float f = fTex + nCntWidth * (MOVE_U / (float)nWidthBlock);

			pVtxTex[nCntWidth + (nCntHeight * (nWidthBlock + 1))] = D3DXVECTOR2
			(
				f,
				nCntHeight * (1.0f / (float)(nHeightBlock))
			);
		}
	}
}
