/*!*************************************************************************
****
\file   options.h
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Apr 6 2021

\brief
This header file outlines an implementation of the options menu in a
game engine.

The functions include:
- options_load
- options_init
- options_update
- options_draw
- options_free
- options_unload
- open_options
- get_options_flag

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/
#pragma once
#include "panel.h"

void options_load();
void options_init();
void options_update();
void options_draw();
void options_free();
void options_unload();

void open_options();
bool get_options_flag();