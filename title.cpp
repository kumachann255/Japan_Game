//=============================================================================
//
// タイトル画面処理 [title.cpp]
// Author : エナ
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "title.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(6)				// テクスチャの数

#define TEXTURE_WIDTH_LOGO			(815 / 2)			// ロゴサイズ
#define TEXTURE_HEIGHT_LOGO			(296 / 2)			// 

#define TEXTURE_WIDTH_START			(443 / 2)			// ロゴサイズ
#define TEXTURE_HEIGHT_START		(178 / 2)			// 

#define TEXTURE_WIDTH_EXIT			(443 / 2)			// ロゴサイズ
#define TEXTURE_HEIGHT_EXIT			(178 / 2)			// 

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/title.png",
	"data/TEXTURE/title_logo.png",
	"data/TEXTURE/title_start.png",
	"data/TEXTURE/title_start_pushed.png",
	"data/TEXTURE/title_exit.png",
	"data/TEXTURE/title_exit_pushed.png",
};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

static float					logo_time;
static BOOL						flag_rot;
static XMFLOAT3					logo_rot;					// ポリゴンの座標

static int						start_texno;
static float					start_w, start_h;			// 幅と高さ
static XMFLOAT3					start_pos;					// ポリゴンの座標

static int						exit_texno;
static float					exit_w, exit_h;				// 幅と高さ
static XMFLOAT3					exit_pos;					// ポリゴンの座標

static BOOL						g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTitle(void)
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

	logo_time = 1.0f;
	flag_rot = TRUE;
	logo_rot = XMFLOAT3(0.0f, 0.0f, 0.1f);

	start_texno = 3;
	start_w = TEXTURE_WIDTH_START;
	start_h = TEXTURE_HEIGHT_START;
	start_pos = XMFLOAT3(g_w / 8, g_h - 60, 0.0f);
	
	exit_texno = 4;
	exit_w = TEXTURE_WIDTH_EXIT - (TEXTURE_WIDTH_EXIT / 3);
	exit_h = TEXTURE_HEIGHT_EXIT - (TEXTURE_HEIGHT_EXIT / 3);
	exit_pos = XMFLOAT3(g_w / 2.7f, g_h - 40, 0.0f);

	// BGM再生
	PlaySound(SOUND_LABEL_BGM_bgm_title);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTitle(void)
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
void UpdateTitle(void)
{
	if (flag_rot == TRUE)
	{
		logo_time -= 0.02f;
		if (logo_time <= 0.0f)
		{
			logo_rot.z = 0.1f;
			logo_time = 0.0f;
			flag_rot = FALSE;
		}
	}
	else
	{
		logo_time += 0.02f;
		if (logo_time >= 1.0f)
		{
			logo_rot.z = -0.1f;
			logo_time = 1.0f;
			flag_rot = TRUE;
		}
	}

	//if (g_Time < 0)
	{
		if (GetKeyboardTrigger(DIK_D) || GetKeyboardTrigger(DIK_A))
		{// S/W押したら、選択を切り替える

			// SEの挿入（切り替え音）
			PlaySound(SOUND_LABEL_SE_titleClick04);

			if (start_texno == 3)
			{
				start_texno--;
				start_w -= TEXTURE_WIDTH_START / 3;
				start_h -= TEXTURE_HEIGHT_START / 3;

				exit_texno++;
				exit_w += TEXTURE_WIDTH_EXIT / 3;
				exit_h += TEXTURE_HEIGHT_EXIT / 3;
			}
			else
			{
				start_texno = 3;
				start_w += TEXTURE_WIDTH_START / 3;
				start_h += TEXTURE_HEIGHT_START / 3;

				exit_texno = 4;
				exit_w -= TEXTURE_WIDTH_EXIT / 3;
				exit_h -= TEXTURE_HEIGHT_EXIT / 3;

			}
		}


		if (GetKeyboardTrigger(DIK_RETURN))
		{// Enter押したら、ステージを切り替える

			// SEの挿入（モード選択音）
			PlaySound(SOUND_LABEL_SE_selectBomb01);

			if (start_texno == 3)
			{
				SetStage(tutorial);
				SetFade(FADE_OUT, MODE_GAME);
		
				// mainスコアを初期化
				ResetMainScore();

			}
			else
			{
				Sleep(800);
				exit(-1);
			}
		}


		// ゲームパッドで入力処理
		if (IsButtonTriggered(0, BUTTON_LEFT) || IsButtonTriggered(0, BUTTON_RIGHT))
		{// S/W押したら、選択を切り替える

			// SEの挿入（切り替え音）
			PlaySound(SOUND_LABEL_SE_titleClick04);

			if (start_texno == 3)
			{
				start_texno--;
				exit_texno++;
			}
			else
			{
				start_texno = 3;
				exit_texno = 4;
			}
		}


		else if (IsButtonTriggered(0, BUTTON_START))
		{
			// SEの挿入（モード選択音）
			PlaySound(SOUND_LABEL_SE_selectBomb01);

			if (start_texno == 3)
			{
				SetStage(tutorial);
				SetFade(FADE_OUT, MODE_GAME);
			
				// mainスコアを初期化
				ResetMainScore();
			}
			else
			{
				Sleep(800);
				exit(-1);
			}
		}
		else if (IsButtonTriggered(0, BUTTON_B))
		{
			// SEの挿入（モード選択音）
			PlaySound(SOUND_LABEL_SE_selectBomb01);

			if (start_texno == 3)
			{
				SetStage(tutorial);
				SetFade(FADE_OUT, MODE_GAME);
			
				// mainスコアを初期化
				ResetMainScore();
			}
			else
			{
				Sleep(800);
				exit(-1);
			}
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTitle(void)
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

	// タイトルの背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// タイトルのロゴを描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		//	SetSprite(g_VertexBuffer, g_Pos.x, g_Pos.y, TEXTURE_WIDTH_LOGO, TEXTURE_HEIGHT_LOGO, 0.0f, 0.0f, 1.0f, 1.0f);
		SetSpriteColorRotation(g_VertexBuffer, g_Pos.x - 200, g_Pos.y - 180, TEXTURE_WIDTH_LOGO, TEXTURE_HEIGHT_LOGO, 0.0f, 0.0f, 1.0f, 1.0f,
						XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), logo_rot.z);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// タイトルのメニューを描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[start_texno]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, start_pos.x, start_pos.y, start_w, start_h, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[exit_texno]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, exit_pos.x, exit_pos.y, exit_w, exit_h, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}





