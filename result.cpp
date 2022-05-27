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
#define TEXTURE_MAX					(12)				// �e�N�X�`���̐�

#define TEXTURE_WIDTH_LOGO			(480)			// ���S�T�C�Y
#define TEXTURE_HEIGHT_LOGO			(80)			// 

#define RESULT_MAX					(6)				// ON/OFF�X�C�b�`���K�v�ȃe�N�X�`����

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/result_bg.png",
	"data/TEXTURE/result_effect1.png",
	"data/TEXTURE/result_effect2.png",
	"data/TEXTURE/stage1.png",
	"data/TEXTURE/stage2.png",
	"data/TEXTURE/stage3.png",
	"data/TEXTURE/stage4.png",
	"data/TEXTURE/max_combo.png",
	"data/TEXTURE/totalscore.png",
	"data/TEXTURE/speech_boom.png",
	"data/TEXTURE/comboNum.png",
	"data/TEXTURE/time0.png",
};


static BOOL						g_Use[RESULT_MAX];						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo[RESULT_MAX];					// �e�N�X�`���ԍ�

static int						g_time;

static int						g_totale;

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
	g_Pos   = { g_w / 5.0f, 60.0f, 0.0f };

	g_time = 0;

	g_totale = 0;

	for (int i = 0; i < stage_max; i++)
	{
		g_totale += GetMainScore(i);
	}


	for (int i = 0; i < RESULT_MAX; i++)
	{
		g_Use[i] = FALSE;

		switch (i)
		{

		case 0:
			g_TexNo[i] = 3;
			break;

		case 1:
			g_TexNo[i] = 4;

			break;

		case 2:
			g_TexNo[i] = 5;
			break;

		case 3:
			g_TexNo[i] = 6;

			break;

		case 4:
			g_TexNo[i] = 8;

			break;

		case 5:
			g_TexNo[i] = 10;

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
				SetSprite(g_VertexBuffer, g_Pos.x, g_Pos.y + (i * 65.0f), TEXTURE_WIDTH_LOGO / 2.0f, TEXTURE_HEIGHT_LOGO / 2.0f, 0.0f, 0.0f, 1.0f, 1.0f);

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
				float pw = 32;			// �X�R�A�̕\����
				float ph = 50;			// �X�R�A�̕\������
				float px = 500.0f - i * pw;	// �X�R�A�̕\���ʒuX
				float py = 60.0f + (m * 65.0f);			// �X�R�A�̕\���ʒuY

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
			SetSpriteLeftTop(g_VertexBuffer, 630.0f, 60.0f, g_w / 4, g_h / 5, 0.0f, 0.0f, 1.0f, 1.0f);

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
			float pw = 80;			// �X�R�A�̕\����
			float ph = 140;			// �X�R�A�̕\������
			float px = 900.0f - i * pw;	// �X�R�A�̕\���ʒuX
			float py = 230.0f;			// �X�R�A�̕\���ʒuY

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
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLeftTop(g_VertexBuffer, 30.0f, 340.0f, g_w / 4, g_h / 5, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

}




