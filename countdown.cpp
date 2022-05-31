//=============================================================================
//
// �J�E���g�_�E����ʏ��� [countdown.cpp]
// Author : �F�V�`�O
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "countdown.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(19)				// �e�N�X�`���̐�

#define TEXTURE_WIDTH_LOGO			(480)			// ���S�T�C�Y
#define TEXTURE_HEIGHT_LOGO			(80)			// 

#define CLEAR_TIME					(300)			// �X�e�[�W�N���A�����t���[���o���Ă�����
#define COUNTDOWN					(60)			// ��1�b

#define TEXTURE_STAGE_MAX			(4)				// 1�X�e�[�W���̃e�N�X�`���͉����H

#define ADD_TEXTURE					(3)				// �ǉ��̂��j���e�N�X�`���̐�
#define ADD_TEXTURE_SPEED			(15)			// �ǉ��̂��j���e�N�X�`�������t���[���ŏo����

#define ADD_TEXTURE_START1			(20)			// 2���ڂ̒ǉ��̂��j���e�N�X�`�����o���܂ł̎���
#define ADD_TEXTURE_START2			(20 + ADD_TEXTURE_START1)		// 3���ڂ̒ǉ��̂��j���e�N�X�`�����o���܂ł̎���


enum {
	cracker = 16,
	celebration,
	balloon,
};
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/stage_clear0.png",
	"data/TEXTURE/stage_count03.png",
	"data/TEXTURE/stage_count02.png",
	"data/TEXTURE/stage_count01.png",
	"data/TEXTURE/stage_clear1.png",
	"data/TEXTURE/stage_count13.png",
	"data/TEXTURE/stage_count12.png",
	"data/TEXTURE/stage_count11.png",
	"data/TEXTURE/stage_clear2.png",
	"data/TEXTURE/stage_count23.png",
	"data/TEXTURE/stage_count22.png",
	"data/TEXTURE/stage_count21.png",
	"data/TEXTURE/stage_clear3.png",
	"data/TEXTURE/stage_count33.png",
	"data/TEXTURE/stage_count32.png",
	"data/TEXTURE/stage_count31.png",
	"data/TEXTURE/cracker.png",
	"data/TEXTURE/celebration.png",
	"data/TEXTURE/balloon.png",
};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

static BOOL						g_Use2[ADD_TEXTURE];		// TRUE:�g���Ă���  FALSE:���g�p
static XMFLOAT3					g_Pos2[ADD_TEXTURE];		// �|���S���̍��W

static int						g_Stage;					// ���݂̃X�e�[�W

static int						g_Count;					// ���ԊǗ�

static BOOL						g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitCountDown(void)
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
	g_Count = 0;

	for (int i = 0; i < ADD_TEXTURE; i++)
	{
		g_Use2[i] = FALSE;
		g_Pos2[i] = XMFLOAT3(g_w / 2, g_h / 2, 0.0f);
	}

	// �ŏ��͉B��Ă���
	g_Pos2[0].y -= TEXTURE_HEIGHT;
	g_Pos2[1].y += TEXTURE_HEIGHT;
	g_Pos2[2].x += TEXTURE_WIDTH;

	g_Stage = GetStage() - 1;
	switch (g_Stage)
	{
	case stage2:
		g_Use2[2] = TRUE;

	case stage1:
		g_Use2[1] = TRUE;

	case stage0:
		g_Use2[0] = TRUE;
		// SE�Đ�
		PlaySound(SOUND_LABEL_SE_cheers05);
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitCountDown(void)
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
void UpdateCountDown(void)
{
	// ���Ԃ�i�߂�
	g_Count++;
	
	// ���Ԍo�߂ƃX�e�[�W���ɉ����ĕ\������e�N�X�`����ύX
	if (g_Count <= CLEAR_TIME)
	{
		g_TexNo = TEXTURE_STAGE_MAX * g_Stage;
	}
	else if (g_Count <= CLEAR_TIME + COUNTDOWN)
	{
		g_TexNo = TEXTURE_STAGE_MAX * g_Stage + 1;
	}
	else if (g_Count <= CLEAR_TIME + (COUNTDOWN * 2))
	{
		g_TexNo = TEXTURE_STAGE_MAX * g_Stage + 2;
	}
	else if (g_Count <= CLEAR_TIME + (COUNTDOWN * 3))
	{
		g_TexNo = TEXTURE_STAGE_MAX * g_Stage + 3;
		SetFade(FADE_OUT, MODE_GAME);
	}


	// ���j���e�N�X�`��
	switch (g_Stage)
	{
	case 2:
		if (g_Count >= ADD_TEXTURE_START2)
		{
			g_Pos2[2].x += (g_w / 2 - g_Pos2[2].x) / ADD_TEXTURE_SPEED;
		}

	case 1:
		if (g_Count >= ADD_TEXTURE_START1)
		{
			g_Pos2[1].y += (g_h / 2 - g_Pos2[1].y) / ADD_TEXTURE_SPEED;
		}

	case 0:
		g_Pos2[0].y += (g_h / 2 - g_Pos2[0].y) / ADD_TEXTURE_SPEED;
		break;

	}

	// �N���A��ʂ���ς�����炨�j���e�N�X�`��������
	if (g_Count >= CLEAR_TIME)
	{
		for (int i = 0; i < ADD_TEXTURE; i++)
		{
			g_Use2[i] = FALSE;
		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawCountDown(void)
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

	// ���S��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// ���j���e�N�X�`����`��
	for(int i = 0 ; i < ADD_TEXTURE ; i++)
	{
		if (!g_Use2[i]) continue;

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[cracker + i]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, g_Pos2[i].x, g_Pos2[i].y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

}