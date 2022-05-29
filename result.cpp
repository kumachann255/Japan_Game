//=============================================================================
//
// ���U���g��ʏ��� [result.cpp]
// Author : GP12B183 �R�c����
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "result.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "score.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(16)				// �e�N�X�`���̐�

#define TEXTURE_WIDTH_LOGO			(480)			// ���S�T�C�Y
#define TEXTURE_HEIGHT_LOGO			(80)			// 

#define RESULT_MAX					(17)				// ON/OFF�X�C�b�`���K�v�ȃe�N�X�`����

#define RANK_A_BORDER				(300)
#define RANK_B_BORDER				(200)
#define RANK_C_BORDER				(100)
#define RANK_S_BORDER				(1000)

#define MOVE_VOLUME					(20.0f)		// �J�[�u�̔��a
#define MOVE_SPEED					(0.1f)		// �J�[�u�̑��x

//enum 
//{
//	result_bg,
//	result_effect,
//	rank_effect,
//	stage1,
//	stage2,
//	stage3,
//	stage4,
//	MaxCombo,
//	TotalScore,
//	BomEffect,
//	ComboNum,
//	FinalScore,
//	RankA,
//	RankB,
//	RankC,
//	RankS
//};
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/result_bg2.png",
	"data/TEXTURE/result_effect.png",
	"data/TEXTURE/rank_effect.png",
	"data/TEXTURE/stage1.png",
	"data/TEXTURE/stage2.png",
	"data/TEXTURE/stage3.png",
	"data/TEXTURE/stage4.png",
	"data/TEXTURE/max_combo.png",
	"data/TEXTURE/totalscore.png",
	"data/TEXTURE/speech_boom.png",
	"data/TEXTURE/comboNum.png",
	"data/TEXTURE/time0.png",
	"data/TEXTURE/rankA.png",
	"data/TEXTURE/rankB.png",
	"data/TEXTURE/rankC.png",
	"data/TEXTURE/rankS.png",

};

//static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p	�f�o�b�O�p

static BOOL						g_Use[RESULT_MAX];						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo[RESULT_MAX];					// �e�N�X�`���ԍ�

static int						g_time;

static int						g_totale;

static XMFLOAT4					g_Colar;

static int						g_timeRank;

