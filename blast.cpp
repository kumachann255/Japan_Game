//=============================================================================
//
// 爆破と当たり判定処理 [blast.cpp]
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
#include "blast.h"
#include "meshfield.h"
#include "speech.h"

#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_BLAST				"data/MODEL/newneba00.obj"		// 読み込むモデル名
#define	MODEL_BLAST_1			"data/MODEL/newneba01.obj"		// 読み込むモデル名
#define	MODEL_BLAST_2			"data/MODEL/newneba02.obj"		// 読み込むモデル名
#define	MODEL_BLAST_3			"data/MODEL/newneba03.obj"		// 読み込むモデル名
#define	MODEL_BLAST_4			"data/MODEL/newneba04.obj"		// 読み込むモデル名


#define	VALUE_MOVE			(5.0f)						// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量

#define BLAST_LIFE			(150)						// 爆破オブジェクトの表示時間
#define BLAST_MOVE_TIME_0	(3)							// ねばねばが広がる時間
#define BLAST_MOVE_TIME_1	(10)						// 広がった状態で止まる時間
#define BLAST_MOVE_TIME_2	(10)						// ねばねばが縮まる時間
#define BLAST_MOVE_TIME_3	(20)						// ねばねばが空中で止まる時間


#define BLASE_ROT			(3.14f)						// 回転の最大値

#define MAX_BLAST_MOVE		(5)							// モーフィングの数



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static BLAST			g_Blast[MAX_BLAST];				// 爆破オブジェクト

static BOOL				g_Load = FALSE;

static MODEL			g_Blast_Vertex[MAX_BLAST_MOVE];		// モーフィング用モデルの頂点情報が入ったデータ配列
static VERTEX_3D		*g_BlastVertex = NULL;				// 途中経過を記録する場所

static float			g_time;							// モーフィングの経過時間
static int				g_downCount;					// 落ち始めてどのくらい時間が経過したか

static int				g_morphingNum;					// モーフィングの番号

static BOOL				g_cameraOn;						// カメラのスイッチ

