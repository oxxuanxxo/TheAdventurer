/*!*****************************************************************************
\file   traps.h
\author Chua Yip Xuan
\par    DP email: yipxuan.chua\@digipen.edu
\date   March 30 2021

\brief
	This header file contains the implmentation of the different types of spikes
	to be used in different levels.

	The header file has functions that consists of:
	- set_aabb
	- game_object_initialize
	- game_object_update
	- game_object_draw
	- traps_load
	- traps_free
	- traps_unload
	- traps_collide

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************************/
#pragma once
#include "main.h"
#include "gameobject.h"

constexpr f32 TRAPS_SIZE = 25.0f;
constexpr u32 TRAPS_LAYER = 2; //2 or 3?

enum class TRAPS_TYPE {
	TRAPS_KILLZONE, //killzone
	TRAPS_SPIKE,
	TRAPS_ICICLES,
	TRAPS_FIRE
};

class Traps : public gameObject {
private:
public:
	s32 damage;
	TRAPS_TYPE traps_type;
	Traps();
	Traps(AEVec2 init_pos, AEVec2 scale, TRAPS_TYPE traps_type, Sprite* spr);
	~Traps();

	void set_aabb();
	void game_object_initialize() override;
	void game_object_update() override;
	void game_object_draw() override;
};

class SpikeTraps : public Traps
{
public:
	SpikeTraps();
	SpikeTraps(AEVec2 init_pos, AEVec2 scale, TRAPS_TYPE traps_type, Sprite* spr);
};

class LavaTraps : public Traps
{
public:
	LavaTraps();
	LavaTraps(AEVec2 init_pos, AEVec2 scale, TRAPS_TYPE traps_type, Sprite* spr);
};
class IciclesTraps : public Traps
{
public:
	AABB outer_bound;
	void set_aabb() override;
public:
	IciclesTraps();
	IciclesTraps(AEVec2 init_pos, AEVec2 scale, TRAPS_TYPE traps_type, Sprite* spr);
};

void traps_load();
void traps_free();
void traps_unload();
void traps_collide(gameObject& obj);