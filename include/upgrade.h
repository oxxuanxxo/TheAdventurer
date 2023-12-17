/*!*************************************************************************
****
\file   upgrade.h
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Apr 4 2021

\brief
This header file outlines an implementation of the game state function 
pointers for the upgrade system UI.

The functions include:
- upgrade_init
- upgrade_load
- upgrade_update
- upgrade_draw
- upgrade_free
- upgrade_unload

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/
#pragma once

void upgrade_init();
void upgrade_load();
void upgrade_update();
void upgrade_draw();
void upgrade_free();
void upgrade_unload();