static int				g_stopTime;						// 爆弾が空中で止まる時間


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBlast(void)
{
	for (int i = 0; i < MAX_BLAST; i++)
	{
		LoadModelMorphing(MODEL_BLAST, &g_Blast[i].model);
		g_Blast[i].load = TRUE;

		g_Blast[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Blast[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Blast[i].scl = XMFLOAT3(1.3f, 1.3f, 1.3f);
		g_Blast[i].size = BLAST_SIZE;
		g_Blast[i].life = 0;
		g_Blast[i].spd = 0.0f;			// 移動スピードクリア
		g_Blast[i].after = 1;

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Blast[i].model, &g_Blast[i].diffuse[0]);

		g_Blast[i].use = FALSE;			// TRUE:生きてる
		g_Blast[i].move = FALSE;		// TRUE:奥へ移動する
		g_Blast[i].shrink = FALSE;		// 縮まり始めているか
	
	}

	// モーフィングするオブジェクトの読み込み
	LoadObj(MODEL_BLAST, &g_Blast_Vertex[0]);
	LoadObj(MODEL_BLAST_1, &g_Blast_Vertex[1]);
	LoadObj(MODEL_BLAST_2, &g_Blast_Vertex[2]);
	LoadObj(MODEL_BLAST_3, &g_Blast_Vertex[3]);
	LoadObj(MODEL_BLAST_4, &g_Blast_Vertex[4]);

	// 中身を配列として使用できるように仕様変更
	g_BlastVertex = new VERTEX_3D[g_Blast_Vertex[0].VertexNum];

	// 差分(途中経過)の初期化
	for (int i = 0; i < g_Blast_Vertex[0].VertexNum; i++)
	{
		g_BlastVertex[i].Position = g_Blast_Vertex[0].VertexArray[i].Position;
		g_BlastVertex[i].Diffuse  = g_Blast_Vertex[0].VertexArray[i].Diffuse;
		g_BlastVertex[i].Normal   = g_Blast_Vertex[0].VertexArray[i].Normal;
		g_BlastVertex[i].TexCoord = g_Blast_Vertex[0].VertexArray[i].TexCoord;
	}

	g_morphingNum = 0;
	g_time = 0.0f;
	g_downCount = 0;
	g_cameraOn = FALSE;
	g_stopTime = 0;

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

			// ねばねばのモーフィングの処理
			{
				int after, brfore;

				// 広がり終えたら縮まる方向へモーフィング
				if (g_Blast[i].life < BLAST_LIFE - BLAST_MOVE_TIME_0)
				{
					g_morphingNum = 1;
				}
				else
				{
					g_morphingNum = 0;
				}


				// モーフィングモデルの番号調整
				switch (g_morphingNum)
				{
				case 0:
					after = g_Blast[i].after;
					brfore = 0;

					break;

				case 1:
					after = 0;
					brfore = g_Blast[i].after;

					break;
				}


				// モーフィング処理
				for (int p = 0; p < g_Blast_Vertex[0].VertexNum; p++)
				{
					g_BlastVertex[p].Position.x = g_Blast_Vertex[after].VertexArray[p].Position.x - g_Blast_Vertex[brfore].VertexArray[p].Position.x;
					g_BlastVertex[p].Position.y = g_Blast_Vertex[after].VertexArray[p].Position.y - g_Blast_Vertex[brfore].VertexArray[p].Position.y;
					g_BlastVertex[p].Position.z = g_Blast_Vertex[after].VertexArray[p].Position.z - g_Blast_Vertex[brfore].VertexArray[p].Position.z;

					g_BlastVertex[p].Position.x *= g_time;
					g_BlastVertex[p].Position.y *= g_time;
					g_BlastVertex[p].Position.z *= g_time;

					g_BlastVertex[p].Position.x += g_Blast_Vertex[brfore].VertexArray[p].Position.x;
					g_BlastVertex[p].Position.y += g_Blast_Vertex[brfore].VertexArray[p].Position.y;
					g_BlastVertex[p].Position.z += g_Blast_Vertex[brfore].VertexArray[p].Position.z;
				}

				// 時間を進める
				switch (g_morphingNum)
				{
				case 0:
					if (g_time < 1.0f) g_time += 1.0f / BLAST_MOVE_TIME_0;

					break;

				case 1:
					if ((g_time < 1.0f) && (g_Blast[i].life < BLAST_LIFE - BLAST_MOVE_TIME_0 - BLAST_MOVE_TIME_1))
					{
						g_time += 1.0f / BLAST_MOVE_TIME_2;
						g_Blast[i].shrink = TRUE;
					}
					break;
				}

				// 第一モーフィングが完了したら次のモーフィングへ
				if ((g_time >= 1.0f) && (g_morphingNum == 0))
				{
					g_time = 0.0f;
					g_morphingNum++;
				}

				// 第二モーフィングが終わった後はちょっと止まる
				if ((g_time >= 1.0f) && (g_morphingNum == 1) && (g_downCount < BLASE_DOWN_SPEED))
				{
					g_stopTime++;
				}

				if ((g_stopTime >= BLAST_MOVE_TIME_3) && (g_downCount < BLASE_DOWN_SPEED))
				{
					g_Blast[i].pos.y -= BLAST_DOWN / BLASE_DOWN_SPEED;
					g_downCount++;

					g_cameraOn = FALSE;
				}
				
				// 落ちた後は床と一緒に奥へ移動する
				if (g_downCount >= BLASE_DOWN_SPEED)
				{
					g_Blast[i].pos.z += FIELD_SPEED;
					g_Blast[i].move = TRUE;
				}
					

				D3D11_SUBRESOURCE_DATA sd;
				ZeroMemory(&sd, sizeof(sd));
				sd.pSysMem = g_BlastVertex;

				// 頂点バッファに値をセットする
				D3D11_MAPPED_SUBRESOURCE msr;
				GetDeviceContext()->Map(g_Blast[0].model.VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
				VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

				// 全頂点情報を毎回上書きしているのはDX11ではこの方が早いからです
				memcpy(pVtx, g_BlastVertex, sizeof(VERTEX_3D)*g_Blast_Vertex[0].VertexNum);

				GetDeviceContext()->Unmap(g_Blast[0].model.VertexBuffer, 0);
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
			// SEのセット
			PlaySound(SOUND_LABEL_SE_stickingSound01);

			g_Blast[i].use = TRUE;
			g_Blast[i].move = FALSE;
			g_Blast[i].shrink = FALSE;

			g_Blast[i].pos = pos;
			g_Blast[i].life = BLAST_LIFE;

			g_Blast[i].after = (rand() % (MAX_BLAST_MOVE - 1)) + 1;

			g_cameraOn = TRUE;

			g_time = 0.0f;
			g_morphingNum = 0;
			g_downCount = 0;
			g_stopTime = 0;

			g_Blast[i].rot.y = RamdomFloat(2, BLASE_ROT, -BLASE_ROT);

			// 吹き出しを表示
			SetBomSpeech();

			return;
		}
	}
}


//=============================================================================
// カメラを切り替えるかどうかのスイッチを知る関数
//=============================================================================
BOOL GetCameraSwitch(void)
{
	return g_cameraOn;
}


//=============================================================================
// カメラを切り替えるかどうかのスイッチ
//=============================================================================
void SetCameraSwitch(BOOL data)
{
	g_cameraOn = data;
}


//=============================================================================
// 現在のモーフィング番号を取得
//=============================================================================
int GetMorphing(void)
{
	return g_morphingNum;
}


//=============================================================================
// 爆弾が空中で止まる時間を返す
//=============================================================================
int GetStopTime(void)
{
	return g_stopTime;
}