/*!*****************************************************************************
\file   enemyattacks.cpp
\author Primary: Chua Yip Xuan					 (71.43%)
		Secondary: Siti Nursafinah Binte Sumsuri (28.57%)
\par    DP email: yipxuan.chua\@digipen.edu
				  sitinursafinah.b\@digipen.edu
\date   Mar 12 2021

\brief
	This source file contains the implmentation of behaviours and attacks
	used in the different enemies.

The source file has functions that consists of:
	- enemy_shoot
	- enemy_slash

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************************/

#include "attack.h"
#include <iostream>
#include "level_manager.h"
#include "enemy.h"

static Audio* range_atk_sound;
void enemy_shoot(gameObject& gameObj);
void enemy_slash(gameObject& gameObj);

/*!*****************************************************************************
  \brief
	Param constructor for Enemy Attack.
*******************************************************************************/
Enemy_Attack::Enemy_Attack(f32 leeway_timer, u32 anim_index, s32 damage, fp attack, AEVec2 flinch_scale)
	:Attack(leeway_timer, anim_index, damage, attack, flinch_scale)
{
}

Enemy_Attack shoot{ 1.0f, 0 , 1, enemy_shoot, { 0.0f, 0.0f } };
Enemy_Attack slash{ 3.0f, 0 ,1, enemy_slash, { 0.0f, 0.0f } };

/*!*****************************************************************************
  \brief
	Load all the assets for enemy attacks.
*******************************************************************************/
void enemyattack_load()
{
	range_atk_sound = new Audio("./Audio/mage_attack.wav", AUDIO_GROUP::SFX);
}

/*!*****************************************************************************
  \brief
	Initialise the enemy attacks for different enemies.
*******************************************************************************/
void enemy_attack_init(gameObject& enemy)
{
	Enemy* enemies = dynamic_cast<Enemy*>(&enemy);
	if (enemies->enemy_type == ENEMY_TYPE::ENEMY_SKELETON_RANGED)
	{
		enemies->curr_attack = &shoot;
	}
	if (enemies->enemy_type == ENEMY_TYPE::ENEMY_SKELETON_MELEE)
	{
		enemies->curr_attack = &slash;
	}
}

/*!*****************************************************************************
  \brief
	Unload all assets that was loaded for the enemy attacks.
*******************************************************************************/
void enemyattack_unload()
{
	delete range_atk_sound;
}

/*!*****************************************************************************
  \brief
	Enemy attack behaviour for ranged enemy.
*******************************************************************************/
void enemy_shoot(gameObject& gameObj)
{
	Enemy* enemy = dynamic_cast<Enemy*>(&gameObj);

	if (!Level::player) return;
	range_atk_sound->play_sound();
	enemy->anim_state = ENEMY_ANIM::ATTACK;
	enemy->collider->flag = true;
	enemy->collider->damage = shoot.damage;
	AEVec2 nrmlz, length;
	AEVec2Sub(&length, &Level::player->curr_pos, &enemy->curr_pos);
	AEVec2Normalize(&nrmlz, &length);
	AEVec2Scale(&enemy->collider->curr_vel, &nrmlz, 400.0f);
	if (enemy->curr_pos.x - Level::player->curr_pos.x < 0)
		enemy->obj_sprite->flip_x = true;
	else
		enemy->obj_sprite->flip_x = false;
	enemy->collider->curr_pos = enemy->curr_pos;
	enemy->cooldown_timer = 3.0f;
}

/*!*****************************************************************************
  \brief
	Enemy attack behaviour for melee enemy.
*******************************************************************************/
void enemy_slash(gameObject& gameObj)
{
	Enemy* enemy = dynamic_cast<Enemy*>(&gameObj);

	if (!Level::player) return;

	enemy->collider->flag = true;
	if (Level::player->curr_pos.x > enemy->curr_pos.x)
	{
		enemy->collider->curr_pos = { enemy->curr_pos.x + 80.0f , enemy->curr_pos.y };
		enemy->collider->obj_sprite->flip_x = false;
	}
	else
	{
		enemy->collider->curr_pos = { enemy->curr_pos.x - 80.0f , enemy->curr_pos.y };
		enemy->collider->obj_sprite->flip_x = true;
	}
	enemy->collider->damage = slash.damage;

	enemy->cooldown_timer = 2.0f;
}