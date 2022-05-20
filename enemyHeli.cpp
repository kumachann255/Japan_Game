//=============================================================================
//
// エネミーヘリモデル処理 [enemyHeli.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "enemyHeli.h"
#include "shadow.h"
#include "light.h"
#include "bullet.h"
#include "meshfield.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_HELI			"data/MODEL/heri.obj"			// 読み込むモデル名
#define	MODEL_HELI_PARTS	"data/MODEL/puropera.obj"		// 読み込むモデル名

#define	VALUE_MOVE			(2.0f)							// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// 回転量

#define HELI_SHADOW_SIZE	(1.0f)							// 影の大きさ
#define HELI_OFFSET_Y		(50.0f)						// エネミーヘリの足元をあわせる

#define HELI_PARTS_MAX		(2)								// エネミーヘリのパーツの数
#define ROT_ADDDTION		(0.5f)							// エネミーヘリのパーツの数

#define HELI_UPPER_ADDTION	(0.2f)							// エネミーヘリの上昇加算値
#define HELI_UNDER_ADDTION	(0.2f)							// エネミーヘリの下降加算値

#define HELI_SPEED			(0.005f)						// ヘリの速度


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static HELI		g_Heli[MAX_HELI];					// エネミーヘリ

static HELI		g_Parts[HELI_PARTS_MAX];			// エネミーヘリのパーツ用

static XMFLOAT3		control0, control1, control2;	// 制御

