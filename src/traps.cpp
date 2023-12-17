/*!*****************************************************************************
\file   traps.cpp
\author Chua Yip Xuan
\par    DP email: yipxuan.chua\@digipen.edu
\date   March 30 2021

\brief
	This source file contains the function implemented in traps.h

	The source file has functions that consists of:
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

#include "traps.h"
#include "attack.h"
#include "globals.h"
#include <iostream>
#include "level_manager.h"

static Sprite* spike_sprite;
static Sprite* lava_sprite;
static Sprite* icicles_sprite;
static gameObject* traps_obj;

AEGfxVertexList* mesh_traps = 0;
AEGfxTexture* tex_traps_spike;
AEGfxTexture* tex_traps_lava;
AEGfxTexture* tex_traps_icicles;

/*!*****************************************************************************
  \brief
	Default constructor of the traps
	Default trap: killzone
*******************************************************************************/
Traps::Traps() : gameObject(), damage(100)
{
	on_collide = traps_collide;
	type = GO_TYPE::GO_TRAPS;
	scale = { TRAPS_SIZE ,TRAPS_SIZE };
	layer = TRAPS_LAYER;
	traps_type = TRAPS_TYPE::TRAPS_KILLZONE;
	is_static = true;
}

/*!*****************************************************************************
  \brief
	Parameterized traps constructor
*******************************************************************************/
Traps::Traps(AEVec2 init_pos, AEVec2 scale, TRAPS_TYPE traps_type, Sprite* spr)
	:damage(100)
{
	type = GO_TYPE::GO_TRAPS;
	this->init_pos = init_pos;
	this->scale = scale;
	this->obj_sprite = spr;
	this->traps_type = traps_type;
}

/*!*****************************************************************************
  \brief
	Default spikes constructor
*******************************************************************************/
SpikeTraps::SpikeTraps() : Traps()
{
	this->damage = 1;
	traps_type = TRAPS_TYPE::TRAPS_SPIKE;
	obj_sprite = new Sprite(tex_traps_spike, mesh_traps, { 50.0f,50.0f });
}

/*!*****************************************************************************
  \brief
	Parameterized spikes constructor
*******************************************************************************/
SpikeTraps::SpikeTraps(AEVec2 init_pos, AEVec2 scale, TRAPS_TYPE traps_type, Sprite* spr)
	: Traps(init_pos, scale, traps_type, spr)
{
	this->damage = 1;
	traps_type = TRAPS_TYPE::TRAPS_SPIKE;
	obj_sprite = new Sprite(tex_traps_spike, mesh_traps, { 50.0f,50.0f });
}

/*!*****************************************************************************
  \brief
	Default lava traps constructor
*******************************************************************************/
LavaTraps::LavaTraps() : Traps()
{
	this->damage = 1;
	obj_sprite = new Sprite(tex_traps_lava, mesh_traps, { 50.0f,50.0f });
	traps_type = TRAPS_TYPE::TRAPS_FIRE;
}

/*!*****************************************************************************
  \brief
	Parameterized lava traps constructor
*******************************************************************************/
LavaTraps::LavaTraps(AEVec2 init_pos, AEVec2 scale, TRAPS_TYPE traps_type, Sprite* spr)
	: Traps(init_pos, scale, traps_type, spr)
{
	this->damage = 1;
	obj_sprite = new Sprite(tex_traps_lava, mesh_traps, { 50.0f,50.0f });
	traps_type = TRAPS_TYPE::TRAPS_FIRE;
}

/*!*****************************************************************************
  \brief
	Default icicles traps constructor
*******************************************************************************/
IciclesTraps::IciclesTraps() : Traps(), outer_bound()
{
	this->damage = 1;
	obj_sprite = new Sprite(tex_traps_icicles, mesh_traps, { 50.0f,50.0f });
	traps_type = TRAPS_TYPE::TRAPS_ICICLES;
	on_collide = traps_collide;
}

/*!*****************************************************************************
  \brief
	Parameterized icicles traps constructor
*******************************************************************************/
IciclesTraps::IciclesTraps(AEVec2 init_pos, AEVec2 scale, TRAPS_TYPE traps_type, Sprite* spr)
	: Traps(init_pos, scale, traps_type, spr), outer_bound()
{
	this->damage = 1;
	obj_sprite = new Sprite(tex_traps_icicles, mesh_traps, { 50.0f,50.0f });
	traps_type = TRAPS_TYPE::TRAPS_ICICLES;
	on_collide = traps_collide;
}

/*!*****************************************************************************
  \brief
	Traps destructor
*******************************************************************************/
Traps::~Traps()
{
	if (traps_type != TRAPS_TYPE::TRAPS_KILLZONE)
	{
		delete obj_sprite;
	}
}

/*!*****************************************************************************
  \brief
	Traps bounding box
*******************************************************************************/
void Traps::set_aabb()
{
	AEVec2 min, max;
	AEVec2Set(&min, -0.5f * scale.x, -0.5f * scale.y);
	AEVec2Set(&max, 0.5f * scale.x, 0.5f * scale.y);
	AEVec2Add(&min, &min, &curr_pos);
	AEVec2Add(&max, &max, &curr_pos);
	bounding_box.min.x = AEMin(min.x, max.x);
	bounding_box.min.y = AEMin(min.y, max.y);
	bounding_box.max.x = AEMax(min.x, max.x);
	bounding_box.max.y = AEMax(min.y, max.y);
}

