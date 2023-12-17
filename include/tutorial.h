/*!*************************************************************************
****
\file   tutorial.h
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Jan 24 2021

\brief
This header file outlines an implementation of the tutorial game state in a
game engine.

The functions include:
- tutorial_load
- tutorial_initialize
- tutorial_update
- tutorial_draw
- tutorial_free
- tutorial_unload

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/
#pragma once
#include "gameobject.h"

void tutorial_load();

void tutorial_initialize();

void tutorial_update();

void tutorial_draw();

void tutorial_free();

void tutorial_unload();
