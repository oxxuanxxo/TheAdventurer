/*!*************************************************************************
****
\file   level2.cpp
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Jan 24 2021

\brief
This program includes the implementation of the level2 game state.

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

#include "level_manager.h"
#include "traps.h"
#include "binarymap.h"
#include "pause.h"

constexpr f32 BG_EPSILON = 0.01f;
static const s8* level_path = "./Level/level2.json";
static const s8* binary_path = "./Level/lvl2_plats.txt";
static const s8* bg_path = "./Background/lava.png";
static Level* level;
static bool fullscreen = false;
static BinaryMap* bmap;
static AEGfxTexture* background;
static AEVec2 bg_size;
static AEVec2 bg_pos;
static AEVec2 bg_scaled_size;

/******************************************************************************
 * @brief Load function of the level2 game state
 * Loads the level, binary map, and background specified for the level.
 * 
******************************************************************************/
void level2_load()
{
	AEGfxSetBackgroundColor(0.77f, 0.658f, 0.501f);
	level->boss = new Boss2;
	level = new Level(level_path);
	bmap = new BinaryMap(binary_path);
	background = AEGfxTextureLoad(bg_path);
	bg_size = read_png_size(bg_path);
	bg_size.x /= 1.3f;
	bg_size.y /= 2.0f;
	load_boss_assets();
}

/******************************************************************************
 * @brief Initialize function of the level2 game state
 * Load all the platforms from the binarymap and initialize the level. 
 * Afterwards, sort the vector of game objects by layer for proper layer 
 * drawing.
 * 
******************************************************************************/
void level2_initialize()
{
	bmap->load_platforms(level);
	level->init();
	std::sort(level->v_gameObjects.begin(), level->v_gameObjects.end(), cmp_layer);
}

/******************************************************************************
 * @brief Update function of the level2 game state
 * Updates the level and its objects as well as updating the collisions and 
 * background position. Accomodate for pause as well.
 * 
******************************************************************************/
void level2_update()
{
	if (!pause)
	{
		level->level_cam.update();
		level->update();
		level_objects_update(level->v_gameObjects);
		collision_update(level->v_gameObjects);
		AEVec2 default_pos = level->level_cam.get_curr_pos();
		bg_pos = default_pos;
	}

	else
		pause_update();
}

/******************************************************************************
 * @brief Draw function of the level2 game state.
 * Draw the background, then draw all the game objects.
 * 
******************************************************************************/
void level2_draw()
{
	draw(background, level->m_meshes.at("Square"), bg_pos, {}, { 1.0f,1.0f,1.0f,1.0f }, 1.0f, bg_size, 0.0f);
	objects_draw(level->v_gameObjects);
	level->draw();
}

/******************************************************************************
 * @brief Free function of the level2 game state
 * Frees the level
 * 
******************************************************************************/
void level2_free()
{
	level->free();
}

/******************************************************************************
 * @brief Unload function of the level2 game state
 * Unloads background textures, boss assets, level and binary map loaded from 
 * the load function
 * 
******************************************************************************/
void level2_unload()
{
	AEGfxTextureUnload(background);
	unload_boss_assets();
	delete level;
	delete bmap;
}