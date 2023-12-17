/*!*************************************************************************
****
\file   pause.cpp
\author Primary: Siti Nursafinah Binte Sumsuri (80.2%)
		Secondary: Cruz Rolly Matthew Capiral  (19.8%)
\par    DP email: sitinursafinah.b@digipen.edu
				  cruzrolly.m\@digipen.edu
\date   Apr 6 2021

\brief
This file contains the implementation of the panel UI class.

The functions include:
- pause_load
- pause_initialize
- pause_update
- pause_draw
- pause_free
- pause_unload
- resume_escape
- combo_list
- control
- confirm_prompt
- level_select
- close_prompt

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/

#include <iostream>
#include "pause.h"
#include "main.h"
#include "game_states.h"
#include "level_manager.h"
#include "button.h"
#include <iostream>
#include "camera.h"
#include "graphics.h"

static AEGfxVertexList* square_mesh = 0;

static AEGfxTexture* wallpaper;
static AEGfxTexture* controls;
static AEVec2 controlsSize = { 700.0f, 600.0f };
static AEVec2 controlsCdn = { 0,0 };
static Sprite* controls_sprite;

static AEGfxTexture* combo_lists;
static AEVec2 combo_list_Size = { 700.0f, 600.0f };
static AEVec2 combo_list_Cdn = { 0,0 };
static Sprite* combo_list_sprite;

static AEGfxTexture* pTex_button_hover;
static AEGfxTexture* pTex_button_normal;
static Sprite* button_sprite_1;
static Sprite* button_sprite_2;
static Sprite* button_sprite_3;
static Sprite* button_sprite_4;
static Sprite* button_sprite_5;
static Sprite* button_sprite_6;
static Sprite* button_sprite_7;
static ButtonManager pause_buttons;
static AEVec2 button_size = { 350, 75 };

static AEVec2 prompt_bg_size = { 575.0f , 300.0f };
static Sprite* prompt_panel_spr;
static PromptPanel level_select_panel;

static bool controlPage = false;
static bool combolistPage = false;

static void resume_escape();
static void combo_list();
static void control();
static void confirm_prompt();
static void level_select();
static void close_prompt();

/*!*****************************************************************************
  \brief
	Function is to load the textures of the buttons and the wallpaper
*******************************************************************************/
void pause_load()
{
	wallpaper = AEGfxTextureLoad("./UI/background.png");
	AE_ASSERT_MESG(wallpaper, "Failed to create wallpaper!! \n");

	combo_lists = AEGfxTextureLoad("./UI/combo_lists.png");
	AE_ASSERT_MESG(combo_lists, "Failed to create combo list!! \n");

	controls = AEGfxTextureLoad("./UI/controls.png");
	AE_ASSERT_MESG(controls, "Failed to create controls!! \n");

	pTex_button_hover = AEGfxTextureLoad("./UI/button_shade.png");
	AE_ASSERT_MESG(pTex_button_hover, "Failed to create shade button!! \n");

	pTex_button_normal = AEGfxTextureLoad("./UI/button_unshade.png");
	AE_ASSERT_MESG(pTex_button_normal, "Failed to create unshaded button!! \n");

	square_mesh = create_square_mesh(1.0f, 1.0f);

	controls_sprite = new Sprite(controls, square_mesh, controlsSize);
	controls_sprite->set_size(read_png_size("./UI/controls.png"));

	combo_list_sprite = new Sprite(combo_lists, square_mesh, combo_list_Size);
	combo_list_sprite->set_size(read_png_size("./UI/combo_lists.png"));

	AEVec2 prompt_btn_size = { 125.0f, 50.0f };
	button_sprite_1 = new Sprite(pTex_button_normal, square_mesh, button_size);
	button_sprite_2 = new Sprite(pTex_button_normal, square_mesh, button_size);
	button_sprite_3 = new Sprite(pTex_button_normal, square_mesh, button_size);
	button_sprite_4 = new Sprite(pTex_button_normal, square_mesh, button_size);
	button_sprite_5 = new Sprite(pTex_button_normal, square_mesh, button_size);
	button_sprite_6 = new Sprite(pTex_button_normal, square_mesh, prompt_btn_size);
	button_sprite_7 = new Sprite(pTex_button_normal, square_mesh, prompt_btn_size);

	prompt_panel_spr = new Sprite(wallpaper, square_mesh, prompt_bg_size);
}

