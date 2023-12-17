/*!*************************************************************************
****
\file   upgrade.cpp
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Jan 24 2021

\brief
This program is an implementation of the tutorial.

The functions include:
- upgrade_wep
- switch_wep
- level_select
- print_descriptions
- upgrade_init
- upgrade_load
- upgrade_update
- upgrade_draw
- upgrade_free
- upgrade_unload

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/
#include "main.h"
#include "player_data.h"
#include "button.h"
#include "upgrade.h"
#include <fstream>

static AEGfxVertexList* square_mesh = 0;

static bool curr_wep;
static u32 sword_upgrade;
static u32 spear_upgrade;
static u32 num_upgrades;
static std::string filepath = "./Data/player.json";
static const std::string sword_desc = "./Data/sword_desc.txt";
static const std::string spear_desc = "./Data/spear_desc.txt";
static std::vector<std::string> sword_description;
static std::vector<std::string> spear_description;

static AEGfxTexture* tex_bg;
static AEGfxTexture* tex_swordshield_icon;
static AEGfxTexture* tex_spear_icon;
static AEGfxTexture* upgrade_btn;
static AEGfxTexture* pTex_button_normal;
static AEGfxTexture* pTex_button_hover;
static AEGfxTexture* upgraded_wep;
static AEGfxTexture* unupgraded_wep;
static Sprite* button_sprite_1;
static Sprite* button_sprite_2;
static Sprite* button_sprite_3;
static vector<Sprite*> v_upgrade_lvl;
static Sprite* wep_icon_spr;
static Sprite* upgrade_desc_spr;
static ButtonManager btn_mgr;

/******************************************************************************
 * @brief Upgrades the appropriate weapon based on the current weapon
 * 
******************************************************************************/
static void upgrade_wep()
{
	if (curr_wep)
		PlayerData::instance()->upgrade_spear();
	else
		PlayerData::instance()->upgrade_sword();
}

/******************************************************************************
 * @brief Switches the current weapon
 * 
******************************************************************************/
static void switch_wep()
{
	curr_wep = !curr_wep;
}

/******************************************************************************
 * @brief Saves the playerdata and transition to level select
 * 
******************************************************************************/
static void level_select()
{
	PlayerData::instance()->save_data(filepath.c_str());
	transition(1.0f, GS_LEVELSELECT);
}

/******************************************************************************
 * @brief Print the descriptions of the weapon upgrades and set the icon to 
 * the textures based on if the player has that upgrade.
 * 
 * @param weapon 
 * Boolean data type representing the currently viewed weapon
******************************************************************************/
static void print_descriptions(bool weapon)
{
	AEVec2 pos = { -120.0f, 0 };
	for (u32 i = 0; i < 3; ++i)
	{
		pos.y = 90.0f - 150.0f * i;
		AEVec2 upgrade_lvl_pos = { pos.x - 360.0f, pos.y };
		if (weapon)
		{
			if (i < spear_upgrade)
			{
				v_upgrade_lvl[i]->texture = upgraded_wep;
			}
			else
			{
				v_upgrade_lvl[i]->texture = unupgraded_wep;
			}
			text_print(desc_font, spear_description[i], &pos, { -270.0f, 5.0f }, 1.0f, { 0.0f,0.0f,0.0f,1.0f });
		}
		else
		{
			if (i < sword_upgrade)
			{
				v_upgrade_lvl[i]->texture = upgraded_wep;
			}
			else
			{
				v_upgrade_lvl[i]->texture = unupgraded_wep;
			}
			text_print(desc_font, sword_description[i], &pos, { -270.0f, 5.0f }, 1.0f, { 0.0f,0.0f,0.0f,1.0f });
		}
		draw_non_relative(v_upgrade_lvl[i], upgrade_lvl_pos, v_upgrade_lvl[i]->scale);
	}
}

