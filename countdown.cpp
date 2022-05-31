//=============================================================================
//
// カウントダウン画面処理 [countdown.cpp]
// Author : 熊澤義弘
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "countdown.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(19)				// テクスチャの数

#define TEXTURE_WIDTH_LOGO			(480)			// ロゴサイズ
#define TEXTURE_HEIGHT_LOGO			(80)			// 

#define CLEAR_TIME					(300)			// ステージクリアを何フレーム出しておくか
#define COUNTDOWN					(60)			// 約1秒

#define TEXTURE_STAGE_MAX			(4)				// 1ステージ分のテクスチャは何枚？

#define ADD_TEXTURE					(3)				// 追加のお祝いテクスチャの数
#define ADD_TEXTURE_SPEED			(15)			// 追加のお祝いテクスチャを何フレームで出すか

#define ADD_TEXTURE_START1			(20)			// 2枚目の追加のお祝いテクスチャを出すまでの時間
#define ADD_TEXTURE_START2			(20 + ADD_TEXTURE_START1)		// 3枚目の追加のお祝いテクスチャを出すまでの時間


enum {
	cracker = 16,
	celebration,
	balloon,
};
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/stage_clear0.png",
	"data/TEXTURE/stage_count03.png",
	"data/TEXTURE/stage_count02.png",
	"data/TEXTURE/stage_count01.png",
	"data/TEXTURE/stage_clear1.png",
	"data/TEXTURE/stage_count13.png",
	"data/TEXTURE/stage_count12.png",
	"data/TEXTURE/stage_count11.png",
	"data/TEXTURE/stage_clear2.png",
	"data/TEXTURE/stage_count23.png",
	"data/TEXTURE/stage_count22.png",
	"data/TEXTURE/stage_count21.png",
	"data/TEXTURE/stage_clear3.png",
	"data/TEXTURE/stage_count33.png",
	"data/TEXTURE/stage_count32.png",
	"data/TEXTURE/stage_count31.png",
	"data/TEXTURE/cracker.png",
	"data/TEXTURE/celebration.png",
	"data/TEXTURE/balloon.png",
};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

static BOOL						g_Use2[ADD_TEXTURE];		// TRUE:使っている  FALSE:未使用
static XMFLOAT3					g_Pos2[ADD_TEXTURE];		// ポリゴンの座標

static int						g_Stage;					// 現在のステージ

static int						g_Count;					// 時間管理

static BOOL						g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitCountDown(void)
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


	// 変数の初期化
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = XMFLOAT3(g_w/2, g_h/2, 0.0f);
	g_TexNo = 0;
	g_Count = 0;

	for (int i = 0; i < ADD_TEXTURE; i++)
	{
		g_Use2[i] = FALSE;
		g_Pos2[i] = XMFLOAT3(g_w / 2, g_h / 2, 0.0f);
	}

	// 最初は隠れている
	g_Pos2[0].y -= TEXTURE_HEIGHT;
	g_Pos2[1].y += TEXTURE_HEIGHT;
	g_Pos2[2].x += TEXTURE_WIDTH;

	g_Stage = GetStage() - 1;
	switch (g_Stage)
	{
	case stage2:
		g_Use2[2] = TRUE;

	case stage1:
		g_Use2[1] = TRUE;

	case stage0:
		g_Use2[0] = TRUE;
		// SE再生
		PlaySound(SOUND_LABEL_SE_cheers05);
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitCountDown(void)
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
void UpdateCountDown(void)
{
	// 時間を進める
	g_Count++;
	
	// 時間経過とステージ数に応じて表示するテクスチャを変更
	if (g_Count <= CLEAR_TIME)
	{
		g_TexNo = TEXTURE_STAGE_MAX * g_Stage;
	}
	else if (g_Count <= CLEAR_TIME + COUNTDOWN)
	{
		g_TexNo = TEXTURE_STAGE_MAX * g_Stage + 1;
	}
	else if (g_Count <= CLEAR_TIME + (COUNTDOWN * 2))
	{
		g_TexNo = TEXTURE_STAGE_MAX * g_Stage + 2;
	}
	else if (g_Count <= CLEAR_TIME + (COUNTDOWN * 3))
	{
		g_TexNo = TEXTURE_STAGE_MAX * g_Stage + 3;
		SetFade(FADE_OUT, MODE_GAME);
	}


	// お祝いテクスチャ
	switch (g_Stage)
	{
	case 2:
		if (g_Count >= ADD_TEXTURE_START2)
		{
			g_Pos2[2].x += (g_w / 2 - g_Pos2[2].x) / ADD_TEXTURE_SPEED;
		}

	case 1:
		if (g_Count >= ADD_TEXTURE_START1)
		{
			g_Pos2[1].y += (g_h / 2 - g_Pos2[1].y) / ADD_TEXTURE_SPEED;
		}

	case 0:
		g_Pos2[0].y += (g_h / 2 - g_Pos2[0].y) / ADD_TEXTURE_SPEED;
		break;

	}

	// クリア画面から変わったらお祝いテクスチャを消す
	if (g_Count >= CLEAR_TIME)
	{
		for (int i = 0; i < ADD_TEXTURE; i++)
		{
			g_Use2[i] = FALSE;
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawCountDown(void)
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

	// ロゴを描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// お祝いテクスチャを描画
	for(int i = 0 ; i < ADD_TEXTURE ; i++)
	{
		if (!g_Use2[i]) continue;

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[cracker + i]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, g_Pos2[i].x, g_Pos2[i].y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

}