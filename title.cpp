//=============================================================================
//
// �^�C�g����ʏ��� [title.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "title.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(6)				// �e�N�X�`���̐�

#define TEXTURE_WIDTH_LOGO			(815 / 2)			// ���S�T�C�Y
#define TEXTURE_HEIGHT_LOGO			(296 / 2)			// 

#define TEXTURE_WIDTH_START			(443 / 2)			// ���S�T�C�Y
#define TEXTURE_HEIGHT_START		(178 / 2)			// 

#define TEXTURE_WIDTH_EXIT			(443 / 2)			// ���S�T�C�Y
#define TEXTURE_HEIGHT_EXIT			(178 / 2)			// 

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/title.png",
	"data/TEXTURE/title_logo.png",
	"data/TEXTURE/title_start.png",
	"data/TEXTURE/title_start_pushed.png",
	"data/TEXTURE/title_exit.png",
	"data/TEXTURE/title_exit_pushed.png",
};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

static float					logo_time;
static BOOL						flag_rot;
static XMFLOAT3					logo_rot;					// �|���S���̍��W

static int						start_texno;
static float					start_w, start_h;			// ���ƍ���
static XMFLOAT3					start_pos;					// �|���S���̍��W

static int						exit_texno;
static float					exit_w, exit_h;				// ���ƍ���
static XMFLOAT3					exit_pos;					// �|���S���̍��W

