/*!*************************************************************************
****
\file   tutorial.cpp
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Jan 24 2021

\brief
This program is an implementation of the tutorial.

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

#include "level_manager.h"
#include "traps.h"
#include "binarymap.h"
#include "pause.h"

static const s8* level_path = "./Level/tut.json";
static const s8* binary_path = "./Level/tut_plats.txt";
constexpr f32 BG_EPSILON = 0.01f;
static Level* level;
static BinaryMap* bmap;
static AEGfxTexture* instructions_tex_1; // movement
static AEGfxTexture* instructions_tex_2; // airdash
static AEGfxTexture* instructions_tex_3; // combos
static AEGfxTexture* instructions_tex_4; // enemy indicator
static AEGfxTexture* instructions_tex_5; // shield_parry
static AEGfxTexture* instructions_tex_6; // parry_tip
static AEGfxTexture* instructions_tex_7; // spear combos
static AEGfxTexture* instructions_tex_8; // spear throw
static AEGfxTexture* instructions_tex_9; // congrats_u_finished
static AEGfxTexture* exit_tex; // exit door
static AEVec2 tut_size_1;
static AEVec2 tut_size_2;
static AEVec2 tut_size_3;
static AEVec2 tut_size_4;
static AEVec2 tut_size_5;
static AEVec2 tut_size_6;
static AEVec2 tut_size_7;
static AEVec2 tut_size_8;
static AEVec2 tut_size_9;
static AEVec2 door_size;

/******************************************************************************
 * @brief Load function of the tutorial game state
 * Loads the level and binary map as well as multiple textures for the 
 * instructions that will pop up in the level.
 * 
******************************************************************************/
void tutorial_load()
{
	AEGfxSetBackgroundColor(0.77f, 0.658f, 0.501f);
	level->boss = new Boss1;
	level = new Level(level_path);
	bmap = new BinaryMap(binary_path);
	instructions_tex_1 = AEGfxTextureLoad("./UI/Tutorial/Movement.png");
	instructions_tex_2 = AEGfxTextureLoad("./UI/Tutorial/Airdash.png");
	instructions_tex_3 = AEGfxTextureLoad("./UI/Tutorial/Combo.png");
	instructions_tex_4 = AEGfxTextureLoad("./UI/Tutorial/Enemy.png");
	instructions_tex_5 = AEGfxTextureLoad("./UI/Tutorial/Shielding.png");
	instructions_tex_6 = AEGfxTextureLoad("./UI/Tutorial/Parry_Tip.png");
	instructions_tex_7 = AEGfxTextureLoad("./UI/Tutorial/spear_combo.png");
	instructions_tex_8 = AEGfxTextureLoad("./UI/Tutorial/Spear_Throw.png");
	instructions_tex_9 = AEGfxTextureLoad("./UI/Tutorial/End_Tutorial.png");
	exit_tex = AEGfxTextureLoad("./Sprites/Environment/Door_open.png");
	tut_size_1 = read_png_size("./UI/Tutorial/Movement.png");
	tut_size_2 = read_png_size("./UI/Tutorial/Airdash.png");
	tut_size_3 = read_png_size("./UI/Tutorial/Combo.png");
	tut_size_4 = read_png_size("./UI/Tutorial/Enemy.png");
	tut_size_5 = read_png_size("./UI/Tutorial/Shielding.png");
	tut_size_6 = read_png_size("./UI/Tutorial/Parry_Tip.png");
	tut_size_7 = read_png_size("./UI/Tutorial/spear_combo.png");
	tut_size_8 = read_png_size("./UI/Tutorial/Spear_Throw.png");
	tut_size_9 = read_png_size("./UI/Tutorial/End_Tutorial.png");
	door_size = read_png_size("./Sprites/Environment/Door_open.png");
	AEVec2Scale(&tut_size_3, &tut_size_3, 0.85f);
	AEVec2Scale(&tut_size_4, &tut_size_4, 0.5f);
	load_boss_assets();
}

/******************************************************************************
 * @brief Initialize function of the tutorial game state
 * Load all the platforms from the binarymap and initialize the level. 
 * Afterwards, sort the vector of game objects by layer for proper layer 
 * drawing.
 * 
******************************************************************************/
void tutorial_initialize()
{
	bmap->load_platforms(level);
	level->init();
	std::sort(level->v_gameObjects.begin(), level->v_gameObjects.end(), cmp_layer);
}

