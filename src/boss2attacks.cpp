/*!*****************************************************************************
\file   boss2attacks.cpp
\author Chua Yip Xuan
\par    DP email: yipxuan.chua\@digipen.edu
\date   April 1 2021

\brief
	This source file contains the implmentation of behviours and attacks
	used in Boss 2.

	The source file has functions that consists of:
	- boss1_attacks_load
	- boss1_attacks_init
	- boss1_attacks_unload
	- boss_jump
	- boss2_projectile_fireball
	- boss2_roots_attack
	- boss2_fireball_attack

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************************/

#include "boss.h"
#include "attack.h"
#include "level_manager.h"
#include "audio.h"

static gameObject* indicator_1{ nullptr };
static gameObject* indicator_2{ nullptr };
static gameObject* indicator_3{ nullptr };
static gameObject* indicator_4{ nullptr };
static gameObject* indicator_5{ nullptr };
static gameObject* indicator_6{ nullptr };
static gameObject* indicator_7{ nullptr };

static Audio* fireball_sfx;
static Audio* roots_sfx;

void boss2_projectile_fireball(gameObject&);
void boss2_roots_attack(gameObject&);
void boss2_fireball_attack(gameObject&);

Boss_Attack projectile_fireball{ 1.0f, 0, 2, boss2_projectile_fireball, 2.0f , 1.0f, {200.0f, 300.0f} };
Boss_Attack fireball_attack{ 1.0f, 0, 1, boss2_fireball_attack, 2.0f , 1.0f, {200.0f, 300.0f} };
Boss_Attack roots_attack{ 1.0f, 0, 3, boss2_roots_attack, 2.0f , 1.0f, {200.0f, 300.0f} };

/*!*****************************************************************************
  \brief
	Loads all assets needed for the boss2 attacks.
*******************************************************************************/
void boss2_attacks_load()
{
	fireball_sfx = new Audio("./Audio/boss2_fireball.wav", AUDIO_GROUP::SFX);
	roots_sfx = new Audio("./Audio/boss2_roots.wav", AUDIO_GROUP::SFX);
	indicator_1 = push_object(GO_TYPE::GO_ATTACKINDICATOR, nullptr);
	indicator_1->flag = false;
	indicator_2 = push_object(GO_TYPE::GO_ATTACKINDICATOR, nullptr);
	indicator_2->flag = false;
	indicator_3 = push_object(GO_TYPE::GO_ATTACKINDICATOR, nullptr);
	indicator_3->flag = false;
	indicator_4 = push_object(GO_TYPE::GO_ATTACKINDICATOR, nullptr);
	indicator_4->flag = false;
	indicator_5 = push_object(GO_TYPE::GO_ATTACKINDICATOR, nullptr);
	indicator_5->flag = false;
	indicator_6 = push_object(GO_TYPE::GO_ATTACKINDICATOR, nullptr);
	indicator_6->flag = false;
	indicator_7 = push_object(GO_TYPE::GO_ATTACKINDICATOR, nullptr);
	indicator_7->flag = false;
}

/*!*****************************************************************************
  \brief
	Initialize all boss 2 attacks and push into the Boss attack vector.
*******************************************************************************/
void boss2_attacks_init(vector<Boss_Attack>& attacks)
{
	attacks.push_back(fireball_attack);
	attacks.push_back(projectile_fireball);
	attacks.push_back(roots_attack);
}

/*!*****************************************************************************
  \brief
	Unload all assets used for boss2 attacks.
*******************************************************************************/
void boss2_attacks_unload()
{
	delete indicator_1->obj_sprite;
	delete indicator_2->obj_sprite;
	delete indicator_3->obj_sprite;
	delete indicator_4->obj_sprite;
	delete indicator_5->obj_sprite;
	delete indicator_6->obj_sprite;
	delete indicator_7->obj_sprite;
	delete fireball_sfx;
	delete roots_sfx;
}

