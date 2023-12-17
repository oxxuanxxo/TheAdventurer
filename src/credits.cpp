/*!*****************************************************************************
\file   credits.h
\author Siti Nursafinah Binte Sumsuri
		Mohammad Hanif Koh Teck Wee (4 lines)
\par    DP email: sitinursafinah.b\@digipen.edu
				  mohammadhanif.t\@digipen.edu
\date   Mar 4 2021

\brief
This file outlines mainmenu structure and how it is drawn.

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

#include "credits.h"
#include "main.h"
#include "game_states.h"
#include "camera.h"
#include "player_data.h"
#include "button.h"
#include "camera.h"
#include <iostream>

static AEGfxVertexList* square_mesh = 0;

static AEVec2 wallpaperSize = { 1280.0f , 720.0f };
static AEVec2 wallpaperCdn = { 0,0 };

static AEGfxTexture* wallpaper_1;
static Sprite* wallpaper_sprite_1;
static AEGfxTexture* wallpaper_2;
static Sprite* wallpaper_sprite_2;
static AEGfxTexture* wallpaper_3;
static Sprite* wallpaper_sprite_3;
static AEGfxTexture* wallpaper_4;
static Sprite* wallpaper_sprite_4;
static AEGfxTexture* wallpaper_5;
static Sprite* wallpaper_sprite_5;

static AEGfxTexture* pTex_button_hover;
static AEGfxTexture* pTex_button_normal;

static Sprite* button_sprite_1;
static Sprite* button_sprite_2;
static Sprite* button_sprite_3;
static Sprite* button_sprite_4;
static Sprite* button_sprite_5;

static ButtonManager credits_buttons;

static bool team_members = false;
static bool faculty_advisors = false;
static bool creators = false;
static bool copyrights = false;

static void roles();
static void instructors();
static void digipen();
static void copyrights_license();
static void main_menu();
static void quit();

/*!*****************************************************************************
  \brief
	Function is to load the textures of the buttons and the wallpaper
*******************************************************************************/
void credits_load()
{
	wallpaper_1 = AEGfxTextureLoad("./UI/credits_TM.png");
	AE_ASSERT_MESG(wallpaper_1, "Failed to create wallpaper!! \n");

	wallpaper_2 = AEGfxTextureLoad("./UI/credits_FA.png");
	AE_ASSERT_MESG(wallpaper_2, "Failed to create wallpaper!! \n");

	wallpaper_3 = AEGfxTextureLoad("./UI/credits_PE.png");
	AE_ASSERT_MESG(wallpaper_3, "Failed to create wallpaper!! \n");

	wallpaper_4 = AEGfxTextureLoad("./UI/credits_copyrights.png");
	AE_ASSERT_MESG(wallpaper_4, "Failed to create wallpaper!! \n");

	wallpaper_5 = AEGfxTextureLoad("./UI/credits_homepage.png");
	AE_ASSERT_MESG(wallpaper_5, "Failed to create wallpaper!! \n");

	pTex_button_hover = AEGfxTextureLoad("./UI/button_shade.png");
	AE_ASSERT_MESG(pTex_button_hover, "Failed to create shade button!! \n");

	pTex_button_normal = AEGfxTextureLoad("./UI/button_unshade.png");
	AE_ASSERT_MESG(pTex_button_normal, "Failed to create unshaded button!! \n");

	square_mesh = create_square_mesh(1.0f, 1.0f);

	wallpaper_sprite_1 = new Sprite(wallpaper_1, square_mesh, wallpaperSize);
	wallpaper_sprite_1->set_size(read_png_size("./UI/credits_TM.png"));

	wallpaper_sprite_2 = new Sprite(wallpaper_2, square_mesh, wallpaperSize);
	wallpaper_sprite_2->set_size(read_png_size("./UI/credits_FA.png"));

	wallpaper_sprite_3 = new Sprite(wallpaper_3, square_mesh, wallpaperSize);
	wallpaper_sprite_3->set_size(read_png_size("./UI/credits_PE.png"));

	wallpaper_sprite_4 = new Sprite(wallpaper_4, square_mesh, wallpaperSize);
	wallpaper_sprite_4->set_size(read_png_size("./UI/credits_copyrights.png"));

	wallpaper_sprite_5 = new Sprite(wallpaper_5, square_mesh, wallpaperSize);
	wallpaper_sprite_5->set_size(read_png_size("./UI/credits_homepage.png"));

	AEVec2 button_size = { 350, 75 };

	button_sprite_1 = new Sprite(pTex_button_normal, square_mesh, button_size);
	button_sprite_2 = new Sprite(pTex_button_normal, square_mesh, button_size);
	button_sprite_3 = new Sprite(pTex_button_normal, square_mesh, button_size);
	button_sprite_4 = new Sprite(pTex_button_normal, square_mesh, button_size);
	button_sprite_5 = new Sprite(pTex_button_normal, square_mesh, button_size);
}