/******************************************************************************
 * @brief Update function of the tutorial game state
 * Updates the level and its objects as well as updating the collisions. 
 * In addition, if the player parries in the area 1 (parry tutorial), 
 * clear the area. If the player's position is past 6000 (the exit door), win 
 * the level. Accomodate for pause as well.
 * 
******************************************************************************/
void tutorial_update()
{
	if (!pause)
	{
		level->level_cam.update();
		level->update();
		level_objects_update(level->v_gameObjects);
		collision_update(level->v_gameObjects);
		if (level->current_area == 1 && level->player->parry)
		{
			level->clear_area(1);
		}
		if (level->player->curr_pos.x >= 6000)
			level->win_level();
	}
	else
		pause_update();
}

/******************************************************************************
 * @brief Draw function of the tutorial game state.
 * Draw the instructions based on the current area. Then, draw all the game 
 * objects.
 * 
******************************************************************************/
void tutorial_draw()
{
	AEVec2 instructions_pos_1 = { 600, 400 };
	AEVec2 instructions_pos_2 = { 1500, 400 };
	AEVec2 instructions_pos_3 = { 2300, 400 };
	AEVec2 instructions_pos_4 = { 3000, 200 };
	AEVec2 instructions_pos_5 = { 2200, 400 };
	AEVec2 instructions_pos_6 = { 2900, 450 };
	AEVec2 instructions_pos_7 = { 2900, 450 };
	AEVec2 instructions_pos_8 = { 4350, 450 };
	AEVec2 instructions_pos_9 = { 5400, 450 };
	AEVec2 exit_pos = { 6000, 200 };
	Color default_tint = { 1.0f,1.0f,1.0f,1.0f };
	switch (level->current_area)
	{
	case 0:
	{
		draw(instructions_tex_1, level->m_meshes.at("Square"), instructions_pos_1, {}, default_tint, 1.0f, tut_size_1, 0.0f);
		draw(instructions_tex_2, level->m_meshes.at("Square"), instructions_pos_2, {}, default_tint, 1.0f, tut_size_2, 0.0f);
		draw(instructions_tex_3, level->m_meshes.at("Square"), instructions_pos_3, {}, default_tint, 1.0f, tut_size_3, 0.0f);
		draw(instructions_tex_4, level->m_meshes.at("Square"), instructions_pos_4, {}, default_tint, 1.0f, tut_size_4, 0.0f);
		break;
	}
	case 1:
	{
		draw(instructions_tex_5, level->m_meshes.at("Square"), instructions_pos_5, {}, default_tint, 1.0f, tut_size_5, 0.0f);
		draw(instructions_tex_6, level->m_meshes.at("Square"), instructions_pos_6, {}, default_tint, 1.0f, tut_size_6, 0.0f);
		break;
	}
	case 2:
	{
		draw(instructions_tex_7, level->m_meshes.at("Square"), instructions_pos_7, {}, default_tint, 1.0f, tut_size_7, 0.0f);
		break;
	}
	case 3:
	{
		draw(instructions_tex_8, level->m_meshes.at("Square"), instructions_pos_8, {}, default_tint, 1.0f, tut_size_8, 0.0f);
		break;
	}
	default:
	{
		draw(instructions_tex_9, level->m_meshes.at("Square"), instructions_pos_9, {}, default_tint, 1.0f, tut_size_9, 0.0f);
		draw(exit_tex, level->m_meshes.at("Square"), exit_pos, {}, default_tint, 1.0f, door_size, 0.0f);
		break;
	}
	}
	objects_draw(level->v_gameObjects);
	level->draw();
}

/******************************************************************************
 * @brief Free function of the tutorial game state
 * Frees the level
 * 
******************************************************************************/
void tutorial_free()
{
	level->free();
}

/******************************************************************************
 * @brief Unload function of the tutorial game state
 * Unloads all textures, boss assets, level and binary map loaded from the 
 * load function
 * 
******************************************************************************/
void tutorial_unload()
{
	AEGfxTextureUnload(instructions_tex_1);
	AEGfxTextureUnload(instructions_tex_2);
	AEGfxTextureUnload(instructions_tex_3);
	AEGfxTextureUnload(instructions_tex_4);
	AEGfxTextureUnload(instructions_tex_5);
	AEGfxTextureUnload(instructions_tex_6);
	AEGfxTextureUnload(instructions_tex_7);
	AEGfxTextureUnload(instructions_tex_8);
	AEGfxTextureUnload(instructions_tex_9);
	AEGfxTextureUnload(exit_tex);
	unload_boss_assets();
	delete level;
	delete bmap;
}