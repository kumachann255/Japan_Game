//=============================================================================
//
// �^�C������ [timeUI.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "time.h"
#include "sprite.h"
#include "timeUI.h"
#include "fade.h"
#include "score.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(45)	// ���ԃT�C�Y
#define TEXTURE_HEIGHT				(90)	// 
#define TEXTURE_MAX					(2)		// �e�N�X�`���̐�

#define TIME_MAX					(10)	// ���Ԑ���

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void SetEndTime(void);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/time0.png",
	"data/TEXTURE/time1.png",
};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

static int						g_Time;						// �c�莞��

static BOOL						g_Load = FALSE;

static int						g_stage;					// ���݂̃X�e�[�W��

static time_t naw_time = 0;		// ���݂̎���
static time_t end_time = 0;		// �I������

//=============================================================================
// ����������
//=============================================================================
HRESULT InitTime(void)
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


	// �v���C���[�̏�����
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = { 880.0f, 80.0f, 0.0f };
	g_TexNo = 0;

	g_Time = 0;	// ���Ԃ̏�����

	g_stage = GetStage();

	// �I�����Ԃ̐ݒ�
	SetEndTime();

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTime(void)
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
void UpdateTime(void)
{
	// �I�����Ԃ��猻�݂̎��Ԃ������Ďc�莞�Ԃ��Z�o����
	g_Time = (int)(end_time - time(NULL));

	// �V�[���J��
	if (g_Time < 0)
	{
		switch (g_stage)
		{
		case stage0:
			if (GetScore() >= SCORE_STAGE0_BORDER)
			{
				SetStage(stage1);
				SetFade(FADE_OUT, MODE_GAME_COUNT);
			}
			//else SetFade(FADE_OUT, MODE_RESULT);

			break;

		case stage1:
			if (GetScore() >= SCORE_STAGE1_BORDER)
			{
				SetStage(stage2);
				SetFade(FADE_OUT, MODE_GAME_COUNT);
			}
			else SetFade(FADE_OUT, MODE_RESULT);

			break;

		case stage2:
			if (GetScore() >= SCORE_STAGE2_BORDER)
			{
				SetStage(stage3);
				SetFade(FADE_OUT, MODE_GAME_COUNT);
			}
			else SetFade(FADE_OUT, MODE_RESULT);

			break;

		case stage3:
			SetFade(FADE_OUT, MODE_RESULT);

			break;

		}
	}

	if (g_Time > 10)
	{
		g_TexNo = 0;
	}
	else
	{
		g_TexNo = 1;
	}



#ifdef _DEBUG	// �f�o�b�O����\������
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTime(void)
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

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	// ��������������
	int number = g_Time;
	for (int i = 0; i < TIME_DIGIT; i++)
	{
		// ����\�����錅�̐���
		float x = (float)(number % 10);

		// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_Pos.x - g_w*i;	// �X�R�A�̕\���ʒuX
		float py = g_Pos.y;			// �X�R�A�̕\���ʒuY
		float pw = g_w;				// �X�R�A�̕\����
		float ph = g_h;				// �X�R�A�̕\������

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


//=============================================================================
// �X�R�A�����Z����
// ����:add :�ǉ�����_���B�}�C�i�X���\
//=============================================================================
void AddTime(int add)
{
	g_Time += add;
	if (g_Time > TIME_MAX)
	{
		g_Time = TIME_MAX;
	}

}


int GetTime(void)
{
	return g_Time;
}


// �I�����Ԃ��Z�b�g����֐�
void SetEndTime(void)
{
	end_time = time(NULL) + TIME_MAX;
}
