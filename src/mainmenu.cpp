/*!*************************************************************************
****
\file   mainmenu.cpp
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Mar 11 2021

\brief
This program is an implementation of the main menu game state.

The functions include:
- level1_load
- level1_initialize
- level1_update
- level1_draw
- level1_free
- level1_unload

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/
#include "mainmenu.h"
#include "main.h"
#include "game_states.h"
#include "camera.h"
#include "player_data.h"
#include "button.h"
#include "panel.h"
#include "camera.h"
#include <iostream>

static s8 const* player_data_path = "./Data/player.json";
static AEGfxVertexList* square_mesh = 0;
static bool new_player;

static AEGfxTexture* wallpaper;
static AEVec2 wallpaperSize = { 1280.0f , 720.0f };
static Sprite* wallpaper_sprite;

static AEGfxTexture* btn_bg;

static AEVec2 prompt_bg_size = { 575.0f , 300.0f };
static Sprite* prompt_panel_spr;

static AEGfxTexture* pTex_button_hover;
static AEGfxTexture* pTex_button_normal;
static Sprite* button_sprite_1;
static Sprite* button_sprite_2;
static Sprite* button_sprite_3;
static Sprite* button_sprite_4;
static Sprite* button_sprite_5;
static Sprite* button_sprite_6;
static Sprite* button_sprite_7;

static void credits();
static ButtonManager main_menu_buttons;
static PromptPanel new_game_panel;
static PromptPanel exit_game_panel;

static void load_game();
static void new_game_prompt();
static void close_prompt();
static void confirm_prompt();
static void exit_prompt();
static void quit();

/*!****************************************************************************
 * @brief Load function pointer of the main menu
 * Load all the necessary textures, mesh, and sprites for the UI elements of 
 * the main menu. Also, tries to open the player_data_path to check if the file
 *  exists. If the file does not exist, it implies a first time play, setting 
 * new_player variable to true.
 * 
******************************************************************************/
void mainmenu_load()
{
	wallpaper = AEGfxTextureLoad("./UI/background2.png");
	AE_ASSERT_MESG(wallpaper, "Failed to create wallpaper!! \n");

	btn_bg = AEGfxTextureLoad("./UI/background.png");
	AE_ASSERT_MESG(btn_bg, "Failed to create wallpaper!! \n");

	pTex_button_hover = AEGfxTextureLoad("./UI/button_shade.png");
	AE_ASSERT_MESG(pTex_button_hover, "Failed to create shade button!! \n");

	pTex_button_normal = AEGfxTextureLoad("./UI/button_unshade.png");
	AE_ASSERT_MESG(pTex_button_normal, "Failed to create unshaded button!! \n");

	square_mesh = create_square_mesh(1.0f, 1.0f);

	wallpaper_sprite = new Sprite(wallpaper, square_mesh, wallpaperSize);
	wallpaper_sprite->set_size(read_png_size("./UI/background.png"));

	prompt_panel_spr = new Sprite(btn_bg, square_mesh, prompt_bg_size);
	prompt_panel_spr->set_size(wallpaper_sprite->size);

	AEVec2 button_size = { 350, 75 };
	AEVec2 credit_button_size = { 200, 75 };
	AEVec2 prompt_btn_size = { 125.0f, 50.0f };
	button_sprite_1 = new Sprite(pTex_button_normal, square_mesh, button_size);
	button_sprite_2 = new Sprite(pTex_button_normal, square_mesh, button_size);
	button_sprite_3 = new Sprite(pTex_button_normal, square_mesh, button_size);
	button_sprite_4 = new Sprite(pTex_button_normal, square_mesh, button_size);
	button_sprite_5 = new Sprite(pTex_button_normal, square_mesh, credit_button_size);
	button_sprite_6 = new Sprite(pTex_button_normal, square_mesh, prompt_btn_size);
	button_sprite_7 = new Sprite(pTex_button_normal, square_mesh, prompt_btn_size);
	std::ifstream file(player_data_path);
	if (file.is_open())
	{
		new_player = file.good() ? false : true;
		file.close();
	}
	else
		new_player = true;
}

