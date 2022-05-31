//=============================================================================
//
// ゲーム画面処理 [game.cpp]
// Author : 熊澤義弘
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "camera.h"
#include "input.h"
#include "sound.h"
#include "fade.h"
#include "game.h"

#include "player.h"
#include "enemy.h"
#include "attackRange.h"
#include "bom.h"
#include "blast.h"
#include "meshfield.h"
#include "meshfield2.h"
#include "meshwall.h"
#include "shadow.h"
#include "fieldobj.h"
#include "sky.h"
#include "bullet.h"
#include "score.h"
#include "particle.h"
#include "orbit.h"
#include "collision.h"
#include "debugproc.h"
#include "timeUI.h"
#include "damageEF.h"
#include "enemyHeli.h"
#include "combo.h"
#include "playerHP.h"
#include "gameUI.h"
#include "speech.h"
#include "tutorial.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void CheckHit(void);



//*****************************************************************************
// グローバル変数
//*****************************************************************************
static int	g_ViewPortType_Game = TYPE_FULL_SCREEN;

static BOOL	g_bPause = TRUE;	// ポーズON/OFF

static int					g_Stage;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGame(void)
{
	g_ViewPortType_Game = TYPE_FULL_SCREEN;

	// フィールドの初期化
	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 1, 7, 700.0f, 400.0f);
	//InitMeshField(XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 1, 1, 5000.0f, 5000.0f);

	InitMeshField2(XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 1, 2, 2000.0f, 2000.0f);

	// ライトを有効化	// 影の初期化処理
	InitShadow();

	// プレイヤーの初期化
	InitPlayer();

	// エネミーの初期化
	InitEnemy();

	// エネミーヘリの初期化
	InitEnemyHeli();

	// 攻撃範囲の初期化
	InitAttackR();

	// ダイナマイトの初期化
	InitBom();

	// 爆破オブジェクトの初期化
	InitBlast();

	//// 壁の初期化
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f,  XM_PI, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);

	//// 壁(裏側用の半透明)
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f,    XM_PI, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f,   XM_PI * 0.50f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);

	// 木を生やす
	InitTree();

	// スカイドームの初期化
	InitSky();

	// 弾の初期化
	InitBullet();

	// スコアの初期化
	InitScore();

	// コンボの初期化
	InitCombo();

	// UIの初期化
	InitGameUI();

	// 制限時間の初期化
	InitTime();

	// プレイヤーのHPの初期化
	InitPlayerHP();

	// パーティクルの初期化
	InitParticle();

	// 軌跡の初期化
	InitOrbit();

	// ダメージエフェクトの初期化
	InitDamageEF();

	// 吹き出しの初期化
	InitSpeech();

	// チュートリアルの初期化
	InitTutorial();

	g_Stage = GetStage();

	// 連続再生のSEを再生
	PlaySound(SOUND_LABEL_SE_stirringSound01);


	switch (g_Stage)
	{
	case tutorial:
		// BGM再生
		PlaySound(SOUND_LABEL_BGM_bgm_stage002);
		break;

	case stage0:
	case stage1:
	case stage2:
	case stage3:
		// BGM再生
		PlaySound(SOUND_LABEL_BGM_bgm_stage002);
		break;
	}


	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGame(void)
{
	// チュートリアルの終了処理
	UninitTutorial();

	// 吹き出しの終了処理
	UninitSpeech();

	// ダメージエフェクトの終了処理
	UninitDamageEF();

	// 軌跡の終了処理
	UninitOrbit();

	// パーティクルの終了処理
	UninitParticle();

	// プレイヤーのHPの終了処理
	UninitPlayerHP();

	// 時間の終了処理
	UninitTime();

	// UIの終了処理
	UninitGameUI();

	// コンボの終了処理
	UninitCombo();

	// スコアの終了処理
	UninitScore();

	// 弾の終了処理
	UninitBullet();

	// スカイドームの終了処理
	UninitSky();

	// 木の終了処理
	UninitTree();

	// 壁の終了処理
	UninitMeshWall();

	// 地面の終了処理
	UninitMeshField2();
	UninitMeshField();

	// 爆破オブジェクトの終了処理
	UninitBlast();

	// ダイナマイトの終了処理
	UninitBom();

	// 攻撃範囲の終了処理
	UninitAttackR();

	// エネミーヘリの終了処理
	UninitEnemyHeli();

	// エネミーの終了処理
	UninitEnemy();

	// プレイヤーの終了処理
	UninitPlayer();

	// 影の終了処理
	UninitShadow();

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateGame(void)
{
#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_V))
	{
		g_ViewPortType_Game = (g_ViewPortType_Game + 1) % TYPE_NONE;
		SetViewPort(g_ViewPortType_Game);
	}

	if (GetKeyboardTrigger(DIK_P))
	{
		g_bPause = g_bPause ? FALSE : TRUE;
	}


#endif

	if(g_bPause == FALSE)
		return;

	// 地面処理の更新
	UpdateMeshField();
	UpdateMeshField2();

	// プレイヤーの更新処理
	UpdatePlayer();

	// エネミーの更新処理
	UpdateEnemy();

	// エネミーヘリの更新処理
	UpdateEnemyHeli();

	// 攻撃範囲の更新処理
	UpdateAttackR();

	// ダイナマイトの更新処理
	UpdateBom();

	// 爆破オブジェクトの更新処理
	UpdateBlast();

	// 壁処理の更新
	UpdateMeshWall();

	// 木の更新処理
	UpdateTree();

	// スカイドームの更新処理
	UpdateSky();

	// 弾の更新処理
	UpdateBullet();

	// パーティクルの更新処理
	UpdateParticle();

	// 軌跡の更新処理
	UpdateOrbit();

	// 影の更新処理
	UpdateShadow();

	// 当たり判定処理
	CheckHit();

	// スコアの更新処理
	UpdateScore();

	// コンボの更新処理
	UpdateCombo();

	// UIの更新処理
	UpdateGameUI();

	// 時間の更新処理
	UpdateTime();

	// プレイヤーのHPの更新処理
	UpdatePlayerHP();

	// ダメージエフェクトの更新処理
	UpdateDamageEF();

	// 吹き出しの更新処理
	UpdateSpeech();

	// チュートリアルの更新処理
	UpdateTutorial();
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGame0(void)
{
	// 3Dの物を描画する処理
	// 地面の描画処理
	DrawMeshField();
	DrawMeshField2();

	// 影の描画処理
	DrawShadow();

	// エネミーヘリの描画処理
	DrawEnemyHeli();

	// エネミーの描画処理
	DrawEnemy();

	// プレイヤーの描画処理
	DrawPlayer();

	// ダイナマイトの描画処理
	DrawBom();

	// 爆破オブジェクトの描画処理
	DrawBlast();

	// 弾の描画処理
	DrawBullet();

	// 壁の描画処理
	DrawMeshWall();

	// 木の描画処理
	DrawTree();

	// スカイドームの描画処理
	DrawSky();

	// パーティクルの描画処理
	DrawParticle();

	// 軌跡の描画処理
	DrawOrbit();

	// 攻撃範囲の描画処理
	DrawAttackR();



	// 2Dの物を描画する処理
	// Z比較なし
	SetDepthEnable(FALSE);

	// ライティングを無効
	SetLightEnable(FALSE);

	// スコアの描画処理
	DrawScore();

	// コンボの描画処理
	DrawCombo();

	// 時間の描画処理
	DrawTime();

	// プレイヤーのHPの描画処理
	DrawPlayerHP();

	// ダメージエフェクトの描画処理
	DrawDamageEF();

	// UIの描画処理
	DrawGameUI();

	// 吹き出しの描画処理
	DrawSpeech();

	// チュートリアルの描画処理
	DrawTutorial();


	// ライティングを有効に
	SetLightEnable(TRUE);

	// Z比較あり
	SetDepthEnable(TRUE);
}


void DrawGame(void)
{
	XMFLOAT3 pos;


#ifdef _DEBUG
	// デバッグ表示
	PrintDebugProc("ViewPortType:%d\n", g_ViewPortType_Game);

#endif

	// プレイヤー視点
	pos = GetPlayer()->pos;
	pos.y = 0.0f;			// カメラ酔いを防ぐためにクリアしている
	SetCameraAT(pos);
	SetCamera();

	switch(g_ViewPortType_Game)
	{
	case TYPE_FULL_SCREEN:
		SetViewPort(TYPE_FULL_SCREEN);
		DrawGame0();
		break;

	case TYPE_LEFT_HALF_SCREEN:
	case TYPE_RIGHT_HALF_SCREEN:
		SetViewPort(TYPE_LEFT_HALF_SCREEN);
		DrawGame0();

		// エネミー視点
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_RIGHT_HALF_SCREEN);
		DrawGame0();
		break;

	case TYPE_UP_HALF_SCREEN:
	case TYPE_DOWN_HALF_SCREEN:
		SetViewPort(TYPE_UP_HALF_SCREEN);
		DrawGame0();

		// エネミー視点
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_DOWN_HALF_SCREEN);
		DrawGame0();
		break;

	}
}


