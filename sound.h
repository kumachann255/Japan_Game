//=============================================================================
//
// サウンド処理 [sound.h]
//
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"						// サウンド処理で必要

//*****************************************************************************
// サウンドファイル
//*****************************************************************************
enum 
{
	// BGM
	SOUND_LABEL_BGM_sample000,	// BGM0
	SOUND_LABEL_BGM_sample001,	// BGM1
	SOUND_LABEL_BGM_sample002,	// BGM2

	//// SE
	SOUND_LABEL_SE_booingSound01,		// ブーイング音
	SOUND_LABEL_SE_carHorn01,			// 車のクラクション音
	SOUND_LABEL_SE_cheers00,			// 歓声音0
	SOUND_LABEL_SE_cheers01,			// 歓声音1
	SOUND_LABEL_SE_cheers02,			// 歓声音2
	SOUND_LABEL_SE_cheers03,			// 歓声音3
	SOUND_LABEL_SE_cheers04,			// 歓声音4
	SOUND_LABEL_SE_cheers05,			// 歓声音5
	SOUND_LABEL_SE_collisionSound01,	// 車の衝突音
	SOUND_LABEL_SE_comboSound01,		// コンボの音（できれば爆弾破裂時の音と交換）
	SOUND_LABEL_SE_countdownSound01,	// 10秒前カウントダウン
	SOUND_LABEL_SE_damagedSound01,		// ダメージ音
	SOUND_LABEL_SE_explosionSound01,	// 粘着物の爆発音
	SOUND_LABEL_SE_gunShot01,			// 銃声1
	SOUND_LABEL_SE_gunShot02,			// 銃声2
	SOUND_LABEL_SE_propellerSound01,	// プロペラ音
	SOUND_LABEL_SE_shrinkingSound01,	// 粘着物の収縮音
	SOUND_LABEL_SE_siren01,				// パトカー音
	SOUND_LABEL_SE_stickingSound01,		// 粘着物の張り付き音
	SOUND_LABEL_SE_stirringSound01,		// 粘着かき混ぜ
	SOUND_LABEL_SE_throwingSound01,		// 粘着物投擲音
	SOUND_LABEL_SE_titleClick01,		// タイトルモード選択音1
	SOUND_LABEL_SE_titleClick02,		// タイトルモード選択音2

	SOUND_LABEL_MAX,	// MAX
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
BOOL InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);
void SetSouceVoice(int label, float volume);

void SetMasterVolume(float volume);
void SetSourceVolume(int label, float volume);
