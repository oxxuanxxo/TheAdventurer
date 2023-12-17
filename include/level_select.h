/*!*****************************************************************************
\file   level_select.h
\author Chua Yip Xuan
\par    DP email: yipxuan.chua\@digipen.edu
\date   Mar 28 2021

\brief
	This header file contains the functions implemented in level_select.cpp

The header file has functions that consists of:
	- level_select_load
	- level_select_initialize
	- level_select_update
	- level_select_draw
	- level_select_free
	- level_select_unload

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************************/

#pragma once
#include "main.h"
#include "gameobject.h"

/*!*****************************************************************************
  \brief
	Loads all assets needed for the level select.
*******************************************************************************/
void level_select_load();
/*!*****************************************************************************
  \brief
	Initialize the buttons needed for level select.
*******************************************************************************/
void level_select_initialize();
/*!*****************************************************************************
  \brief
	Update level select buttons.
*******************************************************************************/
void level_select_update();
/*!*****************************************************************************
  \brief
	Draw the level select background and buttons.
*******************************************************************************/
void level_select_draw();
/*!*****************************************************************************
  \brief
	Free up all the buttons.
*******************************************************************************/
void level_select_free();
/*!*****************************************************************************
  \brief
	Unload all sprites and textures and assets used to implement level select.
*******************************************************************************/
void level_select_unload();