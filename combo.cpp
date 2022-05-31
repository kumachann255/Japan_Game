//=============================================================================
//
// �R���{���� [combo.cpp]
// Author : �F�V�`�O
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "combo.h"
#include "sprite.h"
#include "speech.h"
#include "score.h"
#include "sound.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(100)		// �e�N�X�`���T�C�Y
#define TEXTURE_HEIGHT				(300)		// 
#define TEXTURE_MAX					(1)			// �e�N�X�`���̐�

#define COMBO_POS_Y					(360.0f)	// �R���{�\���̍���
#define COMBO_SPEED					(5)			// �\�����Ă���R���{�̐��������鑬�x
#define COMBO_TIME					(220)		// �\�����Ă���R���{�̐����؂�鎞��

#define COMBO_MOVE_MAX				(40.0f)		// �R���{�\�����ǂꂭ�炢�̍������痎�Ƃ���
#define COMBO_MOVE_SPEED			(5.0f)		// �R���{�\�����ǂꂭ�炢�̑����ŗ��Ƃ���
#define COMBO_MOVE_TIME				(3)			// �R���{�\�������t���[���ŗ��Ƃ���



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void SetComboMove(void);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/comboNum.png",
};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

static int						g_Combo;					// �\������R���{��

static int						g_Combo_result;				// ���������v�R���{��
static int						g_time;						// �o�ߎ���
static int						g_ComboTime;				// �R���{�̎���
static BOOL						g_ComboMove;				// �R���{�ɓ��������邩
static int						g_ComboMoveTime;			// �R���{�\������������

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
	g_Pos   = { 800.0f, COMBO_POS_Y, 0.0f };
	g_TexNo = 0;

	g_Combo = 0;	// �R���{���̏�����
	g_Combo_result = 0;

	g_time = 0;
	g_ComboTime = 0;
	g_ComboMoveTime = 0;

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
		if(g_Combo >= 15) PlaySound(SOUND_LABEL_SE_cheers05);

		// �����o����\��
		SetComboSpeech();

		SetComboMove();

		// �{�[�i�X��
		int bonus = 0;

		// �R���{���ɉ����ă{�[�i�X
		if (g_Combo > COMBO_BONUS_2)
		{
			bonus = COMBO_BONUS_2_VALUE;
		}
		else if (g_Combo > COMBO_BONUS_1)
		{
			bonus = COMBO_BONUS_1_VALUE;
		}
		else if (g_Combo > COMBO_BONUS_0)
		{
			bonus = COMBO_BONUS_0_VALUE;
		}

		for (int i = 0; i < bonus; i++)
		{
			AddScore(100);
		}

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


	// �R���{�������������ɂ�����Əォ�痎���Ă���i������t����
	if (g_ComboMoveTime != 0)
	{
		g_Pos.y = (COMBO_POS_Y - g_Pos.y) / COMBO_MOVE_SPEED + g_Pos.y;
		g_ComboMoveTime--;
	}

	// �R���{����ۑ�
	SetComboMax(g_Combo);
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

		// �R���{���̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_Pos.x - g_w*i;	// �R���{���̕\���ʒuX
		float py = g_Pos.y;			// �R���{���̕\���ʒuY
		float pw = g_w;				// �R���{���̕\����
		float ph = g_h;				// �R���{���̕\������

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
// �R���{�������Z����
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

//=============================================================================
// �R���{���擾
//=============================================================================
int GetCombo(void)
{
	return g_Combo;
}


//=============================================================================
// �R���{�̌p�����Ԃ����Z�b�g
//=============================================================================
void ResetComboTime(void)
{
	g_ComboTime = 0;
}


//=============================================================================
// �R���{��������Ƃ��ɕ\��������������
//=============================================================================
void SetComboMove(void)
{
	// �R���{���������̂ŕ\��������������
	//g_ComboMove = TRUE;
	g_ComboMoveTime = COMBO_MOVE_TIME;
	g_Pos.y = COMBO_POS_Y + COMBO_MOVE_MAX;
}