/******************************************************************************
 * @brief Load function of the upgrade game state
 * Load the sword and spear upgrade descriptions.Then, load the appropriate 
 * textures, mesh and sprites required for this game state.
 * 
******************************************************************************/
void upgrade_load()
{
	std::ifstream sword_file(sword_desc);
	AE_ASSERT_MESG(sword_file.is_open(), "Failed to open sword description!!\n");
	while (!sword_file.eof())
	{
		std::string temp;
		std::getline(sword_file, temp);
		sword_description.emplace_back(temp);
	}
	sword_file.close();

	std::ifstream spear_file(spear_desc);
	AE_ASSERT_MESG(spear_file.is_open(), "Failed to open spear description!!\n");
	while (!spear_file.eof())
	{
		std::string temp;
		std::getline(spear_file, temp);
		spear_description.emplace_back(temp);
	}
	spear_file.close();

	pTex_button_hover = AEGfxTextureLoad("./UI/button_shade.png");
	AE_ASSERT_MESG(pTex_button_hover, "Failed to create shade texture!! \n");

	pTex_button_normal = AEGfxTextureLoad("./UI/button_unshade.png");
	AE_ASSERT_MESG(pTex_button_normal, "Failed to create unshaded texture!! \n");

	upgraded_wep = AEGfxTextureLoad("./UI/Upgrade/upgradeds.png");
	AE_ASSERT_MESG(upgraded_wep, "Failed to create upgraded texture!! \n");

	unupgraded_wep = AEGfxTextureLoad("./UI/Upgrade/not_upgradeds.png");
	AE_ASSERT_MESG(unupgraded_wep, "Failed to create not upgraded texture!! \n");

	tex_swordshield_icon = AEGfxTextureLoad("./Sprites/icon_sword_shield.png");
	AE_ASSERT_MESG(tex_swordshield_icon, "Failed to create sword shield icon texture!! \n");

	tex_spear_icon = AEGfxTextureLoad("./Sprites/icon_spear.png");
	AE_ASSERT_MESG(tex_spear_icon, "Failed to create spear icon texture!! \n");

	upgrade_btn = AEGfxTextureLoad("./UI/Upgrade/anvil.png");
	AE_ASSERT_MESG(upgrade_btn, "Failed to create upgrade texture!! \n");

	tex_bg = AEGfxTextureLoad("./UI/Upgrade/background.png");
	AE_ASSERT_MESG(upgrade_btn, "Failed to create bg texture!! \n");

	square_mesh = create_square_mesh(1.0f, 1.0f);

	AEVec2 upgrade_button_size = { 250.0f, 200.0f };
	AEVec2 button_size = { 350.0f, 75.0f };
	AEVec2 upgrade_lvl_size = { 75.0f, 75.0f };
	AEVec2 upgrade_icon_size = { 150.0f, 150.0f };
	AEVec2 desc_size = { 570.0f, 75.0f };
	button_sprite_1 = new Sprite(upgrade_btn, square_mesh, upgrade_button_size);
	button_sprite_2 = new Sprite(pTex_button_normal, square_mesh, button_size);
	button_sprite_3 = new Sprite(pTex_button_normal, square_mesh, button_size);
	v_upgrade_lvl.emplace_back(new Sprite(unupgraded_wep, square_mesh, upgrade_lvl_size));
	v_upgrade_lvl.emplace_back(new Sprite(unupgraded_wep, square_mesh, upgrade_lvl_size));
	v_upgrade_lvl.emplace_back(new Sprite(unupgraded_wep, square_mesh, upgrade_lvl_size));
	wep_icon_spr = new Sprite(tex_swordshield_icon, square_mesh, upgrade_icon_size);
	upgrade_desc_spr = new Sprite(pTex_button_normal, square_mesh, desc_size);
}

/******************************************************************************
 * @brief Initialize function of the upgrade game state
 * Initialize sword_upgrade, spear_upgrade, and num_upgrades based on the 
 * values in the PlayerData singleton object. Then, set the button variables 
 * and emplace them on the button manager.
 * 
******************************************************************************/
void upgrade_init()
{
	curr_wep = false;
	sword_upgrade = PlayerData::instance()->get_sword_upgrade();
	spear_upgrade = PlayerData::instance()->get_spear_upgrade();
	num_upgrades = PlayerData::instance()->get_num_upgrades();

	Button upgrade_button({ 350, -50 }, button_sprite_1->scale, button_sprite_1, upgrade_btn, upgrade_btn);
	upgrade_button.on_click = upgrade_wep;
	upgrade_button.set_text("Upgrade", -35.0f, 15.0f);
	if (!num_upgrades)
		upgrade_button.button_type = BUTTON_TYPE::BUTTON_INACTIVE;
	upgrade_button.font = desc_font;

	Button switch_wep_btn({ 350, 120 }, button_sprite_2->scale, button_sprite_2, pTex_button_hover, pTex_button_normal);
	switch_wep_btn.on_click = switch_wep;
	switch_wep_btn.text = "Switch Weapon";
	switch_wep_btn.text_offset_x = -110.0f;

	Button level_select_btn({ 350, -250 }, button_sprite_3->scale, button_sprite_3, pTex_button_hover, pTex_button_normal);
	level_select_btn.on_click = level_select;
	level_select_btn.text = "Back to Level Select";
	level_select_btn.text_offset_x = -135.0f;

	btn_mgr.buttons.emplace_back(upgrade_button);
	btn_mgr.buttons.emplace_back(switch_wep_btn);
	btn_mgr.buttons.emplace_back(level_select_btn);
}

