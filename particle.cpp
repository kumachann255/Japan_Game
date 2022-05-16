//=============================================================================
//
// �p�[�e�B�N������ [particle.cpp]
// Author : ��Ńe�C
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "shadow.h"
#include "particle.h"
#include "player.h"
#include "customFunc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX					(2)			// �e�N�X�`���̐�
#define	MAX_EXPLOSION				(50)		// ���j�p�̃p�[�e�B�N��
#define PARTICLE_BASE_SIZE			(10.0f)		// �p�[�e�B�N���̃x�[�X�T�C�Y


//*****************************************************************************
// �\���̒�`
//*****************************************************************************


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexParticle(void);
void SetExplosion(XMFLOAT3 pos, float pop);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static BOOL isLoad = FALSE;		// ���[�h�ς݃t���O

static ID3D11Buffer					*vertexBuffer = NULL;				// ���_�o�b�t�@

static ID3D11ShaderResourceView		*textures[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static char *textureNames[TEXTURE_MAX] =
{
	"data/TEXTURE/effect000.jpg",
	"data/TEXTURE/effect000.jpg",
};

static PARTICLE			particles[MAX_PARTICLE];		// �p�[�e�B�N���\����
static PARTICLE_TYPE	types[PARTICLE_TYPE_MAX];		// �p�[�e�B�N���^�C�v�\����


//=============================================================================
// ����������
//=============================================================================
HRESULT InitParticle(void)
{
	// ���_���̍쐬
	MakeVertexParticle();

	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		textures[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(), textureNames[i], NULL, NULL, &textures[i], NULL);
	}

	// �\���̏�����
	for (int i = 0; i < MAX_PARTICLE; i++)
	{
		particles[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		particles[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		particles[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		particles[i].spd = XMFLOAT3(1.0f, 1.0f, 1.0f);

		ZeroMemory(&particles[i].mat, sizeof(particles[i].mat));
		particles[i].mat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		particles[i].size = XMFLOAT2(PARTICLE_COLLISION_SIZE_X, PARTICLE_COLLISION_SIZE_Y);

		particles[i].life = 0;
		particles[i].inUse = FALSE;
	}

	// �����̏�����
	{
		types[PARTICLE_TYPE_EXPLOSION].texIdx = 1;

		types[PARTICLE_TYPE_EXPLOSION].life[0] = 45;
		types[PARTICLE_TYPE_EXPLOSION].life[1] = 50;

		types[PARTICLE_TYPE_EXPLOSION].cnt[0] = 5;
		types[PARTICLE_TYPE_EXPLOSION].cnt[1] = 10;

		types[PARTICLE_TYPE_EXPLOSION].sclX[0] = 0.5f;
		types[PARTICLE_TYPE_EXPLOSION].sclX[1] = 0.5f;

		types[PARTICLE_TYPE_EXPLOSION].sclY[0] = 0.5f;
		types[PARTICLE_TYPE_EXPLOSION].sclY[1] = 0.5f;

		types[PARTICLE_TYPE_EXPLOSION].sclZ[0] = 0.5f;
		types[PARTICLE_TYPE_EXPLOSION].sclZ[1] = 0.5f;

		types[PARTICLE_TYPE_EXPLOSION].spdX[0] = -0.8f;
		types[PARTICLE_TYPE_EXPLOSION].spdX[1] = 0.8f;

		types[PARTICLE_TYPE_EXPLOSION].spdY[0] = 0.5f;
		types[PARTICLE_TYPE_EXPLOSION].spdY[1] = 1.5f;

		types[PARTICLE_TYPE_EXPLOSION].spdZ[0] = -0.8f;
		types[PARTICLE_TYPE_EXPLOSION].spdZ[1] = 0.8f;

		types[PARTICLE_TYPE_EXPLOSION].pop[0] = 0.0f;
		types[PARTICLE_TYPE_EXPLOSION].pop[1] = 0.0f;

		types[PARTICLE_TYPE_EXPLOSION].col = XMFLOAT4(1.0f, 0.7f, 0.0f, 1.0f);
	}

	isLoad = TRUE;
	return S_OK;
}


//=============================================================================
// �I������
//=============================================================================
void UninitParticle(void)
{
	if (isLoad == FALSE) return;

	//�e�N�X�`���̉��
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (textures[i] != NULL)
		{
			textures[i]->Release();
			textures[i] = NULL;
		}
	}

	// ���_�o�b�t�@�̉��
	if (vertexBuffer != NULL)
	{
		vertexBuffer->Release();
		vertexBuffer = NULL;
	}

	isLoad = FALSE;
}


//=============================================================================
// �X�V����
//=============================================================================
void UpdateParticle(void)
{
	for (int i = 0; i < MAX_PARTICLE; i++)
	{
		if (particles[i].inUse == FALSE) continue;

		// �ړ�����
		particles[i].pos.x += particles[i].spd.x;
		particles[i].pos.y += particles[i].spd.y;
		particles[i].pos.z += particles[i].spd.z;

		// �����̏���
		particles[i].life--;
		if (particles[i].life <= 10)
		{	// �������R�O�t���[���؂�����i�X�����ɂȂ��Ă���
			particles[i].mat.Diffuse.w -= 0.1f;
			if (particles[i].mat.Diffuse.w < 0.0f)
			{
				particles[i].mat.Diffuse.w = 0.0f;
			}
		}
		// �����؂��������
		if (particles[i].life <= 0)
		{
			particles[i].inUse = FALSE;
		}
		// �n�ʂ̉��ɍs�����Ȃ��悤��
		if (particles[i].pos.y < 0)
		{
			particles[i].pos.y = 0.0f;
			particles[i].life = 10;
			particles[i].spd = XMFLOAT3(0.0f, 0.0f, 0.0f);
		}


		// �\���^�C�~���O�̏���
		if (particles[i].pop > 0.0f)
		{
			particles[i].pop--;
		}
	}

	//���j
	if (GetKeyboardTrigger(DIK_3))
	{
		PLAYER *player = GetPlayer();

		//XMFLOAT3 pos = player->pos;
		XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 100.0f);

		types[PARTICLE_TYPE_EXPLOSION].pop[0] = 0.0f;
		types[PARTICLE_TYPE_EXPLOSION].pop[1] = 0.0f;

		SetExplosion(pos, 0.0f);

	}
}


// ���j�̑g������
void SetExplosion(XMFLOAT3 pos, float pop)
{
	//XMFLOAT3 temp = pos;
	XMFLOAT3 move = XMFLOAT3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < MAX_EXPLOSION; i++)
	{

		int col = rand() & 3;
		switch (col)
		{
		case 0:
			types[PARTICLE_TYPE_EXPLOSION].col = XMFLOAT4(0.7f, 0.4f, 0.0f, 1.0f);
			break;

		case 1:
			types[PARTICLE_TYPE_EXPLOSION].col = XMFLOAT4(0.4f, 0.2f, 0.0f, 1.0f);
			break;

		case 2:
			types[PARTICLE_TYPE_EXPLOSION].col = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
			break;
		}

		SetParticle(PARTICLE_TYPE_EXPLOSION, pos);

		move.x = RandomFloat(2, 2.0f, -2.0f);
		move.y = RandomFloat(2, 1.0f, 0.0f);
		move.z = RandomFloat(2, 2.0f, -2.0f);

		pos.x = pos.x + move.x;
		pos.y = pos.y + move.y;
		pos.z = pos.z + move.z;

		// �^�C�~���O�����炵�ĕ\��������
		types[PARTICLE_TYPE_EXPLOSION].pop[0] = pop + 0.1f * i;
		types[PARTICLE_TYPE_EXPLOSION].pop[1] = pop + 0.1f * i;
	}
}