static BOOL						g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitTitle(void)
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
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = XMFLOAT3(g_w/2, g_h/2, 0.0f);
	g_TexNo = 0;

	logo_time = 1.0f;
	flag_rot = TRUE;
	logo_rot = XMFLOAT3(0.0f, 0.0f, 0.1f);

	start_texno = 3;
	start_w = TEXTURE_WIDTH_START;
	start_h = TEXTURE_HEIGHT_START;
	start_pos = XMFLOAT3(g_w / 8, g_h - 60, 0.0f);
	
	exit_texno = 4;
	exit_w = TEXTURE_WIDTH_EXIT - (TEXTURE_WIDTH_EXIT / 3);
	exit_h = TEXTURE_HEIGHT_EXIT - (TEXTURE_HEIGHT_EXIT / 3);
	exit_pos = XMFLOAT3(g_w / 2.7f, g_h - 40, 0.0f);

	// BGM�Đ�
	PlaySound(SOUND_LABEL_BGM_bgm_title);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTitle(void)
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
void UpdateTitle(void)
{

	//if (GetKeyboardTrigger(DIK_RETURN))
	//{// Enter��������A�X�e�[�W��؂�ւ���
	//	SetStage(stage0);
	//	SetFade(FADE_OUT, MODE_GAME);
	//	//SetFade(FADE_OUT, MODE_RESULT);

	//	// main�X�R�A��������
	//	ResetMainScore();
	//}
	//// �Q�[���p�b�h�œ��͏���
	//else if (IsButtonTriggered(0, BUTTON_START))
	//{
	//	SetStage(stage0);
	//	SetFade(FADE_OUT, MODE_GAME);
	//}
	//else if (IsButtonTriggered(0, BUTTON_B))
	//{
	//	SetStage(stage0);
	//	SetFade(FADE_OUT, MODE_GAME);
	//}

	if (flag_rot == TRUE)
	{
		logo_time -= 0.02f;
		if (logo_time <= 0.0f)
		{
			logo_rot.z = 0.1f;
			logo_time = 0.0f;
			flag_rot = FALSE;
		}
	}
	else
	{
		logo_time += 0.02f;
		if (logo_time >= 1.0f)
		{
			logo_rot.z = -0.1f;
			logo_time = 1.0f;
			flag_rot = TRUE;
		}
	}

	//if (g_Time < 0)
	{
		if (GetKeyboardTrigger(DIK_D) || GetKeyboardTrigger(DIK_A))
		{// S/W��������A�I����؂�ւ���

			// SE�̑}���i�؂�ւ����j
			PlaySound(SOUND_LABEL_SE_titleClick04);

			if (start_texno == 3)
			{
				start_texno--;
				start_w -= TEXTURE_WIDTH_START / 3;
				start_h -= TEXTURE_HEIGHT_START / 3;

				exit_texno++;
				exit_w += TEXTURE_WIDTH_EXIT / 3;
				exit_h += TEXTURE_HEIGHT_EXIT / 3;
			}
			else
			{
				start_texno = 3;
				start_w += TEXTURE_WIDTH_START / 3;
				start_h += TEXTURE_HEIGHT_START / 3;

				exit_texno = 4;
				exit_w -= TEXTURE_WIDTH_EXIT / 3;
				exit_h -= TEXTURE_HEIGHT_EXIT / 3;

			}
			//PlaySound(SOUND_LABEL_SE_system_01);
		}


		if (GetKeyboardTrigger(DIK_RETURN))
		{// Enter��������A�X�e�[�W��؂�ւ���

			// SE�̑}���i���[�h�I�����j
			PlaySound(SOUND_LABEL_SE_selectBomb01);

			if (start_texno == 3)
			{
				//PlaySound(SOUND_LABEL_SE_system_02);
				SetStage(stage0);
				SetFade(FADE_OUT, MODE_GAME);
				//SetFade(FADE_OUT, MODE_TUTORIAL);
		
				// main�X�R�A��������
				ResetMainScore();

			}
			else
			{
				//PlaySound(SOUND_LABEL_SE_system_02);
				Sleep(800);
				exit(-1);
			}
		}


		// �Q�[���p�b�h�œ��͏���
		if (IsButtonTriggered(0, BUTTON_LEFT) || IsButtonTriggered(0, BUTTON_RIGHT))
		{// S/W��������A�I����؂�ւ���

			// SE�̑}���i�؂�ւ����j
			PlaySound(SOUND_LABEL_SE_titleClick04);

			if (start_texno == 3)
			{
				start_texno--;
				exit_texno++;
			}
			else
			{
				start_texno = 3;
				exit_texno = 4;
			}
			//PlaySound(SOUND_LABEL_SE_system_01);
		}


		else if (IsButtonTriggered(0, BUTTON_START))
		{
			// SE�̑}���i���[�h�I�����j
			PlaySound(SOUND_LABEL_SE_selectBomb01);

			if (start_texno == 3)
			{
				//PlaySound(SOUND_LABEL_SE_system_02);
				SetStage(stage0);
				SetFade(FADE_OUT, MODE_GAME);
				//SetFade(FADE_OUT, MODE_TUTORIAL);
			
				// main�X�R�A��������
				ResetMainScore();
			}
			else
			{
				//PlaySound(SOUND_LABEL_SE_system_02);
				Sleep(800);
				exit(-1);
			}
		}
		else if (IsButtonTriggered(0, BUTTON_B))
		{
			// SE�̑}���i���[�h�I�����j
			PlaySound(SOUND_LABEL_SE_selectBomb01);

			if (start_texno == 3)
			{
				//PlaySound(SOUND_LABEL_SE_system_02);
				SetStage(stage0);
				SetFade(FADE_OUT, MODE_GAME);
				//SetFade(FADE_OUT, MODE_TUTORIAL);
			
				// main�X�R�A��������
				ResetMainScore();
			}
			else
			{
				//PlaySound(SOUND_LABEL_SE_system_02);
				Sleep(800);
				exit(-1);
			}
		}
	}

	//g_Time--;





#ifdef _DEBUG	// �f�o�b�O����\������
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTitle(void)
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

	// �^�C�g���̔w�i��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// �^�C�g���̃��S��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		//	SetSprite(g_VertexBuffer, g_Pos.x, g_Pos.y, TEXTURE_WIDTH_LOGO, TEXTURE_HEIGHT_LOGO, 0.0f, 0.0f, 1.0f, 1.0f);
		SetSpriteColorRotation(g_VertexBuffer, g_Pos.x - 200, g_Pos.y - 180, TEXTURE_WIDTH_LOGO, TEXTURE_HEIGHT_LOGO, 0.0f, 0.0f, 1.0f, 1.0f,
						XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), logo_rot.z);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// �^�C�g���̃��j���[��`��
	//if (g_Time < 0)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[start_texno]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, start_pos.x, start_pos.y, start_w, start_h, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[exit_texno]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, exit_pos.x, exit_pos.y, exit_w, exit_h, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

//	// �����Z�̃e�X�g
//	SetBlendState(BLEND_MODE_ADD);		// ���Z����
////	SetBlendState(BLEND_MODE_SUBTRACT);	// ���Z����
//	for(int i=0; i<30; i++)
//	{
//		// �e�N�X�`���ݒ�
//		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);
//
//		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
//		float dx = 100.0f;
//		float dy = 100.0f;
//		float sx = (float)(rand() % 100);
//		float sy = (float)(rand() % 100);
//
//
//		SetSpriteColor(g_VertexBuffer, dx+sx, dy+sy, 50, 50, 0.0f, 0.0f, 1.0f, 1.0f,
//			XMFLOAT4(0.3f, 0.3f, 1.0f, 0.5f));
//
//		// �|���S���`��
//		GetDeviceContext()->Draw(4, 0);
//	}
//	SetBlendState(BLEND_MODE_ALPHABLEND);	// ���������������ɖ߂�

}





