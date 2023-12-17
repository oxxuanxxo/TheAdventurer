/*!*****************************************************************************
\file   boss1.cpp
\author Primary: Mohammad Hanif Koh Teck Wee	 (98.06%)
		Secondary: Siti Nursafinah Binte Sumsuri (1.94%)
\par    DP email: mohammadhanifkoh.t\@digipen.edu
\date   Mar 4 2021

\brief
This file contains the implementation of functions used in Boss 1.

The functions include:
	- Boss1::Boss1
	- set_aabb
	- set_attack
	- game_object_load
	- game_object_initialize
	- game_object_draw
	- game_object_free
	- game_object_unload
	- state_machine_1
	- state_machine_2

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************************/
#include "boss.h"
#include "level_manager.h"

#define CRATE_ATTACK boss_attacks[0];
#define BOOMERANG_ATTACK boss_attacks[1];
#define SLASH_ATTACK boss_attacks[2];
#define SLAM_ATTACK boss_attacks[3];

constexpr s32 BOSS_HEALTH = 200;
constexpr f32 BOSS_SIZE = 250;

constexpr s32 BOSS1_SPR_ROW = 6;
constexpr s32 BOSS1_SPR_COL = 10;
const s8* boss1_spritesheet = "./Sprites/Boss/boss1.png";

static AEGfxTexture* pTexBoss = nullptr;
static AEGfxVertexList* pMeshBoss = nullptr;

static void state_machine_1(Boss& boss);
static void state_machine_2(Boss& boss);

/*!*****************************************************************************
  \brief
	Default constructor for Boss1 class.
*******************************************************************************/
Boss1::Boss1() :
	Boss()
{
	full_health = BOSS_HEALTH;
	health = BOSS_HEALTH;
	scale = { BOSS_SIZE, BOSS_SIZE };
}

/*!*****************************************************************************
  \brief
	Sets a specific aabb for the boss.
*******************************************************************************/
void Boss1::set_aabb()
{
	AEVec2Set(&bounding_box.min, -0.5f * scale.x, -0.5f * scale.y);
	AEVec2Set(&bounding_box.max, 0.5f * scale.x, 0.5f * scale.y);
	AEVec2Add(&bounding_box.min, &bounding_box.min, &curr_pos);
	AEVec2Add(&bounding_box.max, &bounding_box.max, &curr_pos);
}

/*!*****************************************************************************
  \brief
	Sets the current attack of the player based on the conditions set in the
	state machine functions, if the cooldown of the attack is over. Has a
	different statemachine when the boss is above and below 50% health.
*******************************************************************************/
void Boss1::set_attack()
{
	if (!is_vul)
	{
		if (cooldown_timer > 0) cooldown_timer -= G_DELTATIME;

		else if (!is_attacking)
		{
			if (health * 2 > full_health)
			{
				state_machine_1(*this);
			}

			else
			{
				state_machine_2(*this);
			}

			is_attacking = true;
			attack_timer = curr_Attack->leeway_timer;
			setup = false;
			attack_phase = 0;
			curr_vel.x = 0;
		}
	}
}

/*!*****************************************************************************
  \brief
	Loads all assets and allocates memory required by Boss 1. Called once in
	level manager load function.
*******************************************************************************/
void Boss1::game_object_load()
{
	pTexBoss = AEGfxTextureLoad(boss1_spritesheet);
	pMeshBoss = create_square_mesh(1.0f / BOSS1_SPR_COL, 1.0f / BOSS1_SPR_ROW);
	AE_ASSERT_MESG(pMeshBoss, "Failed to create boss mesh!");
	obj_sprite = new Sprite(pTexBoss, pMeshBoss, scale);
	obj_sprite->set_size(read_png_size(boss1_spritesheet));

	boss1_attacks_load();

	Animation boss_idle = Animation(1, { 0,0 }, obj_sprite, 151);
	Animation boss_walk = Animation(4, { 0,0 }, obj_sprite, 151, 0.4f);
	Animation boss_leap = Animation(5, { 151.0f, 151.0f }, obj_sprite, 151, 0.1f);
	Animation boss_land = Animation(4, { 906.0f, 151.0f }, obj_sprite, 151, 0.03f);
	Animation boss_slash = Animation(5, { 151.0f, 302.0f }, obj_sprite, 151, 0.4f);
	Animation boss_slam = Animation(5, { 151.0f, 453.0f }, obj_sprite, 151, 0.4f);
	Animation boss_tackle = Animation(3, { 151.0f, 604.0f }, obj_sprite, 151, 0.2f);
	Animation boss_boomerang = Animation(3, { 151.0f, 755.0f }, obj_sprite, 151, 0.07f);
	animations.push_back(boss_idle);
	animations.push_back(boss_walk);
	animations.push_back(boss_leap);
	animations.push_back(boss_land);
	animations.push_back(boss_slash);
	animations.push_back(boss_slam);
	animations.push_back(boss_tackle);
	animations.push_back(boss_boomerang);
}