/*!*****************************************************************************
  \brief
	This function is one of the attack behaviour of boss 2 where it will shoot
	projectile fireballs ranging from the outerbound of the bounding box of the
	boss to the ending space of the boss room.
*******************************************************************************/
void boss2_projectile_fireball(gameObject& gameObj)
{
	Boss2* boss2 = dynamic_cast<Boss2*> (&gameObj);
	static particleSystem* particles_fireball;
	static int num_of_shots = 0;
	static int curr_fireball = 0;

	if (!boss2) return;

	if (!Level::player) return;

	if (!boss2->setup)
	{
		boss2->attack_phase = 1;
		num_of_shots = 0;
		boss2->is_facing_right = Level::player->curr_pos.x > boss2->curr_pos.x;
		for (int i{ 0 }; i < 10; ++i)
		{
			boss2->atkcol_fireballs[i]->curr_pos = boss2->curr_pos;
			AEVec2Zero(&boss2->atkcol_fireballs[i]->acceleration);
			AEVec2Zero(&boss2->atkcol_fireballs[i]->curr_vel);
			boss2->atkcol_fireballs[i]->flag = false;
			boss2->atkcol_fireballs[i]->damage = fireball_attack.damage;
		}
		particles_fireball = create_particlesystem(boss2);
		particles_fireball->scale = { 0.5, 0.5 };
		particles_fireball->color_range[0] = { 1.0f, 0.0f, 0.0f, 0.7f };
		particles_fireball->color_range[1] = { 1.0f, 0.5f, 0.0f, 0.7f };
		particles_fireball->scale_range[0] = particles_fireball->scale_range[1] = { 10.0f, 10.0f };
		particles_fireball->emission_rate = 0.01f;
		particles_fireball->max_particles = 500;
		particles_fireball->lifetime_range[0] = particles_fireball->lifetime_range[1] = 0.3f;
		particles_fireball->speed_range[0] = particles_fireball->speed_range[1] = 700.0f;
		curr_fireball = 0;
		boss2->setup = true;
		boss2->attack_timer = 5.0f;
	}

	if (boss2->attack_phase == 1)
	{
		if (boss2->attack_timer < 2.0f)
		{
			particles_fireball->delete_system();
			++boss2->attack_phase;
		}
	}

	if (boss2->attack_phase == 2)
	{
		if (boss2->attack_timer < 1.5f)
		{
			if (num_of_shots < 20)
			{
				f32 min_dist = boss2->outerBound.max.x - boss2->curr_pos.x;
				f32 max_dist = AEGetWindowWidth() / 2 - 100.f;
				f32 final_dist = ((max_dist - min_dist) * AERandFloat()) + min_dist;
				if (AERandFloat() >= 0.5f)
				{
					final_dist = -final_dist;
				}
				boss2->atkcol_fireballs[curr_fireball]->curr_pos = boss2->curr_pos;
				boss2->atkcol_fireballs[curr_fireball]->flag = true;
				fireball_sfx->play_sound();
				boss2->atkcol_fireballs[curr_fireball]->curr_vel.x = final_dist;
				boss2->atkcol_fireballs[curr_fireball]->curr_vel.y = 1750;
				boss2->atkcol_fireballs[curr_fireball]->acceleration.y = LEVEL_GRAVITY;
				boss2->attack_timer = 1.75f;
				curr_fireball = static_cast<s32>(AEWrap(static_cast<f32>(curr_fireball) + 1.0f, 0.0f, 9.0f));
				++num_of_shots;
			}
		}
	}
}

