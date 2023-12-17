/*!*************************************************************************
****
\file   game_state_manager.h
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Jan 25 2021

\brief
This header file outlines an implementation of the game state manager

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

#include "AEEngine.h"

void gsm_init(u32 startingState);

void gsm_update();