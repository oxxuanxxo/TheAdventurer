/*!*************************************************************************
****
\file   gameobject.cpp
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Mar 12 2021

\brief
This header file outlines an implementation of a game object in the game
engine

The functions include:
- swap
- set_aabb
- Draw_Obj
- game_object_load
- game_object_initialize
- game_object_update
- game_object_draw
- game_object_unload
- collision_update
- cmp_layer

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/
#include "main.h"
#include "gameobject.h"
#include "attack.h"
#include "enemy.h"
#include "level_manager.h"

AEVec2 def_vec{0, 0};

/*!
 * \brief Construct a new game Object::game Object object
 * 
 * 
 */
gameObject::gameObject() : init_pos{}
{
	bounding_box = {def_vec, def_vec};
	curr_pos = def_vec;
	curr_vel = def_vec;
	acceleration = def_vec;
	on_collide = nullptr;
	obj_sprite = nullptr;
	layer = 0;
	state = P_Anim_State::P_NONE;
	scale = {1.0f, 1.0f};
	type = GO_TYPE::GO_PLAYER;
	is_colliding = false;
	is_static = false;
	flag = true;
	on_ground = false;
}

/*!
 * \brief Construct a new game Object::game Object object
 * 
 * \param pos 
 * Current position of the game object
 * \param sprite 
 * Sprite pointer to the object
 * \param layer 
 * Which layer for the game object to draw on
 * \param scale 
 * Scale of the game object
 * \param type 
 * Type of the game object
 * \param is_static 
 * If the game object moves. Important for collision checking
 */
gameObject::gameObject(AEVec2 pos,
					   Sprite *sprite,
					   u32 layer,
					   AEVec2 scale,
					   GO_TYPE type,
					   bool is_static) : gameObject()
{
	curr_pos = pos;
	obj_sprite = sprite;
	this->layer = layer;
	this->scale = scale;
	this->type = type;
	this->is_static = is_static;
}

/*!
 * \brief Construct a new game Object::game Object object
 * Using copy constructor
 *
 * \param rhs 
 * 	Read-only reference to the gameObject to copy
 */
gameObject::gameObject(const gameObject &rhs)
{
	bounding_box = rhs.bounding_box;
	curr_pos = rhs.curr_pos;
	curr_vel = rhs.curr_vel;
	acceleration = rhs.acceleration;
	on_collide = rhs.on_collide;
	obj_sprite = rhs.obj_sprite;
	layer = rhs.layer;
	state = rhs.state;
	scale = rhs.scale;
	type = rhs.type;
	is_colliding = rhs.is_colliding;
	is_static = rhs.is_static;
	flag = rhs.flag;
	on_ground = rhs.on_ground;
	init_pos = rhs.init_pos;
}

/*!
 * \brief Swap the game object values with the rhs
 * 
 * 
 * \param rhs 
 * Reference to a game object to swap values with
 */
void gameObject::swap(gameObject &rhs)
{
	std::swap(animations, rhs.animations);
	std::swap(colliders, rhs.colliders);
	std::swap(bounding_box, rhs.bounding_box);
	std::swap(curr_pos, rhs.curr_pos);
	std::swap(curr_vel, rhs.curr_vel);
	std::swap(acceleration, rhs.acceleration);
	std::swap(on_collide, rhs.on_collide);
	std::swap(obj_sprite, rhs.obj_sprite);
	std::swap(layer, rhs.layer);
	std::swap(state, rhs.state);
	std::swap(scale, rhs.scale);
	std::swap(type, rhs.type);
	std::swap(is_colliding, rhs.is_colliding);
	std::swap(is_static, rhs.is_static);
	std::swap(flag, rhs.flag);
	std::swap(on_ground, rhs.on_ground);
}

/*!
 * \brief Assignment operator overload for gameobject
 * 
 * \param rhs 
 * Reference to a game object to assign value
 * \return gameObject& 
 * 	Copied gameObject result
 */
