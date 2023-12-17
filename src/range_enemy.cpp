/*!*************************************************************************
****
\file   range_enemy.cpp
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Mar 11 2021

\brief
This program includes the implementation of animations for the RangedEnemy.

The functions include:
- game_object_load
- game_object_draw

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/
#include "enemy.h"

/******************************************************************************
 * @brief Construct a new Ranged Enemy:: Ranged Enemy object
 * 
******************************************************************************/
RangedEnemy::RangedEnemy() : Enemy(), range{ 1 }
{
	enemy_type = ENEMY_TYPE::ENEMY_SKELETON_RANGED;
}

/******************************************************************************
 * @brief Construct a new Ranged Enemy:: Ranged Enemy object
 * 
 * @param health 
 * Health of the enemy
 * @param init_pos 
 * Position of enemy to spawn
 * @param scale 
 * Scale of the enemy
 * @param spr 
 * Sprite pointer for the enemy. It should be a unique allocated sprite.
 * @param range 
 * Range for the attack
******************************************************************************/
RangedEnemy::RangedEnemy(s32 health, AEVec2 init_pos, AEVec2 scale, Sprite* spr, f32 range)
	: Enemy(health, init_pos, scale, spr), range{ range }
{
	enemy_type = ENEMY_TYPE::ENEMY_SKELETON_RANGED;
}

/******************************************************************************
 * @brief Override of game_object_load from gameObject
 * Adds the necessary animations for the enemy
******************************************************************************/
void RangedEnemy::game_object_load()
{
	Animation ani_first = Animation(1, { 0.0f, 0.0f }, obj_sprite, 125, 0.1f);
	Animation ani_second = Animation(3, { 0.0f, 0.0f }, obj_sprite, 125, 0.1f);
	Animation ani_third = Animation(1, { 375.0f, 0.0f }, obj_sprite, 125, 0.1f);
	animations.emplace_back(ani_first);
	animations.emplace_back(ani_second);
	animations.emplace_back(ani_third);
}

/******************************************************************************
 * @brief Override of game_object_draw from gameObject
 * Sets the animation based on the animation state, then draw the enemy by
 *  calling its parent class's draw function
******************************************************************************/
void RangedEnemy::game_object_draw()
{
	switch (anim_state) 
	{
	case ENEMY_ANIM::ATTACK:
	{
		animations[1].play_animation(false);
		if (animations[1].at_last_frame())
		{
			animations[1].reset();
			anim_state = ENEMY_ANIM::DEFAULT;
		}
		break;
	}
	case ENEMY_ANIM::HURT:
	{
		animations[2].play_animation();
		if (on_ground)
			anim_state = ENEMY_ANIM::DEFAULT;
		break;
	}
	default:
		animations[0].play_animation();
		break;
	}
	Enemy::game_object_draw();
}