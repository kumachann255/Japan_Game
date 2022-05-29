//=============================================================================
//
// �`���[�g���A������ [tutorial.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "tutorial.h"
#include "sprite.h"
#include "score.h"
#include "input.h"
#include "fade.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(330.0f)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(120.0f)	// 

#define TEXTURE_MAX					(10)			// �e�N�X�`���̐�

#define TEXTURE_OFFSET_Y			(50.0f)		// �\���ʒu����

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����


static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/tutorial00.png",
	"data/TEXTURE/tutorial01.png",
	"data/TEXTURE/tutorial02.png",
	"data/TEXTURE/tutorial03.png",
	"data/TEXTURE/tutorial04.png",
	"data/TEXTURE/tutorial05.png",
	"data/TEXTURE/tutorial06.png",
	"data/TEXTURE/tutorial07.png",
	"data/TEXTURE/tutorial08.png",
};

enum {
	tutorial00,
	tutorial01,
	tutorial02,
	tutorial03,
	tutorial04,
	tutorial05,
	tutorial06,
	tutorial07,
	tutorial08,
};

static TUTORIAL					g_Tutorial[2];	// 0�F�������炢�w�i�@1�F�e�L�X�g

static float					g_time;

static BOOL						g_EnemyDead;

static BOOL						g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitTutorial(void)
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
	for (int i = 0; i < 2; i++)
	{
		g_Tutorial[i].use = TRUE;
		g_Tutorial[i].pos = { SCREEN_WIDTH / 2 ,SCREEN_HEIGHT / 2 , 0.0f };
		g_Tutorial[i].texNo = i;

		if (i == 0)
		{	// �w�i
			g_Tutorial[i].w = SCREEN_WIDTH;
			g_Tutorial[i].h = SCREEN_HEIGHT;
		}
		else
		{	// �e�L�X�g
			g_Tutorial[i].w = SCREEN_WIDTH;
			g_Tutorial[i].h = SCREEN_HEIGHT;

			g_Tutorial[i].pos.y -= TEXTURE_OFFSET_Y;

		}
	}


	g_time = 0.0f;
	g_EnemyDead = FALSE;
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTutorial(void)
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
void UpdateTutorial(void)
{
	if (GetKeyboardTrigger(DIK_RETURN))
	{
		// �G��|���O�܂ł̃`���[�g���A��
		if (!g_EnemyDead)
		{
			if (g_Tutorial[1].texNo < tutorial05) g_Tutorial[1].texNo++;
			else if (g_Tutorial[1].texNo == tutorial05)
			{	// �G��|���t�F�[�Y�ɍs������`���[�g���A��������
				for (int i = 0; i < 2; i++)
				{
					g_Tutorial[i].use = FALSE;
				}
			}
		}
		else
		{	// �G��|������̃`���[�g���A��
			if (g_Tutorial[1].texNo <= tutorial08) g_Tutorial[1].texNo++;
		}

		// �`���[�g���A���e�L�X�g���Ō�܂Ō����Ă�����`���[�g���A���I��
		if (g_Tutorial[1].texNo == tutorial08)
		{
			SetStage(stage0);
			SetFade(FADE_OUT, MODE_GAME);
		}
	}

	// �G���|����Ă�����ēx�`���[�g���A����\��
	if (g_EnemyDead)
	{
		for (int i = 0; i < 2; i++)
		{
			g_Tutorial[i].use = TRUE;
		}
	}


#ifdef _DEBUG	// �f�o�b�O����\������
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTutorial(void)
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
		if (!g_Tutorial[i].use) continue;

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[i]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, g_Tutorial[i].pos.x, g_Tutorial[i].pos.y, g_Tutorial[i].w, g_Tutorial[i].h, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
}