/*!*************************************************************************
****
\file   game_states.h
\author Cruz Rolly Matthew Capiral , Siti Nursafinah Binte Sumsuri
\par    DP email: cruzrolly.m\@digipen.edu
\date   Jan 25 2021

\brief
This header file outlines the game states in the game.

The functions include:
- gsm_init
- gsm_update

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/
#pragma once

enum GS_STATES
{
	GS_STARTUP,
	GS_MAINMENU, //saf
	GS_LEVELSELECT,
	GS_UPGRADE,
	GS_TUTORIAL,
	GS_LEVEL1,
	GS_LEVEL2,
	GS_LEVEL3,
	GS_TEST,

	GS_PAUSE,

	GS_WORLD_MAP,

	GS_WIN, //saf
	GS_GAMEOVER, // saf
	GS_RESTART,
	GS_CREDITS, //saf
	GS_QUIT,
};