static BOOL		g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitHeli(void)
{

	for (int i = 0; i < MAX_HELI; i++)
	{
		LoadModel(MODEL_HELI, &g_Heli[i].model);
		g_Heli[i].load = TRUE;

		g_Heli[i].pos = { 0.0f, HELI_OFFSET_Y, 80.0f };
		g_Heli[i].rot = { 0.314f, 0.0f, 0.0f };
		g_Heli[i].scl = { 1.0f, 1.0f, 1.0f };

		g_Heli[i].spd = 0.0f;			// 移動スピードクリア
		g_Heli[i].size = HELI_SIZE;		// 当たり判定の大きさ

		g_Heli[i].use = TRUE;
		g_Heli[i].switchFlag = TO_GO;
		g_Heli[i].patternFlag = PATTERN_02; // 引数でモードをもらう
		g_Heli[i].speed = HELI_SPEED;
		g_Heli[i].time = 0.0f;			// 時間クリア

		// 受け取った引数で動きのパターンの座標をセット
		switch (g_Heli[i].patternFlag)
		{
		case PATTERN_01:
			control0.x = -50.0f;
			control0.y =  0.0f;

			control1.z = 100.0f;
			control1.y = HELI_OFFSET_Y;

			control2.x = 50.0f;
			control2.y = 0.0f;
			break;

		case PATTERN_02:
			control0.x = -50.0f;
			control0.y = HELI_OFFSET_Y;

			control1.z = 100.0f;
			control1.y = -20.0f;

			control2.x = 50.0f;
			control2.y = HELI_OFFSET_Y;
			break;
		}
		// ここでエネミーヘリ用の影を作成している
		XMFLOAT3 pos = g_Heli[i].pos;
		pos.y -= (HELI_OFFSET_Y - 0.1f);
		g_Heli[i].shadowIdx = CreateShadow(pos, HELI_SHADOW_SIZE, HELI_SHADOW_SIZE);
		//          ↑
		//        このメンバー変数が生成した影のIndex番号

		// 階層アニメーション用の初期化処理
		g_Heli[i].parent = NULL;			// 本体（親）なのでNULLを入れる

			// パーツの初期化
		for (int i = 0; i < HELI_PARTS_MAX; i++)
		{
			g_Parts[i].use = FALSE;

			// 親子関係
			g_Parts[i].parent = &g_Heli[i];		// ← ここに親のアドレスを入れる

			// パーツの読み込みはまだしていない
			g_Parts[i].load = 0;
		}

		// 位置・回転・スケールの初期設定
		g_Parts[0].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[1].pos = XMFLOAT3(7.5f, 0.0f, -32.5f);

		g_Parts[0].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[1].rot = XMFLOAT3(0.0f, 0.0f, 1.57f);

		g_Parts[0].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Parts[1].scl = XMFLOAT3(0.25f, 1.0f, 0.25f);


		g_Parts[0].use = TRUE;
		g_Parts[1].use = TRUE;

		g_Parts[0].parent = &g_Heli[i];		// 親をセット
		g_Parts[0].load = 1;
		LoadModel(MODEL_HELI_PARTS, &g_Parts[0].model);

		g_Parts[1].parent = &g_Heli[i];		// 親をセット
		g_Parts[1].load = 1;
		LoadModel(MODEL_HELI_PARTS, &g_Parts[1].model);


	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitHeli(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_HELI; i++)
	{
		// モデルの解放処理
		if (g_Heli[i].load)
		{
			UnloadModel(&g_Heli[i].model);
			g_Heli[i].load = FALSE;
		}
	}
	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateHeli(void)
{

	for (int i = 0; i < MAX_HELI; i++)
	{

		if (g_Heli[i].use == TRUE)
		{
			// 階層アニメーション
			for (int i = 0; i < HELI_PARTS_MAX; i++)
			{
				// 使われているなら処理する
				if ((g_Parts[i].use == TRUE))
				{
					// 回転処理
					g_Parts[0].rot.y += 0.5f;
					g_Parts[1].rot.x += 0.5f;
				}
			}

			g_Heli[i].pos.x = ((1.0f - g_Heli[i].time) * (1.0f - g_Heli[i].time) * control0.x) +
				(2 * g_Heli[i].time * (1.0f - g_Heli[i].time) * control1.x) +
				(g_Heli[i].time * g_Heli[i].time * control2.x);

			g_Heli[i].pos.z = ((1.0f - g_Heli[i].time) * (1.0f - g_Heli[i].time) * control0.z) +
				(2 * g_Heli[i].time * (1.0f - g_Heli[i].time) * control1.z) +
				(g_Heli[i].time * g_Heli[i].time * control2.z);

			g_Heli[i].pos.y = ((1.0f - g_Heli[i].time) * (1.0f - g_Heli[i].time) * control0.y) +
				(2 * g_Heli[i].time * (1.0f - g_Heli[i].time) * control1.y) +
				(g_Heli[i].time * g_Heli[i].time * control2.y);

			// ヘリの挙動パターン切り替え
			switch (g_Heli[i].switchFlag)
			{
			case TO_GO:
				g_Heli[i].time += g_Heli[i].speed;

				if (1.0f <= g_Heli[i].time)
				{
					g_Heli[i].switchFlag = RETURN;
				}

				break;

			case RETURN:
				g_Heli[i].time -= g_Heli[i].speed;

				if (0.0f >= g_Heli[i].time)
				{
					g_Heli[i].switchFlag = TO_GO;
				}
				break;
			}

			// 影もエネミーヘリの位置に合わせる
			XMFLOAT3 pos = g_Heli[i].pos;
			pos.y = HELI_OFFSET_Y;
			SetPositionShadow(g_Heli[i].shadowIdx, pos);
		}
	}

#ifdef _DEBUG	// デバッグ情報を表示する
	//PrintDebugProc("HELI:↑ → ↓ ←　Space\n");
	//PrintDebugProc("HELI:X:%f Y:%f Z:%f\n", g_Heli[i].pos.x, g_Heli[i].pos.y, g_Heli[i].pos.z);
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawHeli(void)
{
	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	for (int i = 0; i < MAX_HELI; i++)
	{

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Heli[i].scl.x, g_Heli[i].scl.y, g_Heli[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Heli[i].rot.x, g_Heli[i].rot.y + XM_PI, g_Heli[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		//// クォータニオンを反映
		//XMMATRIX quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Heli[i].quaternion));
		//mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Heli[i].pos.x, g_Heli[i].pos.y, g_Heli[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Heli[i].mtxWorld, mtxWorld);

		// モデル描画
		DrawModel(&g_Heli[i].model);


		// パーツの階層アニメーション
		for (int i = 0; i < HELI_PARTS_MAX; i++)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(g_Parts[i].scl.x, g_Parts[i].scl.y, g_Parts[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[i].rot.x, g_Parts[i].rot.y, g_Parts[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Parts[i].pos.x, g_Parts[i].pos.y, g_Parts[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			if (g_Parts[i].parent != NULL)	// 子供だったら親と結合する
			{
				mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i].parent->mtxWorld));
				// ↑
				// g_Heli[i].mtxWorldを指している
			}

			XMStoreFloat4x4(&g_Parts[i].mtxWorld, mtxWorld);

			// 使われているなら処理する。ここまで処理している理由は他のパーツがこのパーツを参照している可能性があるから。
			if (g_Parts[i].use == FALSE) continue;

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);


			// モデル描画
			DrawModel(&g_Parts[i].model);
		}
	}
	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// エネミーヘリ情報を取得
//=============================================================================
HELI *GetHeli(void)
{
	return &g_Heli[0];
}

////=============================================================================
//// エネミーヘリセット
////=============================================================================
//void SetHeli(void)
//{
//	for (int i = 0; i < MAX_HELI; i++)
//	{
//		if (g_Heli[i].use == FALSE)
//		{
//			g_Heli[i].use = TRUE;
//			g_Heli[i].time = 0.0f;
//
//			PLAYER *pPlayer = GetPlayer();
//			ATTACKRANGE *AttackR = GetAttackR();
//
//			control0 = pPlayer->pos;
//			control2 = AttackR->pos;
//
//			float hight = 0.0f;	// 高さ調節
//
//			// 制御点の算出
//			control1.x = (control2.x - control0.x) / 2.0f + control0.x;
//			control1.z = (control2.z - control0.z) / 2.0f + control0.z;
//			control1.y = (control2.y - control0.y) / 2.0f + control0.y;
//
//			hight = BOM_H_MAX;
//
//			control1.y += hight;
//			control2.y += BOM_H;
//
//			// 回転速度の算出
//			rot.x = (float)(rand() % 50) / 1000.0f;
//			rot.y = (float)(rand() % 50) / 1000.0f;
//			rot.z = (float)(rand() % 50) / 1000.0f;
//
//			// 回転の初期化
//			g_Heli[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
//		}
//	}
//}
