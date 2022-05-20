//=============================================================================
//
// エネミーモデル処理 [enemy.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "enemy.h"
#include "blast.h"
#include "meshfield.h"
#include "shadow.h"
#include "collision.h"
#include "damageEF.h"
#include "debugproc.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_ENEMY			"data/MODEL/heri.obj"		// 読み込むモデル名

#define	VALUE_MOVE			(3.0f)						// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量

#define ENEMY_SHADOW_SIZE	(0.4f)						// 影の大きさ

#define ENEMY_POP_Z			(500.0f)					// エネミーの初期ポップ位置(z座標)
#define ENEMY_POP_X			(250)						// エネミーの初期ポップの範囲(x座標)

#define ENEMY_GOAL_Z		(70.0f)					// エネミーのゴール基準位置(z座標)
#define ENEMY_GOAL_Z_OFFSET	(60)						// エネミーのゴール位置の乱数

#define POP_COUNT			(100)						// エネミーのポップ間隔
#define MAX_POP				(20)							// 最大、場に何体エネミーを出すか

#define ENEMY_HIT_MOVE		(0.1f)						// 当たり判定後アニメーション用移動量

#define ENEMY_ATTACK_0		(300)						// エネミーが点滅するまでの時間
#define ENEMY_ATTACK_1		(120 + ENEMY_ATTACK_0)		// 点滅が早くなるまでの時間
#define ENEMY_ATTACK_2		(120 + ENEMY_ATTACK_1)		// 攻撃するまでの時間

#define ENEMY_BLINKING0		(50)						// 点滅の間隔
#define ENEMY_BLINKING1		(14)						// 点滅の間隔

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void SetEnemy(void);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ENEMY			g_Enemy[MAX_ENEMY];				// エネミー

static BOOL				g_Load = FALSE;

static int				count = 0;		// ポップカウント

