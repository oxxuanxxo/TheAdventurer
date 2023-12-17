/*!*****************************************************************************
\file   win.cpp
\author Siti Nursafinah Binte Sumsuri
\par    DP email: sitinursafinah.b\@digipen.edu
\date   Mar 4 2021

\brief
This file outlines win structure and how it is drawn.

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

#include "mainmenu.h"
#include "main.h"
#include "game_states.h"
#include "camera.h"
#include "player_data.h"
#include "button.h"
#include "camera.h"
#include <iostream>

static AEGfxVertexList* square_mesh = 0;

static AEGfxTexture* wallpaper;
static AEVec2 wallpaperSize = { 1280.0f , 720.0f };
static Sprite* wallpaper_sprite;

static AEGfxTexture* pTex_button_hover;
static AEGfxTexture* pTex_button_normal;
static Sprite* button_sprite_1;
static Sprite* button_sprite_2;
static Sprite* button_sprite_3;
static ButtonManager win_buttons;

static Audio* level_complete_sound;

static void upgrades();
static void level_select();

/*!*****************************************************************************
  \brief
	Function is to load the textures of the buttons and the wallpaper
*******************************************************************************/
void win_load()
{
	wallpaper = AEGfxTextureLoad("./UI/win.png");
	AE_ASSERT_MESG(wallpaper, "Failed to create wallpaper!! \n");

	pTex_button_hover = AEGfxTextureLoad("./UI/button_shade.png");
	AE_ASSERT_MESG(pTex_button_hover, "Failed to create shade button!! \n");

	pTex_button_normal = AEGfxTextureLoad("./UI/button_unshade.png");
	AE_ASSERT_MESG(pTex_button_normal, "Failed to create unshaded button!! \n");

	square_mesh = create_square_mesh(1.0f, 1.0f);

	wallpaper_sprite = new Sprite(wallpaper, square_mesh, wallpaperSize);
	wallpaper_sprite->set_size(read_png_size("./UI/win.png"));

	AEVec2 button_size = { 350, 75 };

	button_sprite_1 = new Sprite(pTex_button_normal, square_mesh, button_size);
	button_sprite_2 = new Sprite(pTex_button_normal, square_mesh, button_size);
	button_sprite_3 = new Sprite(pTex_button_normal, square_mesh, button_size);
	level_complete_sound = new Audio("./Audio/level_complete.wav", AUDIO_GROUP::SFX);
}

/*!*****************************************************************************
  \brief
	Function is to position of the drawn buttons
*******************************************************************************/
void win_initialize()
{
	level_complete_sound->play_sound();

	Button level_select_button({ 0, -100 }, button_sprite_1->scale, button_sprite_1, pTex_button_hover, pTex_button_normal);
	level_select_button.on_click = level_select;
	level_select_button.set_text("Level Select", -80.0f);

	Button upgrade_button({ 0, 0 }, button_sprite_2->scale, button_sprite_2, pTex_button_hover, pTex_button_normal);
	upgrade_button.on_click = upgrades;
	upgrade_button.set_text("Weapon Upgrade", -120.0);

	win_buttons.buttons.emplace_back(level_select_button);
	win_buttons.buttons.emplace_back(upgrade_button);
}

/*!*****************************************************************************
  \brief
	Function is to show what will happen when a button is pressed
*******************************************************************************/
void win_update()
{
	win_buttons.update();
}

/*!*****************************************************************************
  \brief
	Function is to draw the wallpaper when the buttons is clicked and met the
	conditions
*******************************************************************************/
void win_draw()
{
	draw_non_relative(wallpaper_sprite, { 0,0 }, wallpaper_sprite->scale);
	win_buttons.draw();
	transition();
}

/*!*****************************************************************************
  \brief
	Function is to free the buttons' data
*******************************************************************************/
void win_free()
{
	win_buttons.buttons.clear();
}

/*!*****************************************************************************
  \brief
	Function is to unload the wallpaper and buttons that were called in
	earlier functions to prevent memory leaks
*******************************************************************************/
void win_unload()
{
	delete level_complete_sound;
	delete wallpaper_sprite;
	delete button_sprite_1;
	delete button_sprite_2;
	delete button_sprite_3;
	AEGfxTextureUnload(wallpaper);
	AEGfxTextureUnload(pTex_button_normal);
	AEGfxTextureUnload(pTex_button_hover);
	AEGfxMeshFree(square_mesh);
}

/*!*****************************************************************************
  \brief
	Function is to check whether the button is being pressed for the specific
	button and when being pressed, there, will be a transition
*******************************************************************************/
static void upgrades()
{
	transition(0.5f, GS_UPGRADE);
}

/*!*****************************************************************************
  \brief
	Function is to check whether the button is being pressed for the specific
	button and when being pressed, there, will be a transition
*******************************************************************************/
static void level_select()
{
	transition(0.5f, GS_LEVELSELECT);
}