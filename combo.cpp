//=============================================================================
//
// コンボ処理 [combo.cpp]
// Author : 熊澤義弘
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "combo.h"
#include "sprite.h"
#include "speech.h"
#include "score.h"
#include "sound.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(100)		// テクスチャサイズ
#define TEXTURE_HEIGHT				(300)		// 
#define TEXTURE_MAX					(1)			// テクスチャの数

#define COMBO_POS_Y					(360.0f)	// コンボ表示の高さ
#define COMBO_SPEED					(5)			// 表示しているコンボの数が増える速度
#define COMBO_TIME					(220)		// 表示しているコンボの数が切れる時間

#define COMBO_MOVE_MAX				(40.0f)		// コンボ表示をどれくらいの高さから落とすか
#define COMBO_MOVE_SPEED			(5.0f)		// コンボ表示をどれくらいの速さで落とすか
#define COMBO_MOVE_TIME				(3)			// コンボ表示を何フレームで落とすか



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void SetComboMove(void);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/comboNum.png",
};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

static int						g_Combo;					// 表示するコンボ数

static int						g_Combo_result;				// 増えた合計コンボ数
static int						g_time;						// 経過時間
static int						g_ComboTime;				// コンボの持続
static BOOL						g_ComboMove;				// コンボに動きがあるか
static int						g_ComboMoveTime;			// コンボ表示が動く時間

static BOOL						g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitCombo(void)
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
	g_Pos   = { 800.0f, COMBO_POS_Y, 0.0f };
	g_TexNo = 0;

	g_Combo = 0;	// コンボ数の初期化
	g_Combo_result = 0;

	g_time = 0;
	g_ComboTime = 0;
	g_ComboMoveTime = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitCombo(void)
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
void UpdateCombo(void)
{
	if ((g_Combo_result != g_Combo) && (g_time % COMBO_SPEED == 0))
	{
		g_Combo++;
		// コンボ数が増える音
		if(g_Combo >= 15) PlaySound(SOUND_LABEL_SE_cheers05);

		// 吹き出しを表示
		SetComboSpeech();

		SetComboMove();

		// ボーナス数
		int bonus = 0;

		// コンボ数に応じてボーナス
		if (g_Combo > COMBO_BONUS_2)
		{
			bonus = COMBO_BONUS_2_VALUE;
		}
		else if (g_Combo > COMBO_BONUS_1)
		{
			bonus = COMBO_BONUS_1_VALUE;
		}
		else if (g_Combo > COMBO_BONUS_0)
		{
			bonus = COMBO_BONUS_0_VALUE;
		}

		for (int i = 0; i < bonus; i++)
		{
			AddScore(100);
		}

	}

	// 時間を進める
	g_time++;
	g_ComboTime++;

	// コンボ継続時間が切れたらリセット
	if (g_ComboTime >= COMBO_TIME)
	{
		g_Combo = 0;
		g_Combo_result = 0;
	}


	// コンボ数が増えた時にちょっと上から落ちてくる（動きを付ける
	if (g_ComboMoveTime != 0)
	{
		g_Pos.y = (COMBO_POS_Y - g_Pos.y) / COMBO_MOVE_SPEED + g_Pos.y;
		g_ComboMoveTime--;
	}

	// コンボ数を保存
	SetComboMax(g_Combo);
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawCombo(void)
{
	if (g_Combo == 0) return;

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
	int number = g_Combo;
	for (int i = 0; i < COMBO_DIGIT; i++)
	{
		// 今回表示する桁の数字
		float x = (float)(number % 10);

		// コンボ数の位置やテクスチャー座標を反映
		float px = g_Pos.x - g_w*i;	// コンボ数の表示位置X
		float py = g_Pos.y;			// コンボ数の表示位置Y
		float pw = g_w;				// コンボ数の表示幅
		float ph = g_h;				// コンボ数の表示高さ

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
// コンボ数を加算する
// 引数:add :追加する点数。マイナスも可能
//=============================================================================
void AddCombo(int add)
{
	g_Combo_result += add;
	if (g_Combo_result > COMBO_MAX)
	{
		g_Combo_result = COMBO_MAX;
	}

}

//=============================================================================
// コンボを取得
//=============================================================================
int GetCombo(void)
{
	return g_Combo;
}


//=============================================================================
// コンボの継続時間をリセット
//=============================================================================
void ResetComboTime(void)
{
	g_ComboTime = 0;
}


//=============================================================================
// コンボが増えるときに表示を少し動かす
//=============================================================================
void SetComboMove(void)
{
	// コンボが増えたので表示を少し動かす
	//g_ComboMove = TRUE;
	g_ComboMoveTime = COMBO_MOVE_TIME;
	g_Pos.y = COMBO_POS_Y + COMBO_MOVE_MAX;
}