//static INTERPOLATION_DATA move_tbl[] = {	// pos, rot, scl, frame
//	{ XMFLOAT3(   0.0f, ENEMY_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60*2 },
//	{ XMFLOAT3(-200.0f, ENEMY_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60*1 },
//	{ XMFLOAT3(-200.0f, ENEMY_OFFSET_Y, 200.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60*0.5f },
//	{ XMFLOAT3(   0.0f, ENEMY_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60*2 },
//
//};


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{	
		LoadModel(MODEL_ENEMY, &g_Enemy[i].model);
		g_Enemy[i].load = TRUE;

		g_Enemy[i].pos = XMFLOAT3(-50.0f + i * 30.0f, ENEMY_OFFSET_Y, 20.0f);
		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Enemy[i].spd = 0.0f;			// 移動スピードクリア
		g_Enemy[i].size = ENEMY_SIZE;	// 当たり判定の大きさ

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Enemy[0].model, &g_Enemy[0].diffuse[0]);

		XMFLOAT3 pos = g_Enemy[i].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		g_Enemy[i].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

		g_Enemy[i].move_time = 0.0f;	// 線形補間用のタイマーをクリア
		g_Enemy[i].tbl_adr = NULL;		// 再生するアニメデータの先頭アドレスをセット
		g_Enemy[i].tbl_size = 0;		// 再生するアニメデータのレコード数をセット

		g_Enemy[i].use = FALSE;			// TRUE:生きてる

		g_Enemy[i].hitPos = XMFLOAT3(0.0f, ENEMY_OFFSET_Y, 0.0f);	// 爆発の中心
		g_Enemy[i].pos_old = XMFLOAT3(0.0f, ENEMY_OFFSET_Y, 0.0f);	// 一歩前の座標
		g_Enemy[i].hitSpd = XMFLOAT3(0.0f, 0.0f, 0.0f);				// 当たり判定後アニメーション用スピード
		g_Enemy[i].isHit = FALSE;					// TRUE:当たってる
		g_Enemy[i].move = FALSE;					// 奥へ移動するフラグ TRUE:移動する
		g_Enemy[i].hitTime = 0;						// 奥へ移動するフラグ TRUE:移動する

		g_Enemy[i].hitMove = ENEMY_HIT_MOVE;		// 当たり判定後アニメーション用、移動スピード


		g_Enemy[i].liveCount = 0;		// 生存時間をリセット

	}


	//// 0番だけ線形補間で動かしてみる
	//g_Enemy[0].move_time = 0.0f;		// 線形補間用のタイマーをクリア
	//g_Enemy[0].tbl_adr = move_tbl;		// 再生するアニメデータの先頭アドレスをセット
	//g_Enemy[0].tbl_size = sizeof(move_tbl) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemy(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].load)
		{
			UnloadModel(&g_Enemy[i].model);
			g_Enemy[i].load = FALSE;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEnemy(void)
{
	// pop処理
	{
		count++;
		int useCount = 0;

		// 今何体出現しているかを確認
		for (int i = 0; i < MAX_ENEMY; i++)
		{
			if (g_Enemy[i].use == TRUE) useCount++;
		}

		// 時間経過とエネミーの出現数次第でポップするか判断
		if ((count % POP_COUNT == 0) && (useCount < MAX_POP))
		{

			SetEnemy();
		}
	}




	// エネミーを動かく場合は、影も合わせて動かす事を忘れないようにね！
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == TRUE)			// このエネミーが使われている？
		{									// Yes
			// 生存時間をカウント
			g_Enemy[i].liveCount++;


			if (g_Enemy[i].isHit == FALSE)

				// 攻撃処理
			if (g_Enemy[i].liveCount > ENEMY_ATTACK_2)
			{	// 攻撃を行う
				// 生存時間をリセット
				g_Enemy[i].liveCount = 0;

				// 色を戻す
				for (int p = 0; p < g_Enemy[i].model.SubsetNum; p++)
				{
					SetModelDiffuse(&g_Enemy[i].model, p, g_Enemy[0].diffuse[p]);
				}


				// 攻撃
				SetDamageEF(TRUE);
				SetCameraShake(20);
				SetPlayerDamage(1);

			}
			else if(g_Enemy[i].liveCount > ENEMY_ATTACK_1)
			{	// 赤い点滅が早くなる
				
				if (g_Enemy[i].liveCount % ENEMY_BLINKING1 < ENEMY_BLINKING1 / 2)
				{	// オブジェクトを赤くする

					XMFLOAT4 color = { 1.0f, 0.2f, 0.2f, 1.0f };
					for (int p = 0; p < g_Enemy[i].model.SubsetNum; p++)
					{
						SetModelDiffuse(&g_Enemy[i].model, p, color);
					}
				}
				else
				{	// オブジェクトの色を戻す

					for (int p = 0; p < g_Enemy[i].model.SubsetNum; p++)
					{
						SetModelDiffuse(&g_Enemy[i].model, p, g_Enemy[0].diffuse[p]);
					}
				}


			}
			else if (g_Enemy[i].liveCount > ENEMY_ATTACK_0)
			{	// 赤く点滅する

				if (g_Enemy[i].liveCount % ENEMY_BLINKING0 < ENEMY_BLINKING0 / 2)
				{	// オブジェクトを赤くする

					XMFLOAT4 color = { 1.0f, 0.2f, 0.2f, 1.0f };
					for (int p = 0; p < g_Enemy[i].model.SubsetNum; p++)
					{
						SetModelDiffuse(&g_Enemy[i].model, p, color);
					}
				}
				else
				{	// オブジェクトの色を戻す

					for (int p = 0; p < g_Enemy[i].model.SubsetNum; p++)
					{
						SetModelDiffuse(&g_Enemy[i].model, p, g_Enemy[0].diffuse[p]);
					}
				}


			}








			// 目標地点まで到達していない場合に移動処理
			if (g_Enemy[i].pos.z > g_Enemy[i].zGoal)
			{
				// 攻撃処理
				if (g_Enemy[i].liveCount > ENEMY_ATTACK_2)
				{	// 攻撃を行う
					// 生存時間をリセット
					g_Enemy[i].liveCount = 0;

					// 色を戻す
					for (int p = 0; p < g_Enemy[i].model.SubsetNum; p++)
					{
						SetModelDiffuse(&g_Enemy[i].model, p, g_Enemy[0].diffuse[p]);
					}


					// 攻撃
					SetDamageEF(TRUE);
					SetCameraShake(20);


				}
				else if (g_Enemy[i].liveCount > ENEMY_ATTACK_1)
				{	// 赤い点滅が早くなる

					if (g_Enemy[i].liveCount % ENEMY_BLINKING1 < ENEMY_BLINKING1 / 2)
					{	// オブジェクトを赤くする

						XMFLOAT4 color = { 1.0f, 0.2f, 0.2f, 1.0f };
						for (int p = 0; p < g_Enemy[i].model.SubsetNum; p++)
						{
							SetModelDiffuse(&g_Enemy[i].model, p, color);
						}
					}
					else
					{	// オブジェクトの色を戻す

						for (int p = 0; p < g_Enemy[i].model.SubsetNum; p++)
						{
							SetModelDiffuse(&g_Enemy[i].model, p, g_Enemy[0].diffuse[p]);
						}
					}


				}
				else if (g_Enemy[i].liveCount > ENEMY_ATTACK_0)
				{	// 赤く点滅する

					if (g_Enemy[i].liveCount % ENEMY_BLINKING0 < ENEMY_BLINKING0 / 2)
					{	// オブジェクトを赤くする

						XMFLOAT4 color = { 1.0f, 0.2f, 0.2f, 1.0f };
						for (int p = 0; p < g_Enemy[i].model.SubsetNum; p++)
						{
							SetModelDiffuse(&g_Enemy[i].model, p, color);
						}
					}
					else
					{	// オブジェクトの色を戻す

						for (int p = 0; p < g_Enemy[i].model.SubsetNum; p++)
						{
							SetModelDiffuse(&g_Enemy[i].model, p, g_Enemy[0].diffuse[p]);
						}
					}


				}




				// 目標地点まで到達していない場合に移動処理
				if (g_Enemy[i].pos.z > g_Enemy[i].zGoal)
				{
					BOOL ans = TRUE;
					// 他のパトカーと当たっていないかを確認
					for (int p = 0; p < MAX_ENEMY; p++)
					{
						//敵の有効フラグをチェックする
						if ((g_Enemy[p].use == FALSE) || (i == p)) continue;

						//BCの当たり判定
						if (CollisionBC(g_Enemy[i].pos, g_Enemy[p].pos, g_Enemy[p].size / 5.0f, g_Enemy[p].size / 5.0f) &&
							(g_Enemy[i].pos.z > g_Enemy[p].pos.z))
						{	// 当たっていない場合に移動

							ans = FALSE;
							break;
						}
					}

					if (ans)
					{
						g_Enemy[i].pos.z -= VALUE_MOVE;
					}
				}

			}






			// エネミーの消去アニメーション
			if (g_Enemy[i].isHit == TRUE)				// 攻撃が当たってるか？
			{											// Yes
				
				//g_Enemy[i].pos_old.x = g_Enemy[i].pos.x;
				//g_Enemy[i].pos_old.y = g_Enemy[i].pos.y;
				//g_Enemy[i].pos_old.z = g_Enemy[i].pos.z;

				//BOOL ans = TRUE;

				//// ほかのパトカーとぶつかってないか？
				//for (int j = 0; j < MAX_ENEMY; j++)
				//{
				//	if ((g_Enemy[j].isHit == FALSE) || (i == j)) continue;	// 攻撃に当たってない奴には当たり判定のチェックをスキップ

				//	if (CollisionBC(g_Enemy[i].pos, g_Enemy[j].pos, g_Enemy[i].size - 20.0f, g_Enemy[j].size - 20.0f))
				//	{
				//		ans = FALSE;
				//		break;
				//	}
				//
				//}

				//// 決まった位置まで来てかいか？
				//if (CollisionBC(g_Enemy[i].pos, g_Enemy[i].hitPos, g_Enemy[i].size, g_Enemy[i].size))
				//{
				//	ans = FALSE;
				//}


				//XMFLOAT3 temp = XMFLOAT3(g_Enemy[i].pos.x - g_Enemy[i].hitPos.x, g_Enemy[i].pos.y - g_Enemy[i].hitPos.y, g_Enemy[i].pos.z - g_Enemy[i].hitPos.z);
				//float lenSq = (temp.x * temp.x) + (temp.y * temp.y) + (temp.z * temp.z);

				//if (lenSq > 5000)
				//{
				//	g_Enemy[i].pos.x -= g_Enemy[i].hitSpd.x;
				//	g_Enemy[i].pos.y -= g_Enemy[i].hitSpd.y;
				//	g_Enemy[i].pos.z -= g_Enemy[i].hitSpd.z;
				//}

				//５回移動する
				if (g_Enemy[i].hitTime > 0)
				{
					g_Enemy[i].pos.x += (g_Enemy[i].hitPos.x - g_Enemy[i].pos.x) / 5.0f;
					g_Enemy[i].pos.y += (g_Enemy[i].hitPos.y - g_Enemy[i].pos.y) / 5.0f;
					g_Enemy[i].pos.z += (g_Enemy[i].hitPos.z - g_Enemy[i].pos.z) / 5.0f;

					//g_Enemy[i].pos.x -= g_Enemy[i].hitSpd.x;
					//g_Enemy[i].pos.y -= g_Enemy[i].hitSpd.y;
					//g_Enemy[i].pos.z -= g_Enemy[i].hitSpd.z;
				
					g_Enemy[i].hitTime--;
				}
				

				BLAST *blast = GetBlast();		// 爆破オブジェクトの初期化

				//if (ans)
				//if (blast[0].move == FALSE)
				//if (g_Enemy[i].move == FALSE)
				{
					//g_Enemy[i].pos.x += (g_Enemy[i].hitPos.x - g_Enemy[i].pos.x) / 5.0f;
					//g_Enemy[i].pos.y += (g_Enemy[i].hitPos.y - g_Enemy[i].pos.y) / 5.0f;
					//g_Enemy[i].pos.z += (g_Enemy[i].hitPos.z - g_Enemy[i].pos.z) / 5.0f;

					//g_Enemy[i].pos.x -= g_Enemy[i].hitSpd.x;
					//g_Enemy[i].pos.y -= g_Enemy[i].hitSpd.y;
					//g_Enemy[i].pos.z -= g_Enemy[i].hitSpd.z;

				}


				//if ((g_Enemy[i].pos.x == g_Enemy[i].pos_old.x) &&
				//	(g_Enemy[i].pos.y == g_Enemy[i].pos_old.y) &&
				//	(g_Enemy[i].pos.z == g_Enemy[i].pos_old.z))
				//{
				//	g_Enemy[i].move = TRUE;
				//}
			
				

				//爆弾と一緒に奥へ移動する
				if ((blast[0].move == TRUE) /*&& (g_Enemy[i].move == TRUE)*/ && (g_Enemy[i].hitTime == 0))
				{
					g_Enemy[i].pos.z += FIELD_SPEED;
				}


				if (blast[0].use == FALSE)
				{
					g_Enemy[i].use = FALSE;
				}
			}











			//if (g_Enemy[i].tbl_adr != NULL)	// 線形補間を実行する？
			//{								// 線形補間の処理
			//	// 移動処理
			//	int		index = (int)g_Enemy[i].move_time;
			//	float	time = g_Enemy[i].move_time - index;
			//	int		size = g_Enemy[i].tbl_size;

			//	float dt = 1.0f / g_Enemy[i].tbl_adr[index].frame;	// 1フレームで進める時間
			//	g_Enemy[i].move_time += dt;							// アニメーションの合計時間に足す

			//	if (index > (size - 2))	// ゴールをオーバーしていたら、最初へ戻す
			//	{
			//		g_Enemy[i].move_time = 0.0f;
			//		index = 0;
			//	}

			//	// 座標を求める	X = StartX + (EndX - StartX) * 今の時間
			//	XMVECTOR p1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].pos);	// 次の場所
			//	XMVECTOR p0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].pos);	// 現在の場所
			//	XMVECTOR vec = p1 - p0;
			//	XMStoreFloat3(&g_Enemy[i].pos, p0 + vec * time);

			//	// 回転を求める	R = StartX + (EndX - StartX) * 今の時間
			//	XMVECTOR r1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].rot);	// 次の角度
			//	XMVECTOR r0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].rot);	// 現在の角度
			//	XMVECTOR rot = r1 - r0;
			//	XMStoreFloat3(&g_Enemy[i].rot, r0 + rot * time);

			//	// scaleを求める S = StartX + (EndX - StartX) * 今の時間
			//	XMVECTOR s1 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 1].scl);	// 次のScale
			//	XMVECTOR s0 = XMLoadFloat3(&g_Enemy[i].tbl_adr[index + 0].scl);	// 現在のScale
			//	XMVECTOR scl = s1 - s0;
			//	XMStoreFloat3(&g_Enemy[i].scl, s0 + scl * time);

			//}

			// 影もプレイヤーの位置に合わせる
			XMFLOAT3 pos = g_Enemy[i].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			SetPositionShadow(g_Enemy[i].shadowIdx, pos);
		}



	}


