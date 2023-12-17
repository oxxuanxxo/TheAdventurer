/*!*************************************************************************
****
\file   globals.h
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Mar 2 2021

\brief
This header file features the global variables declared in the game.

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/
#pragma once
#include "AEEngine.h"
// Manipulated delta time
extern f32 G_DELTATIME;

// Gravity of the level
extern f32 LEVEL_GRAVITY;

// Font
extern s8 default_font;
extern s8 bold_font;
extern s8 desc_font;

extern f32 G_DT;

// Mouse coordinates
extern s32 mouse_x;
extern s32 mouse_y;

// Transition Screen
extern bool transitioning;

// Options
extern s32 bgm_vol;
extern s32 sfx_vol;
extern bool is_fullscreen;
extern bool god_mode;
extern bool pause;