/*!*****************************************************************************
  \brief
	Initializes some data members of Boss 1, and pushes attacks into Boss 1
	vector.
*******************************************************************************/
void Boss1::game_object_initialize()
{
	health = BOSS_HEALTH;
	curr_pos = init_pos;
	boss1_attacks_init(boss_attacks);
}
/*!*****************************************************************************
  \brief
	Update function for Boss1 object, overriding the default game object update
	function. Sets a new velocity of Boss 1 based on where the player is. Also
	updates all timers according to delta time if necessary
*******************************************************************************/
void Boss1::game_object_update()
{
	set_attack();
	attack();
	debuff.update();

	if (cooldown_timer > 0)
	{
		if (is_weak) curr_vel.x = 0;

		else if (!is_vul)
		{
			if (setup == false)
			{
				is_facing_right = false;
				curr_vel.x = -100.0f;
				setup = true;
			}

			if (cooldown_timer >= 0.75f)
			{
				if (AERandFloat() > 0.99f)
				{
					is_facing_right = !is_facing_right;
					curr_vel.x = -curr_vel.x;
				}
			}

			else
			{
				if (fabs(AEVec2SquareDistance(&curr_pos, &Level::player->curr_pos)) >= 100.0f * 100.0f)
				{
					is_facing_right = Level::player->curr_pos.x > curr_pos.x;
					curr_vel.x = is_facing_right ? 100.0f : -100.0f;
				}

				else curr_vel.x = 0;
			}
		}

		else
		{
			if (on_ground) curr_vel.x = 0;
		}
	}

	acceleration.y += LEVEL_GRAVITY * G_DELTATIME * 60;
	AEVec2Add(&curr_vel, &curr_vel, &acceleration);
	AEVec2Zero(&acceleration);
	AEVec2 dt_vel;
	AEVec2Scale(&dt_vel, &curr_vel, G_DELTATIME);
	AEVec2Add(&curr_pos, &curr_pos, &dt_vel);

	vul();
	invul();
	weak();
}

/*!*****************************************************************************
  \brief
	Draw function for Boss1 object, overriding the default game object draw
	function. Draws the hp of the boss and the boss sprite itself.
*******************************************************************************/
void Boss1::game_object_draw()
{
	if (health > 0)
	{
		full_bar();
		f_health();
	}
	if (!curr_Attack && curr_vel.x)
	{
		animations[1].play_animation();
	}
	obj_sprite->flip_x = is_facing_right ? false : true;
	draw(obj_sprite, curr_pos, scale);
}

/*!*****************************************************************************
  \brief
	Free function for Boss1 object, overriding the default game object free
	function. removes all attack colliders belonging to the boss from the vector
	of game objects.
*******************************************************************************/
void Boss1::game_object_free()
{
	while (get_attack_collider(this))
	{
		remove_attack_collider(this);
	}
	obj_sprite->mesh = nullptr;
	obj_sprite->texture = nullptr;
}

/*!*****************************************************************************
  \brief
	Unloads all assets and dellocates memory required by Boss 1. Called once in
	level manager unload function.
*******************************************************************************/
void Boss1::game_object_unload()
{
	delete obj_sprite;
	AEGfxTextureUnload(pTexBoss);
	AEGfxMeshFree(pMeshBoss);
	boss1_attacks_unload();
	boss_attacks.clear();
	boss_attacks.~vector();
}

/*!*****************************************************************************
  \brief
	Selects which attack to use. Uses one attack if the player is far enough
	from the boss, another attack if the player is close enough, and the third
	at a random chance.

  \param boss
	A reference to the boss 1 object.
*******************************************************************************/
void state_machine_1(Boss& boss)
{
	if (AERandFloat() > 0.67f)
	{
		boss.curr_Attack = &boss.SLAM_ATTACK;
	}

	else
	{
		if (fabs(Level::player->curr_pos.x - boss.curr_pos.x) > 300.0f)
		{
			boss.curr_Attack = &boss.BOOMERANG_ATTACK;
		}

		else
		{
			boss.curr_Attack = &boss.SLASH_ATTACK;
		}
	}
}

/*!*****************************************************************************
  \brief
	Selects which attack to use. Uses one attack if the player is far enough
	from the boss, another attack if the player is close enough, and the third
	at a random chance. The fourth attack can now be used, and is more likely
	to be used the lower health the boss 1 has.

  \param boss
	A reference to the boss 1 object.
*******************************************************************************/
void state_machine_2(Boss& boss)
{
	f32 ultimate_chance = static_cast<f32>(boss.health) / static_cast<f32>(boss.full_health) + 0.20f;
	f32 roll = AERandFloat();

	if (roll > ultimate_chance)
	{
		boss.curr_Attack = &boss.CRATE_ATTACK;
	}

	else if (roll > ultimate_chance - 0.20f)
	{
		boss.curr_Attack = &boss.SLAM_ATTACK;
	}

	else
	{
		if (fabs(Level::player->curr_pos.x - boss.curr_pos.x) > 300.0f)
		{
			boss.curr_Attack = &boss.BOOMERANG_ATTACK;
		}

		else
		{
			boss.curr_Attack = &boss.SLASH_ATTACK;
		}
	}
}