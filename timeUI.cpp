//=============================================================================
//
// タイム処理 [timeUI.cpp]
// Author : 熊澤義弘
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "time.h"
#include "sprite.h"
#include "timeUI.h"
#include "fade.h"
#include "score.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(45)	// 時間サイズ
#define TEXTURE_HEIGHT				(90)	// 
#define TEXTURE_MAX					(2)		// テクスチャの数

#define TIME_MAX					(45)	// 時間制限

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void SetEndTime(void);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/time0.png",
	"data/TEXTURE/time1.png",
};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

static int						g_Time;						// 残り時間

static BOOL						g_Load = FALSE;

static int						g_stage;					// 現在のステージ数

static time_t naw_time = 0;		// 現在の時間
static time_t end_time = 0;		// 終了時間

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTime(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// プレイヤーの初期化
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = { 500.0f, 60.0f, 0.0f };
	g_TexNo = 0;

	g_Time = 0;	// 時間の初期化

	g_stage = GetStage();

	// 終了時間の設定
	SetEndTime();

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTime(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTime(void)
{
	// 終了時間から現在の時間を引いて残り時間を算出する
	g_Time = (int)(end_time - time(NULL));

	// 時間が0以下にならないように
	if (g_Time < 0) g_Time = 0;

	// シーン遷移
	if (g_Time == 0)
	{
		switch (g_stage)
		{
		case stage0:
			if (GetScore() >= SCORE_STAGE0_BORDER)
			{
				SetStage(stage1);
				SetFade(FADE_OUT, MODE_GAME_COUNT);
			}
			else SetFade(FADE_OUT, MODE_RESULT);

			break;

		case stage1:
			if (GetScore() >= SCORE_STAGE1_BORDER)
			{
				SetStage(stage2);
				SetFade(FADE_OUT, MODE_GAME_COUNT);
			}
			else SetFade(FADE_OUT, MODE_RESULT);

			break;

		case stage2:
			if (GetScore() >= SCORE_STAGE2_BORDER)
			{
				SetStage(stage3);
				SetFade(FADE_OUT, MODE_GAME_COUNT);
			}
			else SetFade(FADE_OUT, MODE_RESULT);

			break;

		case stage3:
			SetFade(FADE_OUT, MODE_RESULT);

			break;

		}
	}

	if (g_Time > 10)
	{
		g_TexNo = 0;
	}
	else
	{
		g_TexNo = 1;
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTime(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	// 桁数分処理する
	int number = g_Time;
	for (int i = 0; i < TIME_DIGIT; i++)
	{
		// 今回表示する桁の数字
		float x = (float)(number % 10);

		// スコアの位置やテクスチャー座標を反映
		float px = g_Pos.x - g_w*i;	// スコアの表示位置X
		float py = g_Pos.y;			// スコアの表示位置Y
		float pw = g_w;				// スコアの表示幅
		float ph = g_h;				// スコアの表示高さ

		float tw = 1.0f / 10;		// テクスチャの幅
		float th = 1.0f / 1;		// テクスチャの高さ
		float tx = x * tw;			// テクスチャの左上X座標
		float ty = 0.0f;			// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		// 次の桁へ
		number /= 10;
	}
}


//=============================================================================
// タイムを加算する
// 引数:add :追加する点数。マイナスも可能
//=============================================================================
void AddTime(int add)
{
	g_Time += add;
	if (g_Time > TIME_MAX)
	{
		g_Time = TIME_MAX;
	}

}

//=============================================================================
// 残り時間を取得
//=============================================================================
int GetTime(void)
{
	return g_Time;
}


//=============================================================================
// 終了時間をセットする関数
//=============================================================================
void SetEndTime(void)
{
	end_time = time(NULL) + TIME_MAX;
}
