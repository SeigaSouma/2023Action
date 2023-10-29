//=============================================================================
// 
//  �q�b�g�X�R�A���� [hitscore.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "hitscore.h"
#include "manager.h"
#include "texture.h"
#include "renderer.h"
#include "object2D.h"
#include "multinumber.h"
#include "calculation.h"
#include "game.h"
#include "score.h"
#include "effect_addscore.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define TEXTURE			"data\\TEXTURE\\number_blackclover_02.png"	// �e�N�X�`���̃t�@�C��
#define HIT_TEXTURE		"data\\TEXTURE\\hit.png"	// �e�N�X�`���̃t�@�C��
#define NUM_SCORE		(4)			// ����
#define MAX_VALUE		(999999)	// �l�̍ő�l
#define TEX_U			(0.1f)		// U�̕���
#define WIDTH			(40.0f)		// ����
#define HEIGHT			(40.0f)		// �c��
#define MOVETIME		(20)		// �ړ�����
#define COOLTIME		(60 * 5 + MOVETIME)	// �ړ��܂ł̃N�[���^�C��
#define DEST_POSITION	(1000.0f)	// �ڕW�̈ʒu

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CHitScore::CHitScore(int nPriority)
{
	// �l�̃N���A
	m_apNumber = NULL;
	m_pObj2D = NULL;	// �I�u�W�F�N�g2D�̃I�u�W�F�N�g
	m_nTexIdx = 0;		// �e�N�X�`���̃C���f�b�N�X�ԍ�
	m_nCntCooltime = 0;	// �N�[���^�C���̃J�E���^�[
	m_posOrigin = mylib_const::DEFAULT_VECTOR3;	// ���̈ʒu
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CHitScore::~CHitScore()
{

}

//==========================================================================
// ��������
//==========================================================================
CHitScore *CHitScore::Create(void)
{
	// �����p�̃I�u�W�F�N�g
	CHitScore *pScore = NULL;

	if (pScore == NULL)
	{// NULL��������

		// �������̊m��
		pScore = DEBUG_NEW CHitScore;

		if (pScore != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ����������
			pScore->Init();
		}

		return pScore;
	}

	return NULL;
}

//==========================================================================
// ��������
//==========================================================================
CHitScore *CHitScore::Create(D3DXVECTOR3 pos)
{
	// �����p�̃I�u�W�F�N�g
	CHitScore *pScore = NULL;

	if (pScore == NULL)
	{// NULL��������

		// �������̊m��
		pScore = DEBUG_NEW CHitScore;

		if (pScore != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ����������
			pScore->Init(pos);
		}

		return pScore;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CHitScore::Init(void)
{
	// ��������
	m_apNumber = CMultiNumber::Create(mylib_const::DEFAULT_VECTOR3, D3DXVECTOR2(WIDTH, HEIGHT), NUM_SCORE, CNumber::OBJECTTYPE_2D, TEXTURE, true);

	// �l�̐ݒ菈��
	m_apNumber->SetValue(0);
	return S_OK;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CHitScore::Init(D3DXVECTOR3 pos)
{
	// ���̈ʒu
	m_posOrigin = pos;

	// ��������
	m_apNumber = CMultiNumber::Create(m_posOrigin, D3DXVECTOR2(WIDTH, HEIGHT), NUM_SCORE, CNumber::OBJECTTYPE_2D, TEXTURE, true);

	// �I�u�W�F�N�g2D�̃I�u�W�F�N�g
	m_pObj2D = CObject2D::Create(7);
	m_pObj2D->SetType(CObject::TYPE_BALLAST);

	int nHitTex = CManager::GetInstance()->GetTexture()->Regist(HIT_TEXTURE);
	m_pObj2D->BindTexture(nHitTex);

	m_pObj2D->SetSize(CManager::GetInstance()->GetTexture()->GetImageSize(nHitTex) * 0.3f);
	m_pObj2D->SetPosition(D3DXVECTOR3(m_posOrigin.x + (WIDTH * NUM_SCORE) + m_pObj2D->GetSize().x - 20.0f, m_posOrigin.y, m_posOrigin.z));

	// �l�̐ݒ菈��
	m_apNumber->SetValue(0);
	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CHitScore::Uninit(void)
{
	// �����̃I�u�W�F�N�g�̏I������
	if (m_apNumber != NULL)
	{
		m_apNumber->Uninit();
		delete m_apNumber;
		m_apNumber = NULL;
	}
}

//==========================================================================
// �X�V����
//==========================================================================
void CHitScore::Update(void)
{
	// �ړ�����
	UpdateMove();

	// �}���`�i���o�[�̍X�V
	m_apNumber->Update();
}

//==========================================================================
// �ړ�����
//==========================================================================
void CHitScore::UpdateMove(void)
{
	// �N�[���^�C�����Z
	m_nCntCooltime--;

	// �ʒu�擾
	D3DXVECTOR3 pos = m_apNumber->GetPosition();
	D3DXVECTOR3 posOrigin = pos;
	D3DXVECTOR3 Hitpos = m_pObj2D->GetPosition();

	if (m_nCntCooltime == MOVETIME)
	{
		// �X�R�A�v�Z����Z
		CalScore();

		// �q�b�g�����Z�b�g
		CGame::GetHitScore()->Reset();
	}

	if (m_nCntCooltime <= MOVETIME)
	{// �ړ����Ԉȉ��ňړ�

		// �ړ�
		pos.x += (m_posOrigin.x - pos.x) * 0.1f;
	}
	else
	{
		// �ړ�
		pos.x += (DEST_POSITION - pos.x) * 0.1f;
	}

	if (m_nCntCooltime <= 0)
	{
		m_nCntCooltime = 0;
	}

	float fDiff = pos.x - posOrigin.x;
	Hitpos.x += fDiff;

	// �ʒu�ݒ�
	m_apNumber->SetPosition(pos);
	m_pObj2D->SetPosition(Hitpos);
}

//==========================================================================
// �X�R�A�v�Z
//==========================================================================
void CHitScore::CalScore(void)
{
	// �l�̎擾
	int nNum = m_apNumber->GetValue();

	// �{���v�Z
	int nMultiply = nNum / 100;

	// �X�R�A���Z��
	int nAddValue = (1.0f + ((float)nMultiply * 0.1f)) * (nNum * 10);

	// �X�R�A���Z
	CGame::GetScore()->Add(nAddValue);

	// ���Z�G�t�F�N�g
	CEffectAddScore::Create(m_apNumber->GetPosition());

}

//==========================================================================
// �`�揈��
//==========================================================================
void CHitScore::Draw(void)
{
	// �`�揈��
	m_apNumber->Draw();
}

//==========================================================================
// ���Z����
//==========================================================================
void CHitScore::Add(int nValue)
{
	// �l�̎擾
	int nNum = m_apNumber->GetValue();

	// �l���Z
	nNum += nValue;

	if (nNum >= MAX_VALUE)
	{// �ő�l�ŌŒ�
		nNum = MAX_VALUE;
	}

	// �l�̐ݒ�
	m_apNumber->SetValue(nNum);

	// �N�[���^�C���̃J�E���^�[���Z�b�g
	m_nCntCooltime = COOLTIME;

}

//==========================================================================
// ���Z�b�g
//==========================================================================
void CHitScore::Reset(void)
{
	// �l�̐ݒ�
	m_apNumber->SetValue(0);
}
