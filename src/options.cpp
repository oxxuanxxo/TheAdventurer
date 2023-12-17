/*!*************************************************************************
****
\file   options.cpp
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Apr 6 2021

\brief
This file contains the implementation of the options menu in the game engine.

The functions include:
- options_load
- options_init
- options_update
- options_draw
- options_free
- options_unload
- open_options
- get_options_flag
- options_unload
- vol_bgm_up
- vol_bgm_down
- vol_sfx_up
- vol_sfx_down
- full_screen
- close_options

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/
#include "panel.h"
#include "audio.h"
#include "main.h"

static AEGfxTexture* pTex_button_hover;
static AEGfxTexture* pTex_button_default;
static AEGfxTexture* pTex_arrow_hover;
static AEGfxTexture* pTex_arrow_default;
static AEGfxTexture* bg;
static AEGfxVertexList* square_mesh;
static Panel options_panel;
static vector<Sprite> button_sprites;

static AEVec2 bg_size{ 600, 600 };
static Sprite* bg_spr;
static Sprite* volume_spr;

static void vol_bgm_up();
static void vol_bgm_down();
static void vol_sfx_up();
static void vol_sfx_down();
static void full_screen();
static void close_options();

/******************************************************************************
 * @brief Load the options menu
 *
 *****************************************************************************/
void options_load()
{
	bgm_vol = 10;
	sfx_vol = 10;
	is_fullscreen = true;
	pTex_button_hover = AEGfxTextureLoad("./UI/button_shade.png");
	AE_ASSERT_MESG(pTex_button_hover, "Failed to create hover button!! \n");

	pTex_button_default = AEGfxTextureLoad("./UI/button_unshade.png");
	AE_ASSERT_MESG(pTex_button_default, "Failed to create default button!! \n");

	bg = AEGfxTextureLoad("./UI/background.png");
	AE_ASSERT_MESG(bg, "Failed to create bg!! \n");

	pTex_arrow_hover = AEGfxTextureLoad("./UI/grey_right_arrow.png");
	AE_ASSERT_MESG(pTex_arrow_hover, "Failed to create bg!! \n");

	pTex_arrow_default = AEGfxTextureLoad("./UI/right_arrow.png");
	AE_ASSERT_MESG(pTex_arrow_default, "Failed to create bg!! \n");

	square_mesh = create_square_mesh(1.0f, 1.0f, 0xFF000000);

	bg_spr = new Sprite(bg, square_mesh, bg_size);

	volume_spr = new Sprite(nullptr, square_mesh, { 20.0f, 40.0f });

	AEVec2 arrow_btn_scale = { 75.0f,75.0f };
	Sprite button_spr = Sprite(pTex_button_default, square_mesh, arrow_btn_scale);
	for (int i = 0; i < 6; ++i) { button_sprites.emplace_back(button_spr); }

	Button lower_vol_bgm = Button({ -200,-30 }, button_spr.scale, &button_sprites[0], pTex_arrow_hover, pTex_arrow_default);
	lower_vol_bgm.obj_sprite->texture = pTex_arrow_default;
	lower_vol_bgm.obj_sprite->flip_x = true;
	lower_vol_bgm.on_click = vol_bgm_down;
	Button higher_vol_bgm = Button({ 200,-30 }, button_spr.scale, &button_sprites[1], pTex_arrow_hover, pTex_arrow_default);
	higher_vol_bgm.obj_sprite->texture = pTex_arrow_default;
	higher_vol_bgm.on_click = vol_bgm_up;
	Button lower_vol_sfx = Button({ -200,100 }, button_spr.scale, &button_sprites[2], pTex_arrow_hover, pTex_arrow_default);
	lower_vol_sfx.obj_sprite->texture = pTex_arrow_default;
	lower_vol_sfx.obj_sprite->flip_x = true;
	lower_vol_sfx.on_click = vol_sfx_down;
	Button higher_vol_sfx = Button({ 200,100 }, button_spr.scale, &button_sprites[3], pTex_arrow_hover, pTex_arrow_default);
	higher_vol_bgm.obj_sprite->texture = pTex_arrow_default;
	higher_vol_sfx.on_click = vol_sfx_up;

	AEVec2 fullscreen_btn_size = { 275.0f, 75.0f };
	Button toggle_fullscreen_btn = Button({ -125.0f,-200.0f }, fullscreen_btn_size, &button_sprites[4], pTex_button_hover, pTex_button_default);
	toggle_fullscreen_btn.obj_sprite->scale = fullscreen_btn_size;
	toggle_fullscreen_btn.on_click = full_screen;
	toggle_fullscreen_btn.set_text("Toggle Fullscreen", -120.0f);

	AEVec2 back_btn_size = { 100.0f, 75.0f };
	Button back_btn = Button({ 200,-200 }, back_btn_size, &button_sprites[5], pTex_button_hover, pTex_button_default);
	back_btn.obj_sprite->scale = back_btn_size;
	back_btn.on_click = close_options;
	back_btn.set_text("Back", -35.0f);

	options_panel.btns.buttons.emplace_back(lower_vol_bgm);
	options_panel.btns.buttons.emplace_back(higher_vol_bgm);
	options_panel.btns.buttons.emplace_back(lower_vol_sfx);
	options_panel.btns.buttons.emplace_back(higher_vol_sfx);
	options_panel.btns.buttons.emplace_back(toggle_fullscreen_btn);
	options_panel.btns.buttons.emplace_back(back_btn);
	options_panel.background = bg_spr;
}