//=============================================================================
// 当たり判定処理
//=============================================================================
void CheckHit(void)
{
	ENEMY *enemy = GetEnemy();		// エネミーのポインターを初期化
	ENEMY_HELI *enemyHeli = GetEnemyHeli();		// エネミーのポインターを初期化

	PLAYER *player = GetPlayer();	// プレイヤーのポインターを初期化
	BULLET *bullet = GetBullet();	// 弾のポインターを初期化
	BLAST *blast = GetBlast();		// 爆破オブジェクトの初期化

	float offsetX;
	float offsetY;
	float offsetZ;

	// 敵と爆破オブジェクト
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		//敵の有効フラグをチェックする
		if (enemy[i].use == FALSE || enemy[i].isHit == TRUE)
			continue;


		for (int p = 0; p < MAX_BLAST; p++)
		{
			//爆破オブジェクトの有効フラグをチェックする
			if (blast[p].use == FALSE)
				continue;

			//BCの当たり判定
			float size = blast[p].size;
			if (GetMorphing() == 1) size /= 4.0f;

			if (CollisionBC(blast[p].pos, enemy[i].pos, size, enemy[i].size))
			{
				if (enemy[i].isHit == TRUE) break;

				// 敵キャラクターは倒される
				enemy[i].isHit = TRUE;
				enemy[i].hitTime = 15;

				offsetX = RamdomFloat(0, 20.0f, -20.0f);
				offsetY = RamdomFloat(0, 20.0f, ENEMY_OFFSET_Y);
				offsetZ = RamdomFloat(0, -10.0f, -20.0f);

				enemy[i].hitPos.x = blast[p].pos.x + offsetX;
				enemy[i].hitPos.y = blast[p].pos.y + offsetY;
				enemy[i].hitPos.z = blast[p].pos.z + offsetZ;

				ReleaseShadow(enemy[i].shadowIdx);

				// スコアを足す
				AddScore(100);

				// コンボを足す
				AddCombo(1);
				ResetComboTime();
			}
		}
	}

	// 敵(ヘリ)と爆破オブジェクト
	for (int i = 0; i < MAX_ENEMY_HELI; i++)
	{
		//敵の有効フラグをチェックする
		if (enemyHeli[i].use == FALSE || enemyHeli[i].isHit == TRUE)
			continue;


		for (int p = 0; p < MAX_BLAST; p++)
		{
			//爆破オブジェクトの有効フラグをチェックする
			if (blast[p].use == FALSE)
				continue;

			//BCの当たり判定
			float size = blast[p].size;
			if (GetMorphing() == 1) size /= 4.0f;

			if (CollisionBC(blast[p].pos, enemyHeli[i].pos, size, enemyHeli[i].size))
			{
				if (enemyHeli[i].isHit == TRUE) break;

				// 敵キャラクターは倒される
				enemyHeli[i].isHit = TRUE;
				enemyHeli[i].hitTime = 15;

				offsetX = RamdomFloat(0, 5.0f, -5.0f);
				offsetY = RamdomFloat(0, 5.0f, ENEMY_HELI_OFFSET_Y - 20.0f);
				offsetZ = RamdomFloat(0, 0.0f, -5.0f);

				enemyHeli[i].hitPos.x = blast[p].pos.x + offsetX;
				enemyHeli[i].hitPos.y = blast[p].pos.y + offsetY;
				enemyHeli[i].hitPos.z = blast[p].pos.z + offsetZ;

				ReleaseShadow(enemyHeli[i].shadowIdx);

				// スコアを足す
				AddScore(100);

				// コンボを足す
				AddCombo(1);
				ResetComboTime();
			}
		}
	}


	// プレイヤーのHPが0でゲームオーバー
	// リザルト画面へ遷移
	// 最終的にはゲームオーバー画面からリザルト画面へ遷移
	if (player->hp <= 0)
	{
		SetFade(FADE_OUT, MODE_RESULT);
	}
}


