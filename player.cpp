//=============================================================================
//
// モデル処理 [player.cpp]
// Author :山田隆徳
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "player.h"
#include "shadow.h"
#include "light.h"
#include "bullet.h"
#include "meshfield.h"
#include "bom.h"
#include "speech.h"
#include "tutorial.h"
#include "sound.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_CAR					"data/MODEL/car2.obj"			// 読み込むモデル名

#define	MODEL_PLAYER_PARTS_H		"data/MODEL/head.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_PARTS_B		"data/MODEL/body.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_PARTS_R_HAND	"data/MODEL/R_hand.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_PARTS_R_REG	"data/MODEL/R_reg.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_PARTS_L_HAND	"data/MODEL/L_hand.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_PARTS_L_REG	"data/MODEL/L_reg.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_PARTS_STICK	"data/MODEL/stick.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_PARTS_NABE		"data/MODEL/nabe.obj"			// 読み込むモデル名


#define	VALUE_MOVE			(2.0f)							// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// 回転量

#define PLAYER_SHADOW_SIZE	(1.0f)							// 影の大きさ
#define PLAYER_OFFSET_Y		(13.0f)							// プレイヤーの足元をあわせる

#define PLAYER_PARTS_MAX	(8)								// プレイヤーのパーツの数


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static PLAYER		g_Player;						// プレイヤー

static PLAYER		g_Parts[PLAYER_PARTS_MAX];		// プレイヤーのパーツ用
static int			g_Stage;

static BOOL			g_Load = FALSE;

static char *g_ModelName[PLAYER_PARTS_MAX] = {
#define	MODEL_PLAYER_PARTS_H		"data/MODEL/head.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_PARTS_B		"data/MODEL/body.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_PARTS_R_HAND	"data/MODEL/R_hand.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_PARTS_R_REG	"data/MODEL/R_reg.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_PARTS_L_HAND	"data/MODEL/L_hand.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_PARTS_L_REG	"data/MODEL/L_reg.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_PARTS_STICK	"data/MODEL/stick.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_PARTS_NABE		"data/MODEL/nabe.obj"			// 読み込むモデル名

};

