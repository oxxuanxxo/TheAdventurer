/*!*****************************************************************************
\file   win.h
\author Siti Nursafinah Binte Sumsuri
\par    DP email: sitinursafinah.b\@digipen.edu
\date   Mar 4 2021

\brief
This header file outlines gameover structure and how it is drawn.

The functions include:
- win_load
- win_initialize
- win_update
- win_draw
- win_free
- win_unload

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************************/

#pragma once

#include "main.h"
#include "gameobject.h"

void win_load();
void win_initialize();
void win_update();
void win_draw();
void win_free();
void win_unload();