/*!*****************************************************************************
  \brief
	Function is to position of the drawn buttons
*******************************************************************************/
void pause_initialize()
{
	Button resume_button({ 0, 200 }, button_sprite_1->scale, button_sprite_1, pTex_button_hover, pTex_button_normal);
	resume_button.on_click = resume_escape;
	resume_button.set_text("Resume Game", -80.0f);

	Button combo_list_button({ 0, 100 }, button_sprite_2->scale, button_sprite_2, pTex_button_hover, pTex_button_normal);

	combo_list_button.on_click = combo_list;
	combo_list_button.set_text("Combo List", -80.0f);

	Button controls_button({ 0, -0 }, button_sprite_3->scale, button_sprite_3, pTex_button_hover, pTex_button_normal);

	controls_button.on_click = control;
	controls_button.set_text("Controls", -60.0f);

	Button options_button({ 0, -100 }, button_sprite_4->scale, button_sprite_4, pTex_button_hover, pTex_button_normal);

	options_button.on_click = open_options;
	options_button.set_text("Options", -60.0f);

	Button level_select_button({ 0, -200 }, button_sprite_5->scale, button_sprite_5, pTex_button_hover, pTex_button_normal);

	level_select_button.on_click = level_select;
	level_select_button.set_text("Back to Level Select", -140.0f);

	pause_buttons.buttons.emplace_back(resume_button);
	pause_buttons.buttons.emplace_back(combo_list_button);
	pause_buttons.buttons.emplace_back(controls_button);
	pause_buttons.buttons.emplace_back(options_button);
	pause_buttons.buttons.emplace_back(level_select_button);
	Button confirm_btn({ -100, -100 }, button_sprite_6->scale, button_sprite_6, pTex_button_hover, pTex_button_normal);
	confirm_btn.on_click = confirm_prompt;
	confirm_btn.set_text("Yes", -23.0f);

	Button decline_btn({ 100, -100 }, button_sprite_7->scale, button_sprite_7, pTex_button_hover, pTex_button_normal);
	decline_btn.on_click = close_prompt;
	decline_btn.set_text("No", -20.0f);

	level_select_panel = PromptPanel();
	level_select_panel.pos = { 0,0 };
	level_select_panel.title = "Back to Level Select";
	level_select_panel.title_pos.y += 50.0f;
	level_select_panel.title_pos.x -= 135.0f;
	level_select_panel.message = "Moving to level select. Are you sure?";
	level_select_panel.message_pos.x -= 150.0f;
	level_select_panel.background = prompt_panel_spr;
	level_select_panel.btns.buttons.emplace_back(confirm_btn);
	level_select_panel.btns.buttons.emplace_back(decline_btn);
}

/*!*****************************************************************************
  \brief
	Function is to show what will happen when a button is pressed
*******************************************************************************/
void pause_update()
{
	if (!get_options_flag())
	{
		if (!level_select_panel.flag)
		{
			if (!controlPage && !combolistPage)
			{
				pause_buttons.update();
			}
			if (AEInputCheckTriggered(AEVK_ESCAPE))
			{
				if (!controlPage && !combolistPage)
				{
					resume_escape();
				}
				combolistPage = false;
				controlPage = false;
			}
		}
		else
			level_select_panel.update();
	}
}

/*!*****************************************************************************
  \brief
	Function is to draw the wallpaper when the buttons is clicked and met the
	conditions
*******************************************************************************/
void pause_draw()
{
	if (!controlPage && !combolistPage)
	{
		pause_buttons.draw();
	}
	if (controlPage)
	{
		draw_non_relative(controls, square_mesh, controlsCdn, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, 1.0f, controlsSize, 0);
	}
	if (combolistPage)
	{
		draw_non_relative(combo_lists, square_mesh, combo_list_Cdn, { 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, 1.0f, combo_list_Size, 0);
	}
	if (level_select_panel.flag)
		level_select_panel.draw();
	transition();
}

/*!*****************************************************************************
  \brief
	Function is to free the buttons' data
*******************************************************************************/
void pause_free()
{
	pause_buttons.buttons.clear();
	level_select_panel.btns.buttons.clear();
}

/*!*****************************************************************************
  \brief
	Function is to unload the wallpaper and buttons that were called in
	earlier functions to prevent memory leaks
*******************************************************************************/
void pause_unload()
{
	delete controls_sprite;
	delete combo_list_sprite;
	delete button_sprite_1;
	delete button_sprite_2;
	delete button_sprite_3;
	delete button_sprite_4;
	delete button_sprite_5;
	delete button_sprite_6;
	delete button_sprite_7;
	delete prompt_panel_spr;
	AEGfxTextureUnload(wallpaper);
	AEGfxMeshFree(square_mesh);
	AEGfxTextureUnload(controls);
	AEGfxTextureUnload(combo_lists);
	AEGfxTextureUnload(pTex_button_normal);
	AEGfxTextureUnload(pTex_button_hover);
}

/*!*****************************************************************************
  \brief
	Function is to check whether the button is being pressed for the specific
	button
*******************************************************************************/
static void resume_escape()
{
	if (!controlPage && !combolistPage) {
		pause = false;
		AudioSystem::instance()->set_pause_group(AUDIO_GROUP::STAGE_BGM, false);
		AudioSystem::instance()->set_pause_group(AUDIO_GROUP::BOSS_BGM, false);
		AudioSystem::instance()->set_pause_group(AUDIO_GROUP::SFX, false);
	}
}

/*!*****************************************************************************
  \brief
	Function is to check whether the button is being pressed for the specific
	button
*******************************************************************************/
static void combo_list()
{
	if (!controlPage && !combolistPage)combolistPage = true;
}

/*!*****************************************************************************
  \brief
	Function is to check whether the button is being pressed for the specific
	button
*******************************************************************************/
static void control()
{
	if (!controlPage && !combolistPage)controlPage = true;
}

/*!*****************************************************************************
 * @brief Opens the level select prompt
 *
*******************************************************************************/
static void level_select()
{
	if (!controlPage && !combolistPage)level_select_panel.flag = true;
}

/*!*****************************************************************************
 * @brief Transition to the Level Select game state
 *
*******************************************************************************/
static void confirm_prompt()
{
	transition(0.5f, GS_LEVELSELECT);
}

/*!*****************************************************************************
 * @brief Close the level select prompt
 *
*******************************************************************************/
static void close_prompt()
{
	level_select_panel.flag = false;
}