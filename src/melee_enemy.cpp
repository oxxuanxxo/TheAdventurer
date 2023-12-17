/*!*************************************************************************
****
\file   range_enemy.cpp
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Mar 11 2021

\brief
This program includes the implementation of animations for the MeleeEnemy2.

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
 * @brief Override of game_object_load from gameObject
 * Adds the necessary animations for the enemy
******************************************************************************/
void MeleeEnemy2::game_object_load()
{
	Animation ani_first = Animation(1, { 0.0f, 0.0f }, obj_sprite, 125, 0.1f);
	Animation ani_second = Animation(4, { 0.0f, 0.0f }, obj_sprite, 302, 0.1f);
	Animation ani_third = Animation(2, { 0.0f, 280.5f }, obj_sprite, 302, 0.1f);
	Animation ani_forth = Animation(1, { 302.5f, 280.5f }, obj_sprite, 302, 0.1f);
	animations.emplace_back(ani_first);
	animations.emplace_back(ani_second);
	animations.emplace_back(ani_third);
	animations.emplace_back(ani_forth);
}

/******************************************************************************
 * @brief Override of game_object_draw from gameObject
 * Sets the animation based on the animation state, then draw the enemy by
 *  calling its parent class's draw function
******************************************************************************/
void MeleeEnemy2::game_object_draw()
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
	case ENEMY_ANIM::WALK:
	{
		animations[1].play_animation();
		break;
	}
	default:
		animations[0].play_animation();
		break;
	}
	Enemy::game_object_draw();
}