/*!*****************************************************************************
  \brief
	This function is one of the attack behaviour of boss 2 where it spawn attack
	colliders at its roots and speed up to cover the bottom part of the boss.
	The lava traps will then spawn together with the colliders.
	The player will have to then avoid the attacks by jumping onto the temporary
	platforms that is spawned too.
*******************************************************************************/
void boss2_roots_attack(gameObject& gameObj)
{
	Boss2* boss2 = dynamic_cast<Boss2*> (&gameObj);
	static particleSystem* particles_fireball;

	if (!boss2) return;

	if (!Level::player) return;

	if (!boss2->setup)
	{
		Camera::look_at(&boss2->curr_pos, 8.0f);
		boss2->attack_phase = 1;
		boss2->is_facing_right = Level::player->curr_pos.x > boss2->curr_pos.x;
		particles_fireball = create_particlesystem(boss2);
		particles_fireball->scale = { 0.5, 0.5 };
		particles_fireball->color_range[0] = { 0.5f, 0.0f, 0.5f, 0.7f };
		particles_fireball->color_range[1] = { 0.0f, 0.0f, 1.0f, 0.7f };
		particles_fireball->scale_range[0] = particles_fireball->scale_range[1] = { 10.0f, 10.0f };
		particles_fireball->emission_rate = 0.01f;
		particles_fireball->max_particles = 500;
		particles_fireball->lifetime_range[0] = particles_fireball->lifetime_range[1] = 0.3f;
		particles_fireball->speed_range[0] = particles_fireball->speed_range[1] = 700.0f;
		boss2->setup = true;
		boss2->attack_timer = 5.0f;
		for (int i{ 0 }; i < 7; ++i)
		{
			boss2->plats[i]->flag = true;
			AEVec2Zero(&boss2->plats[i]->acceleration);
			AEVec2Zero(&boss2->plats[i]->curr_vel);
		}
		for (int i{ 0 }; i < 2; ++i)
		{
			boss2->atkcol_roots[i]->scale = { 10.0f, 10.0f };
			boss2->atkcol_roots[i]->flag = true;
			boss2->atkcol_roots[i]->damage = roots_attack.damage;
			AEVec2Zero(&boss2->atkcol_roots[i]->acceleration);
			AEVec2Zero(&boss2->atkcol_roots[i]->curr_vel);
		}
		boss2->atkcol_roots[1]->curr_pos = { boss2->curr_pos.x - 100.0f, boss2->curr_pos.y - 50.0f };
		boss2->atkcol_roots[0]->curr_pos = { boss2->curr_pos.x + 100.0f, boss2->curr_pos.y - 50.0f };

		indicator_1->flag = false;
		indicator_1->obj_sprite->texture = nullptr;
		indicator_1->obj_sprite->tint = { 1.0f, 0.3f, 0.1f, 0.3f };
		indicator_1->curr_pos = boss2->curr_pos;
		indicator_1->scale = { boss2->outerBound.max.x, 200.0f };

		indicator_2->flag = false;
		indicator_2->obj_sprite->texture = nullptr;
		indicator_2->obj_sprite->tint = { 1.0f, 1.0f, 0.0f, 0.6f };
		indicator_2->curr_pos = boss2->plats[0]->curr_pos;
		indicator_2->scale = boss2->plats[0]->scale;

		indicator_3->flag = false;
		indicator_3->obj_sprite->texture = nullptr;
		indicator_3->obj_sprite->tint = { 1.0f, 1.0f, 0.0f, 0.6f };
		indicator_3->curr_pos = boss2->plats[2]->curr_pos;
		indicator_3->scale = boss2->plats[2]->scale;

		indicator_4->flag = false;
		indicator_4->obj_sprite->texture = nullptr;
		indicator_4->obj_sprite->tint = { 1.0f, 1.0f, 0.0f, 0.6f };
		indicator_4->curr_pos = boss2->plats[4]->curr_pos;
		indicator_4->scale = boss2->plats[4]->scale;

		indicator_5->flag = false;
		indicator_5->obj_sprite->texture = nullptr;
		indicator_5->obj_sprite->tint = { 1.0f, 1.0f, 0.0f, 0.6f };
		indicator_5->curr_pos = boss2->plats[1]->curr_pos;
		indicator_5->scale = boss2->plats[1]->scale;

		indicator_6->flag = false;
		indicator_6->obj_sprite->texture = nullptr;
		indicator_6->obj_sprite->tint = { 1.0f, 1.0f, 0.0f, 0.6f };
		indicator_6->curr_pos = boss2->plats[3]->curr_pos;
		indicator_6->scale = boss2->plats[3]->scale;

		indicator_7->flag = false;
		indicator_7->obj_sprite->texture = nullptr;
		indicator_7->obj_sprite->tint = { 1.0f, 1.0f, 0.0f, 0.6f };
		indicator_7->curr_pos = boss2->plats[5]->curr_pos;
		indicator_7->scale = boss2->plats[5]->scale;

		boss2->setup = true;
		boss2->attack_timer = 6.0f;
	}
	if (boss2->attack_phase == 1)
	{
		boss2->animations[0].play_animation(true, 2.0f);
		indicator_1->obj_sprite->tint.g -= 1.0f / 60.0f;
		if (indicator_1->flag == false)
		{
			indicator_1->flag = true;
		}
		if ((indicator_2->flag == false) && (indicator_5->flag == false) && (boss2->attack_timer < 6.0f))
		{
			indicator_2->flag = true;
			indicator_5->flag = true;
		}
		if ((indicator_3->flag == false) && (indicator_6->flag == false) && (boss2->attack_timer < 5.5f))
		{
			indicator_3->flag = true;
			indicator_6->flag = true;
		}
		if ((indicator_4->flag == false) && (indicator_7->flag == false) && (boss2->attack_timer < 5.0f))
		{
			indicator_4->flag = true;
			indicator_7->flag = true;
		}
		if (boss2->attack_timer < 2.0f)
		{
			particles_fireball->delete_system();
			++boss2->attack_phase;
		}
	}
	if (boss2->attack_phase == 2)
	{
		boss2->animations[0].play_animation(true, 3.0f);
		indicator_2->obj_sprite->tint.a -= 1.0f / 60.0f;
		indicator_3->obj_sprite->tint.a -= 1.0f / 60.0f;
		indicator_4->obj_sprite->tint.a -= 1.0f / 60.0f;
		indicator_5->obj_sprite->tint.a -= 1.0f / 60.0f;
		indicator_6->obj_sprite->tint.a -= 1.0f / 60.0f;
		indicator_7->obj_sprite->tint.a -= 1.0f / 60.0f;

		if (boss2->atkcol_roots[0]->scale.x < 200.0f)
		{
			AEVec2 tmp{ 5.0f, 5.0f };
			AEVec2Add(&boss2->atkcol_roots[0]->scale, &boss2->atkcol_roots[0]->scale, &tmp);
			AEVec2Add(&boss2->atkcol_roots[1]->scale, &boss2->atkcol_roots[1]->scale, &tmp);
		}
		else
		{
			++boss2->attack_phase;
			boss2->attack_timer = 5.0f;
		}
	}
	if (boss2->attack_phase == 3)
	{
		boss2->animations[0].play_animation();
		static int play_once = 0;
		if (play_once == 0)
		{
			roots_sfx->play_sound();
			play_once++;
		}
		boss2->atkcol_roots[0]->curr_vel = { 1000.0f, 0.0f };
		boss2->atkcol_roots[1]->curr_vel = { -1000.0f, 0.0f };

		if (boss2->attack_timer < 2.0f)
		{
			++boss2->attack_phase;
			play_once = 0;
		}
	}
	if (boss2->attack_phase == 4)
	{
		for (int i{ 0 }; i < 10; ++i)
		{
			boss2->lava_traps[i]->flag = false;
		}

		indicator_1->flag = false;
		indicator_2->flag = false;
		indicator_3->flag = false;
		indicator_4->flag = false;
		indicator_5->flag = false;
		indicator_6->flag = false;
		indicator_7->flag = false;
	}
}

