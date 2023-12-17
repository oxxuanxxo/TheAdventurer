/*!*****************************************************************************
\file   collectables.cpp
\author Mohammad Hanif Koh Teck Wee
\par    DP email: mohammadhanifkoh.t\@digipen.edu
\date   March 12 2021

\brief
This file contains the implementation of all functions related Heart game
objects as outlined in collectables.h

The functions include:
- Heart::Heart (default constructor)
- heart_load
- Heart::game_object_initialize
- Heart::game_object_update
- heart_unload
- adjust_heart_movement
- collectables_collide
- spawn_heart

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************************/
#include "collectables.h"
#include "level_manager.h"

static AEGfxTexture* pTexHeart;
static AEGfxVertexList* pMeshHeart;
static Sprite* pSpriteHeart;
static AEVec2 HEART_SCALE = { 50.0f, 50.0f };
Audio* Heart::pick_up_sound;

/*!*****************************************************************************
  \brief
	Default constructor for the Heart class. Calls gameObject's default
	constructor and sets variables to the default values for hearts.
*******************************************************************************/
Heart::Heart()
	: gameObject()
{
	type = GO_TYPE::GO_COLLECTABLES;
	scale = HEART_SCALE;
	is_static = false;
	on_collide = collectables_collide;
}

/*!*****************************************************************************
  \brief
	Loads all assets required for the creation and the picking up of heart
	objects. Called once at the load function of each level.
*******************************************************************************/
void heart_load()
{
	pTexHeart = AEGfxTextureLoad("./Sprites/fullheart.png");
	AE_ASSERT_MESG(pTexHeart, "Failed to create heart texture!");
	pMeshHeart = create_square_mesh(1.0f, 1.0f);
	AE_ASSERT_MESG(pMeshHeart, "Failed to create heart mesh!");
	Heart::pick_up_sound = new Audio("./Audio/pickup.wav", AUDIO_GROUP::SFX);
	pSpriteHeart = new Sprite(pTexHeart, pMeshHeart, HEART_SCALE);
}

/*!*****************************************************************************
  \brief
	Initialize function for Heart objects, overriding the default game object
	initialize function. Sets the sprite of each created sprite to the same
	sprite, and sets the initial velocity of the heart objects created
*******************************************************************************/
void Heart::game_object_initialize()
{
	obj_sprite = pSpriteHeart;
	curr_vel.y = 1000.0f;
}

/*!*****************************************************************************
  \brief
	Update function for Heart objects, overriding the default game object update
	function. Adds gravity to the Heart object's velocity and calculates its new
	position.
*******************************************************************************/
void Heart::game_object_update()
{
	acceleration.y += LEVEL_GRAVITY;
	AEVec2Add(&curr_vel, &curr_vel, &acceleration);
	AEVec2Zero(&acceleration);

	AEVec2 dt_vel;
	AEVec2Scale(&dt_vel, &curr_vel, G_DELTATIME);
	AEVec2Add(&curr_pos, &curr_pos, &dt_vel);
}

/*!*****************************************************************************
  \brief
	Unloads all assets and memory allocated for Heart Objects.
*******************************************************************************/
void heart_unload()
{
	AEGfxMeshFree(pMeshHeart);
	AEGfxTextureUnload(pTexHeart);
	delete Heart::pick_up_sound;
	delete pSpriteHeart;
}

/*!*****************************************************************************
  \brief
	Function to handle collision between Heart objects and platforms

  \param e_obj
	a reference to a game object, assumed to be the heart game object itself.

  \param obj
	a reference to a game object, assumed to be the platform that the heart is
	colliding with
*******************************************************************************/
void adjust_heart_movement(gameObject& e_obj, const gameObject& obj)
{
	Heart* heart = dynamic_cast<Heart*>(&e_obj);

	f32 heart_hw = (heart->bounding_box.max.x - heart->bounding_box.min.x) / 2;
	f32 heart_hh = (heart->bounding_box.max.y - heart->bounding_box.min.y) / 2;
	f32 obj_halfscale_x = (obj.bounding_box.max.x - obj.bounding_box.min.x) / 2;
	f32 obj_halfscale_y = (obj.bounding_box.max.y - obj.bounding_box.min.y) / 2;

	AEVec2 boss_middle{ heart->bounding_box.min.x + heart_hw, heart->bounding_box.min.y + heart_hh };

	AEVec2 diff_pos = { obj.bounding_box.min.x + obj_halfscale_x, obj.bounding_box.min.y + obj_halfscale_y };
	AEVec2Sub(&diff_pos, &boss_middle, &diff_pos);

	f32 min_distx = heart_hw + obj_halfscale_x;
	f32 min_disty = heart_hh + obj_halfscale_y;

	f32 depth_x = min_distx - fabs(diff_pos.x);

	f32 depth_y = min_disty - fabs(diff_pos.y);
	if (depth_x != 0 && depth_y != 0)
	{
		if (fabs(depth_x) < fabs(depth_y))
		{
			heart->curr_pos.x += diff_pos.x < 0 ? -depth_x : depth_x;
		}
		else
		{
			heart->curr_pos.y += diff_pos.y < 0 ? -depth_y : depth_y;
			heart->curr_vel.y = 0;

			if (diff_pos.y > 0)
			{
				heart->on_ground = true;
			}
		}
	}
}

/*!*****************************************************************************
  \brief
	Function to handle collision of hearts with other game objects. Checks
	through all objects the heart is currently colliding with, and calls the
	appropriate function to handle each collision.

  \param obj
	a reference to a game object, assumed to be the heart game object itself.
*******************************************************************************/
void collectables_collide(gameObject& obj)
{
	Heart* heart = dynamic_cast<Heart*>(&obj);
	vector<gameObject*>& colliders = obj.colliders;
	for (vector<gameObject*>::iterator i = colliders.begin(); i != colliders.end(); ++i)
	{
		gameObject& obj_col = **i;
		if (obj_col.type == GO_TYPE::GO_PLATFORM)
		{
			adjust_heart_movement(obj, obj_col);
		}
	}
}

/*!*****************************************************************************
  \brief
	Function to spawn a heart object randomly.

  \param enemy
	a reference to a game object, assumed to be the enemy who was just killed
	by the player.
*******************************************************************************/
void spawn_heart(gameObject& enemy)
{
	if (AERandFloat() > 0.8f)
	{
		Heart* pHeart = new Heart();
		pHeart->game_object_initialize();
		pHeart->curr_pos = enemy.curr_pos;
		Level::v_gameObjects.push_back(pHeart);
	}
}