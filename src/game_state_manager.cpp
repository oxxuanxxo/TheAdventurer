/*!*************************************************************************
****
\file   game_state_manager.cpp
\author Cruz Rolly Matthew Capiral
		Siti Nursafinah Binte Sumsuri (24 lines)
\par    DP email: cruzrolly.m\@digipen.edu
				  sitinursafinah.b\@digipen.edu
\date   Jan 25 2021

\brief
This file includes the implementation of the game state manager

The functions include:
- gsm_init
- gsm_update

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/
#include "game_states.h"
#include "game_state_manager.h"
#include "mainmenu.h"
#include "tutorial.h"
#include "level1.h"
#include "level2.h"
#include "level3.h"
#include "upgrade.h"
#include "gameover.h"
#include "level_select.h"
#include "win.h"
#include "credits.h"
#include "digipen.h"

u32 gAEGameStateCurr = 0, gAEGameStatePrev = 0, gAEGameStateNext = 0;

void (*AEGameStateLoad)(void) = nullptr;    
void (*AEGameStateInit)(void) = nullptr;    
void (*AEGameStateUpdate)(void) = nullptr;  
void (*AEGameStateDraw)(void) = nullptr;    
void (*AEGameStateFree)(void) = nullptr;    
void (*AEGameStateUnload)(void) = nullptr;  

/*!****************************************************************************
 * @brief Initialize the game state manager
 * 
 * @param startingState 
 * Initial state of the game state
******************************************************************************/
void gsm_init(u32 startingState)
{
	AEGameStateMgrAdd(GS_UPGRADE, upgrade_load, upgrade_init, upgrade_update, upgrade_draw, upgrade_free, upgrade_unload);
	AEGameStateMgrAdd(GS_MAINMENU, mainmenu_load, mainmenu_initialize, mainmenu_update, mainmenu_draw, mainmenu_free, mainmenu_unload);
	AEGameStateMgrAdd(GS_TUTORIAL, tutorial_load, tutorial_initialize, tutorial_update, tutorial_draw, tutorial_free, tutorial_unload);
	AEGameStateMgrAdd(GS_LEVEL1, level1_load, level1_initialize, level1_update, level1_draw, level1_free, level1_unload);
	AEGameStateMgrAdd(GS_LEVEL2, level2_load, level2_initialize, level2_update, level2_draw, level2_free, level2_unload);
	AEGameStateMgrAdd(GS_LEVEL3, level3_load, level3_initialize, level3_update, level3_draw, level3_free, level3_unload);
	AEGameStateMgrAdd(GS_LEVELSELECT, level_select_load, level_select_initialize, level_select_update, level_select_draw, level_select_free, level_select_unload);
	AEGameStateMgrAdd(GS_WIN, win_load, win_initialize, win_update, win_draw, win_free, win_unload);
	AEGameStateMgrAdd(GS_CREDITS, credits_load, credits_initialize, credits_update, credits_draw, credits_free, credits_unload);
	AEGameStateMgrAdd(GS_STARTUP, digipen_load, digipen_initialize, digipen_update, digipen_draw, digipen_free, digipen_unload);
	AEGameStateMgrInit(startingState);
}

/*!****************************************************************************
 * @brief Updates the game state manager
 * 
******************************************************************************/
void gsm_update()
{
	AEGameStateMgrUpdate();

	switch (gAEGameStateNext)
	{
	case GS_MAINMENU:
		AEGameStateLoad = mainmenu_load;
		AEGameStateInit = mainmenu_initialize;  
		AEGameStateUpdate = mainmenu_update;  
		AEGameStateDraw = mainmenu_draw;  
		AEGameStateFree = mainmenu_free;  
		AEGameStateUnload = mainmenu_unload;  
		break;

	case GS_LEVELSELECT:
		AEGameStateLoad = level_select_load;
		AEGameStateInit = level_select_initialize;  
		AEGameStateUpdate = level_select_update;  
		AEGameStateDraw = level_select_draw;  
		AEGameStateFree = level_select_free;  
		AEGameStateUnload = level_select_unload;  
		break;

	case GS_UPGRADE:
		AEGameStateLoad = upgrade_load;
		AEGameStateInit = upgrade_init;  
		AEGameStateUpdate = upgrade_update;  
		AEGameStateDraw = upgrade_draw;  
		AEGameStateFree = upgrade_free;  
		AEGameStateUnload = upgrade_unload;  
		break;

	case GS_TUTORIAL:
		AEGameStateLoad = tutorial_load;
		AEGameStateInit = tutorial_initialize;  
		AEGameStateUpdate = tutorial_update;  
		AEGameStateDraw = tutorial_draw;  
		AEGameStateFree = tutorial_free;  
		AEGameStateUnload = tutorial_unload;  
		break;
	case GS_LEVEL1:
		AEGameStateLoad = level1_load;
		AEGameStateInit = level1_initialize;  
		AEGameStateUpdate = level1_update;  
		AEGameStateDraw = level1_draw;  
		AEGameStateFree = level1_free;  
		AEGameStateUnload = level1_unload;  
		break;
	case GS_LEVEL2:
		AEGameStateLoad = level2_load;
		AEGameStateInit = level2_initialize;  
		AEGameStateUpdate = level2_update;  
		AEGameStateDraw = level2_draw;  
		AEGameStateFree = level2_free;  
		AEGameStateUnload = level2_unload;  
		break;
	case GS_LEVEL3:
		AEGameStateLoad = level3_load;
		AEGameStateInit = level3_initialize;  
		AEGameStateUpdate = level3_update;  
		AEGameStateDraw = level3_draw;  
		AEGameStateFree = level3_free;  
		AEGameStateUnload = level3_unload;  
		break;
	case GS_GAMEOVER:
		AEGameStateLoad = gameover_load;
		AEGameStateInit = gameover_initialize;  
		AEGameStateUpdate = gameover_update;  
		AEGameStateDraw = gameover_draw;  
		AEGameStateFree = gameover_free;  
		AEGameStateUnload = gameover_unload;  
		break;
	case GS_WIN:
		AEGameStateLoad = win_load;
		AEGameStateInit = win_initialize;  
		AEGameStateUpdate = win_update;  
		AEGameStateDraw = win_draw;  
		AEGameStateFree = win_free;  
		AEGameStateUnload = win_unload;  
		break;
	case GS_CREDITS:
		AEGameStateLoad = credits_load;
		AEGameStateInit = credits_initialize;  
		AEGameStateUpdate = credits_update;  
		AEGameStateDraw = credits_draw;  
		AEGameStateFree = credits_free;  
		AEGameStateUnload = credits_unload;  
		break;

	case GS_STARTUP:
		AEGameStateLoad = digipen_load;
		AEGameStateInit = digipen_initialize;  
		AEGameStateUpdate = digipen_update;  
		AEGameStateDraw = digipen_draw;  
		AEGameStateFree = digipen_free;  
		AEGameStateUnload = digipen_unload;  
		break;
	}
}