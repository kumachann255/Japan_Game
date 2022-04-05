//=============================================================================
//
// 爆破と当たり判定処理 [blast.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "attackRange.h"
#include "shadow.h"
#include "blast.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_BLAST			"data/MODEL/blast.obj"		// 読み込むモデル名

#define	VALUE_MOVE			(5.0f)						// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量

#define BLAST_LIFE			(30)						// 爆破オブジェクトの表示時間

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static BLAST			g_Blast[MAX_BLAST];				// 爆破オブジェクト

static BOOL				g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBlast(void)
{
	for (int i = 0; i < MAX_BLAST; i++)
	{
		LoadModel(MODEL_BLAST, &g_Blast[i].model);
		g_Blast[i].load = TRUE;

		g_Blast[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Blast[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Blast[i].scl = XMFLOAT3(2.0f, 2.0f, 2.0f);
		g_Blast[i].size = BLAST_SIZE;
		g_Blast[i].life = 0;
		g_Blast[i].spd = 0.0f;			// 移動スピードクリア

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Blast[i].model, &g_Blast[i].diffuse[0]);

		// 色を少し変える
		XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 0.5f };

		// 色をセット
		SetModelDiffuse(&g_Blast[i].model, 0, color);

		g_Blast[i].use = FALSE;			// TRUE:生きてる
	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBlast(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_BLAST; i++)
	{
		if (g_Blast[i].load)
		{
			UnloadModel(&g_Blast[i].model);
			g_Blast[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateBlast(void)
{
	for (int i = 0; i < MAX_BLAST; i++)
	{
		if (g_Blast[i].use == TRUE)			// この攻撃範囲が使われている？
		{									// Yes
			// 寿命を減らす
			g_Blast[i].life--;

			// 寿命が尽きたら未使用に
			if (g_Blast[i].life < 0)
			{
				g_Blast[i].use = FALSE;
			}
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBlast(void)
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

	for (int i = 0; i < MAX_BLAST; i++)
	{
		if (g_Blast[i].use == TRUE)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(g_Blast[i].scl.x, g_Blast[i].scl.y, g_Blast[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_Blast[i].rot.x, g_Blast[i].rot.y + XM_PI, g_Blast[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Blast[i].pos.x, g_Blast[i].pos.y, g_Blast[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Blast[i].mtxWorld, mtxWorld);


			// モデル描画
			DrawModel(&g_Blast[i].model);
		}
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
// 爆破の取得
//=============================================================================
BLAST *GetBlast()
{
	return &g_Blast[0];
}


//=============================================================================
// 爆破の発生
//=============================================================================
void SetBlast(XMFLOAT3 pos)
{
	for (int i = 0; i < MAX_BLAST; i++)
	{
		if (g_Blast[i].use == FALSE)
		{
			g_Blast[i].use = TRUE;
			g_Blast[i].pos = pos;
			g_Blast[i].life = BLAST_LIFE;


			return;
		}
	}
}
