//=============================================================================
//
// �ӂ��������� [speech.cpp]
// Author : �F�V�`�O
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "speech.h"
#include "sprite.h"
#include "combo.h"

#include "sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define GO_WIDTH					(450.0f)	// �L�����T�C�Y
#define GO_HEIGHT					(450.0f)	// 

#define BOM_WIDTH					(300.0f)	// �L�����T�C�Y
#define BOM_HEIGHT					(300.0f)	// 

#define COMBO_WIDTH					(150.0f)	// �L�����T�C�Y
#define COMBO_HEIGHT				(150.0f)	// 

#define DAMAGE_WIDTH				(300.0f)	// �L�����T�C�Y
#define DAMAGE_HEIGHT				(300.0f)	// 

#define TEXTURE_MAX					(9)		// �e�N�X�`���̐�

#define SPEECH_LIFE					(60)	// ���t���[�������o�����o���Ă�����
#define SPEECH_OFFSET_Y				(50.0f)		// �ŏ��̕\���Œ��S���班�����炷

#define SPEECH_RAND_MAX				(300.0f)	// �����_���l

#define SPEECH_COMBO_X				(800.0f)	// �R���{�̕\�����W
#define SPEECH_COMBO_Y				(250.0f)	// �R���{�̕\�����W

#define SPEECH_BOM_X				(SCREEN_CENTER_X / 2)	// �R���{�̕\�����W
#define SPEECH_BOM_Y				(SCREEN_CENTER_Y / 2)	// �R���{�̕\�����W

#define SCALE_BIG_TIME				(10)		// �����o�����傫���Ȃ鎞��
#define SCALE_SMOLL_TIME			(5)			// �����o�����������Ȃ鎞��
#define SCALE_SPEED					(1.01f)		// �����o�����傫���Ȃ鑬�x
#define SCALE_DOWN					(0.8f)		// �����o�����������Ȃ鑬�x


enum {
	go,
	bam,
	boom,
	yeah,
	crash,
	smash,
	party,
	oops,
	ouch,

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
	"data/TEXTURE/speech_go.png",
	"data/TEXTURE/speech_bam.png",
	"data/TEXTURE/speech_boom.png",
	"data/TEXTURE/speech_yeah.png",
	"data/TEXTURE/speech_crash.png",
	"data/TEXTURE/speech_smash.png",
	"data/TEXTURE/speech_party.png",
	"data/TEXTURE/speech_oops.png",
	"data/TEXTURE/speech_ouch.png",
};

static SPEECH					g_Speech[TEXTURE_MAX];		// �ӂ�����

static int						g_Score;					// �X�R�A

static BOOL						g_Border;					// �ڕW�X�R�A�𒴂��Ă��邩�ǂ���
static int						g_BorderScore;				// �ڕW�X�R�A

static float					g_time;

static BOOL						g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitSpeech(void)
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
		g_Speech[i].use = FALSE;
		g_Speech[i].pos = { 0.0f,0.0f,0.0f };
		g_Speech[i].texNo = i;
		g_Speech[i].w = GO_WIDTH;
		g_Speech[i].h = GO_HEIGHT;
		g_Speech[i].life = SPEECH_LIFE;
		g_Speech[i].combo = FALSE;
	}

	// �ŏ��ɐ����o����\��
	SetStartSpeech();

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitSpeech(void)
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
void UpdateSpeech(void)
{
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (!g_Speech[i].use) continue;

		// ���������炷
		g_Speech[i].life--;

		// �������s�������\����
		if (g_Speech[i].life <= 0)
		{
			g_Speech[i].use = FALSE;
		}


		// �R���{�̐����o���͔������ɑ傫���Ȃ�
		if ((g_Speech[i].life > SPEECH_LIFE - SCALE_BIG_TIME))
			{
			g_Speech[i].w *= SCALE_SPEED;
			g_Speech[i].h *= SCALE_SPEED;

		}
		else if (g_Speech[i].life <= SCALE_SMOLL_TIME)
		{
			g_Speech[i].w *= SCALE_DOWN;
			g_Speech[i].h *= SCALE_DOWN;
		}


	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawSpeech(void)
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
		if (!g_Speech[i].use) continue;

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Speech[i].texNo]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, g_Speech[i].pos.x, g_Speech[i].pos.y, g_Speech[i].w, g_Speech[i].h, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
}


