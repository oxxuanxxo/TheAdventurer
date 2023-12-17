/*!*************************************************************************
****
\file   player_data.cpp
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Mar 22 2021

\brief
This file includes the implementation of the player data class.

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
#include "player_data.h"
#include "level_manager.h"
#include "filewritestream.h"
#include <writer.h>

static s8 const * filepath = "./Data/player.json";
PlayerData* PlayerData::s_instance;

/*!*****************************************************************************
 * @brief Construct a new Player Data:: Player Data object
 * 
*******************************************************************************/
PlayerData::PlayerData()
	: player{ Level::player }, sword_upgrade{0}, 
	spear_upgrade{0}, num_upgrades{0}, level_cleared{0} {}

/*!*****************************************************************************
 * @brief Resets the player data to 0
 * 
*******************************************************************************/
void PlayerData::reset_data()
{
	sword_upgrade = 0;
	spear_upgrade = 0;
	num_upgrades = 0;
	level_cleared = 0;
}

/*!*****************************************************************************
 * @brief Load the player data from a file
 * 
 * @param path 
*******************************************************************************/
void PlayerData::load_data(s8 const* path)
{
	FILE* fp;
	rapidjson::Document d;
	errno_t err;
	err = fopen_s(&fp, path, "rb");
	if (!err)
	{
		char* readBuffer = new char[FILE_BUFFER];
		rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
		d.ParseStream(is);
		load_data(d);
		delete[] readBuffer;
		if (fp)
			fclose(fp);
	}
}

/*!*****************************************************************************
 * @brief Load the player data from a rapidjson document
 * 
 * @param d 
 * RapidJson document object
*******************************************************************************/
void PlayerData::load_data(rapidjson::Document const& d)
{
	AE_ASSERT(d.IsObject());
	sword_upgrade = d["SwordUpgrades"].GetUint();
	spear_upgrade = d["SpearUpgrades"].GetUint();
	num_upgrades = d["NumUpgrades"].GetUint();
	level_cleared = d["LevelCleared"].GetUint();
}

/*!*****************************************************************************
 * @brief Sets the player object's sword and spear upgrade according to the
 * ones in PlayerData
 * 
*******************************************************************************/
void PlayerData::set_player()
{
	player = Level::player;
	player->sword_upgrade = static_cast<u8>(sword_upgrade);
	player->spear_upgrade = static_cast<u8>(spear_upgrade);
}

/*!*****************************************************************************
 * @brief Upgrade the sword
 * If there is upgrades available, increment sword_upgrade and decrement num_upgrades
 * 
 * @return true 
 * Successfully upgraded
 * @return false 
 * Upgrade is not successful
*******************************************************************************/
bool PlayerData::upgrade_sword()
{
	if (!num_upgrades)
		return false;
	++sword_upgrade;
	--num_upgrades;
	return true;
}

/*!*****************************************************************************
 * @brief Upgrade the spear
 * If there is upgrades available, increment spear_upgrade and decrement num_upgrades
 * 
 * @return true 
 * Successfully upgraded
 * @return false 
 * Upgrade unsuccessful
*******************************************************************************/
bool PlayerData::upgrade_spear()
{
	if (!num_upgrades)
		return false;
	++spear_upgrade;
	--num_upgrades;
	return true;
}

/*!*****************************************************************************
 * @brief Increment num_upgrades and level_cleared if the player have not completed
 * the particular level yet
 * 
 * @param level 
 * Unsigned int representing the level value
*******************************************************************************/
void PlayerData::level_complete(u32 level)
{
	if (level > level_cleared)
	{
		++num_upgrades;
		++level_cleared;
	}
}

/*!*****************************************************************************
 * @brief Saves player data to a path
 * 
 * @param path 
 * File path to save to
*******************************************************************************/
void PlayerData::save_data(s8 const* path)
{
	using namespace rapidjson;
	FILE* fp;
	errno_t err;
	err = fopen_s(&fp, path, "wb+");
	if (!err)
	{
		char* write_buffer = new char[FILE_BUFFER];
		rapidjson::FileWriteStream os(fp, write_buffer, sizeof(write_buffer));
		Writer<FileWriteStream> writer(os);
		writer.StartObject();
		writer.Key("SwordUpgrades");
		writer.Uint(sword_upgrade);
		writer.Key("SpearUpgrades");
		writer.Uint(spear_upgrade);
		writer.Key("NumUpgrades");
		writer.Uint(num_upgrades);
		writer.Key("LevelCleared");
		writer.Uint(level_cleared);
		writer.EndObject();
		delete[] write_buffer;
		if (fp)
			fclose(fp);
	}
}