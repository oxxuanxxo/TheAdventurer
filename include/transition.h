/*!*****************************************************************************
\file   transiion.h
\author Mohammad Hanif Koh Teck Wee
\par    DP email: mohammadhanifkoh.t\@digipen.edu
\date   April 3 2021

\brief
This header file outlines an implementation of the transition function used
when transiting between pages or gamestates.

The functions include:
- load_transition_assets
- unload_transition_assets
- transition
- transition
- stop_transition
- lerp

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************************/
#pragma once
#include "globals.h"
void load_transition_assets();
void unload_transition_assets();
void transition();
void transition(f32 duration, u32 gamestate);
void stop_transition();
float lerp(float flGoal, float flCurrent, float dt);