//=============================================================================
//
// �J�������� [camera.cpp]
// Author : �F�V�`�O
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "player.h"
#include "bom.h"
#include "blast.h"
#include "player.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	POS_X_CAM			(0.0f)			// �J�����̏����ʒu(X���W)
#define	POS_Y_CAM			(60.0f)			// �J�����̏����ʒu(Y���W)
#define	POS_Z_CAM			(-30.0f)		// �J�����̏����ʒu(Z���W)

#define CAMERA_OFFSET_Y		(50.0f)			// AT�̒���

//#define	POS_X_CAM		(0.0f)			// �J�����̏����ʒu(X���W)
//#define	POS_Y_CAM		(200.0f)		// �J�����̏����ʒu(Y���W)
//#define	POS_Z_CAM		(-400.0f)		// �J�����̏����ʒu(Z���W)


#define	VIEW_ANGLE		(XMConvertToRadians(70.0f))						// �r���[���ʂ̎���p
#define	VIEW_ASPECT		((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)	// �r���[���ʂ̃A�X�y�N�g��	
#define	VIEW_NEAR_Z		(10.0f)											// �r���[���ʂ�NearZ�l
#define	VIEW_FAR_Z		(20000.0f)										// �r���[���ʂ�FarZ�l

#define	VALUE_MOVE_CAMERA	(2.0f)										// �J�����̈ړ���
#define	VALUE_ROTATE_CAMERA	(XM_PI * 0.01f)								// �J�����̉�]��

#define CAMERA_OFFSET	(1.0f)				// ��Ԃ̋��e�͈͒l
#define CAMERA_VALUE	(10.0f)				// ��Ԃ̑��x

#define CAMERA_SHAKE_MAX		(1.5f)		// ��ʂ��h���ő�l
#define CAMERA_SHAKE_INTERVAL	(2)			// ��ʂ��h���Ԋu

#define CAMERA_MOVE_SPEED		(0.02f)		// �J�����������X�s�[�h
#define CAMERA_DISTANCE			(50.0f)		// �����_����J�����܂ł̋���
//#define CAMERA_

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static CAMERA			g_Camera;		// �J�����f�[�^

static int				g_ViewPortType = TYPE_FULL_SCREEN;

static BOOL				g_Shake;		// ��ʗh������邩�ǂ���
static int				g_ShakeCount;	// ��ʂ��h���c�莞��
static XMFLOAT3			g_ShakePos;		// �ǂ̂��炢�h�炷���̋�����ۑ�

static BOOL				g_Move;			// 
static float			g_Rot;			// �J�����̉�]�p�x

static XMFLOAT3			g_pos = {0.0f , 15.0f , 25.0f};


//=============================================================================
// ����������
//=============================================================================
void InitCamera(void)
{
	g_Camera.pos = { POS_X_CAM, POS_Y_CAM, POS_Z_CAM };
	g_Camera.at  = { 0.0f, 0.0f, 0.0f };
	g_Camera.up  = { 0.0f, 1.0f, 0.0f };
	g_Camera.rot = { 0.0f, 0.0f, 0.0f };

	// ���_�ƒ����_�̋������v�Z
	float vx, vz;
	vx = g_Camera.pos.x - g_Camera.at.x;
	vz = g_Camera.pos.z - g_Camera.at.z;
	g_Camera.len = sqrtf(vx * vx + vz * vz);
	
	// �r���[�|�[�g�^�C�v�̏�����
	g_ViewPortType = TYPE_FULL_SCREEN;
}



//=============================================================================
// �J�����̏I������
//=============================================================================
void UninitCamera(void)
{

}