/*!*****************************************************************************
  \brief
	Function is to position of the drawn buttons
*******************************************************************************/
void credits_initialize()
{
	Button role_button({ 300, -100 }, button_sprite_1->scale, button_sprite_1, pTex_button_hover, pTex_button_normal);
	role_button.on_click = roles;
	role_button.set_text("Team Members", -100.0f);

	Button instructor_button({ 300, 100 }, button_sprite_2->scale, button_sprite_2, pTex_button_hover, pTex_button_normal);
	instructor_button.on_click = instructors;
	instructor_button.set_text("Faculty & Advisors", -135.0);

	Button digipens_button({ -300, 100 }, button_sprite_3->scale, button_sprite_3, pTex_button_hover, pTex_button_normal);
	digipens_button.on_click = digipen;
	digipens_button.set_text("President & Executives", -163.0);

	Button copyright_button({ -300, -100 }, button_sprite_4->scale, button_sprite_4, pTex_button_hover, pTex_button_normal);
	copyright_button.on_click = copyrights_license;
	copyright_button.set_text("Copyrights", -70.0);

	Button	main_menu_button({ -400, -300 }, button_sprite_5->scale, button_sprite_5, pTex_button_hover, pTex_button_normal);
	main_menu_button.on_click = main_menu;
	main_menu_button.set_text("Back to Main Menu", -140.0);

	credits_buttons.buttons.emplace_back(role_button);
	credits_buttons.buttons.emplace_back(instructor_button);
	credits_buttons.buttons.emplace_back(digipens_button);
	credits_buttons.buttons.emplace_back(copyright_button);
	credits_buttons.buttons.emplace_back(main_menu_button);
}

/*!*****************************************************************************
  \brief
	Function is to show what will happen when a button is pressed
*******************************************************************************/
void credits_update()
{
	if (!transitioning)
	{
		credits_buttons.update();

		if (AEInputCheckTriggered(AEVK_ESCAPE))
		{
			if (team_members)
			{
				team_members = false;
			}

			if (faculty_advisors)
			{
				faculty_advisors = false;
			}

			if (creators)
			{
				creators = false;
			}

			if (copyrights)
			{
				copyrights = false;
			}
		}
	}
}

/*!*****************************************************************************
  \brief
	Function is to draw the wallpaper when the buttons is clicked and met the
	conditions
*******************************************************************************/
void credits_draw()
{
	draw_non_relative(wallpaper_sprite_5, wallpaperCdn, wallpaper_sprite_5->scale);

	credits_buttons.draw();

	if (team_members)
	{
		draw_non_relative(wallpaper_1, square_mesh, wallpaperCdn, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, 1.0f, wallpaperSize, 0);
	}
	if (faculty_advisors)
	{
		draw_non_relative(wallpaper_2, square_mesh, wallpaperCdn, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, 1.0f, wallpaperSize, 0);
	}
	if (creators)
	{
		draw_non_relative(wallpaper_3, square_mesh, wallpaperCdn, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, 1.0f, wallpaperSize, 0);
	}
	if (copyrights)
	{
		draw_non_relative(wallpaper_4, square_mesh, wallpaperCdn, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, 1.0f, wallpaperSize, 0);
	}

	transition();
}

/*!*****************************************************************************
  \brief
	Function is to free the buttons' data
*******************************************************************************/
void credits_free()
{
	credits_buttons.buttons.clear();
}

/*!*****************************************************************************
  \brief
	Function is to unload the wallpaper and buttons that were called in
	earlier functions to prevent memory leaks
*******************************************************************************/
void credits_unload()
{
	delete wallpaper_sprite_1;
	delete wallpaper_sprite_2;
	delete wallpaper_sprite_3;
	delete wallpaper_sprite_4;
	delete wallpaper_sprite_5;
	delete button_sprite_1;
	delete button_sprite_2;
	delete button_sprite_3;
	delete button_sprite_4;
	delete button_sprite_5;

	AEGfxTextureUnload(wallpaper_1);
	AEGfxTextureUnload(wallpaper_2);
	AEGfxTextureUnload(wallpaper_3);
	AEGfxTextureUnload(wallpaper_4);
	AEGfxTextureUnload(wallpaper_5);
	AEGfxTextureUnload(pTex_button_normal);
	AEGfxTextureUnload(pTex_button_hover);
	AEGfxMeshFree(square_mesh);
}

/*!*****************************************************************************
  \brief
	Function is to check whether the button is being pressed for the specific
	button
*******************************************************************************/
static void roles()
{
	team_members = true;
}

/*!*****************************************************************************
  \brief
	Function is to check whether the button is being pressed for the specific
	button
*******************************************************************************/
static void instructors()
{
	faculty_advisors = true;
}

/*!*****************************************************************************
  \brief
	Function is to check whether the button is being pressed for the specific
	button
*******************************************************************************/
static void digipen()
{
	creators = true;
}

/*!*****************************************************************************
  \brief
	Function is to check whether the button is being pressed for the specific
	button
*******************************************************************************/
static void copyrights_license()
{
	copyrights = true;
}

/*!*****************************************************************************
  \brief
	Function is to check whether the button is being pressed for the specific
	button and when being pressed, there, will be a transition
*******************************************************************************/
static void main_menu()
{
	transition(0.5f, GS_MAINMENU);
}