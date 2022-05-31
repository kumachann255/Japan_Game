//=============================================================================
//
// ヘリエネミーモデル処理 [enemyHeli.cpp]
// Author : 大塚勝亮
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "enemyHeli.h"
#include "blast.h"
#include "meshfield.h"
#include "shadow.h"
#include "collision.h"
#include "damageEF.h"
#include "debugproc.h"
#include "player.h"
#include "timeUI.h"

#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_ENEMY_HELI		"data/MODEL/heri.obj"		// 読み込むモデル名
#define	MODEL_HELI_PARTS		"data/MODEL/puropera.obj"	// 読み込むモデル名

#define	VALUE_MOVE				(3.0f)						// 移動量
#define	VALUE_ROTATE			(0.5f)						// 回転量
#define ENEMY_HELI_SPEED		(0.0025f)					// 点滅の間隔

#define ENEMY_HELI_SHADOW_SIZE	(0.4f)						// 影の大きさ

#define ENEMY_HELI_POP_Z		(500.0f)					// ヘリエネミーの初期ポップ位置(z座標)
#define ENEMY_HELI_POP_X		(250)						// ヘリエネミーの初期ポップの範囲(x座標)

#define ENEMY_HELI_GOAL_Z		(70.0f)						// ヘリエネミーのゴール基準位置(z座標)
#define ENEMY_HELI_GOAL_Z_OFFSET (60)						// ヘリエネミーのゴール位置の乱数

#define POP_COUNT				(300)						// ヘリエネミーのポップ間隔
#define MAX_POP					(2)							// 最大、場に何体ヘリエネミーを出すか

#define ENEMY_HELI_HIT_MOVE		(5.0f)						// 当たり判定後アニメーション用移動量

#define ENEMY_HELI_ATTACK_0		(300)						// ヘリエネミーが点滅するまでの時間
#define ENEMY_HELI_ATTACK_1		(120 + ENEMY_HELI_ATTACK_0)	// 点滅が早くなるまでの時間
#define ENEMY_HELI_ATTACK_2		(120 + ENEMY_HELI_ATTACK_1)	// 攻撃するまでの時間

#define ENEMY_HELI_BLINKING_0	(50)						// 点滅の間隔
#define ENEMY_HELI_BLINKING_1	(14)						// 点滅の間隔

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void SetEnemyHeli(void);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ENEMY_HELI	g_EnemyHeli[MAX_ENEMY_HELI];	// ヘリエネミー

static ENEMY_HELI	g_Parts[HELI_PARTS_MAX];		// ヘリエネミー

static XMFLOAT3		control0, control1, control2;	// ヘリエネミーの挙動制御

static BOOL			g_Load = FALSE;

static int			count = 0;						// ポップカウント

