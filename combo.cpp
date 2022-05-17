//=============================================================================
//
// �R���{���� [combo.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "combo.h"
#include "sprite.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(60)	// �e�N�X�`���T�C�Y
#define TEXTURE_HEIGHT				(120)	// 
#define TEXTURE_MAX					(1)		// �e�N�X�`���̐�

#define COMBO_SPEED					(5)		// �\�����Ă���R���{�̐��������鑬�x
#define COMBO_TIME					(120)	// �\�����Ă���R���{�̐��������鑬�x


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/number16x32.png",
};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

static int						g_Combo;					// �\������R���{��

static int						g_Combo_result;				// ���������v�R���{��
static int						g_time;						// �o�ߎ���
static int						g_ComboTime;				// �R���{�̎���

static BOOL						g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitCombo(void)
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
	g_Pos   = { 800.0f, 400.0f, 0.0f };
	g_TexNo = 0;

	g_Combo = 0;	// �R���{���̏�����
	g_Combo_result = 0;

	g_time = 0;
	g_ComboTime = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitCombo(void)
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
void UpdateCombo(void)
{
	if ((g_Combo_result != g_Combo) && (g_time % COMBO_SPEED == 0))
	{
		g_Combo++;
		// �R���{���������鉹
		// PlaySound(SOUND_LABEL_SE_shot000);

	}

	// ���Ԃ�i�߂�
	g_time++;
	g_ComboTime++;

	// �R���{�p�����Ԃ��؂ꂽ�烊�Z�b�g
	if (g_ComboTime >= COMBO_TIME)
	{
		g_Combo = 0;
		g_Combo_result = 0;
	}


#ifdef _DEBUG	// �f�o�b�O����\������
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawCombo(void)
{
	if (g_Combo == 0) return;

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
	int number = g_Combo;
	for (int i = 0; i < COMBO_DIGIT; i++)
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
void AddCombo(int add)
{
	g_Combo_result += add;
	if (g_Combo_result > COMBO_MAX)
	{
		g_Combo_result = COMBO_MAX;
	}

}


int GetCombo(void)
{
	return g_Combo;
}


// �R���{�̌p�����Ԃ����Z�b�g
void ResetComboTime(void)
{
	g_ComboTime = 0;
}