/******************************************************************************
 * @brief Update function of the upgrade game state
 * Update sword_upgrade, spear_upgrade, and num_upgrades values based on the 
 * values in the PlayerData singleton object. Then, update the button manager.
 * Set the first button in the button manager if there are no more upgrades 
 * left.
 * 
******************************************************************************/
void upgrade_update()
{
	sword_upgrade = PlayerData::instance()->get_sword_upgrade();
	spear_upgrade = PlayerData::instance()->get_spear_upgrade();
	num_upgrades = PlayerData::instance()->get_num_upgrades();
	if (btn_mgr.buttons[0].button_type != BUTTON_TYPE::BUTTON_INACTIVE && !num_upgrades)
		btn_mgr.buttons[0].button_type = BUTTON_TYPE::BUTTON_INACTIVE;
	btn_mgr.update();
}

/******************************************************************************
 * @brief Draw function of the upgrade game state.
 * Draw the background, then draw all the UI elements.
 * 
******************************************************************************/
void upgrade_draw()
{
	std::string wep_level, upgrades_left;
	draw_non_relative(tex_bg, square_mesh, { 0,0 }, { 0,0 }, { 1.0f, 1.0f, 1.0f, 1.0f }, 1.0f, { 1280.0f, 720.0f });
	AEVec2 wep_icon_pos = { 350.0f, 275.0f };
	AEVec2 wep_level_pos = { -275.0f, 140.0f };
	btn_mgr.draw();
	print_descriptions(curr_wep);
	upgrades_left = "Upgrade Points Left: " + std::to_string(num_upgrades);
	if (curr_wep)
	{
		wep_icon_spr->texture = tex_spear_icon;
		wep_level = "Weapon Level: " + std::to_string(spear_upgrade);
	}
	else
	{
		wep_icon_spr->texture = tex_swordshield_icon;
		wep_level = "Weapon Level: " + std::to_string(sword_upgrade);
	}
	draw_non_relative(wep_icon_spr, wep_icon_pos, wep_icon_spr->scale);
	text_print(default_font, wep_level, &wep_level_pos);
	wep_icon_pos.x -= 100.0f;
	wep_icon_pos.y -= 480.0f;
	text_print(desc_font, upgrades_left, &wep_icon_pos);
	transition();
}

/******************************************************************************
 * @brief Free function of the upgrade game state
 * Clear the buttons in the button manager.
 * 
******************************************************************************/
void upgrade_free()
{
	btn_mgr.buttons.clear();
}

/******************************************************************************
 * @brief Unload function of the upgrade game state
 * Unloads background textures and assets loaded from the load function.
 * 
******************************************************************************/
void upgrade_unload()
{
	delete button_sprite_1;
	delete button_sprite_2;
	delete button_sprite_3;
	delete wep_icon_spr;
	delete upgrade_desc_spr;
	for (Sprite* s : v_upgrade_lvl)
		delete s;
	v_upgrade_lvl.clear();
	AEGfxTextureUnload(tex_swordshield_icon);
	AEGfxTextureUnload(tex_spear_icon);
	AEGfxTextureUnload(upgrade_btn);
	AEGfxTextureUnload(pTex_button_normal);
	AEGfxTextureUnload(pTex_button_hover);
	AEGfxTextureUnload(upgraded_wep);
	AEGfxTextureUnload(unupgraded_wep);
	AEGfxTextureUnload(tex_bg);
	AEGfxMeshFree(square_mesh);
}