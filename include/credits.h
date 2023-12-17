/*!*****************************************************************************
\file   credits.h
\author Siti Nursafinah Binte Sumsuri
\par    DP email: sitinursafinah.b\@digipen.edu
\date   Mar 4 2021

\brief
This header file outlines mainmenu structure and how it is drawn.

The functions include:
- credits_load
- credits_initialize
- credits_update
- credits_draw
- credits_free
- credits_unload

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************************/

#pragma once

#include "main.h"
#include "gameobject.h"

void credits_load();
void credits_initialize();
void credits_update();
void credits_draw();
void credits_free();
void credits_unload();