//=============================================================================
//
// カメラ処理 [camera.cpp]
// Author : 熊澤義弘
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
// マクロ定義
//*****************************************************************************
#define	POS_X_CAM			(0.0f)			// カメラの初期位置(X座標)
#define	POS_Y_CAM			(60.0f)			// カメラの初期位置(Y座標)
#define	POS_Z_CAM			(-30.0f)		// カメラの初期位置(Z座標)

#define CAMERA_OFFSET_Y		(50.0f)			// ATの調整

//#define	POS_X_CAM		(0.0f)			// カメラの初期位置(X座標)
//#define	POS_Y_CAM		(200.0f)		// カメラの初期位置(Y座標)
//#define	POS_Z_CAM		(-400.0f)		// カメラの初期位置(Z座標)


#define	VIEW_ANGLE		(XMConvertToRadians(70.0f))						// ビュー平面の視野角
#define	VIEW_ASPECT		((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)	// ビュー平面のアスペクト比	
#define	VIEW_NEAR_Z		(10.0f)											// ビュー平面のNearZ値
#define	VIEW_FAR_Z		(20000.0f)										// ビュー平面のFarZ値

#define	VALUE_MOVE_CAMERA	(2.0f)										// カメラの移動量
#define	VALUE_ROTATE_CAMERA	(XM_PI * 0.01f)								// カメラの回転量

#define CAMERA_OFFSET	(1.0f)				// 補間の許容範囲値
#define CAMERA_VALUE	(10.0f)				// 補間の速度

#define CAMERA_SHAKE_MAX		(1.5f)		// 画面が揺れる最大値
#define CAMERA_SHAKE_INTERVAL	(2)			// 画面が揺れる間隔

#define CAMERA_MOVE_SPEED		(0.02f)		// カメラが動くスピード
#define CAMERA_DISTANCE			(50.0f)		// 注視点からカメラまでの距離
//#define CAMERA_

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static CAMERA			g_Camera;		// カメラデータ

static int				g_ViewPortType = TYPE_FULL_SCREEN;

static BOOL				g_Shake;		// 画面揺れをするかどうか
static int				g_ShakeCount;	// 画面が揺れる残り時間
static XMFLOAT3			g_ShakePos;		// どのくらい揺らすかの距離を保存

static BOOL				g_Move;			// 
static float			g_Rot;			// カメラの回転角度

static XMFLOAT3			g_pos = {0.0f , 15.0f , 25.0f};


//=============================================================================
// 初期化処理
//=============================================================================
void InitCamera(void)
{
	g_Camera.pos = { POS_X_CAM, POS_Y_CAM, POS_Z_CAM };
	g_Camera.at  = { 0.0f, 0.0f, 0.0f };
	g_Camera.up  = { 0.0f, 1.0f, 0.0f };
	g_Camera.rot = { 0.0f, 0.0f, 0.0f };

	// 視点と注視点の距離を計算
	float vx, vz;
	vx = g_Camera.pos.x - g_Camera.at.x;
	vz = g_Camera.pos.z - g_Camera.at.z;
	g_Camera.len = sqrtf(vx * vx + vz * vz);
	
	// ビューポートタイプの初期化
	g_ViewPortType = TYPE_FULL_SCREEN;
}



//=============================================================================
// カメラの終了処理
//=============================================================================
void UninitCamera(void)
{

}


