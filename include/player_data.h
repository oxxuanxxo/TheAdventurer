/*!*************************************************************************
****
\file   player_data.h
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Mar 22 2021

\brief
This header file outlines an implementation of the player data class.

The functions include:
- reset_data
- load_data
- load_data
- set_player
- upgrade_sword
- upgrade_spear
- level_complete
- save_data
- get_sword_upgrade
- get_spear_upgrade
- get_num_upgrades
- getlevel_cleared

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/

#pragma once
#include "main.h"
#include "player.h"
#include "rapidjson.h"
#include "document.h"

class PlayerData
{
	Player* player;
	static PlayerData* s_instance;
	u32 sword_upgrade;
	u32 spear_upgrade;
	u32 num_upgrades;
	u32 level_cleared;
public:
	PlayerData();
	static PlayerData* instance()
	{
		if (!s_instance)
			s_instance = new PlayerData;
		return s_instance;
	};
	static void release_instance()
	{
		if (s_instance)
			delete s_instance;
	};
	void reset_data();
	void load_data(s8 const *);
	void load_data(rapidjson::Document const&);
	void set_player();
	bool upgrade_sword();
	bool upgrade_spear();
	void level_complete(u32 level);
	void save_data(s8 const *);
	u32 get_sword_upgrade() { return sword_upgrade; }
	u32 get_spear_upgrade() { return spear_upgrade; }
	u32 get_num_upgrades() { return num_upgrades; };
	u32 getlevel_cleared()
	{
		return level_cleared;
	}
};