static BOOL						g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitResult(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// �ϐ��̏�����
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = { g_w / 3.6f, 130.0f, 0.0f };

	g_time = 0;

	g_totale = 0;

	g_Colar.w = 0.0f;

	g_timeRank = 0;

	for (int i = 0; i < stage_max; i++)
	{
		g_totale += GetMainScore(i);
	}

	//g_Use = TRUE;		// �f�o�b�O�p

	for (int i = 0; i < RESULT_MAX; i++)
	{
		g_Use[i] = FALSE;

		switch (i)
		{

		case 0:
			g_TexNo[i] = 3;	// �X�e�[�W�P���S
			break;

		case 1:
			g_TexNo[i] = 4;	// �X�e�[�W2���S

			break;

		case 2:
			g_TexNo[i] = 5;	// �X�e�[�W3���S
			break;

		case 3:
			g_TexNo[i] = 6;	// �X�e�[�W4���S

			break;

		case 4:
			g_TexNo[i] = 8;	//�g�[�^�����S

			break;

		case 5:
			g_TexNo[i] = 10; // �g�[�^���X�R�A
			break;

		case 6:
			g_TexNo[i] = 7;	// �ő�R���{���S
			break;

		case 7:
			g_TexNo[i] = 10; // �ő�R���{��
			break;

		case 8:
			g_TexNo[i] = 12;	// rankA
			break;

		case 9:
			g_TexNo[i] = 13;	// rankB
			break;

		case 10:
			g_TexNo[i] = 14;	// rankC
			break;

		case 11:
			g_TexNo[i] = 15;	// rankS
			break;

		case 12:
			g_TexNo[i] = 9;		// �����N�\���G�t�F�N�g
			break;

		case 13:
			g_TexNo[i] = 2;		// �����NS�G�t�F�N�g
			break;

		case 14:
			g_TexNo[i] = 9;		// �����NS�\���G�t�F�N�g
			break;

		default:
			break;
		}
	}

	// BGM�Đ�
	//PlaySound(SOUND_LABEL_BGM_sample002);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitResult(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateResult(void)
{

	if (GetKeyboardTrigger(DIK_RETURN))
	{// Enter��������A�X�e�[�W��؂�ւ���
		SetFade(FADE_OUT, MODE_ENDROLL);
	}
	// �Q�[���p�b�h�œ��͏���
	else if (IsButtonTriggered(0, BUTTON_START))
	{
		SetFade(FADE_OUT, MODE_ENDROLL);
	}
	else if (IsButtonTriggered(0, BUTTON_B))
	{
		SetFade(FADE_OUT, MODE_ENDROLL);
	}



	g_time++;

	for (int i = 0; i < stage_max; i++)
	{
		if (g_time >= 45 * (i + 1))
		{
			g_Use[i] = TRUE;
		}
	}

	if (g_time >= 250 )
	{
		g_Use[4] = TRUE;

	}

	if (g_time >= 330)
	{
		g_Use[5] = TRUE;

	}

	if (g_time >= 390)
	{
		g_Use[6] = TRUE;
		g_Use[7] = TRUE;
	}

	if (g_time >= 440)
	{

		if (g_timeRank == 0)
		{
			g_Colar.w += 0.04f;

		}

		if (g_Colar.w >= 1.0f)
		{
			g_Colar.w = 1.0f;
			g_timeRank++;


			if (0 <= g_totale && g_totale < RANK_C_BORDER)
			{
				g_Use[10] = TRUE;
				g_Use[12] = TRUE;


			}
			if (RANK_C_BORDER <= g_totale && g_totale < RANK_B_BORDER)
			{
				g_Use[9] = TRUE;
				g_Use[12] = TRUE;


			}
			if (RANK_B_BORDER <= g_totale && g_totale < RANK_A_BORDER)
			{
				g_Use[8] = TRUE;
				g_Use[12] = TRUE;


			}
			if ( g_totale >= RANK_S_BORDER)
			{
				g_Use[11] = TRUE;
				g_Use[13] = TRUE;
				g_Use[14] = TRUE;


			}


		}

		if ((g_timeRank >60) && (g_Colar.w > 0.0f))
		{
			g_Colar.w -= 0.04f;

		}




		//if (g_totale >= RANK_S_BORDER)

		//{
		//	g_Use[11] = TRUE;
		//	g_Use[13] = TRUE;

		//}
	}



#ifdef _DEBUG	// �f�o�b�O����\������
	
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawResult(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// ���U���g�̔w�i��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	// ���U���g�̃G�t�F�N�g��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}




	// �X�e�[�W�̃��S��`��
	{
		for (int i = 0; i < stage_max; i++)
		{

			if (g_Use[i] == TRUE)
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo[i]]);

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSprite(g_VertexBuffer, g_Pos.x, g_Pos.y + (i * 55.0f), TEXTURE_WIDTH_LOGO / 3.5f, TEXTURE_HEIGHT_LOGO / 3.0f, 0.0f, 0.0f, 1.0f, 1.0f);

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);

			}

		}
		// �e�N�X�`���ݒ�


		//// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		//SetSprite(g_VertexBuffer, g_Pos.x, g_Pos.y, TEXTURE_WIDTH_LOGO, TEXTURE_HEIGHT_LOGO, 0.0f, 0.0f, 1.0f, 1.0f);

		//// �|���S���`��
		//GetDeviceContext()->Draw(4, 0);
	}


	// �e�X�e�[�W�̃X�R�A�\��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[11]);

		for (int m = 0; m < stage_max; m++)
		{

			// ��������������
			int number = GetMainScore(m);
			for (int i = 0; i < SCORE_DIGIT; i++)
			{
				// ����\�����錅�̐���
				float x = (float)(number % 10);

				// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
				float pw = 22;			// �X�R�A�̕\����
				float ph = 40;			// �X�R�A�̕\������
				float px = 450.0f - i * pw;	// �X�R�A�̕\���ʒuX
				float py = 130.0f + (m * 55.0f);			// �X�R�A�̕\���ʒuY

				float tw = 1.0f / 10;		// �e�N�X�`���̕�
				float th = 1.0f / 1;		// �e�N�X�`���̍���
				float tx = x * tw;			// �e�N�X�`���̍���X���W
				float ty = 0.0f;			// �e�N�X�`���̍���Y���W

				if (g_Use[m] == TRUE)
				{
					// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
					SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

					// �|���S���`��
					GetDeviceContext()->Draw(4, 0);

				}

				// ���̌���
				number /= 10;
			}
		}

	}


	// �g�[�^�����S��`��
	if (g_Use[4] == TRUE)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[8]);


		{

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLeftTop(g_VertexBuffer, 510.0f, 120.0f, g_w / 4, g_h / 8, 0.0f, 0.0f, 1.0f, 1.0f);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}


	// ���v�X�R�A�\��
	if (g_Use[5] == TRUE)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[10]);

		// ��������������
		int number = g_totale;
		for (int i = 0; i < SCORE_DIGIT; i++)
		{
			// ����\�����錅�̐���
			float x = (float)(number % 10);

			// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
			float pw = 60;			// �X�R�A�̕\����
			float ph = 130;			// �X�R�A�̕\������
			float px = 750.0f - i * pw;	// �X�R�A�̕\���ʒuX
			float py = 250.0f;			// �X�R�A�̕\���ʒuY

			float tw = 1.0f / 10;		// �e�N�X�`���̕�
			float th = 1.0f / 1;		// �e�N�X�`���̍���
			float tx = x * tw;			// �e�N�X�`���̍���X���W
			float ty = 0.0f;			// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
				


			// ���̌���
			number /= 10;
		}
	 }


	// �ő�R���{�̃��S��`��
	if(g_Use[6] == TRUE)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, 200.0f, 350.0f, g_w / 6, g_h / 10, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	// �ő�R���{����`��
	if(g_Use[7] == TRUE)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[10]);

		// ��������������
		int number = GetComboMax();
		for (int i = 0; i < COMBO_MAX_DIGIT; i++)
		{
			// ����\�����錅�̐���
			float x = (float)(number % 10);

			// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
			float pw = 40;			// �X�R�A�̕\����
			float ph = 110;			// �X�R�A�̕\������
			float px = 470.0f - i * pw;	// �X�R�A�̕\���ʒuX
			float py = 380.0f;			// �X�R�A�̕\���ʒuY

			float tw = 1.0f / 10;		// �e�N�X�`���̕�
			float th = 1.0f / 1;		// �e�N�X�`���̍���
			float tx = x * tw;			// �e�N�X�`���̍���X���W
			float ty = 0.0f;			// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);



			// ���̌���
			number /= 10;
		}
	}



	// �����N��`��
	if (g_Use[8] == TRUE)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo[8]]);


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, 490.0f, 280.0f, g_w / 4, g_h / 3, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

	}

	if (g_Use[9] == TRUE)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[13]);


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, 490.0f, 280.0f, g_w / 4, g_h / 3, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

	}

	if (g_Use[10] == TRUE)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo[10]]);


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, 490.0f, 280.0f, g_w / 4, g_h / 3, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

	}

	if (g_Use[11] == TRUE)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[15]);


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, 320, 130 , g_w / 3, g_h / 2 , 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
		
	}


	// �����NS�G�t�F�N�g��`��
	if (g_Use[13] == TRUE)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	// �����N�\���O�̃G�t�F�N�g��`��
	if (g_Use[12] == TRUE)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[9]);

		// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
		float pw = 270;			// �X�R�A�̕\����
		float ph = 270;			// �X�R�A�̕\������
		float px = 610.0f;		// �X�R�A�̕\���ʒuX
		float py = 360.0f;			// �X�R�A�̕\���ʒuY

		float tw = 1.0f;			// �e�N�X�`���̕�
		float th = 1.0f;			// �e�N�X�`���̍���
		float tx = 0.0f;			// �e�N�X�`���̍���X���W
		float ty = 0.0f;			// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, g_Colar.w));


		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

	}

	// �����NS�\���O�̃G�t�F�N�g��`��
	if (g_Use[14] == TRUE)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[9]);

		// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
		float pw = g_w / 1.5f ;			// �X�R�A�̕\����
		float ph = g_h ;			// �X�R�A�̕\������
		float px = 480.0f;		// �X�R�A�̕\���ʒuX
		float py = 260.0f;			// �X�R�A�̕\���ʒuY

		float tw = 1.0f;			// �e�N�X�`���̕�
		float th = 1.0f;			// �e�N�X�`���̍���
		float tx = 0.0f;			// �e�N�X�`���̍���X���W
		float ty = 0.0f;			// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, g_Colar.w));


		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

	}

}