gameObject &gameObject::operator=(const gameObject &rhs)
{
	gameObject tmp{rhs};
	swap(tmp);
	return *this;
}

/*!
 * \brief Draws the object if the flag is true
 * 
 */
void gameObject::Draw_Obj()
{
	if (flag)
		draw(obj_sprite, curr_pos, scale);
}

/*!
 * \brief Sets the outward bound of the game object
 * 
 */
void gameObject::set_aabb()
{
	AEVec2Set(&bounding_box.min, -0.5f * scale.x, -0.5f * scale.y);
	AEVec2Set(&bounding_box.max, 0.5f * scale.x, 0.5f * scale.y);
	AEVec2Add(&bounding_box.min, &bounding_box.min, &curr_pos);
	AEVec2Add(&bounding_box.max, &bounding_box.max, &curr_pos);
}

/*!
 * \brief Loads the game object
 * 
 * 
 */
void gameObject::game_object_load()
{
}

/*!
 * \brief Initialize the game object by setting its
 * current position to init_pos
 * 
 */
void gameObject::game_object_initialize()
{
	curr_pos = init_pos;
}

/*!
 * \brief Update function of the game object
 * If the object is a boss attack collider or a player attack collider, 
 * adds its position by velocity and acceleration.
 * 
 * Same goes to enemy attack collider but with the addition of checking if the 
 * cooldown timer is <= 0. If it is, set the collider flag to false.
 * 
 */
void gameObject::game_object_update()
{
	if (type == GO_TYPE::GO_BOSSATTACKCOLLIDER || type == GO_TYPE::GO_PLAYERATTACKCOLLIDER)
	{
		AEVec2Add(&curr_vel, &curr_vel, &acceleration);
		AEVec2 dt_vel;
		AEVec2Scale(&dt_vel, &curr_vel, G_DELTATIME);
		AEVec2Add(&curr_pos, &dt_vel, &curr_pos);
	}

	if (type == GO_TYPE::GO_ENEMYATTACKCOLLIDER)
	{
		Attack_Collider *atk_col = dynamic_cast<Attack_Collider *>(this);
		if (atk_col->owner)
		{
			AEVec2 dt_vel;
			AEVec2Scale(&dt_vel, &curr_vel, G_DELTATIME);
			AEVec2Add(&curr_pos, &dt_vel, &curr_pos);

			Attack_Collider *collider = dynamic_cast<Attack_Collider *>(this);
			Enemy *enemy = dynamic_cast<Enemy *>(collider->owner);
			if (enemy && enemy->cooldown_timer <= 0)
			{
				collider->flag = false;
			}
		}
		else
		{
			AEVec2Add(&curr_vel, &curr_vel, &acceleration);
			AEVec2 dt_vel;
			AEVec2Scale(&dt_vel, &curr_vel, G_DELTATIME);
			AEVec2Add(&curr_pos, &dt_vel, &curr_pos);
		}
	}
}

/*!
 * \brief Default draw function for the game object
 * 
 * If there are animations, it will play its first animation by default on loop.
 * 
 * If the game object has its flag set to true, the draw function will be called
 */
void gameObject::game_object_draw()
{
	if (!animations.empty())
	{
		animations.at(0).play_animation();
	}
	if (flag)
		draw(obj_sprite, curr_pos, scale);
}

/*!
 * \brief Free function of the game object
 * 
 * 
 */
void gameObject::game_object_free()
{
}

/*!
 * \brief Unload function of the game object
 * 
 * 
 */
void gameObject::game_object_unload()
{
}

/*!
 * \brief Compare function required for sorting. Compares the layer of the 
 * game objects
 * 
 * \param lhs 
 * Left hand side object
 * \param rhs 
 * Right hand side object
 * \return true 
 * lhs layer is lower than rhs layer
 * \return false 
 * rhs layer is lower than lhs layer
 */
bool cmp_layer(gameObject *const &lhs, gameObject *const &rhs)
{
	return (lhs->layer < rhs->layer);
}