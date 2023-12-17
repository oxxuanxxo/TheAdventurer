/*!*************************************************************************
****
\file   binarymap.cpp
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Mar 29 2021

\brief
This file includes the implementation of the binary map in the
game engine. This system is mainly used to generate platforms and set player
position in a level.

The functions include:
- load_platforms

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/
#include "binarymap.h"

/******************************************************************************/
/*!
	This function frees the memory that was allocated for the 2 arrays MapData
	& BinaryCollisionArray which was allocated in the "ImportMapDataFromFile"
	function
*/
/******************************************************************************/
BinaryMap::~BinaryMap()
{
	for (int i{ 0 }; i < map_height; ++i)
	{
		delete[] MapData[i];
	}
	delete[] MapData;
}

/******************************************************************************/
/*!
	Opens the file name "FileName" and retrieves all the map data.
	This function opens the file name "FileName" and retrieves all the map data.
	It allocates memory for the 2 arrays: MapData & BinaryCollisionArray
	The first line in this file is the width of the map.
	The second line in this file is the height of the map.
	The remaining part of the file is a series of numbers
	Each number represents the ID (or value) of a different element in the
	double dimensionaly array.

	Finally, the function returns 1 if the file named "FileName" exists,
	otherwise it returns 0
*/
/******************************************************************************/
BinaryMap::BinaryMap(const char* FileName) : MapData{ 0 }, map_width{ 0 }, map_height{ 0 }
{
	std::ifstream file;
	file.open(FileName, std::ios::in);
	if (file.is_open())
	{
		file.ignore(6) >> map_width;
		file.ignore(8) >> map_height;
		file.ignore(7) >> Level::level_scale.x;
		file.ignore(7) >> Level::level_scale.y;
		MapData = new int* [map_height];
		for (int i{ 0 }; i < map_height; ++i)
		{
			MapData[i] = new int[map_width];
			for (int j{ 0 }; j < map_width; ++j)
			{
				file.ignore();
				file >> MapData[i][j];
			}
		}
		file.close();
	}
	// Include assert
}

/**
 * @brief Loads the platform to the level
 * As the game was developed in an unnormalized coordinate system, the 
 * position of the binary map have to be scaled by the level_scale of the 
 * level object
 * 
 * @param level 
 * Pointer to the level
 */
void BinaryMap::load_platforms(Level* level)
{
	AEVec2 pos;
	for (int i{ 0 }; i < map_width; ++i)
	{
		for (int j{ 0 }; j < map_height; ++j)
		{
			// Change pos to scale to game world
			pos = { ((f32)i + 0.5f) * Level::level_scale.x,
				((f32)j + 0.5f) * Level::level_scale.y };
			if (MapData[j][i] == static_cast<int>(BINARY_OBJECT::TYPE_PLATFORM))
			{
				level->add_platform(pos);
			}
			if (MapData[j][i] == static_cast<int>(BINARY_OBJECT::TYPE_HERO))
			{
				level->player->init_pos = pos;
			}
		}
	}
}