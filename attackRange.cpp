//=============================================================================
//
// 攻撃範囲処理 [attackRange.cpp]
// Author : 熊澤義弘
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "attackRange.h"
#include "shadow.h"
#include "bom.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_ATTACKRANGE			"data/MODEL/red.obj"		// 読み込むモデル名

#define	VALUE_MOVE			(5.0f)						// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量

#define RANGE_ELEA_X		(160.0f)					// 爆弾を投げられる範囲
#define RANGE_ELEA_Z_FLONT	(300.0f)					// 爆弾を投げられる範囲
#define RANGE_ELEA_Z_BACK	(50.0f)					// 爆弾を投げられる範囲


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ATTACKRANGE		g_AttackR;				// 攻撃範囲

static BOOL				g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitAttackR(void)
{
	LoadModel(MODEL_ATTACKRANGE, &g_AttackR.model);
	g_AttackR.load = TRUE;

	g_AttackR.pos = XMFLOAT3(0.0f, 0.0f, 60.0f);
	g_AttackR.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_AttackR.scl = XMFLOAT3(3.5f, 4.0f, 3.5f);

	g_AttackR.spd = 0.0f;			// 移動スピードクリア

	// モデルのディフューズを保存しておく。色変え対応の為。
	GetModelDiffuse(&g_AttackR.model, &g_AttackR.diffuse[0]);

	// 色を少し変える// 今和使わない
	XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 0.5f };

	// 色をセット
	SetModelDiffuse(&g_AttackR.model, 0, color);

	g_AttackR.use = TRUE;			// TRUE:生きてる


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitAttackR(void)
{
	if (g_Load == FALSE) return;

	if (g_AttackR.load)
	{
		UnloadModel(&g_AttackR.model);
		g_AttackR.load = FALSE;
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateAttackR(void)
{
	BOM *bom = GetBom();

	// 攻撃範囲を表示するかどうか
	if (bom->use == FALSE)
	{
		g_AttackR.use = TRUE;
	}
	if (bom->use == TRUE)
	{
		g_AttackR.use = FALSE;
	}

	{

		// 移動させちゃう
		if ((GetKeyboardPress(DIK_A)) || (IsButtonPressed(0, BUTTON_LEFT)))
		{	// 左へ移動
			if(g_AttackR.pos.x > -RANGE_ELEA_X) g_AttackR.pos.x -= VALUE_MOVE;
		}
		if ((GetKeyboardPress(DIK_D)) || (IsButtonPressed(0, BUTTON_RIGHT)))
		{	// 右へ移動
			if(g_AttackR.pos.x < RANGE_ELEA_X) g_AttackR.pos.x += VALUE_MOVE;
		}
		if ((GetKeyboardPress(DIK_W)) || (IsButtonPressed(0, BUTTON_UP)))
		{	// 上へ移動
			if(g_AttackR.pos.z < RANGE_ELEA_Z_FLONT) g_AttackR.pos.z += VALUE_MOVE;
		}
		if ((GetKeyboardPress(DIK_S)) || (IsButtonPressed(0, BUTTON_DOWN)))
		{	// 下へ移動
			if(g_AttackR.pos.z > RANGE_ELEA_Z_BACK) g_AttackR.pos.z -= VALUE_MOVE;
		}

	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawAttackR(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;


	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	// αテストを有効に
	SetAlphaTestEnable(TRUE);

	// 加算合成に設定
	SetBlendState(BLEND_MODE_ADD);

	//フォグを無効に
	SetFogEnable(FALSE);

	if (g_AttackR.use == TRUE)
	{
		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_AttackR.scl.x, g_AttackR.scl.y, g_AttackR.scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_AttackR.rot.x, g_AttackR.rot.y + XM_PI, g_AttackR.rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_AttackR.pos.x, g_AttackR.pos.y, g_AttackR.pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_AttackR.mtxWorld, mtxWorld);


		// モデル描画
		DrawModel(&g_AttackR.model);
	}

	// 通常ブレンドに戻す
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);

	// αテストを無効に
	SetAlphaTestEnable(FALSE);

	// フォグを有効に
	SetFogEnable(TRUE);

}

//=============================================================================
// 攻撃範囲の取得
//=============================================================================
ATTACKRANGE *GetAttackR()
{
	return &g_AttackR;
}
