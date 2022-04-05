//=============================================================================
//
// ダイナマイト処理 [bom.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "bom.h"
#include "shadow.h"
#include "player.h"
#include "attackRange.h"
#include "blast.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_BOM			"data/MODEL/dynamite.obj"		// 読み込むモデル名

#define	VALUE_MOVE			(5.0f)						// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量

#define BOM_SPEED			(0.01f)						// ボムの速度


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static BOM				g_Bom;				// 攻撃範囲

static BOOL				g_Load = FALSE;

static XMFLOAT3			control0, control1, control2;	// 制御点

static XMFLOAT3			rot;	// 回転

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBom(void)
{
	LoadModel(MODEL_BOM, &g_Bom.model);
	g_Bom.load = TRUE;

	g_Bom.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Bom.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Bom.scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

	g_Bom.speed = BOM_SPEED;			// 移動スピードクリア
	g_Bom.time = 0.0f;

	// モデルのディフューズを保存しておく。色変え対応の為。
	GetModelDiffuse(&g_Bom.model, &g_Bom.diffuse[0]);

	// 色を少し変える
	XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 0.5f };

	// 色をセット
	SetModelDiffuse(&g_Bom.model, 0, color);

	g_Bom.use = FALSE;			// TRUE:生きてる


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBom(void)
{
	if (g_Load == FALSE) return;

	if (g_Bom.load)
	{
		UnloadModel(&g_Bom.model);
		g_Bom.load = FALSE;
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateBom(void)
{
	if (g_Bom.use == TRUE)			// この攻撃範囲が使われている？
	{									// Yes
		// 移動処理
		// ベジェ曲線算出
		g_Bom.pos.x = ((1.0f - g_Bom.time) * (1.0f - g_Bom.time) * control0.x) +
			(2 * g_Bom.time * (1.0f - g_Bom.time) * control1.x) +
			(g_Bom.time * g_Bom.time * control2.x);

		g_Bom.pos.z = ((1.0f - g_Bom.time) * (1.0f - g_Bom.time) * control0.z) +
			(2 * g_Bom.time * (1.0f - g_Bom.time) * control1.z) +
			(g_Bom.time * g_Bom.time * control2.z);

		g_Bom.pos.y = ((1.0f - g_Bom.time) * (1.0f - g_Bom.time) * control0.y) +
			(2 * g_Bom.time * (1.0f - g_Bom.time) * control1.y) +
			(g_Bom.time * g_Bom.time * control2.y);


		// 時間を進める
		if (g_Bom.time < 1.0f)
		{
			g_Bom.time += g_Bom.speed;
		}
		else
		{	// 目的地に着いたら消す
			g_Bom.use = FALSE;

			// 爆破オブジェクトを発生
			SetBlast(g_Bom.pos);
		}


		// 回転処理
		g_Bom.rot.x += rot.x;
		g_Bom.rot.y += rot.y;
		//g_Bom.rot.z += rot.z;





	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBom(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	if (g_Bom.use == FALSE) return;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// スケールを反映
	mtxScl = XMMatrixScaling(g_Bom.scl.x, g_Bom.scl.y, g_Bom.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(g_Bom.rot.x, g_Bom.rot.y + XM_PI, g_Bom.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(g_Bom.pos.x, g_Bom.pos.y, g_Bom.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Bom.mtxWorld, mtxWorld);


	// モデル描画
	DrawModel(&g_Bom.model);

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// ダイナマイトの取得
//=============================================================================
BOM *GetBom()
{
	return &g_Bom;
}


//=============================================================================
// ダイナマイトの発射処理
//=============================================================================
void SetBom(void)
{
	if (g_Bom.use == FALSE)
	{
		g_Bom.use = TRUE;
		g_Bom.time = 0.0f;

		PLAYER *pPlayer = GetPlayer();
		ATTACKRANGE *AttackR = GetAttackR();

		control0 = pPlayer->pos;
		control2 = AttackR->pos;

		float hight = 0.0f;	// 高さ調節

		// 制御点の算出
		control1.x = (control2.x - control0.x) / 2.0f + control0.x;
		control1.z = (control2.z - control0.z) / 2.0f + control0.z;
		control1.y = (control2.y - control0.y) / 2.0f + control0.y;

		hight = BOM_H;

		control1.y += hight;

		// 回転速度の算出
		rot.x = (float)(rand() % 50) / 1000.0f;
		rot.y = (float)(rand() % 50) / 1000.0f;
		rot.z = (float)(rand() % 50) / 1000.0f;

		// 回転の初期化
		g_Bom.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	}
}