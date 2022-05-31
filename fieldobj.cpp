//=============================================================================
//
// フィールドオブジェクトの処理 [fieldobj.cpp]
// Author : 熊澤義弘
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "debugproc.h"
#include "model.h"
#include "fieldobj.h"
#include "meshfield.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_BUILDING			"data/MODEL/biru.obj"		// 読み込むモデル名
#define	MODEL_GUARDRAIL			"data/MODEL/guardrail.obj"	// 読み込むモデル名
#define	MODEL_POLE				"data/MODEL/denchu.obj"		// 読み込むモデル名
#define	MODEL_HOUSE				"data/MODEL/house.obj"		// 読み込むモデル名
#define	MODEL_SIGN00			"data/MODEL/sign00.obj"		// 読み込むモデル名
#define	MODEL_SIGN01			"data/MODEL/sign01.obj"		// 読み込むモデル名

#define	VALUE_ROTATE			(XM_PI * 0.02f)				// 回転量

#define FOBJ_X					(430.0f)					// 横に置く場所
#define FOBJ_X_HOUSE			(400.0f)					// 横に置く場所(家の場合
#define FOBJ_X_GUAD				(180.0f)					// 横に置く場所(ガードレールの場合
#define FOBJ_X_POLE				(210.0f)					// 横に置く場所(電柱の場合
#define FOBJ_X_SIGN				(210.0f)					// 横に置く場所(電柱の場合
#define FOBF_Z_MAX				(2000.0f)					// オブジェクトを消すz座標
#define FOBF_Z_START			(-500.0f)					// オブジェクトを出すz座標
#define FOBJ_Y					(50)						// 高さの調整(ランダム限界)
#define FOBJ_Y_Pole				(70)						// 電柱の高さの調整
#define FOBJ_Y_SIGN				(10)						// 看板の高さの調整

#define FOBJ_DISTANCE			(500.0f)					// オブジェクトの間隔
#define FOBJ_DISTANCE_GUAD		(100.0f)					// ガードレールの間隔
#define FOBJ_DISTANCE_SIGN		(1200.0f)					// 看板の間隔

enum {
	left,
	right,
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void SetBilding(BOOL LR);
void SetHouse(BOOL LR);
void SetGuardrail(BOOL LR);
void SetPole(BOOL LR);
void SetSign0(BOOL LR);
void SetSign1(BOOL LR);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static FOBJ				g_Bilding[MAX_FOBJ];			// ビル
static FOBJ				g_House[MAX_FOBJ];				// 家
static FOBJ				g_Guardrail[MAX_FOBJ_GUAD];		// ガードレール
static FOBJ				g_Pole[MAX_FOBJ];				// 電柱
static FOBJ				g_Sign0[MAX_FOBJ_SIGN];			// 看板（日本
static FOBJ				g_Sign1[MAX_FOBJ_SIGN];			// 看板（英語

static BOOL				g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTree(void)
{
	LoadModel(MODEL_BUILDING, &g_Bilding[0].model);
	LoadModel(MODEL_HOUSE, &g_House[0].model);
	LoadModel(MODEL_GUARDRAIL, &g_Guardrail[0].model);
	LoadModel(MODEL_POLE, &g_Pole[0].model);
	LoadModel(MODEL_SIGN00, &g_Sign0[0].model);
	LoadModel(MODEL_SIGN01, &g_Sign1[0].model);

	for (int i = 0; i < MAX_FOBJ; i++)
	{
		g_Bilding[i].load = TRUE;
		g_Bilding[i].pos = XMFLOAT3(-370.0f, 0.0f, 0.0f);
		g_Bilding[i].rot = XMFLOAT3(0.0f, 1.57f, 0.0f);
		g_Bilding[i].scl = XMFLOAT3(18.0f, 18.0f, 18.0f);
		g_Bilding[i].use = FALSE;			// TRUE:生きてる
		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Bilding[i].model, &g_Bilding[i].diffuse[0]);
	}