//=============================================================================
// カメラの更新処理
//=============================================================================
void UpdateCamera(void)
{
	PLAYER *pPlayer = GetPlayer();

	// 画面が揺れる処理
	if (g_Shake)
	{
		// 画面揺れの残り時間を減らす
		g_ShakeCount--;

		// 残り時間が残っている場合
		if (g_ShakeCount > 0)
		{
			g_Camera.pos.x += g_ShakePos.x;
			g_Camera.pos.y += g_ShakePos.y;
			g_Camera.pos.z += g_ShakePos.z;

			// 画面揺れ幅をリセット
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
	{// 視点旋回「左」
		g_Camera.rot.y += VALUE_ROTATE_CAMERA;
		if (g_Camera.rot.y > XM_PI)
		{
			g_Camera.rot.y -= XM_PI * 2.0f;
		}

		g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
	}

	if (GetKeyboardPress(DIK_C))
	{// 視点旋回「右」
		g_Camera.rot.y -= VALUE_ROTATE_CAMERA;
		if (g_Camera.rot.y < -XM_PI)
		{
			g_Camera.rot.y += XM_PI * 2.0f;
		}

		g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
	}

	if (GetKeyboardPress(DIK_Y))
	{// 視点移動「上」
		g_Camera.pos.y += VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_N))
	{// 視点移動「下」
		g_Camera.pos.y -= VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_Q))
	{// 注視点旋回「左」
		g_Camera.rot.y -= VALUE_ROTATE_CAMERA;
		if (g_Camera.rot.y < -XM_PI)
		{
			g_Camera.rot.y += XM_PI * 2.0f;
		}

		g_Camera.at.x = g_Camera.pos.x + sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.at.z = g_Camera.pos.z + cosf(g_Camera.rot.y) * g_Camera.len;
	}

	if (GetKeyboardPress(DIK_E))
	{// 注視点旋回「右」
		g_Camera.rot.y += VALUE_ROTATE_CAMERA;
		if (g_Camera.rot.y > XM_PI)
		{
			g_Camera.rot.y -= XM_PI * 2.0f;
		}

		g_Camera.at.x = g_Camera.pos.x + sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.at.z = g_Camera.pos.z + cosf(g_Camera.rot.y) * g_Camera.len;
	}

	if (GetKeyboardPress(DIK_T))
	{// 注視点移動「上」
		g_Camera.at.y += VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_B))
	{// 注視点移動「下」
		g_Camera.at.y -= VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_U))
	{// 近づく
		g_Camera.len -= VALUE_MOVE_CAMERA;
		g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
	}

	if (GetKeyboardPress(DIK_M))
	{// 離れる
		g_Camera.len += VALUE_MOVE_CAMERA;
		g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
	}

	// カメラを初期に戻す
	if (GetKeyboardPress(DIK_R))
	{
		UninitCamera();
		InitCamera();
	}

#endif



#ifdef _DEBUG	// デバッグ情報を表示する
	PrintDebugProc("Camera:ZC QE TB YN UM R\n");
#endif
}


//=============================================================================
// カメラの更新
//=============================================================================
void SetCamera(void) 
{
	// ビューマトリックス設定
	XMMATRIX mtxView;
	mtxView = XMMatrixLookAtLH(XMLoadFloat3(&g_Camera.pos), XMLoadFloat3(&g_Camera.at), XMLoadFloat3(&g_Camera.up));
	SetViewMatrix(&mtxView);
	XMStoreFloat4x4(&g_Camera.mtxView, mtxView);

	XMMATRIX mtxInvView;
	mtxInvView = XMMatrixInverse(nullptr, mtxView);
	XMStoreFloat4x4(&g_Camera.mtxInvView, mtxInvView);


	// プロジェクションマトリックス設定
	XMMATRIX mtxProjection;
	mtxProjection = XMMatrixPerspectiveFovLH(VIEW_ANGLE, VIEW_ASPECT, VIEW_NEAR_Z, VIEW_FAR_Z);

	SetProjectionMatrix(&mtxProjection);
	XMStoreFloat4x4(&g_Camera.mtxProjection, mtxProjection);

	SetShaderCamera(g_Camera.pos);
}


//=============================================================================
// カメラの取得
//=============================================================================
CAMERA *GetCamera(void) 
{
	return &g_Camera;
}

//=============================================================================
// ビューポートの設定
//=============================================================================
void SetViewPort(int type)
{
	ID3D11DeviceContext *g_ImmediateContext = GetDeviceContext();
	D3D11_VIEWPORT vp;

	g_ViewPortType = type;

	// ビューポート設定
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
// カメラの視点と注視点をセット
//=============================================================================
void SetCameraAT(XMFLOAT3 pos)
{
	// カメラの注視点をプレイヤーの座標にしてみる

	BOM *bom = GetBom();
	BLAST *blast = GetBlast();

	XMFLOAT3 targetAT;

	// 爆弾が使用されていたら爆弾に視線を送る
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

		// シーンに応じてATの調整
		targetAT.y += CAMERA_OFFSET_Y;

		{
			g_Camera.at.x = g_Camera.at.x + ((targetAT.x - g_Camera.at.x) / CAMERA_VALUE * 2.0f);
			g_Camera.at.y = g_Camera.at.y + ((targetAT.y - g_Camera.at.y) / CAMERA_VALUE * 2.0f);
			g_Camera.at.z = g_Camera.at.z + ((targetAT.z - g_Camera.at.z) / CAMERA_VALUE * 2.0f);
		}


	}
}


//=============================================================================
// 画面を揺らす処理を開始
//=============================================================================
void SetCameraShake(int time)
{
	g_Shake = TRUE;
	g_ShakeCount = time;
	g_ShakePos = { 0.0f, 0.0f ,0.0f };
}


