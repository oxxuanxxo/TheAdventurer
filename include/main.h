/*!*************************************************************************
****
\file   main.h
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Mar 25 2021

\brief
This header file outlines all the includes that is required in the game.

The functions include:
- level2_load
- level2_initialize
- level2_update
- level2_draw
- level2_free
- level2_unload

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/
#pragma once

#include <math.h>
#include <algorithm>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include "AEEngine.h"
#include "fmod.hpp"

// I'm sorry I can't let this spelling mistake go
#define AEToggleFullScreen AEToogleFullScreen

#include "game_enums.h"
#include "game_states.h"
#include "game_state_manager.h"
#include "globals.h"
#include "audio.h"
#include "timer.h"
#include "graphics.h"
#include "animation.h"
#include "collision.h"
#include "gameobject.h"
#include "button.h"
#include "panel.h"
#include "transition.h"
#include "options.h"
