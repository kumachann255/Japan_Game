//=============================================================================
//
// �G���h���[����ʏ��� [endroll.cpp]
// Author :
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "endroll.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(3445)			// 
#define TEXTURE_MAX					(1)				// �e�N�X�`���̐�
#define START_ROLL					(80)			// �X�N���[���J�n
#define STOP_ROLL					(230)			// �X�N���[����~
#define SPEED_ROLL					(1.5f)			// �X�N���[���̑��x

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/ending.png",
};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

static float					startPosY, endPosY;			// �������X�N���[�����n�߂����̈ʒu, �X�N���[���I�������̈ʒu
static float					g_offset;					// �摜�̈ړ���
static int						endTime;					// �^�C�g�����[�h�ւ̎����J�ڂ��Ǘ�
static int						startTime;					// �X�N���[���J�n����

static BOOL						g_Load = FALSE;	

//=============================================================================
// ����������
//=============================================================================
HRESULT InitEndroll(void)
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
	g_Pos   = { 0.0f, 0.0f, 0.0f };
	g_TexNo = 0;

	startPosY = g_Pos.y;
	endPosY = (float)(0.0f - (TEXTURE_HEIGHT - SCREEN_HEIGHT));
	g_offset = SPEED_ROLL;

	startTime = 0;
	endTime = 0;


	// BGM�Đ�
	PlaySound(SOUND_LABEL_BGM_bgm_endroll);
	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEndroll(void)
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
void UpdateEndroll(void)
{
	startTime++;

	if (startTime > START_ROLL)
	{
		//�������X�N���[�������鏈��
		startPosY -= g_offset;

		if (startPosY < endPosY)
		{
			startPosY = endPosY;
		}

		if (startPosY == endPosY)
		{
			endTime++;

			if (endTime > STOP_ROLL)
			{
				SetFade(FADE_OUT, MODE_TITLE);
				startTime = 0;
				endTime = 0;
			}
		}
	}




	// �X�L�b�v����
	if (GetKeyboardTrigger(DIK_RETURN))
	{// Enter��������A�X�e�[�W��؂�ւ���
		SetFade(FADE_OUT, MODE_TITLE);
	}
	// �Q�[���p�b�h�œ��͏���
	else if (IsButtonTriggered(0, BUTTON_START))
	{
		SetFade(FADE_OUT, MODE_TITLE);
	}
	else if (IsButtonTriggered(0, BUTTON_B))
	{
		SetFade(FADE_OUT, MODE_TITLE);
	}

	

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEndroll(void)
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
		SetSpriteLeftTop(g_VertexBuffer, g_Pos.x, startPosY, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

}