/******************************************************************************
 * @brief Initialize the options menu
 *
 *****************************************************************************/
void options_init()
{
	options_panel.flag = false;
}

/******************************************************************************
 * @brief Update the options menu
 *
 *****************************************************************************/
void options_update()
{
	if (options_panel.flag)
	{
		options_panel.update();
	}
}

/******************************************************************************
 * @brief Draw the options menu
 *
 *****************************************************************************/
void options_draw()
{
	if (options_panel.flag)
	{
		options_panel.draw();
		AEVec2 title_pos = { -50.0f, 230.0f };
		AEVec2 sfx_pos = { -90.0f, 160.0f };
		AEVec2 bgm_pos = { -90.0f, 30.0f };
		text_print(bold_font, "Options", &title_pos);
		text_print(default_font, "Sound Volume", &sfx_pos);
		text_print(default_font, "Music Volume", &bgm_pos);

		AEVec2 coordinates = { -167, -30 };

		int offset = 30;
		for (s32 i = 0; i < bgm_vol; ++i)
		{
			coordinates.x += offset;
			draw_non_relative(volume_spr, coordinates, volume_spr->scale);
		}

		coordinates = { -167, 100 };

		for (s32 i = 0; i < sfx_vol; ++i)
		{
			coordinates.x += offset;
			draw_non_relative(volume_spr, coordinates, volume_spr->scale);
		}
	}
}

/******************************************************************************
 * @brief Free the options menu by setting its flag to false
 *
 *****************************************************************************/
void options_free()
{
	options_panel.flag = false;
}

/******************************************************************************
 * @brief Unload resources allocated for the options menu
 *
 *****************************************************************************/
void options_unload()
{
	delete bg_spr;
	delete volume_spr;
	AEGfxTextureUnload(pTex_button_hover);
	AEGfxTextureUnload(pTex_button_default);
	AEGfxTextureUnload(pTex_arrow_hover);
	AEGfxTextureUnload(pTex_arrow_default);
	AEGfxTextureUnload(bg);
	AEGfxMeshFree(square_mesh);
	button_sprites.clear();
}

/******************************************************************************
 * @brief Opens the options menu
 *
 *****************************************************************************/
void open_options()
{
	options_panel.flag = true;
}

/******************************************************************************
 * @brief Get the options flag object
 *
 * @return true
 * Options menu is opened
 * @return false
 * Options menu is closed
 *****************************************************************************/
bool get_options_flag()
{
	return options_panel.flag;
}

/******************************************************************************
 * @brief Closes the options menu
 *
 *****************************************************************************/
static void close_options()
{
	options_panel.flag = false;
}

/******************************************************************************
 * @brief Set the bgm vol
 *
 *****************************************************************************/
static void set_bgm_vol()
{
	AudioSystem::instance()->set_group_vol_multi(AUDIO_GROUP::STAGE_BGM, 0.1f * bgm_vol);
	AudioSystem::instance()->set_group_vol_multi(AUDIO_GROUP::BOSS_BGM, 0.1f * bgm_vol);
}

/******************************************************************************
 * @brief Increment the volume of the BGM
 *
 *****************************************************************************/
static void vol_bgm_up()
{
	++bgm_vol;
	bgm_vol = static_cast<s32>(AEClamp(static_cast<f32>(bgm_vol), 0, 10.0f));
	set_bgm_vol();
}

/******************************************************************************
 * @brief Decrement the volume of the BGM
 *
 *****************************************************************************/
static void vol_bgm_down()
{
	--bgm_vol;
	bgm_vol = static_cast<s32>(AEClamp(static_cast<f32>(bgm_vol), 0, 10.0f));
	set_bgm_vol();
}

/******************************************************************************
 * @brief Increment the volume of the sfx
 *
 *****************************************************************************/
static void vol_sfx_up()
{
	++sfx_vol;
	sfx_vol = static_cast<s32>(AEClamp(static_cast<f32>(sfx_vol), 0, 10.0f));
	AudioSystem::instance()->set_group_vol_multi(AUDIO_GROUP::SFX, 0.1f * sfx_vol);
}

/******************************************************************************
 * @brief Decrement the volume of the sfx
 *
 *****************************************************************************/
static void vol_sfx_down()
{
	--sfx_vol;
	sfx_vol = static_cast<s32>(AEClamp(static_cast<f32>(sfx_vol), 0, 10.0f));
	AudioSystem::instance()->set_group_vol_multi(AUDIO_GROUP::SFX, 0.1f * sfx_vol);
}

/******************************************************************************
 * @brief Toggles the full screen
 *
 *****************************************************************************/
static void full_screen()
{
	is_fullscreen = !is_fullscreen;
	AEToggleFullScreen(is_fullscreen);
}