//=============================================================================
// �J�����̍X�V����
//=============================================================================
void UpdateCamera(void)
{
	PLAYER *pPlayer = GetPlayer();

	// ��ʂ��h��鏈��
	if (g_Shake)
	{
		// ��ʗh��̎c�莞�Ԃ����炷
		g_ShakeCount--;

		// �c�莞�Ԃ��c���Ă���ꍇ
		if (g_ShakeCount > 0)
		{
			g_Camera.pos.x += g_ShakePos.x;
			g_Camera.pos.y += g_ShakePos.y;
			g_Camera.pos.z += g_ShakePos.z;

			// ��ʗh�ꕝ�����Z�b�g
			if (g_ShakeCount % CAMERA_SHAKE_INTERVAL == 0)
			{
				g_ShakePos.x = RamdomFloat(3, CAMERA_SHAKE_MAX, -CAMERA_SHAKE_MAX);
				g_ShakePos.y = RamdomFloat(3, CAMERA_SHAKE_MAX, -CAMERA_SHAKE_MAX);
				g_ShakePos.z = RamdomFloat(3, CAMERA_SHAKE_MAX, -CAMERA_SHAKE_MAX);
			}
		}
		else
		{
			g_Shake = FALSE;
		}
	}
	else
	{
		SetCameraAT(pPlayer->pos);
		g_Camera.pos = { POS_X_CAM, POS_Y_CAM, POS_Z_CAM };
	}

	if (g_Move)
	{
		g_Camera.pos.x = g_pos.x - sinf(g_Rot) * CAMERA_DISTANCE;
		g_Camera.pos.z = g_pos.z - cosf(g_Rot) * CAMERA_DISTANCE;
		g_Camera.pos.y = 30.0f;
		g_Rot += CAMERA_MOVE_SPEED;
	}

	if (GetKeyboardPress(DIK_O))
	{
		g_Move = TRUE;
	}
	else
	{
		g_Move = FALSE;
	}


#ifdef _DEBUG

	if (GetKeyboardPress(DIK_Z))
	{// ���_����u���v
		g_Camera.rot.y += VALUE_ROTATE_CAMERA;
		if (g_Camera.rot.y > XM_PI)
		{
			g_Camera.rot.y -= XM_PI * 2.0f;
		}

		g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
	}

	if (GetKeyboardPress(DIK_C))
	{// ���_����u�E�v
		g_Camera.rot.y -= VALUE_ROTATE_CAMERA;
		if (g_Camera.rot.y < -XM_PI)
		{
			g_Camera.rot.y += XM_PI * 2.0f;
		}

		g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
	}

	if (GetKeyboardPress(DIK_Y))
	{// ���_�ړ��u��v
		g_Camera.pos.y += VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_N))
	{// ���_�ړ��u���v
		g_Camera.pos.y -= VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_Q))
	{// �����_����u���v
		g_Camera.rot.y -= VALUE_ROTATE_CAMERA;
		if (g_Camera.rot.y < -XM_PI)
		{
			g_Camera.rot.y += XM_PI * 2.0f;
		}

		g_Camera.at.x = g_Camera.pos.x + sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.at.z = g_Camera.pos.z + cosf(g_Camera.rot.y) * g_Camera.len;
	}

	if (GetKeyboardPress(DIK_E))
	{// �����_����u�E�v
		g_Camera.rot.y += VALUE_ROTATE_CAMERA;
		if (g_Camera.rot.y > XM_PI)
		{
			g_Camera.rot.y -= XM_PI * 2.0f;
		}

		g_Camera.at.x = g_Camera.pos.x + sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.at.z = g_Camera.pos.z + cosf(g_Camera.rot.y) * g_Camera.len;
	}

	if (GetKeyboardPress(DIK_T))
	{// �����_�ړ��u��v
		g_Camera.at.y += VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_B))
	{// �����_�ړ��u���v
		g_Camera.at.y -= VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_U))
	{// �߂Â�
		g_Camera.len -= VALUE_MOVE_CAMERA;
		g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
	}

	if (GetKeyboardPress(DIK_M))
	{// �����
		g_Camera.len += VALUE_MOVE_CAMERA;
		g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
	}

	// �J�����������ɖ߂�
	if (GetKeyboardPress(DIK_R))
	{
		UninitCamera();
		InitCamera();
	}

#endif



#ifdef _DEBUG	// �f�o�b�O����\������
	PrintDebugProc("Camera:ZC QE TB YN UM R\n");
#endif
}