	for (int i = 0; i < MAX_FOBJ; i++)
	{
		g_House[i].load = TRUE;
		g_House[i].pos = XMFLOAT3(-370.0f, 0.0f, 0.0f);
		g_House[i].rot = XMFLOAT3(0.0f, 1.57f, 0.0f);
		g_House[i].scl = XMFLOAT3(11.0f, 11.0f, 11.0f);
		g_House[i].use = FALSE;			// TRUE:生きてる
		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_House[i].model, &g_House[i].diffuse[0]);
	}

	for (int i = 0; i < MAX_FOBJ_GUAD; i++)
	{
		g_Guardrail[i].load = TRUE;
		g_Guardrail[i].pos = XMFLOAT3(-370.0f, 0.0f, 0.0f);
		g_Guardrail[i].rot = XMFLOAT3(0.0f, 1.57f, 0.0f);
		g_Guardrail[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Guardrail[i].use = FALSE;			// TRUE:生きてる
		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Guardrail[i].model, &g_Guardrail[i].diffuse[0]);
	}

	for (int i = 0; i < MAX_FOBJ; i++)
	{
		g_Pole[i].load = TRUE;
		g_Pole[i].pos = XMFLOAT3(-370.0f, FOBJ_Y_Pole, 0.0f);
		g_Pole[i].rot = XMFLOAT3(0.0f, 1.57f, 0.0f);
		g_Pole[i].scl = XMFLOAT3(2.5f, 2.5f, 2.5f);
		g_Pole[i].use = FALSE;			// TRUE:生きてる
		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Pole[i].model, &g_Pole[i].diffuse[0]);
	}

	for (int i = 0; i < MAX_FOBJ_SIGN; i++)
	{
		g_Sign0[i].load = TRUE;
		g_Sign0[i].pos = XMFLOAT3(-FOBJ_X_SIGN, FOBJ_Y_SIGN, 0.0f);
		g_Sign0[i].rot = XMFLOAT3(0.0f, 1.57f, 0.0f);
		g_Sign0[i].scl = XMFLOAT3(1.5f, 1.5f, 1.5f);
		g_Sign0[i].use = FALSE;			// TRUE:生きてる
		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Sign0[i].model, &g_Sign0[i].diffuse[0]);
	}

	for (int i = 0; i < MAX_FOBJ_SIGN; i++)
	{
		g_Sign1[i].load = TRUE;
		g_Sign1[i].pos = XMFLOAT3(-FOBJ_X_SIGN, FOBJ_Y_SIGN, 0.0f);
		g_Sign1[i].rot = XMFLOAT3(0.0f, 1.57f, 0.0f);
		g_Sign1[i].scl = XMFLOAT3(1.5f, 1.5f, 1.5f);
		g_Sign1[i].use = FALSE;			// TRUE:生きてる
		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Sign1[i].model, &g_Sign1[i].diffuse[0]);
	}


	// 左側のフィールドオブジェクトの初期配置
	// ビル
	for (int i = 0; i < 5; i++)
	{
		g_Bilding[i].use = TRUE;
		g_Bilding[i].pos = XMFLOAT3(-FOBJ_X_HOUSE, 0.0f, FOBF_Z_START + FOBJ_DISTANCE * i);
		g_Bilding[i].rot = XMFLOAT3(0.0f, -1.57f, 0.0f);
	}

	// 家
	for (int i = 0; i < 5; i++)
	{
		g_House[i].use = TRUE;
		g_House[i].pos = XMFLOAT3(-FOBJ_X_HOUSE, 0.0f, FOBF_Z_START + FOBJ_DISTANCE * i + FOBJ_DISTANCE / 2.0f);
		g_House[i].rot = XMFLOAT3(0.0f, -1.57f, 0.0f);

	}

	// ガードレール
	for (int i = 0; i < 25; i++)
	{
		g_Guardrail[i].use = TRUE;
		g_Guardrail[i].pos = XMFLOAT3(-FOBJ_X_GUAD, 0.0f, FOBF_Z_START + FOBJ_DISTANCE_GUAD * i);
		g_Guardrail[i].rot = XMFLOAT3(0.0f, 3.14f, 0.0f);

	}

	// 電柱
	for (int i = 0; i < 5; i++)
	{
		g_Pole[i].use = TRUE;
		g_Pole[i].pos = XMFLOAT3(-FOBJ_X_POLE, FOBJ_Y_Pole, FOBF_Z_START + FOBJ_DISTANCE * i);
		g_Pole[i].rot = XMFLOAT3(0.0f, 3.14f, 0.0f);

	}

	// 看板
	for (int i = 0; i < 2; i++)
	{
		g_Sign1[i].use = TRUE;
		g_Sign1[i].pos = XMFLOAT3(-FOBJ_X_SIGN, FOBJ_Y_SIGN, FOBF_Z_START + FOBJ_DISTANCE_SIGN * (i + 1) - FOBJ_DISTANCE_SIGN / 2.0f);
		g_Sign1[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

	}


	// 右側のフィールドオブジェクトの初期配置
	// ビル
	for (int i = 0; i < 5; i++)
	{
		g_Bilding[i + 5].use = TRUE;
		g_Bilding[i + 5].pos = XMFLOAT3(FOBJ_X_HOUSE, 0.0f, FOBF_Z_START + FOBJ_DISTANCE * i + FOBJ_DISTANCE / 2.0f);
		g_Bilding[i + 5].rot = XMFLOAT3(0.0f, 1.57f, 0.0f);
	}

	// 家
	for (int i = 0; i < 5; i++)
	{
		g_House[i + 5].use = TRUE;
		g_House[i + 5].pos = XMFLOAT3(FOBJ_X_HOUSE, 0.0f, FOBF_Z_START + FOBJ_DISTANCE * i);
		g_House[i + 5].rot = XMFLOAT3(0.0f, 1.57f, 0.0f);

	}

	// ガードレール
	for (int i = 0; i < 25; i++)
	{
		g_Guardrail[i + 25].use = TRUE;
		g_Guardrail[i + 25].pos = XMFLOAT3(FOBJ_X_GUAD, 0.0f, FOBF_Z_START + FOBJ_DISTANCE_GUAD * i);
		g_Guardrail[i + 25].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

	}

	// 電柱
	for (int i = 0; i < 5; i++)
	{
		g_Pole[i + 5].use = TRUE;
		g_Pole[i + 5].pos = XMFLOAT3(FOBJ_X_POLE, FOBJ_Y_Pole, FOBF_Z_START + FOBJ_DISTANCE * i);
		g_Pole[i + 5].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

	}

	// 看板
	for (int i = 0; i < 2; i++)
	{
		g_Sign0[i].use = TRUE;
		g_Sign0[i].pos = XMFLOAT3(FOBJ_X_SIGN, FOBJ_Y_SIGN, FOBF_Z_START + FOBJ_DISTANCE_SIGN * (i + 1));
		g_Sign0[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTree(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_FOBJ; i++)
	{
		if (g_Bilding[i].load)
		{
			UnloadModel(&g_Bilding[i].model);
			g_Bilding[i].load = FALSE;
		}
	}

	for (int i = 0; i < MAX_FOBJ; i++)
	{
		if (g_House[i].load)
		{
			UnloadModel(&g_House[i].model);
			g_House[i].load = FALSE;
		}
	}


	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTree(void)
{
	// ビルの処理
	for (int i = 0; i < MAX_FOBJ; i++)
	{
		if (g_Bilding[i].use == TRUE)			// このエネミーが使われている？
		{									// Yes
			// 地面と一緒に動く
			g_Bilding[i].pos.z += FIELD_SPEED;

			if (g_Bilding[i].pos.z >= FOBF_Z_MAX)
			{
				g_Bilding[i].use = FALSE;
				g_Bilding[i].pos.z = 0;

				// 左右どっちに表示するか
				BOOL LR = left;
				if(g_Bilding[i].pos.x > 0) LR = right;

				// ランダムでビルか家を設置
				int data = 0;
				data = rand() % 2;

				switch (data)
				{
				case 0:
					SetBilding(LR);
					break;

				case 1:
					SetHouse(LR);
					break;
				}
			}
		}
	}

	// 家の処理
	for (int i = 0; i < MAX_FOBJ; i++)
	{
		if (g_House[i].use == TRUE)			// このエネミーが使われている？
		{									// Yes
			// 地面と一緒に動く
			g_House[i].pos.z += FIELD_SPEED;

			if (g_House[i].pos.z >= FOBF_Z_MAX)
			{
				g_House[i].use = FALSE;

				// 左右どっちに表示するか
				BOOL LR = left;
				if (g_House[i].pos.x > 0) LR = right;

				// ランダムでビルか家を設置
				int data = 0;
				data = rand() % 2;

				switch (data)
				{
				case 0:
					SetBilding(LR);
					break;

				case 1:
					SetHouse(LR);
					break;
				}
			}
		}
	}

	// ガードレールの処理
	for (int i = 0; i < MAX_FOBJ_GUAD; i++)
	{
		if (g_Guardrail[i].use == TRUE)			// このエネミーが使われている？
		{									// Yes
			// 地面と一緒に動く
			g_Guardrail[i].pos.z += FIELD_SPEED;

			if (g_Guardrail[i].pos.z >= FOBF_Z_MAX)
			{
				g_Guardrail[i].use = FALSE;

				// 左右どっちに表示するか
				BOOL LR = left;
				if (g_Guardrail[i].pos.x > 0) LR = right;

				// ガードレール設置
				SetGuardrail(LR);
			}
		}
	}

	// 電柱の処理
	for (int i = 0; i < MAX_FOBJ; i++)
	{
		if (g_Pole[i].use == TRUE)			// このエネミーが使われている？
		{									// Yes
			// 地面と一緒に動く
			g_Pole[i].pos.z += FIELD_SPEED;

			if (g_Pole[i].pos.z >= FOBF_Z_MAX)
			{
				g_Pole[i].use = FALSE;

				// 左右どっちに表示するか
				BOOL LR = left;
				if (g_Pole[i].pos.x > 0) LR = right;

				// 電柱設置
				SetPole(LR);
			}
		}
	}

	// 看板(日本)の処理
	for (int i = 0; i < MAX_FOBJ_SIGN; i++)
	{
		if (g_Sign0[i].use == TRUE)			// このエネミーが使われている？
		{									// Yes
			// 地面と一緒に動く
			g_Sign0[i].pos.z += FIELD_SPEED;

			if (g_Sign0[i].pos.z >= FOBF_Z_MAX)
			{
				g_Sign0[i].use = FALSE;

				// 左右どっちに表示するか
				BOOL LR = left;
				if (g_Sign0[i].pos.x > 0) LR = right;

				// ランダムで看板を設置
				int data = 0;
				data = rand() % 2;

				// 看板設置
				switch (data)
				{
				case 0:
					SetSign0(LR);
					break;

				case 1:
					SetSign1(LR);
					break;
				}
			}
		}
	}

	// 看板(アメリカ)の処理
	for (int i = 0; i < MAX_FOBJ_SIGN; i++)
	{
		if (g_Sign1[i].use == TRUE)			// このエネミーが使われている？
		{									// Yes
			// 地面と一緒に動く
			g_Sign1[i].pos.z += FIELD_SPEED;

			if (g_Sign1[i].pos.z >= FOBF_Z_MAX)
			{
				g_Sign1[i].use = FALSE;

				// 左右どっちに表示するか
				BOOL LR = left;
				if (g_Sign1[i].pos.x > 0) LR = right;

				// ランダムで看板を設置
				int data = 0;
				data = rand() % 2;

				// 看板設置
				switch (data)
				{
				case 0:
					SetSign0(LR);
					break;

				case 1:
					SetSign1(LR);
					break;
				}
			}
		}
	}


}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTree(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ビルの描画
	for (int i = 0; i < MAX_FOBJ; i++)
	{
		if (g_Bilding[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Bilding[i].scl.x, g_Bilding[i].scl.y, g_Bilding[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Bilding[i].rot.x, g_Bilding[i].rot.y + XM_PI, g_Bilding[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Bilding[i].pos.x, g_Bilding[i].pos.y, g_Bilding[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Bilding[i].mtxWorld, mtxWorld);

		// モデル描画
		DrawModel(&g_Bilding[0].model);

	}

	// 家の描画
	for (int i = 0; i < MAX_FOBJ; i++)
	{
		if (g_House[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_House[i].scl.x, g_House[i].scl.y, g_House[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_House[i].rot.x, g_House[i].rot.y + XM_PI, g_House[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_House[i].pos.x, g_House[i].pos.y, g_House[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_House[i].mtxWorld, mtxWorld);

		// モデル描画
		DrawModel(&g_House[0].model);

	}

	// ガードレールの描画
	for (int i = 0; i < MAX_FOBJ_GUAD; i++)
	{
		if (g_Guardrail[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Guardrail[i].scl.x, g_Guardrail[i].scl.y, g_Guardrail[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Guardrail[i].rot.x, g_Guardrail[i].rot.y + XM_PI, g_Guardrail[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Guardrail[i].pos.x, g_Guardrail[i].pos.y, g_Guardrail[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Guardrail[i].mtxWorld, mtxWorld);

		// モデル描画
		DrawModel(&g_Guardrail[0].model);

	}

	// 電柱の描画
	for (int i = 0; i < MAX_FOBJ_GUAD; i++)
	{
		if (g_Pole[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Pole[i].scl.x, g_Pole[i].scl.y, g_Pole[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Pole[i].rot.x, g_Pole[i].rot.y + XM_PI, g_Pole[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Pole[i].pos.x, g_Pole[i].pos.y, g_Pole[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Pole[i].mtxWorld, mtxWorld);

		// モデル描画
		DrawModel(&g_Pole[0].model);

	}

	// 看板(日本)の描画
	for (int i = 0; i < MAX_FOBJ_SIGN; i++)
	{
		if (g_Sign0[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Sign0[i].scl.x, g_Sign0[i].scl.y, g_Sign0[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Sign0[i].rot.x, g_Sign0[i].rot.y + XM_PI, g_Sign0[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Sign0[i].pos.x, g_Sign0[i].pos.y, g_Sign0[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Sign0[i].mtxWorld, mtxWorld);

		// モデル描画
		DrawModel(&g_Sign0[0].model);

	}

	// 看板(アメリカ)の描画
	for (int i = 0; i < MAX_FOBJ_SIGN; i++)
	{
		if (g_Sign1[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Sign1[i].scl.x, g_Sign1[i].scl.y, g_Sign1[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Sign1[i].rot.x, g_Sign1[i].rot.y + XM_PI, g_Sign1[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Sign1[i].pos.x, g_Sign1[i].pos.y, g_Sign1[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Sign1[i].mtxWorld, mtxWorld);

		// モデル描画
		DrawModel(&g_Sign1[0].model);

	}
}

//=============================================================================
// エネミーの取得
//=============================================================================
FOBJ *GetBilding()
{
	return &g_Bilding[0];
}


//=============================================================================
// ビルを設置
// LR : L == 0,R == 1
//=============================================================================
void SetBilding(BOOL LR)
{
	for (int i = 0; i < MAX_FOBJ; i++)
	{
		if (g_Bilding[i].use == FALSE)
		{
			g_Bilding[i].use = TRUE;

			g_Bilding[i].pos.z = FOBF_Z_START;

			if (LR)
			{	// 右に設置
				g_Bilding[i].pos.x = FOBJ_X;
				g_Bilding[i].rot = XMFLOAT3(0.0f, 1.57f, 0.0f);
			}
			else
			{	// 左に設置
				g_Bilding[i].pos.x = -FOBJ_X;
				g_Bilding[i].rot = XMFLOAT3(0.0f, -1.57f, 0.0f);
			}

			g_Bilding[i].pos.y -= (float)(rand() % FOBJ_Y);

			return;
		}
	}
}


//=============================================================================
// 家を設置
// LR : L == 0,R == 1
//=============================================================================
void SetHouse(BOOL LR)
{
	for (int i = 0; i < MAX_FOBJ; i++)
	{
		if (g_House[i].use == FALSE)
		{
			g_House[i].use = TRUE;

			g_House[i].pos.z = FOBF_Z_START;

			if (LR)
			{	// 右に設置
				g_House[i].pos.x = FOBJ_X_HOUSE;
				g_House[i].rot = XMFLOAT3(0.0f, 1.57f, 0.0f);
			}
			else
			{	// 左に設置
				g_House[i].pos.x = -FOBJ_X_HOUSE;
				g_House[i].rot = XMFLOAT3(0.0f, -1.57f, 0.0f);
			}

			return;
		}
	}
}


//=============================================================================
// ガードレールを設置
// LR : L == 0,R == 1
//=============================================================================
void SetGuardrail(BOOL LR)
{
	for (int i = 0; i < MAX_FOBJ_GUAD; i++)
	{
		if (g_Guardrail[i].use == FALSE)
		{
			g_Guardrail[i].use = TRUE;

			g_Guardrail[i].pos.z = FOBF_Z_START;

			if (LR)
			{	// 右に設置
				g_Guardrail[i].pos.x = FOBJ_X_GUAD;
				g_Guardrail[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			}
			else
			{	// 左に設置
				g_Guardrail[i].pos.x = -FOBJ_X_GUAD;
				g_Guardrail[i].rot = XMFLOAT3(0.0f, 3.14f, 0.0f);
			}

			return;
		}
	}
}


//=============================================================================
// 電柱を設置
// LR : L == 0,R == 1
//=============================================================================
void SetPole(BOOL LR)
{
	for (int i = 0; i < MAX_FOBJ_GUAD; i++)
	{
		if (g_Pole[i].use == FALSE)
		{
			g_Pole[i].use = TRUE;

			g_Pole[i].pos.z = FOBF_Z_START;

			if (LR)
			{	// 右に設置
				g_Pole[i].pos.x = FOBJ_X_POLE;
				g_Pole[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			}
			else
			{	// 左に設置
				g_Pole[i].pos.x = -FOBJ_X_POLE;
				g_Pole[i].rot = XMFLOAT3(0.0f, 3.14f, 0.0f);
			}

			return;
		}
	}
}


//=============================================================================
// 看板(日本)を設置
// LR : L == 0,R == 1
//=============================================================================
void SetSign0(BOOL LR)
{
	for (int i = 0; i < MAX_FOBJ_SIGN; i++)
	{
		if (g_Sign0[i].use == FALSE)
		{
			g_Sign0[i].use = TRUE;

			g_Sign0[i].pos.z = FOBF_Z_START;

			if (LR)
			{	// 右に設置
				g_Sign0[i].pos.x = FOBJ_X_SIGN;
				g_Sign0[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			}
			else
			{	// 左に設置
				g_Sign0[i].pos.x = -FOBJ_X_SIGN;
				g_Sign0[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			}

			return;
		}
	}
}


//=============================================================================
// 看板(アメリカ)を設置
// LR : L == 0,R == 1
//=============================================================================
void SetSign1(BOOL LR)
{
	for (int i = 0; i < MAX_FOBJ_SIGN; i++)
	{
		if (g_Sign1[i].use == FALSE)
		{
			g_Sign1[i].use = TRUE;

			g_Sign1[i].pos.z = FOBF_Z_START;

			if (LR)
			{	// 右に設置
				g_Sign1[i].pos.x = FOBJ_X_SIGN;
				g_Sign1[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			}
			else
			{	// 左に設置
				g_Sign1[i].pos.x = -FOBJ_X_SIGN;
				g_Sign1[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			}

			return;
		}
	}
}