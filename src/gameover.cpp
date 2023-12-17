/*!*****************************************************************************
\file   gameover.cpp
\author Siti Nursafinah Binte Sumsuri
\par    DP email: sitinursafinah.b\@digipen.edu
\date   Mar 4 2021

\brief
This file outlines gameover structure and how it is drawn.

The functions include:
- gameover_load
- gameover_initialize
- gameover_update
- gameover_draw
- gameover_free
- gameover_unload

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
static ButtonManager gameover_buttons;

static void credits();
static void level_select();

/*!*****************************************************************************
  \brief
	Function is to load the textures of the buttons and the wallpaper
*******************************************************************************/
void gameover_load()
{
	wallpaper = AEGfxTextureLoad("./UI/gameover.png");
	AE_ASSERT_MESG(wallpaper, "Failed to create wallpaper!! \n");

	pTex_button_hover = AEGfxTextureLoad("./UI/button_shade.png");
	AE_ASSERT_MESG(pTex_button_hover, "Failed to create shade button!! \n");

	pTex_button_normal = AEGfxTextureLoad("./UI/button_unshade.png");
	AE_ASSERT_MESG(pTex_button_normal, "Failed to create unshaded button!! \n");

	square_mesh = create_square_mesh(1.0f, 1.0f);

	wallpaper_sprite = new Sprite(wallpaper, square_mesh, wallpaperSize);
	wallpaper_sprite->set_size(read_png_size("./UI/gameover.png"));

	AEVec2 button_size = { 350, 75 };

	button_sprite_1 = new Sprite(pTex_button_normal, square_mesh, button_size);
	button_sprite_2 = new Sprite(pTex_button_normal, square_mesh, button_size);
}

/*!*****************************************************************************
  \brief
	Function is to position of the drawn buttons
*******************************************************************************/
void gameover_initialize()
{
	Button level_select_button({ 300, -100 }, button_sprite_1->scale, button_sprite_1, pTex_button_hover, pTex_button_normal);
	level_select_button.on_click = level_select;
	level_select_button.set_text("Level Select", -80.0f);

	Button credit_button({ 300, -200 }, button_sprite_2->scale, button_sprite_2, pTex_button_hover, pTex_button_normal);
	credit_button.on_click = credits;
	credit_button.set_text("Credits", -50.0);

	gameover_buttons.buttons.emplace_back(level_select_button);
	gameover_buttons.buttons.emplace_back(credit_button);
}

/*!*****************************************************************************
  \brief
	Function is to show what will happen when a button is pressed
*******************************************************************************/
void gameover_update()
{
	gameover_buttons.update( );
}

/*!*****************************************************************************
  \brief
	Function is to draw the wallpaper when the buttons is clicked and met the
	conditions
*******************************************************************************/
void gameover_draw()
{
	draw_non_relative(wallpaper_sprite, { 0,0 }, wallpaper_sprite->scale);
	gameover_buttons.draw();
	transition();
}

/*!*****************************************************************************
  \brief
	Function is to free the buttons' data and the respective sprites
*******************************************************************************/
void gameover_free()
{
	delete wallpaper_sprite;
	delete button_sprite_1;
	delete button_sprite_2;

	gameover_buttons.buttons.clear();
}

/*!*****************************************************************************
  \brief
	Function is to unload the wallpaper and buttons that were called in
	earlier functions to prevent memory leaks
*******************************************************************************/
void gameover_unload()
{
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
static void credits()
{
	transition(0.5f, GS_CREDITS);
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