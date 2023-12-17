/*!*****************************************************************************
\file   level_select.h
\author Chua Yip Xuan
\par    DP email: yipxuan.chua\@digipen.edu
\date   Mar 28 2021

\brief
	This source file consists of functions to implement the "world map"
	aka level_select.cpp

The source file has functions that consists of:
	- level_select_load
	- level_select_initialize
	- level_select_update
	- level_select_draw
	- level_select_free
	- level_select_unload
	- tutorial_stage
	- level1_stage
	- level2_stage
	- level3_stage
	- upgrade_stage
	- mainmenu_stage

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************************/

#include "game_states.h"
#include "level_select.h"
#include "camera.h"
#include "button.h"
#include "player_data.h"
#include "transition.h"

AEVec2 button_scale{ 350.0f, 200.0f };

AEVec2 tut_pos{ -200.0f, 150.0f };
AEVec2 level1_pos{ 250.0f, 150.0f };
AEVec2 level2_pos{ -320.0f, -70.0f };
AEVec2 level3_pos{ 220.0f, -100.0f };
AEVec2 upgrade_pos{ 310.0f, 300.0f };
AEVec2 back_pos{ -400.0f, 300.0f };

static Sprite* bg_sprite;
static Sprite* back_sprite;
static Sprite* tutorial_sprite;
static Sprite* level1_sprite;
static Sprite* level2_sprite;
static Sprite* level3_sprite;
static Sprite* upgrade_sprite;

static AEGfxTexture* back_tex;
static AEGfxTexture* bg_texture;
static AEGfxTexture* tutorial_tex;
static AEGfxTexture* level1_tex;
static AEGfxTexture* level2_tex;
static AEGfxTexture* level3_tex;
static AEGfxTexture* upgrade_tex;

static AEGfxTexture* back_select_tex;
static AEGfxTexture* tutorial_select_tex;
static AEGfxTexture* level1_select_tex;
static AEGfxTexture* level2_select_tex;
static AEGfxTexture* level3_select_tex;
static AEGfxTexture* upgrade_select_tex;

static AEGfxVertexList* mesh_level_button = 0;
static AEGfxVertexList* mesh_bg = 0;
static Camera cam;
static ButtonManager level_select_buttons;

static void tutorial_stage(void);
static void level1_stage(void);
static void level2_stage(void);
static void level3_stage(void);
static void upgrade_stage(void);
static void mainmenu_stage(void);