//=============================================================================
// �`�揈��
//=============================================================================
void DrawParticle(void)
{
	CAMERA *cam = GetCamera();
	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;

	// ���C�e�B���O�𖳌���
	SetLightEnable(false);

	// ���Z�����ɐݒ�
	SetBlendState(BLEND_MODE_ADD);

	// Z��r����
	SetDepthEnable(false);

	// �t�H�O����
	SetFogEnable(false);

	int state = rand() % 4;

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 0; i < MAX_PARTICLE; i++)
	{
		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �r���[�}�g���b�N�X���擾
		mtxView = XMLoadFloat4x4(&cam->mtxView);

		// �r���{�[�h����
		mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
		mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
		mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

		mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
		mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
		mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

		mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
		mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
		mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(particles[i].scl.x, particles[i].scl.y, particles[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(particles[i].pos.x, particles[i].pos.y, particles[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		// �e�N�X�`���ݒ�
		int texIndex = types[particles[i].type].texIdx;
		GetDeviceContext()->PSSetShaderResources(0, 1, &textures[texIndex]);

		// �}�e���A����ݒ�
		SetMaterial(particles[i].mat);

		// �|���S���̕`��
		GetDeviceContext()->Draw(4, 0);
	}


	// ���C�e�B���O��L����
	SetLightEnable(true);

	// �ʏ�u�����h�ɖ߂�
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// Z��r�L��
	SetDepthEnable(TRUE);

}


//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexParticle(void)
{
	float a = PARTICLE_BASE_SIZE;

	VERTEX_3D vertexArray[4] =
	{
		{ XMFLOAT3(-5.0f, +5.0f, 0.0f),  XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(+5.0f, +5.0f, 0.0f),  XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-5.0f, -5.0f, 0.0f),  XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(+5.0f, -5.0f, 0.0f),  XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
	};

	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = vertexArray;

	GetDevice()->CreateBuffer(&bd, &sd, &vertexBuffer);

	return S_OK;
}


//=============================================================================
// �p�[�e�B�N���̔�������
//=============================================================================
void SetParticle(int type, XMFLOAT3 pos)
{
	// ��������G�t�F�N�g��I��
	PARTICLE_TYPE *curType = &types[type];

	// ������������
	int cnt = RandomRange(curType->cnt[0], curType->cnt[1]);

	for (int i = 0; i < cnt; i++)
	{
		for (int j = 0; j < MAX_PARTICLE; j++)
		{
			// �p�[�e�B�N������
			if (particles[j].inUse == FALSE)
			{
				particles[j].type = type;

				particles[j].pos = pos;

				particles[j].life = RandomRange(curType->life[0], curType->life[1]);

				particles[j].scl.x = (float)(RandomRange((int)(curType->sclX[0] * 10), (int)(curType->sclX[1] * 10))) * 0.1f;
				particles[j].scl.y = (float)(RandomRange((int)(curType->sclY[0] * 10), (int)(curType->sclY[1] * 10))) * 0.1f;
				particles[j].scl.z = (float)(RandomRange((int)(curType->sclZ[0] * 10), (int)(curType->sclZ[1] * 10))) * 0.1f;

				particles[j].spd.x = (float)(RandomRange((int)(curType->spdX[0] * 10), (int)(curType->spdX[1] * 10))) * 0.1f;
				particles[j].spd.y = (float)(RandomRange((int)(curType->spdY[0] * 10), (int)(curType->spdY[1] * 10))) * 0.1f;
				particles[j].spd.z = (float)(RandomRange((int)(curType->spdZ[0] * 10), (int)(curType->spdZ[1] * 10))) * 0.1f;

				particles[j].pop = (float)(RandomRange((int)(curType->pop[0] * 10), (int)(curType->pop[1] * 10))) * 0.1f;

				particles[j].mat.Diffuse = curType->col;


				particles[j].inUse = TRUE;
				break;
			}
		}
	}
}


//=============================================================================
// �p�[�e�B�N�����̎擾
//=============================================================================
PARTICLE *GetParticle()
{
	return &particles[0];
}