/*!*****************************************************************************
  \brief
	This function is one of the attack behaviour of boss 2 where it will shoot
	fireballs according to the player position.
*******************************************************************************/
void boss2_fireball_attack(gameObject& gameObj)
{
	Boss2* boss2 = dynamic_cast<Boss2*> (&gameObj);
	static particleSystem* particles_fireball;
	static int num_of_shots = 0;

	if (!boss2) return;

	if (!Level::player) return;

	if (!boss2->setup)
	{
		boss2->attack_phase = 1;
		num_of_shots = 0;
		boss2->is_facing_right = Level::player->curr_pos.x > boss2->curr_pos.x;
		particles_fireball = create_particlesystem(boss2);
		particles_fireball->scale = { 0.5, 0.5 };
		particles_fireball->color_range[0] = { 1.0f, 0.0f, 0.0f, 0.7f };
		particles_fireball->color_range[1] = { 1.0f, 0.5f, 0.0f, 0.7f };
		particles_fireball->scale_range[0] = particles_fireball->scale_range[1] = { 10.0f, 10.0f };
		particles_fireball->emission_rate = 0.01f;
		particles_fireball->max_particles = 500;
		particles_fireball->lifetime_range[0] = particles_fireball->lifetime_range[1] = 0.3f;
		particles_fireball->speed_range[0] = particles_fireball->speed_range[1] = 700.0f;

		for (int i{ 0 }; i < 10; ++i)
		{
			AEVec2Zero(&boss2->atkcol_fireballs[i]->acceleration);
			AEVec2Zero(&boss2->atkcol_fireballs[i]->curr_vel);
			boss2->atkcol_fireballs[i]->curr_pos = boss2->curr_pos;
			boss2->atkcol_fireballs[i]->damage = fireball_attack.damage;
			boss2->atkcol_fireballs[i]->flag = false;
		}
		boss2->setup = true;
		boss2->attack_timer = 7.0f;
	}

	if (boss2->attack_phase == 1)
	{
		boss2->animations[0].play_animation(true, 3.0f);
		if (boss2->attack_timer < 2.0f)
		{
			particles_fireball->delete_system();
			++boss2->attack_phase;
		}
	}
	//charging
	if (boss2->attack_phase == 2)
	{
		if (boss2->attack_timer < 1.5f)
		{
			boss2->animations[0].play_animation(true);
			if (num_of_shots < 10)
			{
				boss2->atkcol_fireballs[num_of_shots]->flag = true;
				AEVec2 dir, P2B;
				AEVec2Sub(&P2B, &Level::player->curr_pos, &boss2->curr_pos);
				AEVec2Normalize(&dir, &P2B);
				fireball_sfx->play_sound();
				boss2->atkcol_fireballs[num_of_shots]->curr_vel.x = dir.x * 600;
				boss2->atkcol_fireballs[num_of_shots]->curr_vel.y = dir.y * 600;
				boss2->attack_timer = 2.0f;
				++num_of_shots;
			}
		}
	}
}