//=============================================================================
// �X�^�[�g���̐����o��
//=============================================================================
void SetStartSpeech(void)
{
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (!g_Speech[i].use)
		{
			// SE�̃Z�b�g
			PlaySound(SOUND_LABEL_SE_carHorn01);

			g_Speech[i].use = TRUE;
			g_Speech[i].life = SPEECH_LIFE;
			g_Speech[i].combo = FALSE;
			g_Speech[i].texNo = go;
			g_Speech[i].w = GO_WIDTH;
			g_Speech[i].h = GO_HEIGHT;

			g_Speech[i].pos = { SCREEN_CENTER_X , SCREEN_CENTER_Y - SPEECH_OFFSET_Y , 0.0f };
			return;
		}
	}
}


//=============================================================================
// ���e�����������ۂ̐����o��
//=============================================================================
void SetBomSpeech(void)
{
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (!g_Speech[i].use)
		{
			g_Speech[i].use = TRUE;
			g_Speech[i].life = SPEECH_LIFE;
			g_Speech[i].combo = FALSE;
			g_Speech[i].w = BOM_WIDTH;
			g_Speech[i].h = BOM_HEIGHT;

			int data = rand() % 2;

			g_Speech[i].texNo = data + bam;

			float x = RamdomFloat(1, SPEECH_BOM_X / 2, -SPEECH_BOM_X / 2);
			float y = RamdomFloat(1, SPEECH_BOM_Y / 2, -SPEECH_BOM_Y / 2);

			if (x > 0)
			{
				g_Speech[i].pos.x = SCREEN_CENTER_X + SPEECH_BOM_X / 2 + x;
			}
			else
			{
				g_Speech[i].pos.x = SCREEN_CENTER_X - SPEECH_BOM_X / 2 + x;
			}

			if (y > 0)
			{
				g_Speech[i].pos.y = SCREEN_CENTER_Y + SPEECH_BOM_Y / 2 + x;
			}
			else
			{
				g_Speech[i].pos.y = SCREEN_CENTER_Y - SPEECH_BOM_Y / 2 + x;
			}

			return;
		}
	}
}


//=============================================================================
// �R���{�������������̐����o��
//=============================================================================
void SetComboSpeech(void)
{
	// SE�̑}��
	PlaySound(SOUND_LABEL_SE_shrinkingSound01);


	// �R���{�̐����o�����܂�����
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Speech[i].combo)
		{
			g_Speech[i].use = FALSE;
		}
	}

	// ����
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (!g_Speech[i].use)
		{
			g_Speech[i].use = TRUE;
			g_Speech[i].life = SPEECH_LIFE;
			g_Speech[i].combo = TRUE;
			g_Speech[i].w = COMBO_WIDTH;
			g_Speech[i].h = COMBO_HEIGHT;

			// �R���{���ɉ����ăe�N�X�`����ύX
			if (GetCombo() > COMBO_BONUS_2)
			{
				g_Speech[i].texNo = crash;
			}
			else if (GetCombo() > COMBO_BONUS_1)
			{
				g_Speech[i].texNo = smash;
			}
			else if (GetCombo() > COMBO_BONUS_0)
			{
				g_Speech[i].texNo = party;
			}
			else
			{
				g_Speech[i].texNo = yeah;
			}

			g_Speech[i].pos = { SPEECH_COMBO_X , SPEECH_COMBO_Y, 0.0f };

			return;
		}
	}


}


//=============================================================================
// �_���[�W���󂯂����̐����o��
//=============================================================================
void SetDamageSpeech(void)
{
	// SE�̑}���i�j�󂳂�Ă���i�K�Ńu�[�C���O�����鏈���j
	PlaySound(SOUND_LABEL_SE_booingSound01);

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (!g_Speech[i].use)
		{
			g_Speech[i].use = TRUE;
			g_Speech[i].life = SPEECH_LIFE;
			g_Speech[i].combo = FALSE;
			g_Speech[i].w = DAMAGE_WIDTH;
			g_Speech[i].h = DAMAGE_HEIGHT;

			int data = rand() % 2;

			g_Speech[i].texNo = data + oops;

			g_Speech[i].pos.x = SCREEN_CENTER_X + RamdomFloat(1, SPEECH_RAND_MAX, -SPEECH_RAND_MAX);
			g_Speech[i].pos.y = SCREEN_CENTER_Y + RamdomFloat(1, SPEECH_RAND_MAX / 3.0f, -SPEECH_RAND_MAX / 3.0f);

			return;
		}
	}
}