/*!*****************************************************************************
  \brief
	Loads all assets needed for the level select.
*******************************************************************************/
void level_select_load()
{
	mesh_bg = create_square_mesh(1.0f, 1.0f);
	AE_ASSERT_MESG(mesh_bg, "Failed to create mesh wallpaper!! \n");

	mesh_level_button = create_square_mesh(1.0f, 1.0f);;
	AE_ASSERT_MESG(mesh_level_button, "Failed to create mesh wallpaper!! \n");

	bg_texture = AEGfxTextureLoad("./Background/Level_Select/map.png");
	AE_ASSERT_MESG(bg_texture, "Failed to load bg_texture!! \n");

	tutorial_tex = AEGfxTextureLoad("./Background/Level_Select/tutorial.png");
	AE_ASSERT_MESG(tutorial_tex, "Failed to load tutorial_tex!! \n");
	tutorial_select_tex = AEGfxTextureLoad("./Background/Level_Select/tutorialselect.png");
	AE_ASSERT_MESG(tutorial_select_tex, "Failed to load tutorial_select_tex!! \n");

	level1_tex = AEGfxTextureLoad("./Background/Level_Select/level1.png");
	AE_ASSERT_MESG(level1_tex, "Failed to load background!! \n");
	level1_select_tex = AEGfxTextureLoad("./Background/Level_Select/level1select.png");
	AE_ASSERT_MESG(level1_select_tex, "Failed to load level1_select_tex!! \n");

	level2_tex = AEGfxTextureLoad("./Background/Level_Select/level2.png");
	AE_ASSERT_MESG(level2_tex, "Failed to load level2_tex!! \n");
	level2_select_tex = AEGfxTextureLoad("./Background/Level_Select/level2select.png");
	AE_ASSERT_MESG(level2_select_tex, "Failed to load level2_select_tex!! \n");

	level3_tex = AEGfxTextureLoad("./Background/Level_Select/level3.png");
	AE_ASSERT_MESG(level3_tex, "Failed to load background!! \n");
	level3_select_tex = AEGfxTextureLoad("./Background/Level_Select/level3select.png");
	AE_ASSERT_MESG(level3_select_tex, "Failed to load level3_select_tex!! \n");

	upgrade_tex = AEGfxTextureLoad("./Background/Level_Select/weapon_upgrade.png");
	AE_ASSERT_MESG(upgrade_tex, "Failed to load background!! \n");
	upgrade_select_tex = AEGfxTextureLoad("./Background/Level_Select/select_weapon_upgrade.png");
	AE_ASSERT_MESG(upgrade_select_tex, "Failed to load upgrade_select_tex!! \n");

	back_tex = AEGfxTextureLoad("./UI/button_unshade.png");
	AE_ASSERT_MESG(back_tex, "Failed to load background!! \n");
	back_select_tex = AEGfxTextureLoad("./UI/button_shade.png");
	AE_ASSERT_MESG(back_select_tex, "Failed to load upgrade_select_tex!! \n");

	bg_sprite = new Sprite(bg_texture, mesh_bg, { (f32)AEGetWindowWidth(), (f32)AEGetWindowHeight() });
	tutorial_sprite = new Sprite(tutorial_tex, mesh_level_button, button_scale);
	level1_sprite = new Sprite(level1_tex, mesh_level_button, button_scale);
	level2_sprite = new Sprite(level2_tex, mesh_level_button, button_scale);
	level3_sprite = new Sprite(level3_tex, mesh_level_button, button_scale);
	upgrade_sprite = new Sprite(upgrade_tex, mesh_level_button, { 100.0f,100.0f });
	back_sprite = new Sprite(back_tex, mesh_level_button, { 350.0f,70.0f });
}

/*!*****************************************************************************
  \brief
	Initialize the buttons needed for level select.
*******************************************************************************/
void level_select_initialize()
{
	cam.init();

	Button tutorial_button(tut_pos, button_scale, tutorial_sprite, tutorial_select_tex, tutorial_tex);
	tutorial_button.on_click = tutorial_stage;
	tutorial_button.set_text("Tutorial", -55.0f);

	Button level1_button(level1_pos, button_scale, level1_sprite, level1_select_tex, level1_tex);
	level1_button.on_click = level1_stage;
	level1_button.set_text("Level 1", -55.0f);
	level1_button.text_offset_y = 40.0f;
	level1_button.button_type = BUTTON_TYPE::BUTTON_INACTIVE;

	Button level2_button(level2_pos, button_scale, level2_sprite, level2_select_tex, level2_tex);
	level2_button.on_click = level2_stage;
	level2_button.set_text("Level 2", -55.0f);
	level2_button.text_offset_y = 20.0f;
	level2_button.button_type = BUTTON_TYPE::BUTTON_INACTIVE;

	Button level3_button(level3_pos, button_scale, level3_sprite, level3_select_tex, level3_tex);
	level3_button.on_click = level3_stage;
	level3_button.set_text("Level 3", -55.0f);
	level3_button.text_offset_y = 40.0f;
	level3_button.button_type = BUTTON_TYPE::BUTTON_INACTIVE;

	Button upgrade_button(upgrade_pos, { 100.0f,50.0f }, upgrade_sprite, upgrade_select_tex, upgrade_tex);
	upgrade_button.on_click = upgrade_stage;
	upgrade_button.set_text("Weapon Upgrade", 50.0f);

	Button back_button(back_pos, { 350.0f,70.0f }, back_sprite, back_select_tex, back_tex);
	back_button.on_click = mainmenu_stage;
	back_button.set_text("Return to Main Menu", -150.0f);

	u32 level_cleared = PlayerData::instance()->getlevel_cleared();
	if (level_cleared >= 1)
	{
		level1_button.button_type = BUTTON_TYPE::BUTTON_NORMAL;
	}
	if (level_cleared >= 2)
	{
		level2_button.button_type = BUTTON_TYPE::BUTTON_NORMAL;
	}
	if (level_cleared >= 3)
	{
		level3_button.button_type = BUTTON_TYPE::BUTTON_NORMAL;
	}

	level_select_buttons.buttons.emplace_back(tutorial_button);
	level_select_buttons.buttons.emplace_back(level1_button);
	level_select_buttons.buttons.emplace_back(level2_button);
	level_select_buttons.buttons.emplace_back(level3_button);
	level_select_buttons.buttons.emplace_back(upgrade_button);
	level_select_buttons.buttons.emplace_back(back_button);
}

