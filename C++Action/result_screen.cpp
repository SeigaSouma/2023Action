//=============================================================================
// 
//  ���U���g��ʏ��� [result_screen.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "result_screen.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "object2D.h"
#include "calculation.h"
#include "multinumber.h"
#include "resultscore.h"
#include "sound.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define NUMBERTEX	"data\\TEXTURE\\number_blackclover_01.png"
#define DETAILSCORE_POSX		(400.0f)		// �X�R�A�ڍׂ�Y�ʒu
#define DETAILSCORE_POSY		(450.0f)		// �X�R�A�ڍׂ�Y�ʒu
#define DETAILSCORE_DISTANCE	(420.0f * 0.6f)	// �X�R�A�ڍׂ̊Ԋu
#define NUMBERDIS_X				(30.0f)			// �����̊��o
#define INIT_POSX				(1600.0f)		// �����ʒu
#define TEXT_MOVETIME			(20)			// �ړ�����
#define INT_NUMBERSET			(60)			// �X�R�A�̕ϓ�����

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
const char *CResultScreen::m_apTextureFile[VTX_LOGO_MAX] =				// �e�N�X�`���̃t�@�C��
{
	"data\\TEXTURE\\resultlogo_01.png",
	"data\\TEXTURE\\resultscore_01.png",
	"data\\TEXTURE\\resultscore_02.png",
	"data\\TEXTURE\\resultscore_03.png",
};

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CResultScreen::CResultScreen(int nPriority) : CObject(nPriority)
{
	// �l�̃N���A
	for (int nCntTex = 0; nCntTex < VTX_LOGO_MAX; nCntTex++)
	{
		m_nTexIdx[nCntTex] = 0;		// �e�N�X�`���̃C���f�b�N�X�ԍ�
		m_pObj2D[nCntTex] = NULL;
	}
	memset(&m_fPosDestObj2D[0], NULL, sizeof(m_fPosDestObj2D));	// �I�u�W�F2D�̖ڕW�ʒu
	memset(&m_fPosDestNumber[0], NULL, sizeof(m_fPosDestNumber));	// �����̖ڕW�ʒu
	memset(&m_apNumber[0], NULL, sizeof(m_apNumber));	// �����̃I�u�W�F�N�g
	memset(&m_nNumDest[0], NULL, sizeof(m_nNumDest));	// �{�[�i�X���̖ڕW�l
	memset(&m_fNum[0], NULL, sizeof(m_fNum));	// �{�[�i�X��
	m_nTexIdxNumber = 0;								// �����e�N�X�`���̃C���f�b�N�X�ԍ�
	m_nArrival = false;	// �����������̔���
	m_bSetNumber = false;										// �ݒ肵�����̔���
	m_pResultScore = NULL;						// ���U���g�X�R�A�̃I�u�W�F�N�g
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CResultScreen::~CResultScreen()
{

}

//==========================================================================
// ��������
//==========================================================================
CResultScreen *CResultScreen::Create(void)
{
	// �����p�̃I�u�W�F�N�g
	CResultScreen *pTitleScreen = NULL;

	if (pTitleScreen == NULL)
	{// NULL��������

		// �������̊m��
		pTitleScreen = DEBUG_NEW CResultScreen;

		if (pTitleScreen != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ����������
			pTitleScreen->Init();
		}

		return pTitleScreen;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CResultScreen::Init(void)
{
	// ��ނ̐ݒ�
	SetType(TYPE_RESULTSCREEN);

	for (int nCntSelect = 0; nCntSelect < VTX_LOGO_MAX; nCntSelect++)
	{
		// ��������
		m_pObj2D[nCntSelect] = CObject2D::Create(7);

		// ��ނ̐ݒ�
		m_pObj2D[nCntSelect]->SetType(TYPE_RESULTSELECT);

		// �e�N�X�`���̊��蓖��
		m_nTexIdx[nCntSelect] = CManager::GetInstance()->GetTexture()->Regist(m_apTextureFile[nCntSelect]);

		// �e�N�X�`���̊��蓖��
		m_pObj2D[nCntSelect]->BindTexture(m_nTexIdx[nCntSelect]);

		int nNum = nCntSelect - VTX_RESULTSCORE_RED;
		float fPosX = DETAILSCORE_POSX + nNum * DETAILSCORE_DISTANCE;

		if (nCntSelect >= VTX_RESULTSCORE_NORMAL)
		{
			int nIdx = nCntSelect - VTX_RESULTSCORE_NORMAL;
			m_fPosDestObj2D[nIdx] = fPosX;
		}

		// �e��ϐ��̏�����
		switch (nCntSelect)
		{
		case VTX_LOGO:
			m_pObj2D[nCntSelect]->SetSize(CManager::GetInstance()->GetTexture()->GetImageSize(m_nTexIdx[nCntSelect]) * 0.4f);	// �T�C�Y
			m_pObj2D[nCntSelect]->SetPosition(D3DXVECTOR3(640.0f, 150.0f, 0.0f));	// �ʒu
			break;

		case VTX_RESULTSCORE_NORMAL:
			m_pObj2D[nCntSelect]->SetSize(CManager::GetInstance()->GetTexture()->GetImageSize(m_nTexIdx[nCntSelect]) * 0.25f);	// �T�C�Y
			m_pObj2D[nCntSelect]->SetPosition(D3DXVECTOR3(INIT_POSX, DETAILSCORE_POSY, 0.0f));	// �ʒu
			break;

		case VTX_RESULTSCORE_RED:
			m_pObj2D[nCntSelect]->SetSize(CManager::GetInstance()->GetTexture()->GetImageSize(m_nTexIdx[nCntSelect]) * 0.25f);	// �T�C�Y
			m_pObj2D[nCntSelect]->SetPosition(D3DXVECTOR3(INIT_POSX, DETAILSCORE_POSY, 0.0f));	// �ʒu
			break;

		case VTX_RESULTSCORE_GOLD:
			m_pObj2D[nCntSelect]->SetSize(CManager::GetInstance()->GetTexture()->GetImageSize(m_nTexIdx[nCntSelect]) * 0.25f);	// �T�C�Y
			m_pObj2D[nCntSelect]->SetPosition(D3DXVECTOR3(INIT_POSX, DETAILSCORE_POSY, 0.0f));	// �ʒu
			break;
		}
	}


	// �����e�N�X�`���̊��蓖��
	m_nTexIdxNumber = CManager::GetInstance()->GetTexture()->Regist(NUMBERTEX);

	for (int nCntType = 0; nCntType < MAX_SCORETYPE; nCntType++)
	{
		float fPosX = (DETAILSCORE_POSX + 40.0f) + (nCntType - 1) * DETAILSCORE_DISTANCE;

		// ��������
		m_apNumber[nCntType] = CMultiNumber::Create(D3DXVECTOR3(INIT_POSX, DETAILSCORE_POSY + 5.0f, 0.0f), D3DXVECTOR2(40.0f, 50.0f) * 0.6f, SCORETYPE_DIGIT, CNumber::OBJECTTYPE_2D);

		//m_fPosDestNumber[nCntType] = fPosX + nCntScore * NUMBERDIS_X;
		m_fPosDestNumber[nCntType] = fPosX;
	}

	// ���U���g�̃X�R�A�ݒ�
	m_pResultScore = CResultScore::Create();

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CResultScreen::Uninit(void)
{
	// ���U���g�X�R�A
	m_pResultScore = NULL;

	for (int nCntSelect = 0; nCntSelect < VTX_LOGO_MAX; nCntSelect++)
	{
		if (m_pObj2D[nCntSelect] != NULL)
		{// NULL����Ȃ�������

			// �I������
			//m_pObj2D[nCntSelect]->Uninit();
			m_pObj2D[nCntSelect] = NULL;
		}
	}

	for (int nCntType = 0; nCntType < MAX_SCORETYPE; nCntType++)
	{
		// �����̃I�u�W�F�N�g�̏I������
		if (m_apNumber[nCntType] != NULL)
		{
			m_apNumber[nCntType]->Uninit();
			delete m_apNumber[nCntType];
			m_apNumber[nCntType] = NULL;
		}
	}

	// ���폜
	Release();
}

//==========================================================================
// �X�V����
//==========================================================================
void CResultScreen::Update(void)
{
	if (m_nArrival == false)
	{
		for (int nCntSelect = 0; nCntSelect < VTX_LOGO_MAX; nCntSelect++)
		{
			if (m_pObj2D[nCntSelect] != NULL)
			{// NULL����Ȃ�������

				if (m_pResultScore->IsSetBase() == true && nCntSelect >= VTX_RESULTSCORE_NORMAL)
				{
					// �I�u�W�F2D�̈ړ�����
					MoveObj2D(nCntSelect);
				}

				// ���_���ݒ�
				SetVtx(nCntSelect);
			}
		}
	}

	if (m_bSetNumber == false)
	{
		for (int nCntType = 0; nCntType < MAX_SCORETYPE; nCntType++)
		{
			if (m_pResultScore->IsSetBase() == true)
			{
				// �����̈ړ�����
				MoveNumber(nCntType);
			}

			// ���폜
			m_apNumber[nCntType]->SetValue((int)m_fNum[nCntType]);
		}
	}
}

//==========================================================================
// �I�u�W�F2D�̈ړ�����
//==========================================================================
void CResultScreen::MoveObj2D(int nCntType)
{
	int nIdx = nCntType - VTX_RESULTSCORE_NORMAL;

	// �ʒu�擾
	D3DXVECTOR3 pos = m_pObj2D[nCntType]->GetPosition();

	// �ړ�
	pos.x += (m_fPosDestObj2D[nIdx] - INIT_POSX) / (float)TEXT_MOVETIME;
	if (pos.x <= m_fPosDestObj2D[nIdx])
	{// �␳����
		pos.x = m_fPosDestObj2D[nIdx];
		m_nArrival = true;	// �����̔���
	}

	// �ʒu�ݒ�
	m_pObj2D[nCntType]->SetPosition(pos);
}

//==========================================================================
// �����̈ړ�����
//==========================================================================
void CResultScreen::MoveNumber(int nCntType)
{

	// �ʒu�擾
	D3DXVECTOR3 pos = m_apNumber[nCntType]->GetPosition();
	if (pos.x == INIT_POSX)
	{
		// �T�E���h�Đ�
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_NUMBERMOVE);
	}

	// �ړ�
	pos.x += (m_fPosDestNumber[nCntType] - INIT_POSX) / (float)TEXT_MOVETIME;
	if (pos.x <= m_fPosDestNumber[nCntType])
	{// �␳����
		pos.x = m_fPosDestNumber[nCntType];
		//m_nArrival = true;	// �����̔���
	}

	// �ʒu�ݒ�
	m_apNumber[nCntType]->SetPosition(pos);

	if (m_nArrival == true)
	{// �������Ă�����

		// �ڕW�̐��l
#if _DEBUG
		m_nNumDest[nCntType] = 100;
#else
		m_nNumDest[nCntType] = CManager::GetInstance()->GetResultManager()->GetScoreTypeNum((CResultManager::ADDTYPE)(nCntType + 1));
#endif
		if (m_nNumDest[nCntType] > m_fNum[nCntType])
		{// �K�莞�Ԃ����ĕ␳

			// �T�E���h�Đ�
			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_NUMBER);

			m_fNum[nCntType] += ((float)m_nNumDest[nCntType] / (float)INT_NUMBERSET);
		}
		else
		{// �ڕW�̒l�ŌŒ�
			m_fNum[nCntType] = (float)m_nNumDest[nCntType];
			m_bSetNumber = true;
		}
	}

}

//==========================================================================
// �S�Ă̓����ݒ�
//==========================================================================
void CResultScreen::SetAllArrival(void)
{

	// �S�Ă̓�������
	m_pResultScore->SetAllArrival();

	m_nArrival = true;		// �����������̔���
	m_bSetNumber = true;	// �ݒ肵�����̔���

	for (int nCntSelect = 0; nCntSelect < VTX_LOGO_MAX; nCntSelect++)
	{
		if (m_pObj2D[nCntSelect] != NULL && nCntSelect >= VTX_RESULTSCORE_NORMAL)
		{// NULL����Ȃ�������

			int nIdx = nCntSelect - VTX_RESULTSCORE_NORMAL;

			// �ʒu�擾
			D3DXVECTOR3 pos = m_pObj2D[nCntSelect]->GetPosition();

			// �ړ�
			pos.x = m_fPosDestObj2D[nIdx];

			// �ʒu�ݒ�
			m_pObj2D[nCntSelect]->SetPosition(pos);

			// ���_���ݒ�
			SetVtx(nCntSelect);
		}
	}

	for (int nCntType = 0; nCntType < MAX_SCORETYPE; nCntType++)
	{
		// �ʒu�擾
		D3DXVECTOR3 pos = m_apNumber[nCntType]->GetPosition();

		// �ړ�
		pos.x = m_fPosDestNumber[nCntType];

		// �ʒu�ݒ�
		m_apNumber[nCntType]->SetPosition(pos);

		// �ڕW�̒l�擾
		m_nNumDest[nCntType] = CManager::GetInstance()->GetResultManager()->GetScoreTypeNum((CResultManager::ADDTYPE)(nCntType + 1));
		m_fNum[nCntType] = (float)m_nNumDest[nCntType];

		// ���폜
		m_apNumber[nCntType]->SetValue((int)m_fNum[nCntType]);
	}

}

//==========================================================================
// �`�揈��
//==========================================================================
void CResultScreen::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	//// �A���t�@�e�X�g��L���ɂ���
	//pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	//pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	//pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	//for (int nCntSelect = 0; nCntSelect < VTX_LOGO_MAX; nCntSelect++)
	//{
	//	// �`�揈��
	//	m_pObj2D[nCntSelect]->Draw();
	//}

	//// �A���t�@�e�X�g�𖳌��ɂ���
	//pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	//pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	//pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}

//==========================================================================
// ���_���ݒ菈��
//==========================================================================
void CResultScreen::SetVtx(void)
{

}

//==========================================================================
// ���_���ݒ菈��
//==========================================================================
void CResultScreen::SetVtx(int nCntSelect)
{
	// ���_�ݒ�
	m_pObj2D[nCntSelect]->SetVtx();
}