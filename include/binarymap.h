/*!*************************************************************************
****
\file   binarymap.h
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Mar 29 2021

\brief
This header file outlines an implementation of the binary map in the
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
#pragma once
#include <AEEngine.h>
#include <fstream>
#include "level_manager.h"

enum class BINARY_OBJECT : int
{
	TYPE_EMPTY,
	TYPE_PLATFORM,
	TYPE_HERO
};

class BinaryMap
{
	int** MapData;
	int map_width;
	int map_height;
public:
	BinaryMap() = delete;
	BinaryMap(const char*);
	BinaryMap(BinaryMap const&) = delete;
	BinaryMap& operator=(BinaryMap const&) = delete;
	~BinaryMap();
	void load_platforms(Level*);
};