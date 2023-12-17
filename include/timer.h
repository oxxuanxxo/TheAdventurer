/*!*************************************************************************
****
\file   timer.h
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Mar 4 2021

\brief
This header file outlines an implementation of the time scaling system in 
the game engine.

The functions include:
- time_load
- time_initialize
- time_update
- time_free
- set_time_multi
- get_time_multi
- temp_scale_timer

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/
#pragma once
#include "AEEngine.h"
#include "globals.h"

void time_load(void);

void time_initialize(void);

void time_update(void);

void time_free(void);

void set_time_multi(f32 multi);

f32 get_time_multi(void);

void temp_scale_timer(f32 time_scale, f32 duration);