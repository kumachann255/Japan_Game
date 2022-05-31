//=============================================================================
//
// �Q�[��UI���� [gameUI.cpp]
// Author : �F�V�`�O
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "gameUI.h"
#include "sprite.h"
#include "score.h"

#include "sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define HOWTO_WIDTH					(330.0f)	// �L�����T�C�Y
#define HOWTO_HEIGHT				(120.0f)	// 

#define SCORE0_WIDTH				(100.0f)	// �L�����T�C�Y
#define SCORE0_HEIGHT				(50.0f)		// 

#define SCORE1_WIDTH				(120.0f)	// �L�����T�C�Y
#define SCORE1_HEIGHT				(40.0f)		// 

#define TEXTURE_MAX					(6)			// �e�N�X�`���̐�

#define MOVE_VOLUME					(20.0f)		// �J�[�u�̔��a
#define MOVE_SPEED					(0.1f)		// �J�[�u�̑��x


enum {
	HowTo,
	score0,
	score1,
	under,
	up,
	right,


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
	"data/TEXTURE/hoetouse_ui.png",
	"data/TEXTURE/score_00.png",
	"data/TEXTURE/score_01.png",
	"data/TEXTURE/partypeople_down.png",
	"data/TEXTURE/partypeople_up.png",
	"data/TEXTURE/partypeople_right.png",
};

static UI						g_UI[TEXTURE_MAX];			// UI

static int						g_Score;					// �X�R�A

static BOOL						g_Border;					// �ڕW�X�R�A�𒴂��Ă��邩�ǂ���
static int						g_BorderScore;				// �ڕW�X�R�A

static float					g_time;

static BOOL						g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitGameUI(void)
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


	// ������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_UI[i].use = TRUE;
		g_UI[i].texNo = i;

		// ���j���e�N�X�`���͍ŏ���\��
		if (i >= under)
		{
			g_UI[i].use = FALSE;
			g_UI[i].pos = { SCREEN_WIDTH / 2 ,SCREEN_HEIGHT / 2 , 0.0f };
			g_UI[i].w = SCREEN_WIDTH;
			g_UI[i].h = SCREEN_HEIGHT;
		}

		switch (i)
		{
		case HowTo:
			g_UI[i].pos = { 170.0f , 480.0f , 0.0f };
			g_UI[i].w = HOWTO_WIDTH;
			g_UI[i].h = HOWTO_HEIGHT;

			break;

		case score0:
			g_UI[i].pos = { 700.0f , 50.0f , 0.0f };
			g_UI[i].w = SCORE0_WIDTH;
			g_UI[i].h = SCORE0_HEIGHT;

			break;

		case score1:
			g_UI[i].pos = { 750.0f , 120.0f , 0.0f };
			g_UI[i].w = SCORE1_WIDTH;
			g_UI[i].h = SCORE1_HEIGHT;

			break;

		}
	}


	// �ڕW�X�R�A�̏�����
	switch (GetStage())
	{
	case stage0:
	case tutorial:
		g_BorderScore = SCORE_STAGE0_BORDER;

		break;

	case stage1:
		g_BorderScore = SCORE_STAGE1_BORDER;

		break;

	case stage2:
		g_BorderScore = SCORE_STAGE2_BORDER;

		break;

	case stage3:
		g_BorderScore = SCORE_STAGE3_BORDER;

		break;

	}


	g_Score = 0;	// �X�R�A�̏�����
	g_time = 0.0f;
	g_Border = FALSE;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitGameUI(void)
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
void UpdateGameUI(void)
{
	// �ڕW�X�R�A�𒴂����炨�j���e�N�X�`����\��
	if (g_BorderScore <= GetScore())
	{
		for (int i = under; i < under + 3; i++)
		{
			g_UI[i].use = TRUE;

		}
	}

	// ���j���e�N�X�`���̏���
	for (int i = under; i < under + 3; i++)
	{
		switch (i)
		{
		case under:
			g_UI[i].pos.y = SCREEN_HEIGHT / 2 + (cosf(g_time) * MOVE_VOLUME) + MOVE_VOLUME;

			break;

		case up:
			g_UI[i].pos.y = SCREEN_HEIGHT / 2 - (cosf(g_time) * MOVE_VOLUME) - MOVE_VOLUME;

			break;

		case right:
			g_UI[i].pos.x = SCREEN_WIDTH / 2 - (sinf(g_time) * MOVE_VOLUME) + MOVE_VOLUME;
			break;
		}
	}

	g_time += MOVE_SPEED;

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawGameUI(void)
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

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (!g_UI[i].use) continue;

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[i]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, g_UI[i].pos.x, g_UI[i].pos.y, g_UI[i].w, g_UI[i].h, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
}



//=============================================================================
// �ڕW�X�R�A��Ԃ�
//=============================================================================
int GetBorderScore(void)
{
	return g_BorderScore;
}


//=============================================================================
// ���j���e�N�X�`�����\������Ă��邩��Ԃ�
//=============================================================================
BOOL GetCelebration(void)
{
	return g_UI[under].use;
}