/*!*****************************************************************************
  \brief
	Icicles bounding box
*******************************************************************************/
void IciclesTraps::set_aabb()
{
	AEVec2Set(&outer_bound.min, -0.5f * scale.x, -5.0f * scale.y);
	AEVec2Set(&outer_bound.max, 0.5f * scale.x, 5.0f * scale.y);
	AEVec2Add(&outer_bound.min, &outer_bound.min, &curr_pos);
	AEVec2Add(&outer_bound.max, &outer_bound.max, &curr_pos);

	AEVec2Set(&bounding_box.min, -0.5f * scale.x, -0.5f * scale.y);
	AEVec2Set(&bounding_box.max, 0.5f * scale.x, 0.5f * scale.y);
	AEVec2Add(&bounding_box.min, &bounding_box.min, &curr_pos);
	AEVec2Add(&bounding_box.max, &bounding_box.max, &curr_pos);
}

/*!*****************************************************************************
  \brief
	This function loads all the assets for traps
*******************************************************************************/
void traps_load()
{
	mesh_traps = create_square_mesh(1.0f, 1.0f);
	//spikes
	tex_traps_spike = AEGfxTextureLoad("./Sprites/spikes.png");
	AE_ASSERT_MESG(tex_traps_spike, "Failed to create traps texture!!");

	tex_traps_lava = AEGfxTextureLoad("./Sprites/lava_trap.png");
	AE_ASSERT_MESG(tex_traps_spike, "Failed to create traps texture!!");

	tex_traps_icicles = AEGfxTextureLoad("./Sprites/icicles_trap.png");
	AE_ASSERT_MESG(tex_traps_spike, "Failed to create traps texture!!");
}

/*!*****************************************************************************
  \brief
	This function frees all the assets for traps
*******************************************************************************/
void traps_free()
{
}

/*!*****************************************************************************
  \brief
	This function unloads all the assets for traps
*******************************************************************************/
void traps_unload()
{
	delete spike_sprite;
	delete lava_sprite;
	delete icicles_sprite;
	AEGfxMeshFree(mesh_traps);
	AEGfxTextureUnload(tex_traps_spike);
	AEGfxTextureUnload(tex_traps_lava);
	AEGfxTextureUnload(tex_traps_icicles);
}

/*!*****************************************************************************
  \brief
	This function initialize all the variables for the traps
*******************************************************************************/
void Traps::game_object_initialize()
{
	curr_pos = init_pos;
}

/*!*****************************************************************************
  \brief
	This function updates all the variables for the traps
*******************************************************************************/
void Traps::game_object_update()
{
	if (traps_type == TRAPS_TYPE::TRAPS_ICICLES)
	{
		IciclesTraps* tmp = dynamic_cast<IciclesTraps*>(this);
		if (Level::player->curr_pos.x > tmp->outer_bound.min.x
			&& Level::player->curr_pos.x < tmp->outer_bound.max.x
			&& Level::player->curr_pos.y > tmp->outer_bound.min.y
			&& Level::player->curr_pos.y < tmp->outer_bound.max.y)
		{
			acceleration.y = LEVEL_GRAVITY;
			/*if (on_ground)
			{
				this->flag = false;
			}*/
			obj_sprite->tint = { 1.0f, 0.0f, 0.1f, 1.0f };
		}
		AEVec2Add(&curr_vel, &curr_vel, &acceleration);
		AEVec2 dt_vel;
		AEVec2Scale(&dt_vel, &curr_vel, G_DELTATIME);
		AEVec2Add(&curr_pos, &curr_pos, &dt_vel);
	}
}

/*!*****************************************************************************
  \brief
	This function draws the traps
*******************************************************************************/
void Traps::game_object_draw()
{
	if (traps_type == TRAPS_TYPE::TRAPS_SPIKE)
	{
		obj_sprite->mesh = mesh_traps;
		obj_sprite->texture = tex_traps_spike;
		obj_sprite->set_size(read_png_size("./Sprites/spikes.png"));
		draw(obj_sprite, curr_pos, scale);
	}
	if (traps_type == TRAPS_TYPE::TRAPS_KILLZONE)
	{
		draw(nullptr, curr_pos, scale);
	}
	if (traps_type == TRAPS_TYPE::TRAPS_FIRE)
	{
		obj_sprite->mesh = mesh_traps;
		obj_sprite->texture = tex_traps_lava;
		obj_sprite->set_size(read_png_size("./Sprites/lava_trap.png"));
		draw(obj_sprite, curr_pos, scale);
	}
	if (traps_type == TRAPS_TYPE::TRAPS_ICICLES)
	{
		obj_sprite->mesh = mesh_traps;
		obj_sprite->texture = tex_traps_icicles;
		obj_sprite->set_size(read_png_size("./Sprites/icicles_trap.png"));
		draw(obj_sprite, curr_pos, scale);
	}
}

/*!*****************************************************************************
  \brief
	This function sets the oncollide for the traps, if oncollide, the traps
	will carry out certain behaviour.
*******************************************************************************/
void traps_collide(gameObject& traps_obj)
{
	Traps* traps = dynamic_cast<Traps*>(&traps_obj);
	vector<gameObject*>& colliders = traps_obj.colliders;

	for (vector<gameObject*>::iterator i = colliders.begin(); i != colliders.end(); ++i)
	{
		gameObject& obj_col = **i;

		if (((obj_col.type == GO_TYPE::GO_PLATFORM) && (traps->traps_type == TRAPS_TYPE::TRAPS_ICICLES)) ||
			((obj_col.type == GO_TYPE::GO_PLAYER) && (traps->traps_type == TRAPS_TYPE::TRAPS_ICICLES)))
		{
			traps->flag = false;
		}
	}
}