static int			g_Stage;



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemyHeli(void)
{

	for (int i = 0; i < MAX_ENEMY_HELI; i++)
	{
		g_EnemyHeli[i].load = TRUE;
		LoadModel(MODEL_ENEMY_HELI, &g_EnemyHeli[i].model);

		g_EnemyHeli[i].pos = XMFLOAT3(0.0f, ENEMY_HELI_OFFSET_Y, 20.0f);
		g_EnemyHeli[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_EnemyHeli[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_EnemyHeli[i].spd = 0.0f;			// 移動スピードクリア
		g_EnemyHeli[i].size = ENEMY_HELI_SIZE;	// 当たり判定の大きさ

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_EnemyHeli[i].model, &g_EnemyHeli[i].diffuse[0]);

		//XMFLOAT3 pos = g_EnemyHeli[i].pos;
		//pos.y -= (ENEMY_HELI_OFFSET_Y - 0.1f);
		//g_EnemyHeli[i].shadowIdx = CreateShadow(pos, ENEMY_HELI_SHADOW_SIZE, ENEMY_HELI_SHADOW_SIZE);

		g_EnemyHeli[i].use = FALSE;			// TRUE:生きてる
		g_EnemyHeli[i].switchFlag = TRUE;	// ベジェ曲線フラグ TRUE = time+ / FALSE = time-
		g_EnemyHeli[i].time = 0.0f;			// ベジェ曲線用時間リセット
		g_EnemyHeli[i].speed = ENEMY_HELI_SPEED;


		g_EnemyHeli[i].hitPos = XMFLOAT3(0.0f, ENEMY_HELI_OFFSET_Y, 0.0f);	// 爆発の中心
		g_EnemyHeli[i].hitRot = XMFLOAT3(0.0f, 0.0f, 0.0f);				// 当たり判定後アニメーション用スピード
		g_EnemyHeli[i].isHit = FALSE;					// TRUE:当たってる
		g_EnemyHeli[i].move = FALSE;					// 奥へ移動するフラグ TRUE:移動する
		g_EnemyHeli[i].hitTime = 0;						// 奥へ移動するフラグ TRUE:移動する

		g_EnemyHeli[i].liveCount = 0;		// 生存時間をリセット

		g_EnemyHeli[i].fuchi = FALSE;

		// エネミーヘリのパーツ
		// パーツの初期化
		for (int i = 0; i < HELI_PARTS_MAX; i++)
		{
			g_Parts[i].use = FALSE;

			// 親子関係
			g_Parts[i].parent = &g_EnemyHeli[i];		// ← ここに親のアドレスを入れる

			// パーツの読み込みはまだしていない
			g_Parts[i].load = 0;
		}

		g_Parts[0].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[1].pos = XMFLOAT3(7.5f, 0.0f, -32.5f);

		g_Parts[0].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[1].rot = XMFLOAT3(0.0f, 0.0f, 1.57f);

		g_Parts[0].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Parts[1].scl = XMFLOAT3(0.25f, 1.0f, 0.25f);


		g_Parts[0].use = TRUE;
		g_Parts[1].use = TRUE;

		g_Parts[0].parent = &g_EnemyHeli[i];		// 親をセット
		g_Parts[0].load = 1;
		LoadModel(MODEL_HELI_PARTS, &g_Parts[0].model);

		g_Parts[1].parent = &g_EnemyHeli[i];		// 親をセット
		g_Parts[1].load = 1;
		LoadModel(MODEL_HELI_PARTS, &g_Parts[1].model);

	}

	g_Stage = GetStage();

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemyHeli(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_ENEMY_HELI; i++)
	{
		if (g_EnemyHeli[i].load)
		{
			UnloadModel(&g_EnemyHeli[i].model);
			g_EnemyHeli[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEnemyHeli(void)
{
	if (g_Stage != tutorial)
	{	// pop処理
		count++;
		int useCount = 0;

		// 今何体出現しているかを確認
		for (int i = 0; i < MAX_ENEMY_HELI; i++)
		{
			if (g_EnemyHeli[i].use == TRUE) useCount++;
		}

		// 時間経過とヘリエネミーの出現数次第でポップするか判断
		if ((count % POP_COUNT == 0) && (useCount < MAX_POP))
		{

			SetEnemyHeli();
		}
	}


	// ヘリエネミーを動かす場合は、影も合わせて動かす事を忘れないようにね！
	for (int i = 0; i < MAX_ENEMY_HELI; i++)
	{
		if (g_EnemyHeli[i].use == TRUE)			// このヘリエネミーが使われている？
		{									// Yes
			// 生存時間をカウント
			g_EnemyHeli[i].liveCount++;

			// 攻撃を食らっていなけらば攻撃処理
			if (g_EnemyHeli[i].isHit == FALSE)
			{
				// プロペラ回転処理
				{
					g_Parts[0].rot.y += VALUE_ROTATE;
					g_Parts[1].rot.x += VALUE_ROTATE;
				}

				// ベジェ曲線での移動
				{
					g_EnemyHeli[i].pos.x = ((1.0f - g_EnemyHeli[i].time) * (1.0f - g_EnemyHeli[i].time) * control0.x) +
						(2 * g_EnemyHeli[i].time * (1.0f - g_EnemyHeli[i].time) * control1.x) +
						(g_EnemyHeli[i].time * g_EnemyHeli[i].time * control2.x);

					g_EnemyHeli[i].pos.y = ((1.0f - g_EnemyHeli[i].time) * (1.0f - g_EnemyHeli[i].time) * control0.y) +
						(2 * g_EnemyHeli[i].time * (1.0f - g_EnemyHeli[i].time) * control1.y) +
						(g_EnemyHeli[i].time * g_EnemyHeli[i].time * control2.y);
				}

				// ヘリの挙動パターン切り替え
				switch (g_EnemyHeli[i].switchFlag)
				{
				case TRUE:
					g_EnemyHeli[i].time += g_EnemyHeli[i].speed;

					if (1.0f <= g_EnemyHeli[i].time)
					{
						g_EnemyHeli[i].switchFlag = FALSE;
					}

					break;

				case FALSE:
					g_EnemyHeli[i].time -= g_EnemyHeli[i].speed;

					if (0.0f >= g_EnemyHeli[i].time)
					{
						g_EnemyHeli[i].switchFlag = TRUE;
					}
					break;
				}

				// 攻撃処理
				if (g_EnemyHeli[i].liveCount > ENEMY_HELI_ATTACK_2)
				{	// 攻撃を行う
					// 生存時間をリセット
					g_EnemyHeli[i].liveCount = 0;

					// リムライトオフ
					g_EnemyHeli[i].fuchi = FALSE;


					// 攻撃
					SetDamageEF(TRUE);
					SetCameraShake(20);
					SetPlayerDamage(1);

				}
				else if (g_EnemyHeli[i].liveCount > ENEMY_HELI_ATTACK_1)
				{	// 赤い点滅が早くなる

					if (g_EnemyHeli[i].liveCount % ENEMY_HELI_BLINKING_1 < ENEMY_HELI_BLINKING_1 / 2)
					{	// オブジェクトを赤くする

						// リムライトオン
						g_EnemyHeli[i].fuchi = TRUE;
					}
					else
					{	// オブジェクトの色を戻す

						// リムライトオフ
						g_EnemyHeli[i].fuchi = FALSE;
					}


				}
				else if (g_EnemyHeli[i].liveCount > ENEMY_HELI_ATTACK_0)
				{	// 赤く点滅する

					if (g_EnemyHeli[i].liveCount % ENEMY_HELI_BLINKING_0 < ENEMY_HELI_BLINKING_0 / 2)
					{	// オブジェクトを赤くする

						// リムライトオン
						g_EnemyHeli[i].fuchi = TRUE;
					}
					else
					{	// オブジェクトの色を戻す

						// リムライトオフ
						g_EnemyHeli[i].fuchi = FALSE;
					}

				}
			}


			// 目標地点まで到達していない場合に移動処理
			if (g_EnemyHeli[i].pos.z > g_EnemyHeli[i].zGoal)
			{
				BOOL ans = TRUE;
				// 他のパトカーと当たっていないかを確認
				for (int p = 0; p < MAX_ENEMY_HELI; p++)
				{
					//敵の有効フラグをチェックする
					if ((g_EnemyHeli[p].use == FALSE) || (i == p)) continue;

					//BCの当たり判定
					if (CollisionBC(g_EnemyHeli[i].pos, g_EnemyHeli[p].pos, g_EnemyHeli[p].size / 5.0f, g_EnemyHeli[p].size / 5.0f) &&
						(g_EnemyHeli[i].pos.z > g_EnemyHeli[p].pos.z))
					{	// 当たっていない場合に移動

						ans = FALSE;
						break;
					}
				}

				if ((ans) && (GetTime() > 1))
				{
					g_EnemyHeli[i].pos.z -= VALUE_MOVE;
				}
				else if((ans) && (GetTime() <= 1))
				{
					g_EnemyHeli[i].pos.z += VALUE_MOVE;
				}
			}



			// ヘリエネミーの消去アニメーション
			if (g_EnemyHeli[i].isHit == TRUE)				// 攻撃が当たってるか？
			{											// Yes
				// SEの停止
				StopSound(SOUND_LABEL_SE_propellerSound01);

				// リムライトオフ
				g_EnemyHeli[i].fuchi = FALSE;

				BLAST *blast = GetBlast();		// 爆破オブジェクトの初期化

				// 縮まる処理
				if ((blast[0].shrink) && (g_EnemyHeli[i].hitTime > 0))
				{
					g_EnemyHeli[i].pos.x += (g_EnemyHeli[i].hitPos.x - g_EnemyHeli[i].pos.x) / ENEMY_HELI_HIT_MOVE;
					g_EnemyHeli[i].pos.y += (g_EnemyHeli[i].hitPos.y - g_EnemyHeli[i].pos.y) / ENEMY_HELI_HIT_MOVE;
					g_EnemyHeli[i].pos.z += (g_EnemyHeli[i].hitPos.z - g_EnemyHeli[i].pos.z) / ENEMY_HELI_HIT_MOVE;

					// ランダムに回転させる
					g_EnemyHeli[i].hitRot.x = RamdomFloat(2, 6.28f, -6.28f);
					g_EnemyHeli[i].hitRot.y = RamdomFloat(2, 6.28f, -6.28f);
					g_EnemyHeli[i].hitRot.z = RamdomFloat(2, 6.28f, -6.28f);

					if (g_EnemyHeli[i].hitTime == 15)
					{
						g_EnemyHeli[i].rot.x += g_EnemyHeli[i].hitRot.x;
						g_EnemyHeli[i].rot.y += g_EnemyHeli[i].hitRot.y;
						g_EnemyHeli[i].rot.z += g_EnemyHeli[i].hitRot.z;
					}

					g_EnemyHeli[i].hitTime--;
				}



				// 爆弾と一緒に落下する
				BOOL camera = GetCameraSwitch();

				if (camera == FALSE && blast[0].move == FALSE)
				{
					g_EnemyHeli[i].pos.y -= BLAST_DOWN / BLASE_DOWN_SPEED;

					if (g_EnemyHeli[i].pos.y < ENEMY_HELI_OFFSET_Y)
					{
						g_EnemyHeli[i].pos.y = ENEMY_HELI_OFFSET_Y;
					}
				}

				//爆弾と一緒に奥へ移動する
				if (blast[0].move == TRUE) /*&& (g_EnemyHeli[i].move == TRUE)*/ //&& (g_EnemyHeli[i].hitTime == 0))
				{
					g_EnemyHeli[i].pos.z += FIELD_SPEED + 2.0f;
				}


				if (blast[0].use == FALSE)
				{
					g_EnemyHeli[i].use = FALSE;
				}
			}


			// 影もプレイヤーの位置に合わせる
			XMFLOAT3 pos = g_EnemyHeli[i].pos;
			pos.y -= (ENEMY_HELI_OFFSET_Y - 0.1f);
			SetPositionShadow(g_EnemyHeli[i].shadowIdx, pos);
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemyHeli(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_ENEMY_HELI; i++)
	{
		if (g_EnemyHeli[i].use == FALSE) continue;

		// リムライトの設定
		SetFuchi(g_EnemyHeli[i].fuchi);

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_EnemyHeli[i].scl.x, g_EnemyHeli[i].scl.y, g_EnemyHeli[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_EnemyHeli[i].rot.x, g_EnemyHeli[i].rot.y + XM_PI, g_EnemyHeli[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_EnemyHeli[i].pos.x, g_EnemyHeli[i].pos.y, g_EnemyHeli[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_EnemyHeli[i].mtxWorld, mtxWorld);


		// モデル描画
		DrawModel(&g_EnemyHeli[i].model);

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
				// g_EnemyHeli[i].mtxWorldを指している
			}

			XMStoreFloat4x4(&g_Parts[i].mtxWorld, mtxWorld);

			// 使われているなら処理する。ここまで処理している理由は他のパーツがこのパーツを参照している可能性があるから。
			if (g_Parts[i].use == FALSE) continue;

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);


			// モデル描画
			DrawModel(&g_Parts[i].model);
		}

		// リムライトの設定
		SetFuchi(FALSE);
	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// ヘリエネミーの取得
//=============================================================================
ENEMY_HELI *GetEnemyHeli()
{
	return &g_EnemyHeli[0];
}


//=============================================================================
// ヘリエネミーの発生
//=============================================================================
void SetEnemyHeli(void)
{
	for (int i = 0; i < MAX_ENEMY_HELI; i++)
	{
		if (g_EnemyHeli[i].use == FALSE)
		{
			// SEのセット
			PlaySound(SOUND_LABEL_SE_propellerSound01);

			g_EnemyHeli[i].use = TRUE;
			g_EnemyHeli[i].pos.z = ENEMY_HELI_POP_Z;
			g_EnemyHeli[i].pos.y = ENEMY_HELI_OFFSET_Y;
			g_EnemyHeli[i].isHit = FALSE;
			g_EnemyHeli[i].move = FALSE;
			g_EnemyHeli[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

			// 到達地点もランダム
			g_EnemyHeli[i].zGoal = (float)(rand() % ENEMY_HELI_GOAL_Z_OFFSET) + ENEMY_HELI_GOAL_Z;

			{
				control0.x = -50.0f;
				control0.y = ENEMY_HELI_OFFSET_Y;
				control0.z = g_EnemyHeli[i].zGoal;

				control1.x = 0.0f;
				control1.y = ENEMY_HELI_OFFSET_Y + 75.0f;
				control1.z = g_EnemyHeli[i].zGoal;

				control2.x = 50.0f;
				control2.y = ENEMY_HELI_OFFSET_Y;
				control2.z = g_EnemyHeli[i].zGoal;
			}


			g_EnemyHeli[i].liveCount = 0;

			// リムライトオフ
			g_EnemyHeli[i].fuchi = FALSE;

			XMFLOAT3 pos = g_EnemyHeli[i].pos;
			pos.y -= (ENEMY_HELI_OFFSET_Y - 0.1f);
			g_EnemyHeli[i].shadowIdx = CreateShadow(pos, ENEMY_HELI_SHADOW_SIZE, ENEMY_HELI_SHADOW_SIZE);


			return;
		}
	}
}