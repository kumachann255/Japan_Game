//=============================================================================
//
// �_���[�W�G�t�F�N�g���� [damageEF.cpp]
// Author : �F�V�`�O
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "damageEF.h"
#include "sound.h"
#include "sprite.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(4)				// �e�N�X�`���̐�

#define DAMAGEEF_MAX_TIME			(20)			// ���t���[���_���[�W�G�t�F�N�g��\�����邩

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/glass0.png",
	"data/TEXTURE/glass1.png",
	"data/TEXTURE/glass2.png",
	"data/TEXTURE/glass3.png",
};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

static int						g_time;						// �\�����Ԃ̃J�E���g

static XMFLOAT4						g_Color;					// �t�F�[�h�̃J���[�i���l�j

static BOOL						g_Load = FALSE;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitDamageEF(void)
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
	g_Pos   = { 0.0f, 0.0f, 0.0f };
	g_TexNo = 0;

	g_time = 0;

	g_Color = { 1.0, 1.0, 1.0, 1.0 };

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitDamageEF(void)
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
void UpdateDamageEF(void)
{
	// �_���[�W���󂯂Ă���ꍇ�A�\��
	if (g_Use)
	{
		g_time++;

		// ���Ԍo�߂Ŕ�\����
		if (g_time > DAMAGEEF_MAX_TIME)
		{
			g_Use = FALSE;
			g_time = 0;
		}
	}
}


//=============================================================================
// �`�揈��
//=============================================================================
void DrawDamageEF(void)
{
	if (!g_Use) return;	// �_���[�W���󂯂Ă��Ȃ���Ε\�����Ȃ�

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

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
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		//SetVertex(0.0f, 0.0f, SCREEN_WIDTH, TEXTURE_WIDTH, 0.0f, 0.0f, 1.0f, 1.0f);
		SetSpriteColor(g_VertexBuffer, SCREEN_WIDTH/2, TEXTURE_HEIGHT /2, SCREEN_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f,
			g_Color);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

}


//=============================================================================
// �_���[�W�G�t�F�N�g�̃X�C�b�`
//=============================================================================
void SetDamageEF(BOOL data)
{
	g_Use = data;
	g_time = 0;

	// �X�C�b�`�I���̏ꍇ
	if (data == TRUE)
	{
		// �\������e�N�X�`���������_���ŕύX
		g_TexNo = rand() % TEXTURE_MAX;
	}
}



