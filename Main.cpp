/*!*************************************************************************
****
\file   Main.cpp
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Jan 15 2021

\brief
This file includes the entry point of the game.

The functions include:
- gsm_init
- gsm_update

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/

#include "main.h"
#include "audio.h"
#include "player_data.h"
#include "graphics.h"
#include <memory>

static char iconpath[] = "Sprites\\EXE_Icon.ico";
AEVec2 god_mode_pos = { -(1280 / 2), -720 / 2 };
FMOD::System* sound_system;
FMOD::Channel* channel = 0;
FMOD_RESULT fmod_result;

/******************************************************************************
 * @brief Main function of the console application
 * Initializes the Alpha Engine System. Then, sets the icon of the game by
 * getting the application's window handle and sending the message WM_SETICON
 *  on it. AFterwards, initialize the audio system and load all the
 * necessary fonts for the game, transition assets, game states, and option
 * assets.
 *
 * At this point, the game will initialize and be on a constant game loop.
 * Some system such as the time scale, options menu, and audio will update
 * regardless of the game state. God mode and hard capping of frame rate to 60
 *  will be done here as well. Upon quitting, fonts, option menu assets,
 * transition assets, and the audio system will be unloaded before the Alpha
 * Engine system is released, ending the console application.
 *
 * @param hInstance
 * Instance of this console application
 * @param hPrevInstance
 * @param lpCmdLine
 * @param nCmdShow
 * @return int
 *****************************************************************************/
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	//Break point
	//_CrtSetBreakAlloc(983);

	AESysInit(hInstance, nCmdShow, 1280, 720, 0, 60, true, NULL);

	HWND handle = AESysGetWindowHandle();
	HICON hIcon = (HICON)LoadImage((HINSTANCE)GetWindowLongPtr(handle, GWLP_HINSTANCE), (LPCTSTR)iconpath, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
	SendMessage(handle, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	SendMessage(handle, WM_SETICON, (WPARAM)ICON_BIG, (LPARAM)hIcon);

	AudioSystem::instance();

	default_font = AEGfxCreateFont("./Font/walleye.regular.ttf", 32);
	bold_font = AEGfxCreateFont("./Font/walleye.bold.ttf", 32);
	desc_font = AEGfxCreateFont("./Font/walleye.regular.ttf", 20);
	AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);
	AESysSetWindowTitle("The Adventurer");
	load_transition_assets();

	gsm_init(GS_STARTUP);

	options_load();
	AEToggleFullScreen(is_fullscreen);
	while (gAEGameStateCurr != AE_GS_QUIT)
	{
		AESysReset();
		if (gAEGameStateCurr == AE_GS_RESTART)
		{
			gAEGameStateCurr = gAEGameStatePrev;
			gAEGameStateNext = gAEGameStatePrev;
		}
		else
		{
			gsm_update();
			AEGameStateLoad();
			time_load();
		}
		AEGameStateInit();
		time_initialize();
		while (gAEGameStateCurr == gAEGameStateNext)
		{
			AESysFrameStart();

			if (GetFocus() != AESysGetWindowHandle())
			{
				pause = true;
				AudioSystem::instance()->set_pause_group(AUDIO_GROUP::STAGE_BGM, true);
				AudioSystem::instance()->set_pause_group(AUDIO_GROUP::BOSS_BGM, true);
				AudioSystem::instance()->set_pause_group(AUDIO_GROUP::SFX, true);
			}
			AEInputUpdate();
			AEGameStateUpdate();
			time_update();
			options_update();
			AEGameStateDraw();
			options_draw();
			if (god_mode)
				text_print(desc_font, "God Mode On", &god_mode_pos);
			AudioSystem::instance()->update();

			AESysFrameEnd();

			if (AEInputCheckTriggered(AEVK_0))
				god_mode = !god_mode;
			AEInputGetCursorPosition(&mouse_x, &mouse_y);
			if (AESysDoesWindowExist() == false)
				gAEGameStateNext = AE_GS_QUIT;
			G_DT = (f32)AEFrameRateControllerGetFrameTime();
			if (G_DT > 0.01667f)
				G_DT = 0.01667f;
		}
		AEGameStateFree();
		time_free();
		if (gAEGameStateNext != AE_GS_RESTART)
			AEGameStateUnload();
		gAEGameStatePrev = gAEGameStateCurr;
		gAEGameStateCurr = gAEGameStateNext;
	}
	options_unload();
	AEGfxDestroyFont(default_font);
	AEGfxDestroyFont(bold_font);
	AEGfxDestroyFont(desc_font);
	unload_transition_assets();
	AudioSystem::release_instance();
	AESysExit();
}