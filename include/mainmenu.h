/******************************************************************************/
/*!
\file		mainmenu.h
\author 	Cruz Rolly Matthew Capiral
\par    	email: cruzrolly.m@digipen.edu
\date   	February 11, 2021
\brief		This file outlines the game state function pointers for the main menu
			game state.

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#pragma once
#include "main.h"
#include "gameobject.h"

void mainmenu_load();
void mainmenu_initialize();
void mainmenu_update();
void mainmenu_draw();
void mainmenu_free();
void mainmenu_unload();
