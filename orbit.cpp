//=============================================================================
//
// �O������ [orbit.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "shadow.h"
#include "orbit.h"
#include "player.h"
#include "attackRange.h"
#include "bom.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(1)			// �e�N�X�`���̐�

#define	ORBIT_SIZE_X		(2.0f)		// ���_�T�C�Y
#define	ORBIT_SIZE_Y		(2.0f)		// ���_�T�C�Y

#define	MAX_ORBIT			(32)		// �p�[�e�B�N���ő吔

#define	DISP_SHADOW						// �e�̕\��
//#undef DISP_SHADOW

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef struct
{
	XMFLOAT3		pos;			// �ʒu
	XMFLOAT3		rot;			// ��]
	XMFLOAT3		scale;			// �X�P�[��
	XMFLOAT3		move;			// �ړ���
	MATERIAL		material;		// �}�e���A��
	float			fSizeX;			// ��
	float			fSizeY;			// ����
	int				nIdxShadow;		// �eID
	int				nLife;			// ����
	BOOL			bUse;			// �g�p���Ă��邩�ǂ���

} ORBIT;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexOrbit(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;		// ���_�o�b�t�@

static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static int							g_TexNo;					// �e�N�X�`���ԍ�

static ORBIT					g_Orbit[MAX_ORBIT];		// �p�[�e�B�N�����[�N
static XMFLOAT3					g_posBase;						// �r���{�[�h�����ʒu
static float					g_fWidthBase = 5.0f;			// ��̕�
static float					g_fHeightBase = 10.0f;			// ��̍���
static float					g_roty = 0.0f;					// �ړ�����
static float					g_spd = 0.0f;					// �ړ��X�s�[�h

static char *g_TextureName[TEXTURE_MAX] =
{
	"data/TEXTURE/effect000.jpg",
};

static BOOL						g_Load = FALSE;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitOrbit(void)
{
	// ���_���̍쐬
	MakeVertexOrbit();

	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 0;

	// �p�[�e�B�N�����[�N�̏�����
	for(int nCntParticle = 0; nCntParticle < MAX_ORBIT; nCntParticle++)
	{
		g_Orbit[nCntParticle].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Orbit[nCntParticle].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Orbit[nCntParticle].scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Orbit[nCntParticle].move = XMFLOAT3(1.0f, 1.0f, 1.0f);

		ZeroMemory(&g_Orbit[nCntParticle].material, sizeof(g_Orbit[nCntParticle].material));
		g_Orbit[nCntParticle].material.Diffuse = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

		g_Orbit[nCntParticle].fSizeX = ORBIT_SIZE_X;
		g_Orbit[nCntParticle].fSizeY = ORBIT_SIZE_Y;
		g_Orbit[nCntParticle].nIdxShadow = -1;
		g_Orbit[nCntParticle].nLife = 0;
		g_Orbit[nCntParticle].bUse = TRUE;
	}

	g_posBase = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_roty = 0.0f;
	g_spd = 0.0f;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitOrbit(void)
{
	if (g_Load == FALSE) return;

	//�e�N�X�`���̉��
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	// ���_�o�b�t�@�̉��
	if (g_VertexBuffer != NULL)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateOrbit(void)
{
	PLAYER *pPlayer = GetPlayer();
	ATTACKRANGE *AttackR = GetAttackR();

	XMFLOAT3 control;	// ����_
	float hight = 0.0f;	// ��������

	// ����_�̎Z�o
	control.x = (AttackR->pos.x - pPlayer->pos.x) / 2.0f + pPlayer->pos.x;
	control.z = (AttackR->pos.z - pPlayer->pos.z) / 2.0f + pPlayer->pos.z;
	control.y = (AttackR->pos.y - pPlayer->pos.y) / 2.0f + pPlayer->pos.y;

	hight = BOM_H;

	control.y += hight;

	float time = 1.0f;

	{
		for(int nCntParticle = 0; nCntParticle < MAX_ORBIT; nCntParticle++)
		{
			if(g_Orbit[nCntParticle].bUse)
			{// �g�p��

				// ���Ԃ̏�����
				time = 1.0f;
				time /= MAX_ORBIT;
				time *= nCntParticle;

				// �x�W�F�Ȑ��Z�o
				g_Orbit[nCntParticle].pos.x =	((1.0f - time) * (1.0f - time) * pPlayer->pos.x) + 
												(2 * time * (1.0f - time) * control.x) + 
												(time * time * AttackR->pos.x);

				g_Orbit[nCntParticle].pos.z = ((1.0f - time) * (1.0f - time) * pPlayer->pos.z) +
					(2 * time * (1.0f - time) * control.z) +
					(time * time * AttackR->pos.z);

				g_Orbit[nCntParticle].pos.y = ((1.0f - time) * (1.0f - time) * pPlayer->pos.y) +
					(2 * time * (1.0f - time) * control.y) +
					(time * time * AttackR->pos.y);







				//g_Orbit[nCntParticle].pos.x += g_Orbit[nCntParticle].move.x;
				//g_Orbit[nCntParticle].pos.z += g_Orbit[nCntParticle].move.z;

				//g_Orbit[nCntParticle].pos.y += g_Orbit[nCntParticle].move.y;
				//if(g_Orbit[nCntParticle].pos.y <= g_Orbit[nCntParticle].fSizeY / 2)
				//{// ���n����
				//	g_Orbit[nCntParticle].pos.y = g_Orbit[nCntParticle].fSizeY / 2;
				//	g_Orbit[nCntParticle].move.y = -g_Orbit[nCntParticle].move.y * 0.75f;
				//}

				//g_Orbit[nCntParticle].move.x += (0.0f - g_Orbit[nCntParticle].move.x) * 0.015f;
				//g_Orbit[nCntParticle].move.y -= 0.25f;
				//g_Orbit[nCntParticle].move.z += (0.0f - g_Orbit[nCntParticle].move.z) * 0.015f;

			}
		}

		//// �p�[�e�B�N������
		//{
		//	XMFLOAT3 pos;
		//	XMFLOAT3 move;
		//	float fAngle, fLength;
		//	int nLife;
		//	float fSize;

		//	pos = g_posBase;

		//	fAngle = (float)(rand() % 628 - 314) / 100.0f;
		//	fLength = rand() % (int)(g_fWidthBase * 200 ) / 100.0f - g_fWidthBase;
		//	move.x = sinf(fAngle) * fLength;
		//	move.y = rand() % 300 / 100.0f + g_fHeightBase;
		//	move.z = cosf(fAngle) * fLength;

		//	nLife = rand() % 100 + 150;  

		//	fSize = (float)(rand() % 30 + 20);

		//	pos.y = fSize / 2;

		//	// �r���{�[�h�̐ݒ�
		//	SetParticle(pos, move, XMFLOAT4(0.8f, 0.7f, 0.2f, 0.85f), fSize, fSize, nLife);
		//}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawOrbit(void)
{
	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();

	// ���C�e�B���O�𖳌���
	SetLightEnable(FALSE);

	// ���Z�����ɐݒ�
	SetBlendState(BLEND_MODE_ADD);

	// Z��r����
	SetDepthEnable(FALSE);

	// �t�H�O����
	//SetFogEnable(FALSE);

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	for(int nCntParticle = 0; nCntParticle < MAX_ORBIT; nCntParticle++)
	{
		if(g_Orbit[nCntParticle].bUse)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �r���[�}�g���b�N�X���擾
			mtxView = XMLoadFloat4x4(&cam->mtxView);

			//mtxWorld = XMMatrixInverse(nullptr, mtxView);
			//mtxWorld.r[3].m128_f32[0] = 0.0f;
			//mtxWorld.r[3].m128_f32[1] = 0.0f;
			//mtxWorld.r[3].m128_f32[2] = 0.0f;

			// ������������������
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
			mtxScl = XMMatrixScaling(g_Orbit[nCntParticle].scale.x, g_Orbit[nCntParticle].scale.y, g_Orbit[nCntParticle].scale.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Orbit[nCntParticle].pos.x, g_Orbit[nCntParticle].pos.y, g_Orbit[nCntParticle].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			// �}�e���A���ݒ�
			SetMaterial(g_Orbit[nCntParticle].material);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

	// �ʏ�u�����h�ɖ߂�
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// Z��r�L��
	SetDepthEnable(TRUE);

	// �t�H�O�L��
	//SetFogEnable(TRUE);

}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexOrbit(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	{//���_�o�b�t�@�̒��g�𖄂߂�
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		// ���_���W�̐ݒ�
		vertex[0].Position = XMFLOAT3(-ORBIT_SIZE_X / 2,  ORBIT_SIZE_Y / 2, 0.0f);
		vertex[1].Position = XMFLOAT3( ORBIT_SIZE_X / 2,  ORBIT_SIZE_Y / 2, 0.0f);
		vertex[2].Position = XMFLOAT3(-ORBIT_SIZE_X / 2, -ORBIT_SIZE_Y / 2, 0.0f);
		vertex[3].Position = XMFLOAT3( ORBIT_SIZE_X / 2, -ORBIT_SIZE_Y / 2, 0.0f);

		// �@���̐ݒ�
		vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

		// ���ˌ��̐ݒ�
		vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		// �e�N�X�`�����W�̐ݒ�
		vertex[0].TexCoord = { 0.0f, 0.0f };
		vertex[1].TexCoord = { 1.0f, 0.0f };
		vertex[2].TexCoord = { 0.0f, 1.0f };
		vertex[3].TexCoord = { 1.0f, 1.0f };

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	return S_OK;
}

//=============================================================================
// �}�e���A���J���[�̐ݒ�
//=============================================================================
void SetColorOrbit(int nIdxParticle, XMFLOAT4 col)
{
	g_Orbit[nIdxParticle].material.Diffuse = col;
}

//=============================================================================
// �p�[�e�B�N���̔�������
//=============================================================================
int SetOrbit(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife)
{
	int nIdxParticle = -1;

	for(int nCntParticle = 0; nCntParticle < MAX_ORBIT; nCntParticle++)
	{
		if(!g_Orbit[nCntParticle].bUse)
		{
			g_Orbit[nCntParticle].pos = pos;
			g_Orbit[nCntParticle].rot   = { 0.0f, 0.0f, 0.0f };
			g_Orbit[nCntParticle].scale = { 1.0f, 1.0f, 1.0f };
			g_Orbit[nCntParticle].move = move;
			g_Orbit[nCntParticle].material.Diffuse = col;
			g_Orbit[nCntParticle].fSizeX = fSizeX;
			g_Orbit[nCntParticle].fSizeY = fSizeY;
			g_Orbit[nCntParticle].nLife = nLife;
			g_Orbit[nCntParticle].bUse = TRUE;

			nIdxParticle = nCntParticle;

#ifdef DISP_SHADOW
			// �e�̐ݒ�
			g_Orbit[nCntParticle].nIdxShadow = CreateShadow(XMFLOAT3(pos.x, 0.1f, pos.z), 0.8f, 0.8f);		// �e�̐ݒ�
			if(g_Orbit[nCntParticle].nIdxShadow != -1)
			{
				SetColorShadow(g_Orbit[nCntParticle].nIdxShadow, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f));
			}
#endif

			break;
		}
	}

	return nIdxParticle;
}