/*!****************************************************************************
 * @brief Initialize function pointer of the main menu game state
 * Resets the singleton PlayerData instance. This is to ensure the proper 
 * functionality of "Start Game" and "Load Saved Game" buttons.
 * This initialize function also sets up all the buttons and panels.
 * 
******************************************************************************/
void mainmenu_initialize()
{
	PlayerData::instance()->reset_data();

	Button new_game_button({ 350, 100 }, button_sprite_1->scale, button_sprite_1, pTex_button_hover, pTex_button_normal);
	new_game_button.on_click = new_game_prompt;
	new_game_button.set_text("Start Game", -70.0f);

	Button load_game_button({ 350, 0 }, button_sprite_2->scale, button_sprite_2, pTex_button_hover, pTex_button_normal);
	load_game_button.on_click = load_game;
	load_game_button.button_type = new_player ? BUTTON_TYPE::BUTTON_INACTIVE : BUTTON_TYPE::BUTTON_NORMAL;
	load_game_button.set_text("Load Saved Game", -110.0f);

	Button options_button({ 350, -100 }, button_sprite_3->scale, button_sprite_3, pTex_button_hover, pTex_button_normal);
	options_button.on_click = open_options;
	options_button.set_text("Options", -50.0f);

	Button exit_button({ 350,-200 }, button_sprite_4->scale, button_sprite_4, pTex_button_hover, pTex_button_normal);
	exit_button.on_click = exit_prompt;
	exit_button.set_text("Quit Game", -70.0f);

	Button credits_button({ -521, -305 }, button_sprite_5->scale, button_sprite_5, pTex_button_hover, pTex_button_normal);
	credits_button.on_click = credits;
	credits_button.set_text("Credits", -50.0f);

	Button confirm_btn({ -100, -100 }, button_sprite_6->scale, button_sprite_6, pTex_button_hover, pTex_button_normal);
	confirm_btn.on_click = confirm_prompt;
	confirm_btn.set_text("Yes", -23.0f);

	Button decline_btn({ 100, -100 }, button_sprite_7->scale, button_sprite_7, pTex_button_hover, pTex_button_normal);
	decline_btn.on_click = close_prompt;
	decline_btn.set_text("No", -20.0f);

	main_menu_buttons.buttons.emplace_back(new_game_button);
	main_menu_buttons.buttons.emplace_back(load_game_button);
	main_menu_buttons.buttons.emplace_back(options_button);
	main_menu_buttons.buttons.emplace_back(exit_button);
	main_menu_buttons.buttons.emplace_back(credits_button);

	new_game_panel = PromptPanel();
	new_game_panel.pos = { 0,0 };
	new_game_panel.title = "Creating New Game";
	new_game_panel.title_pos.y += 50.0f;
	new_game_panel.title_pos.x -= 135.0f;
	new_game_panel.message = "Start Game will overwrite the old save file. Continue?";
	new_game_panel.message_pos.x -= 200.0f;
	new_game_panel.background = prompt_panel_spr;
	new_game_panel.btns.buttons.emplace_back(confirm_btn);
	new_game_panel.btns.buttons.emplace_back(decline_btn);

	exit_game_panel = PromptPanel();
	exit_game_panel.pos = { 0,0 };
	exit_game_panel.title = "Quit Game";
	exit_game_panel.title_pos.y += 50.0f;
	exit_game_panel.title_pos.x -= 65.0f;
	exit_game_panel.message = "Are you sure? Okay...";
	exit_game_panel.message_pos.x -= 80.0f;
	exit_game_panel.background = prompt_panel_spr;
	exit_game_panel.btns.buttons.emplace_back(confirm_btn);
	exit_game_panel.btns.buttons.emplace_back(decline_btn);
}