//=============================================================================
// �J�����̍X�V
//=============================================================================
void SetCamera(void) 
{
	// �r���[�}�g���b�N�X�ݒ�
	XMMATRIX mtxView;
	mtxView = XMMatrixLookAtLH(XMLoadFloat3(&g_Camera.pos), XMLoadFloat3(&g_Camera.at), XMLoadFloat3(&g_Camera.up));
	SetViewMatrix(&mtxView);
	XMStoreFloat4x4(&g_Camera.mtxView, mtxView);

	XMMATRIX mtxInvView;
	mtxInvView = XMMatrixInverse(nullptr, mtxView);
	XMStoreFloat4x4(&g_Camera.mtxInvView, mtxInvView);


	// �v���W�F�N�V�����}�g���b�N�X�ݒ�
	XMMATRIX mtxProjection;
	mtxProjection = XMMatrixPerspectiveFovLH(VIEW_ANGLE, VIEW_ASPECT, VIEW_NEAR_Z, VIEW_FAR_Z);

	SetProjectionMatrix(&mtxProjection);
	XMStoreFloat4x4(&g_Camera.mtxProjection, mtxProjection);

	SetShaderCamera(g_Camera.pos);
}


//=============================================================================
// �J�����̎擾
//=============================================================================
CAMERA *GetCamera(void) 
{
	return &g_Camera;
}

//=============================================================================
// �r���[�|�[�g�̐ݒ�
//=============================================================================
void SetViewPort(int type)
{
	ID3D11DeviceContext *g_ImmediateContext = GetDeviceContext();
	D3D11_VIEWPORT vp;

	g_ViewPortType = type;

	// �r���[�|�[�g�ݒ�
	switch (g_ViewPortType)
	{
	case TYPE_FULL_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH;
		vp.Height = (FLOAT)SCREEN_HEIGHT;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		break;

	case TYPE_LEFT_HALF_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH / 2;
		vp.Height = (FLOAT)SCREEN_HEIGHT;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		break;

	case TYPE_RIGHT_HALF_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH / 2;
		vp.Height = (FLOAT)SCREEN_HEIGHT;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = (FLOAT)SCREEN_WIDTH / 2;
		vp.TopLeftY = 0;
		break;

	case TYPE_UP_HALF_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH;
		vp.Height = (FLOAT)SCREEN_HEIGHT / 2;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		break;

	case TYPE_DOWN_HALF_SCREEN:
		vp.Width = (FLOAT)SCREEN_WIDTH;
		vp.Height = (FLOAT)SCREEN_HEIGHT / 2;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = (FLOAT)SCREEN_HEIGHT / 2;
		break;


	}
	g_ImmediateContext->RSSetViewports(1, &vp);

}




int GetViewPortType(void)
{
	return g_ViewPortType;
}


//=============================================================================
// �J�����̎��_�ƒ����_���Z�b�g
//=============================================================================
void SetCameraAT(XMFLOAT3 pos)
{
	// �J�����̒����_���v���C���[�̍��W�ɂ��Ă݂�

	BOM *bom = GetBom();
	BLAST *blast = GetBlast();

	XMFLOAT3 targetAT;

	// ���e���g�p����Ă����甚�e�Ɏ����𑗂�
	if (((bom->use) && (bom->time > 0.15f)) || (GetCameraSwitch()))
	{
		if(bom->use) targetAT = bom->pos;
		else targetAT = blast->pos;

		{
			g_Camera.at.x = g_Camera.at.x + ((targetAT.x - g_Camera.at.x) / CAMERA_VALUE);
			g_Camera.at.y = g_Camera.at.y + ((targetAT.y - g_Camera.at.y) / CAMERA_VALUE);
			g_Camera.at.z = g_Camera.at.z + ((targetAT.z - g_Camera.at.z) / CAMERA_VALUE);
		}
	}
	else if(g_Move)
	{
		g_Camera.at = g_pos;
	}
	else
	{
		XMFLOAT3 targetAT = pos;

		// �V�[���ɉ�����AT�̒���
		targetAT.y += CAMERA_OFFSET_Y;

		{
			g_Camera.at.x = g_Camera.at.x + ((targetAT.x - g_Camera.at.x) / CAMERA_VALUE * 2.0f);
			g_Camera.at.y = g_Camera.at.y + ((targetAT.y - g_Camera.at.y) / CAMERA_VALUE * 2.0f);
			g_Camera.at.z = g_Camera.at.z + ((targetAT.z - g_Camera.at.z) / CAMERA_VALUE * 2.0f);
		}


	}
}


//=============================================================================
// ��ʂ�h�炷�������J�n
//=============================================================================
void SetCameraShake(int time)
{
	g_Shake = TRUE;
	g_ShakeCount = time;
	g_ShakePos = { 0.0f, 0.0f ,0.0f };
}


