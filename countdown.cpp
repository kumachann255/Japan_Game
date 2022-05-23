//=============================================================================
//
// カウントダウン画面処理 [countdown.cpp]
// Author : 
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
#define TEXTURE_MAX					(16)				// テクスチャの数

#define TEXTURE_WIDTH_LOGO			(480)			// ロゴサイズ
#define TEXTURE_HEIGHT_LOGO			(80)			// 

#define CLEAR_TIME					(120)			// ステージクリアを何フレーム出しておくか
#define COUNTDOWN					(60)			// 約1秒

#define TEXTURE_STAGE_MAX			(4)				// 1ステージ分のテクスチャは何枚？


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
};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

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

	// BGM再生
	//PlaySound(SOUND_LABEL_BGM_sample000);

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
	
	int stage = GetStage() - 1;

	// 時間経過とステージ数に応じて表示するテクスチャを変更
	if (g_Count <= CLEAR_TIME)
	{
		g_TexNo = TEXTURE_STAGE_MAX * stage;
	}
	else if (g_Count <= CLEAR_TIME + COUNTDOWN)
	{
		g_TexNo = TEXTURE_STAGE_MAX * stage + 1;
	}
	else if (g_Count <= CLEAR_TIME + (COUNTDOWN * 2))
	{
		g_TexNo = TEXTURE_STAGE_MAX * stage + 2;
	}
	else if (g_Count <= CLEAR_TIME + (COUNTDOWN * 3))
	{
		g_TexNo = TEXTURE_STAGE_MAX * stage + 3;
		SetFade(FADE_OUT, MODE_GAME);
	}
	//else if (g_Count <= CLEAR_TIME + (COUNTDOWN * 3) + COUNTDOWN / 2)
	//{
	//	g_Count = 0;
	//	SetFade(FADE_OUT, MODE_GAME);
	//}



	if (GetKeyboardTrigger(DIK_RETURN))
	{// Enter押したら、ステージを切り替える
		//SetFade(FADE_OUT, MODE_RESULT);
	}
	// ゲームパッドで入力処理
	else if (IsButtonTriggered(0, BUTTON_START))
	{
		SetFade(FADE_OUT, MODE_TITLE_DirectX);
	}
	else if (IsButtonTriggered(0, BUTTON_B))
	{
		SetFade(FADE_OUT, MODE_TITLE_DirectX);
	}




#ifdef _DEBUG	// デバッグ情報を表示する
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

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
}