/*!****************************************************************************
 * @brief Update function of the main menu game state
 * Only updates the respective panels and not other UI elements if the flag is 
 * true so that only the focused panel will respond to the user.
 * 
******************************************************************************/
void mainmenu_update()
{
	if (new_game_panel.flag)
		new_game_panel.update();
	else if (exit_game_panel.flag)
		exit_game_panel.update();
	else if (get_options_flag() == false)
		main_menu_buttons.update();
}

/*!****************************************************************************
 * @brief Draw function of the main menu game state
 * Draws the wallpaper before all other elements. Calls transition to ensure
 *  transition effect
 * 
******************************************************************************/
void mainmenu_draw()
{
	draw_non_relative(wallpaper_sprite, { 0,0 }, wallpaper_sprite->scale);
	main_menu_buttons.draw();
	if (new_game_panel.flag)
		new_game_panel.draw();
	if (exit_game_panel.flag)
		exit_game_panel.draw();

	transition();
}

/*!****************************************************************************
 * @brief Free function of the main menu game state
 * Clears all the buttons in the button managers
 * 
******************************************************************************/
void mainmenu_free()
{
	main_menu_buttons.buttons.clear();
	exit_game_panel.btns.buttons.clear();
	new_game_panel.btns.buttons.clear();
}

/*!****************************************************************************
 * @brief Unload function of the main menu game state
 * Unloads all textures and sprites loaded from the load function
 * 
******************************************************************************/
void mainmenu_unload()
{
	delete wallpaper_sprite;
	delete prompt_panel_spr;
	delete button_sprite_1;
	delete button_sprite_2;
	delete button_sprite_3;
	delete button_sprite_4;
	delete button_sprite_5;
	delete button_sprite_6;
	delete button_sprite_7;
	AEGfxTextureUnload(wallpaper);
	AEGfxTextureUnload(btn_bg);
	AEGfxTextureUnload(pTex_button_normal);
	AEGfxTextureUnload(pTex_button_hover);
	AEGfxMeshFree(square_mesh);
}

/*!****************************************************************************
 * @brief Loads the existing player data and transition to level select
 * 
******************************************************************************/
static void load_game()
{
	PlayerData::instance()->load_data(player_data_path);
	transition(1.0f, GS_LEVELSELECT);
}

/*!****************************************************************************
 * @brief Transitions to the level select game state if a new player. Else, 
 * sets the new_game_panel prompt flag to true.
 * 
******************************************************************************/
static void new_game_prompt()
{
	if (new_player)
	{
		PlayerData::instance();
		transition(0.5f, GS_LEVELSELECT);
	}
	else
		new_game_panel.flag = true;
}

/*!****************************************************************************
 * @brief Sets the panels flag to false
 * 
******************************************************************************/
static void close_prompt()
{
	new_game_panel.flag = false;
	exit_game_panel.flag = false;
}

/*!****************************************************************************
 * @brief Transition to level select if new_game_panel is true, else quit the 
 * game if the exit_game_panel flag is true
 * 
 ******************************************************************************/
static void confirm_prompt()
{
	if (new_game_panel.flag)
	{
		PlayerData::instance();
		transition(0.5f, GS_LEVELSELECT);
	}
	else if (exit_game_panel.flag)
		quit();
}

/*!****************************************************************************
 * @brief Closes the exit_game_panel prompt
 * 
 ******************************************************************************/
static void exit_prompt()
{
	exit_game_panel.flag = true;
}

/*!****************************************************************************
 * @brief Release the PlayerData singleton object and quit the game
 * 
 ******************************************************************************/
static void quit()
{
	PlayerData::release_instance();
	gAEGameStateNext = AE_GS_QUIT;
}

/*!****************************************************************************
 * @brief Transition to the credits page
 * 
 ******************************************************************************/
static void credits()
{
	transition(1.0f, GS_CREDITS);
}