//*****************************************************************************
// 車のモーションテーブル
//*****************************************************************************
static INTERPOLATION_DATA car[] = {
	// pos,								rot,						 scl,						 frame
	{ XMFLOAT3(0.0f, PLAYER_OFFSET_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 0.6f },
	{ XMFLOAT3(0.0f, PLAYER_OFFSET_Y + 0.8f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 0.6f },
	{ XMFLOAT3(0.0f, PLAYER_OFFSET_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 0.6f },


};



//*****************************************************************************
// 待機時のモーションテーブル
//*****************************************************************************

static INTERPOLATION_DATA s_head[] = {
	// pos,								rot,						 scl,						 frame
	{ XMFLOAT3(0.0f, 20.0f, 0.0f), XMFLOAT3(0.0f, XM_PI, 0.0f), XMFLOAT3(0.9f, 0.9f, 0.9f), 60 * 1.0f },
};

static INTERPOLATION_DATA s_body[] = {
	// pos,								rot,						 scl,						 frame
	{ XMFLOAT3(4.0f, 8.0f, -30.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.5f, 0.5f, 0.5f), 60 * 1.0f },
};

static INTERPOLATION_DATA s_R_hand[] = {
	// pos,								rot,						 scl,						 frame
	{ XMFLOAT3(-16.0f, 14.0f,  0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.3f },
	{ XMFLOAT3(-17.5f, 14.0f, -3.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.2f },
	{ XMFLOAT3(-19.0f, 14.0f, -5.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.3f },
	{ XMFLOAT3(-20.5f, 14.0f, -3.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.2f },
	{ XMFLOAT3(-22.0f, 14.0f,  0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.3f },
	{ XMFLOAT3(-20.5f, 14.0f,  3.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.2f },
	{ XMFLOAT3(-19.0f, 14.0f,  5.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.3f },
	{ XMFLOAT3(-17.5f, 14.0f,  3.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.2f },
	{ XMFLOAT3(-16.0f, 14.0f,  0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.3f },

};

static INTERPOLATION_DATA s_R_reg[] = {
	// pos,								rot,						 scl,						 frame
	{ XMFLOAT3(7.0f, -13.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1.0f },
};

static INTERPOLATION_DATA s_L_hand[] = {
	// pos,								rot,						 scl,						 frame
	{ XMFLOAT3(-16.0f, 8.0f,  0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.3f },
	{ XMFLOAT3(-17.5f, 8.0f, -3.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.2f },
	{ XMFLOAT3(-19.0f, 8.0f, -5.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.3f },
	{ XMFLOAT3(-20.5f, 8.0f, -3.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.2f },
	{ XMFLOAT3(-22.0f, 8.0f,  0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.3f },
	{ XMFLOAT3(-20.5f, 8.0f,  3.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.2f },
	{ XMFLOAT3(-19.0f, 8.0f,  5.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.3f },
	{ XMFLOAT3(-17.5f, 8.0f,  3.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.2f },
	{ XMFLOAT3(-16.0f, 8.0f,  0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.3f },
};

static INTERPOLATION_DATA s_L_reg[] = {
	// pos,								rot,						 scl,						 frame
	{ XMFLOAT3(-7.0f, -13.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1.0f },
};

static INTERPOLATION_DATA s_stick[] = {
	// pos,								rot,						 scl,						 frame
	{ XMFLOAT3(0.0f, -8.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, XM_PI), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 1.0f },
};

static INTERPOLATION_DATA s_nabe[] = {
	// pos,								rot,						 scl,						 frame
	{ XMFLOAT3(-19.0f, -7.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(2.0f, 2.0f, 2.0f), 60 * 1.0f },
};

//*****************************************************************************
// 投擲時のモーションテーブル
//*****************************************************************************
static INTERPOLATION_DATA m_head[] = {
	// pos,								rot,						 scl,						 frame
	{ XMFLOAT3(0.0f, 20.0f, 0.0f), XMFLOAT3(0.0f, XM_PI, 0.0f), XMFLOAT3(0.9f, 0.9f, 0.9f), 60 * 1.0f },
};

static INTERPOLATION_DATA m_body[] = {
	// pos,								rot,						 scl,						 frame
	{ XMFLOAT3(4.0f, 8.0f, -30.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.5f, 0.5f, 0.5f), 60 * 1.0f },
};

static INTERPOLATION_DATA m_R_hand[] = {
	// pos,								rot,						 scl,						 frame
	{ XMFLOAT3(7.0f, 0.0f, -15.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.2f },
	{ XMFLOAT3(7.0f, 0.0f,	 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.2f },
	{ XMFLOAT3(7.0f, 0.0f,  15.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.2f },


};

static INTERPOLATION_DATA m_R_reg[] = {
	// pos,								rot,						 scl,						 frame
	{ XMFLOAT3(7.0f, -13.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1.0f },
};

static INTERPOLATION_DATA m_L_hand[] = {
	// pos,								rot,						 scl,						 frame
	{ XMFLOAT3(-12.0f, 18.0f, 15.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),			 XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.1f },
	{ XMFLOAT3(-12.0f, 17.5f,  7.5f), XMFLOAT3(-XM_PI * 0.125f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.1f },
	{ XMFLOAT3(-12.0f, 16.0f,  0.0f), XMFLOAT3(-XM_PI * 0.25f, 0.0f, 0.0f),  XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.1f },
	{ XMFLOAT3(-12.0f, 10.0f, -7.5f), XMFLOAT3(-XM_PI * 0.375f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.1f },
	{ XMFLOAT3(-12.0f, 0.0f, -15.0f), XMFLOAT3(-XM_PI * 0.5f, 0.0f, 0.0f),	 XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 0.1f },
};

static INTERPOLATION_DATA m_L_reg[] = {
	// pos,								rot,						 scl,						 frame
	{ XMFLOAT3(-7.0f, -13.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1.0f },
};

static INTERPOLATION_DATA m_stick[] = {
	// pos,								rot,						 scl,						 frame
	{ XMFLOAT3(0.0f, 15.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.7f, 0.7f, 0.7f), 60 * 1.0f },
};

static INTERPOLATION_DATA m_nabe[] = {
	// pos,								rot,						 scl,						 frame
	{ XMFLOAT3(-19.0f, -7.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(2.0f, 2.0f, 2.0f), 60 * 1.0f },
};




//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{
	LoadModel(MODEL_CAR, &g_Player.model);
	g_Player.load = TRUE;

	g_Player.pos = { 0.0f, PLAYER_OFFSET_Y, 200.0f };
	g_Player.rot = { 0.0f, 0.0f, 0.0f };
	g_Player.scl = { 1.0f, 1.0f, 1.0f };

	g_Player.spd = 0.0f;			// 移動スピードクリア
	g_Player.size = PLAYER_SIZE;	// 当たり判定の大きさ

	g_Player.hp = PLAYER_MAX_HP;

	g_Player.action = FALSE;

	g_Player.use = TRUE;

	// ここでプレイヤー用の影を作成している
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	g_Player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);
	//          ↑
	//        このメンバー変数が生成した影のIndex番号

	// 階層アニメーション用の初期化処理
	g_Player.parent = NULL;			// 本体（親）なのでNULLを入れる




	// パーツの初期化
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		g_Parts[i].use = TRUE;

		// 位置・回転・スケールの初期設定
		g_Parts[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		// 親子関係
		g_Parts[i].parent = &g_Player;		// ← ここに親のアドレスを入れる

		// 階層アニメーション用のメンバー変数の初期化
		g_Parts[i].tbl_adr = NULL;		// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i].move_time = 0.0f;	// 実行時間をクリア
		g_Parts[i].tbl_size = 0;		// 再生するアニメデータのレコード数をセット

		// パーツの読み込みはまだしていない
		g_Parts[i].load = 0;
	}
//
	g_Parts[0].use = TRUE;

	//========================================================================
	// 親子関係(主に末端パーツとのペアリング)
	//
	//作成例
	//	g_Parts[腕].parent= &g_Player;		// 腕だったら親は本体（プレイヤー）
	//	g_Parts[手].parent= &g_Paerts[腕];	// 指が腕の子供だった場合の例
	//========================================================================
	g_Parts[0].parent = &g_Parts[1];						//	頭をボディに
	g_Parts[2].parent = &g_Parts[1];						//	右手をボディに
	g_Parts[3].parent = &g_Parts[1];						//	右足をボディに
	g_Parts[4].parent = &g_Parts[1];						//	左手をボディに
	g_Parts[5].parent = &g_Parts[1];						//	左足をボディに
	g_Parts[6].parent = &g_Parts[4];						//	スティックを右手に
	g_Parts[7].parent = &g_Parts[1];						//	スティックを右手に


	SetStayMotion();

	g_Parts[0].load = TRUE;


	g_Stage = GetStage();

	//=============================================================================
// ここで各パーツの読み込み
//=============================================================================
	LoadModel(MODEL_PLAYER_PARTS_H,		 &g_Parts[0].model);	 // ０番目のモデルを読み込む
	LoadModel(MODEL_PLAYER_PARTS_B,		 &g_Parts[1].model);	 // １番目のモデルを読み込む
	LoadModel(MODEL_PLAYER_PARTS_R_HAND, &g_Parts[2].model);	 // ２番目のモデルを読み込む
	LoadModel(MODEL_PLAYER_PARTS_R_REG,  &g_Parts[3].model);	 // ３番目のモデルを読み込む
	LoadModel(MODEL_PLAYER_PARTS_L_HAND, &g_Parts[4].model);	 // ４番目のモデルを読み込む
	LoadModel(MODEL_PLAYER_PARTS_L_REG,	 &g_Parts[5].model);	 // ５番目のモデルを読み込む
	LoadModel(MODEL_PLAYER_PARTS_STICK,  &g_Parts[6].model);	 // ６番目のモデルを読み込む
	LoadModel(MODEL_PLAYER_PARTS_NABE,   &g_Parts[7].model);	 // ７番目のモデルを読み込む

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
{
	if (g_Load == FALSE) return;

	// モデルの解放処理
	if (g_Player.load)
	{
		UnloadModel(&g_Player.model);
		g_Player.load = FALSE;
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{
	CAMERA *cam = GetCamera();

	g_Player.pos.z -= 2.0f;

	if (g_Player.pos.z <= 0.0f)
	{
		g_Player.pos.z = 0.0f;
	}
	if (g_Player.pos.z == 0.0f)
	{
		SetCarMotion();
	}

#ifdef _DEBUG
	if (GetKeyboardPress(DIK_R))
	{
		g_Player.pos.z = g_Player.pos.x = 0.0f;
		g_Player.rot.y = g_Player.dir = 0.0f;
		g_Player.spd = 0.0f;
	}
#endif


	//	// Key入力があったら移動処理する
	if (g_Player.spd > 0.0f)
	{
		g_Player.rot.y = g_Player.dir + cam->rot.y;

		// 入力のあった方向へプレイヤーを向かせて移動させる
		g_Player.pos.x -= sinf(g_Player.rot.y) * g_Player.spd;
		g_Player.pos.z -= cosf(g_Player.rot.y) * g_Player.spd;
	}



	// レイキャストして足元の高さを求める
	XMFLOAT3 normal = { 0.0f, 1.0f, 0.0f };				// ぶつかったポリゴンの法線ベクトル（向き）
	XMFLOAT3 hitPosition;								// 交点
	hitPosition.y = g_Player.pos.y - PLAYER_OFFSET_Y;	// 外れた時用に初期化しておく
	bool ans = RayHitField(g_Player.pos, &hitPosition, &normal);
	g_Player.pos.y = hitPosition.y + PLAYER_OFFSET_Y;
	g_Player.pos.y = PLAYER_OFFSET_Y;


	// 影もプレイヤーの位置に合わせる
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	SetPositionShadow(g_Player.shadowIdx, pos);

	// 弾発射処理
	if (g_Stage != tutorial)
	{
		if (((GetKeyboardTrigger(DIK_SPACE)) || (IsButtonTriggered(0, BUTTON_B))) && (GetCoolTime() == 0))
		{
			g_Player.action = TRUE;
			SetBomthrowMotion();

			SetBom();
		}
	}
	else
	{
		if (((GetKeyboardTrigger(DIK_SPACE)) || (IsButtonTriggered(0, BUTTON_B))) 
			&& (!GetTutorialUse()))
		{
			g_Player.action = TRUE;
			SetBomthrowMotion();

			SetBom();
		}
	}


	g_Player.spd *= 0.5f;


	// 階層アニメーション
			// 使われているなら処理する
	if ((g_Player.use == TRUE) && (g_Player.tbl_adr != NULL))
	{
		// 移動処理
		int		index = (int)g_Player.move_time;
		float	time = g_Player.move_time - index;
		int		size = g_Player.tbl_size;

		float dt = 1.0f / g_Player.tbl_adr[index].frame;	// 1フレームで進める時間
		g_Player.move_time += dt;					// アニメーションの合計時間に足す

		if (index > (size - 2))	// ゴールをオーバーしていたら、最初へ戻す
		{
			g_Player.move_time = 0.0f;
			index = 0;
		}

		// 座標を求める	X = StartX + (EndX - StartX) * 今の時間
		XMVECTOR p1 = XMLoadFloat3(&g_Player.tbl_adr[index + 1].pos);	// 次の場所
		XMVECTOR p0 = XMLoadFloat3(&g_Player.tbl_adr[index + 0].pos);	// 現在の場所
		XMVECTOR vec = p1 - p0;
		XMStoreFloat3(&g_Player.pos, p0 + vec * time);

		// 回転を求める	R = StartX + (EndX - StartX) * 今の時間
		XMVECTOR r1 = XMLoadFloat3(&g_Player.tbl_adr[index + 1].rot);	// 次の角度
		XMVECTOR r0 = XMLoadFloat3(&g_Player.tbl_adr[index + 0].rot);	// 現在の角度
		XMVECTOR rot = r1 - r0;
		XMStoreFloat3(&g_Player.rot, r0 + rot * time);

		// scaleを求める S = StartX + (EndX - StartX) * 今の時間
		XMVECTOR s1 = XMLoadFloat3(&g_Player.tbl_adr[index + 1].scl);	// 次のScale
		XMVECTOR s0 = XMLoadFloat3(&g_Player.tbl_adr[index + 0].scl);	// 現在のScale
		XMVECTOR scl = s1 - s0;
		XMStoreFloat3(&g_Player.scl, s0 + scl * time);

	}


	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		// 使われているなら処理する
		if ((g_Parts[i].use == TRUE)&&(g_Parts[i].tbl_adr != NULL))
		{
			// 移動処理
			int		index = (int)g_Parts[i].move_time;
			float	time = g_Parts[i].move_time - index;
			int		size = g_Parts[i].tbl_size;

			float dt = 1.0f / g_Parts[i].tbl_adr[index].frame;	// 1フレームで進める時間
			g_Parts[i].move_time += dt;					// アニメーションの合計時間に足す

			if ((g_Player.action == TRUE) && (g_Parts[i].move_time >= size - 1) && (i == 4))
			{
				SetStayMotion();
				g_Player.action = FALSE;

			}
			if (index > (size - 2))	// ゴールをオーバーしていたら、最初へ戻す
			{
				g_Parts[i].move_time = 0.0f;
				index = 0;
			}

			// 座標を求める	X = StartX + (EndX - StartX) * 今の時間
			XMVECTOR p1 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 1].pos);	// 次の場所
			XMVECTOR p0 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 0].pos);	// 現在の場所
			XMVECTOR vec = p1 - p0;
			XMStoreFloat3(&g_Parts[i].pos, p0 + vec * time);

			// 回転を求める	R = StartX + (EndX - StartX) * 今の時間
			XMVECTOR r1 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 1].rot);	// 次の角度
			XMVECTOR r0 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 0].rot);	// 現在の角度
			XMVECTOR rot = r1 - r0;
			XMStoreFloat3(&g_Parts[i].rot, r0 + rot * time);

			// scaleを求める S = StartX + (EndX - StartX) * 今の時間
			XMVECTOR s1 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 1].scl);	// 次のScale
			XMVECTOR s0 = XMLoadFloat3(&g_Parts[i].tbl_adr[index + 0].scl);	// 現在のScale
			XMVECTOR scl = s1 - s0;
			XMStoreFloat3(&g_Parts[i].scl, s0 + scl * time);

		}
	}





	{	// ポイントライトのテスト
		LIGHT *light = GetLightData(1);
		XMFLOAT3 pos = g_Player.pos;
		pos.y += 20.0f;

		light->Position = pos;
		light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		light->Type = LIGHT_TYPE_POINT;
		light->Enable = TRUE;
		SetLightData(1, light);
	}


	//////////////////////////////////////////////////////////////////////
	// 姿勢制御
	//////////////////////////////////////////////////////////////////////

	XMVECTOR vx, nvx, up;
	XMVECTOR quat;
	float len, angle;

	// ２つのベクトルの外積を取って任意の回転軸を求める
	g_Player.upVector = normal;
	up = { 0.0f, 1.0f, 0.0f, 0.0f };
	vx = XMVector3Cross(up, XMLoadFloat3(&g_Player.upVector));

	// 求めた回転軸からクォータニオンを作り出す
	nvx = XMVector3Length(vx);
	XMStoreFloat(&len, nvx);
	nvx = XMVector3Normalize(vx);
	angle = asinf(len);
	quat = XMQuaternionRotationNormal(nvx, angle);

	// 前回のクォータニオンから今回のクォータニオンまでの回転を滑らかにする
	quat = XMQuaternionSlerp(XMLoadFloat4(&g_Player.quaternion), quat, 0.05f);

	// 今回のクォータニオンの結果を保存する
	XMStoreFloat4(&g_Player.quaternion, quat);



#ifdef _DEBUG	// デバッグ情報を表示する
	PrintDebugProc("Player:↑ → ↓ ←　Space\n");
	PrintDebugProc("Player:X:%f Y:%f Z:%f\n", g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
{
	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// スケールを反映
	mtxScl = XMMatrixScaling(g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(g_Player.rot.x, g_Player.rot.y + XM_PI, g_Player.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	//// クォータニオンを反映
	//XMMATRIX quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Player.quaternion));
	//mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);
	
	// 移動を反映
	mtxTranslate = XMMatrixTranslation(g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);


	// モデル描画
	DrawModel(&g_Player.model);



	// パーツの階層アニメーション
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		//if (i != 2) return;
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
			// g_Player.mtxWorldを指している
		}

		XMStoreFloat4x4(&g_Parts[i].mtxWorld, mtxWorld);

		// 使われているなら処理する。ここまで処理している理由は他のパーツがこのパーツを参照している可能性があるから。
		if (g_Parts[i].use == FALSE) continue;

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);


		// モデル描画
		DrawModel(&g_Parts[i].model);

	}



	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// プレイヤー情報を取得
//=============================================================================
PLAYER *GetPlayer(void)
{
	return &g_Player;
}

void SetPlayerDamage(int damage)
{
	// SEのセット
	PlaySound(SOUND_LABEL_SE_damagedSound01);

	g_Player.hp -= damage;

	SetDamageSpeech();

}


void SetCarMotion(void)
{
	g_Player.tbl_adr = car;	// 再生するアニメデータの先頭アドレスをセット
	g_Player.tbl_size = sizeof(car) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット


}




void SetStayMotion(void)
{
//*****************************************************************************
// 待機時のモーションテーブル
//*****************************************************************************

	g_Parts[0].tbl_adr = s_head;	// 再生するアニメデータの先頭アドレスをセット
	g_Parts[0].tbl_size = sizeof(s_head) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[0].load = 1;

	g_Parts[1].tbl_adr = s_body;	// 再生するアニメデータの先頭アドレスをセット
	g_Parts[1].tbl_size = sizeof(s_body) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[1].load = 1;

	g_Parts[2].tbl_adr = s_R_hand;	// 再生するアニメデータの先頭アドレスをセット
	g_Parts[2].tbl_size = sizeof(s_R_hand) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[2].load = 1;

	g_Parts[3].tbl_adr = s_R_reg;	// 再生するアニメデータの先頭アドレスをセット
	g_Parts[3].tbl_size = sizeof(s_R_reg) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[3].load = 1;

	g_Parts[4].tbl_adr = s_L_hand;	// 再生するアニメデータの先頭アドレスをセット
	g_Parts[4].tbl_size = sizeof(s_L_hand) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[4].load = 1;

	g_Parts[5].tbl_adr = s_L_reg;	// 再生するアニメデータの先頭アドレスをセット
	g_Parts[5].tbl_size = sizeof(s_L_reg) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[5].load = 1;

	g_Parts[6].tbl_adr = s_stick;	// 再生するアニメデータの先頭アドレスをセット
	g_Parts[6].tbl_size = sizeof(s_stick) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[6].load = 1;

	g_Parts[7].tbl_adr = s_nabe;	// 再生するアニメデータの先頭アドレスをセット
	g_Parts[7].tbl_size = sizeof(s_nabe) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[7].load = 1;


	for (int i = 0; i < 8; i++)
	{
		g_Parts[i].move_time = 0.0f;
		g_Parts[6].use = TRUE;
	}

}


void SetBomthrowMotion(void)
{
//*****************************************************************************
// 投擲時のモーションテーブル
//*****************************************************************************
	g_Parts[0].tbl_adr = m_head;	// 再生するアニメデータの先頭アドレスをセット
	g_Parts[0].tbl_size = sizeof(m_head) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[0].load = 1;

	g_Parts[1].tbl_adr = m_body;	// 再生するアニメデータの先頭アドレスをセット
	g_Parts[1].tbl_size = sizeof(m_body) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[1].load = 1;

	g_Parts[2].tbl_adr = m_R_hand;	// 再生するアニメデータの先頭アドレスをセット
	g_Parts[2].tbl_size = sizeof(m_R_hand) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[2].load = 1;

	g_Parts[3].tbl_adr = m_R_reg;	// 再生するアニメデータの先頭アドレスをセット
	g_Parts[3].tbl_size = sizeof(m_R_reg) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[3].load = 1;

	g_Parts[4].tbl_adr = m_L_hand;	// 再生するアニメデータの先頭アドレスをセット
	g_Parts[4].tbl_size = sizeof(m_L_hand) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[4].load = 1;

	g_Parts[5].tbl_adr = m_L_reg;	// 再生するアニメデータの先頭アドレスをセット
	g_Parts[5].tbl_size = sizeof(m_L_reg) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[5].load = 1;

	g_Parts[6].tbl_adr = m_stick;	// 再生するアニメデータの先頭アドレスをセット
	g_Parts[6].tbl_size = sizeof(m_stick) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[6].load = 1;

	g_Parts[7].tbl_adr = m_nabe;	// 再生するアニメデータの先頭アドレスをセット
	g_Parts[7].tbl_size = sizeof(m_nabe) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[7].load = 1;


	for (int i = 0; i < 8; i++)
	{
		g_Parts[i].move_time = 0.0f;
		g_Parts[6].use = FALSE;
	}


}