/*!*****************************************************************************
  \brief
	When called, it will transit to tutorial stage.
*******************************************************************************/
void tutorial_stage()
{
	transition(1.0f, GS_TUTORIAL);
}

/*!*****************************************************************************
  \brief
	When called, it will transit to level 1.
*******************************************************************************/
void level1_stage()
{
	transition(1.0f, GS_LEVEL1);
}

/*!*****************************************************************************
  \brief
	When called, it will transit to level 2.
*******************************************************************************/
void level2_stage()
{
	transition(1.0f, GS_LEVEL2);
}

/*!*****************************************************************************
  \brief
	When called, it will transit to level 3.
*******************************************************************************/
void level3_stage()
{
	transition(1.0f, GS_LEVEL3);
}

/*!*****************************************************************************
  \brief
	When called, it will transit to upgrade menu.
*******************************************************************************/
void upgrade_stage()
{
	transition(1.0f, GS_UPGRADE);
}

/*!*****************************************************************************
  \brief
	When called, it will transit to main menu.
*******************************************************************************/
void mainmenu_stage(void)
{
	transition(1.0f, GS_MAINMENU);
}

/*!*****************************************************************************
  \brief
	Update level select buttons.
*******************************************************************************/
void level_select_update()
{
	level_select_buttons.update();
	if (AEInputCheckTriggered(AEVK_ESCAPE))
	{
		transition(1.5f, GS_MAINMENU);
	}
}

/*!*****************************************************************************
  \brief
	Draw the level select background and buttons.
*******************************************************************************/
void level_select_draw()
{
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);
	draw_non_relative(bg_sprite, { 0, 0 }, { (f32)AEGetWindowWidth(), (f32)AEGetWindowHeight() });
	level_select_buttons.draw();
	transition();
}

/*!*****************************************************************************
  \brief
	Free up all the buttons.
*******************************************************************************/
void level_select_free()
{
	level_select_buttons.buttons.clear();
}

/*!*****************************************************************************
  \brief
	Unload all sprites and textures and assets used to implement level select.
*******************************************************************************/
void level_select_unload()
{
	delete bg_sprite;
	delete tutorial_sprite;
	delete level1_sprite;
	delete level2_sprite;
	delete level3_sprite;
	delete upgrade_sprite;
	delete back_sprite;
	AEGfxTextureUnload(bg_texture);
	AEGfxTextureUnload(tutorial_tex);
	AEGfxTextureUnload(back_tex);
	AEGfxTextureUnload(level1_tex);
	AEGfxTextureUnload(level2_tex);
	AEGfxTextureUnload(level3_tex);
	AEGfxTextureUnload(upgrade_tex);
	AEGfxTextureUnload(tutorial_select_tex);
	AEGfxTextureUnload(back_select_tex);
	AEGfxTextureUnload(level1_select_tex);
	AEGfxTextureUnload(level2_select_tex);
	AEGfxTextureUnload(level3_select_tex);
	AEGfxTextureUnload(upgrade_select_tex);
	AEGfxMeshFree(mesh_bg);
	AEGfxMeshFree(mesh_level_button);
}