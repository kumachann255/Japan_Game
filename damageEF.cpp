//=============================================================================
//
// ダメージエフェクト処理 [damageEF.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "damageEF.h"
#include "sound.h"
#include "sprite.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(4)				// テクスチャの数

#define DAMAGEEF_MAX_TIME			(20)			// 何フレームダメージエフェクトを表示するか

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/glass0.png",
	"data/TEXTURE/glass1.png",
	"data/TEXTURE/glass2.png",
	"data/TEXTURE/glass3.png",
};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

static int						g_time;						// 表示時間のカウント

static XMFLOAT4						g_Color;					// フェードのカラー（α値）

static BOOL						g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitDamageEF(void)
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
	g_Pos   = { 0.0f, 0.0f, 0.0f };
	g_TexNo = 0;

	g_time = 0;

	g_Color = { 1.0, 1.0, 1.0, 1.0 };

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitDamageEF(void)
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
void UpdateDamageEF(void)
{
	// ダメージを受けている場合、表示
	if (g_Use)
	{
		g_time++;

		// 時間経過で非表示に
		if (g_time > DAMAGEEF_MAX_TIME)
		{
			g_Use = FALSE;
			g_time = 0;
		}
	}





	//if (g_Fade != FADE_NONE)
	//{// フェード処理中
	//	if (g_Fade == FADE_OUT)
	//	{// フェードアウト処理
	//		g_Color.w += FADE_RATE;		// α値を加算して画面を消していく
	//		if (g_Color.w >= 1.0f)
	//		{
	//			// 鳴っている曲を全部止める
	//			StopSound();

	//			// フェードイン処理に切り替え
	//			g_Color.w = 1.0f;
	//			SetFade(FADE_IN, g_ModeNext);

	//			// モードを設定
	//			SetMode(g_ModeNext);
	//		}

	//	}
	//	else if (g_Fade == FADE_IN)
	//	{// フェードイン処理
	//		g_Color.w -= FADE_RATE;		// α値を減算して画面を浮き上がらせる
	//		if (g_Color.w <= 0.0f)
	//		{
	//			// フェード処理終了
	//			g_Color.w = 0.0f;
	//			SetFade(FADE_NONE, g_ModeNext);
	//		}

	//	}
	//}


#ifdef _DEBUG	// デバッグ情報を表示する
	// PrintDebugProc("\n");

#endif

}


//=============================================================================
// 描画処理
//=============================================================================
void DrawDamageEF(void)
{
	if (!g_Use) return;	// ダメージを受けていなければ表示しない

	// 加算合成に設定
	//SetBlendState(BLEND_MODE_ADD);

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);


	// タイトルの背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		//SetVertex(0.0f, 0.0f, SCREEN_WIDTH, TEXTURE_WIDTH, 0.0f, 0.0f, 1.0f, 1.0f);
		SetSpriteColor(g_VertexBuffer, SCREEN_WIDTH/2, TEXTURE_HEIGHT /2, SCREEN_WIDTH, TEXTURE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f,
			g_Color);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

}


//=============================================================================
// ダメージエフェクトのスイッチ
//=============================================================================
void SetDamageEF(BOOL data)
{
	g_Use = data;
	g_time = 0;

	// スイッチオンの場合
	if (data == TRUE)
	{
		// 表示するテクスチャをランダムで変更
		g_TexNo = rand() % TEXTURE_MAX;
	}
}



