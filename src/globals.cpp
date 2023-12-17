/*!*************************************************************************
****
\file   globals.cpp
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Mar 2 2021

\brief
This file defines the global variables declared in the game.

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/
#include "globals.h"

f32 G_DELTATIME = 0;
f32 G_DT = 0;
f32 LEVEL_GRAVITY = -50.0f;
s8 default_font = 0;
s8 bold_font = 0;
s8 desc_font = 0;
s32 mouse_x, mouse_y;
s32 bgm_vol;
s32 sfx_vol;
bool is_fullscreen;
bool god_mode = false;
bool pause = false;