#ifdef _DEBUG	// デバッグ情報を表示する
	PrintDebugProc("enmey spd x:%f y:%f z:%f \n ", g_Enemy[0].hitSpd.x, g_Enemy[0].hitSpd.y, g_Enemy[0].hitSpd.z);
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);


		// モデル描画
		DrawModel(&g_Enemy[i].model);
	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// エネミーの取得
//=============================================================================
ENEMY *GetEnemy()
{
	return &g_Enemy[0];
}


//=============================================================================
// エネミーの発生
//=============================================================================
void SetEnemy(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == FALSE)
		{
			g_Enemy[i].use = TRUE;
			g_Enemy[i].pos.z = ENEMY_POP_Z;
			g_Enemy[i].pos.y = ENEMY_OFFSET_Y;
			g_Enemy[i].isHit = FALSE;
			g_Enemy[i].move = FALSE;

			// x座標はランダム
			g_Enemy[i].pos.x = (float)(rand() % ENEMY_POP_X) - ((float)ENEMY_POP_X / 2.0f);

			// 到達地点もランダム
			g_Enemy[i].zGoal = (float)(rand() % ENEMY_GOAL_Z_OFFSET) + ENEMY_GOAL_Z;

			g_Enemy[i].liveCount = 0;

			// 色を戻す
			for (int p = 0; p < g_Enemy[i].model.SubsetNum; p++)
			{
				SetModelDiffuse(&g_Enemy[i].model, p, g_Enemy[0].diffuse[p]);
			}



			return;
		}
	}
}



