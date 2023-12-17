/*!*****************************************************************************
\file   boss.cpp
\author Mohammad Hanif Koh Teck Wee
		Siti Nursafinah Binte Sumsuri (26 lines)
\par    DP email: mohammadhanifkoh.t\@digipen.edu
				  sitinursafinah.b\@digipen.edu
\date   Mar 4 2021

\brief
This file contains the implementation of functions shared by all Bosses as
outlined in boss.h

The functions include:
- start_invul
- invul
- start_vul
- vul
- weak
- f_health
- full_bar
- take_damage
- boss_collide
- attack
- set_attack
- load_boss_assets
- unload_boss_assets

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************************/
#include "boss.h"
#include "main.h"
#include "audio.h"
#include "game_states.h"
#include "level_manager.h"
#include "fmod.hpp"
#include "player_data.h"
#include "graphics.h"
#include "camera.h"

constexpr u32 BOSS_LAYER = 3;
static bool weakened = 0;
static Audio* weak_sound;
static Audio* hit_weak_sound;
static Audio* hit_strong_sound;
AEGfxTexture* pTexVulnerable;
AEGfxTexture* pTexVulnerable2;

static AEGfxVertexList* pMeshHeart = 0;

static AEGfxTexture* healthBoss;
static AEVec2 healthSize = { 1000.0f, 50.0f };
static AEVec2 fullhealthSize = { 1010.0f, 60.0f };
static Camera cam;

/*!*****************************************************************************
  \brief
	Function is to draw the corresponding health length during the boss fight.
*******************************************************************************/
void Boss::f_health()
{
	AEVec2 coordinates = { -500, 200 };
	if (health != 0)
	{
		f32 current = static_cast<f32> (health) / static_cast<f32> (full_health) * healthSize.x;
		draw_non_relative(0, pMeshHeart, coordinates, { 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }, 1.0f, { current, healthSize.y }, 0);
	}
}

/*!*****************************************************************************
  \brief
	Function is to draw the corresponding health length during the boss fight.
*******************************************************************************/
void Boss::full_bar()
{
	AEVec2 coordinatesFull = { -505, 205 };

	if (health != 0)
	{
		draw_non_relative(0, pMeshHeart, coordinatesFull, { 0,0 }, { 0.0f, 1.0f, 1.0f,1.0f }, 1.0f, { fullhealthSize }, 0);
	}
}

/*!*****************************************************************************
  \brief
	Default constructor for Boss class.
*******************************************************************************/
Boss::Boss() :
	gameObject(), full_health(), health(), attack_timer(), invul_timer(), is_attacking(), is_invul(), is_vul(), is_weak(),
	vul_timer(), is_facing_right(), attack_phase(), weak_timer(), cooldown_timer(3.0f), setup(), flinch_resistance(0.9f), debuff{}
{
	on_collide = boss_collide;
	type = GO_TYPE::GO_BOSS;
	layer = BOSS_LAYER;
	is_static = false;
	curr_Attack = nullptr;
	debuff.owner = this;
}

/*!*****************************************************************************
  \brief
	Loads all assets and allocates memory used across the different boss objects.
	Called once at the load function of each level.
*******************************************************************************/
void load_boss_assets()
{
	AEGfxMeshStart();

	// This shape has 2 triangles
	AEGfxTriAdd( // 0,0
		0.0f, 0.0f, 0xFFFF0000, 0.0f, 0.0f,
		1.0f, -1.0f, 0xFFFF0000, 1.0f, 1.0f,
		1.0f, 0.0f, 0xFFFF0000, 1.0f, 0.0f);

	AEGfxTriAdd(
		0.0f, 0.0f, 0xFFFF0000, 0.0f, 0.0f,
		0.0f, -1.0f, 0xFFFF0000, 0.0f, 1.0f,
		1.0f, -1.0f, 0xFFFF0000, 1.0f, 1.0f);

	// Saving the mesh (list of triangles) in pMesh2

	pMeshHeart = AEGfxMeshEnd();
	AE_ASSERT_MESG(pMeshHeart, "Failed to create mesh heart!!");

	pTexVulnerable = AEGfxTextureLoad("./Sprites/boss_vul.png");
	AE_ASSERT_MESG(pTexVulnerable, "Failed to create boss vul texture!!");

	pTexVulnerable2 = AEGfxTextureLoad("./Sprites/Effects/boss_vuln.png");
	AE_ASSERT_MESG(pMeshHeart, "Failed to create boss vul texture!!");

	weak_sound = new Audio("./Audio/zoom1.wav", AUDIO_GROUP::SFX);
	hit_weak_sound = new Audio("./Audio/boss_hurt_weak.wav", AUDIO_GROUP::SFX);
	hit_strong_sound = new Audio("./Audio/boss_hurt_strong.wav", AUDIO_GROUP::SFX);
	weakened = false;
}

/*!*****************************************************************************
  \brief
	Unloads all assets and memory allocated for Boss Objects. Called once in the
	load function of all levels.
*******************************************************************************/
void unload_boss_assets()
{
	delete weak_sound;
	delete hit_weak_sound;
	delete hit_strong_sound;
	remove_attack_collider(Level::boss);
	AEGfxTextureUnload(pTexVulnerable);
	AEGfxTextureUnload(pTexVulnerable2);

	AEGfxMeshFree(pMeshHeart);
	weakened = false;
}

/*!*****************************************************************************
  \brief
	Function to handle collision between Boss objects and platforms

  \param e_obj
	a reference to a game object, assumed to be the boss game object itself.

  \param obj
	a reference to a game object, assumed to be the platform that the heart is
	colliding with
*******************************************************************************/
void adjust_boss_movement(gameObject& e_obj, const gameObject& obj)
{
	Boss* boss = dynamic_cast<Boss*>(&e_obj);

	f32 boss_hw = (boss->bounding_box.max.x - boss->bounding_box.min.x) / 2;
	f32 boss_hh = (boss->bounding_box.max.y - boss->bounding_box.min.y) / 2;
	f32 obj_halfscale_x = (obj.bounding_box.max.x - obj.bounding_box.min.x) / 2;
	f32 obj_halfscale_y = (obj.bounding_box.max.y - obj.bounding_box.min.y) / 2;

	AEVec2 boss_middle{ boss->bounding_box.min.x + boss_hw, boss->bounding_box.min.y + boss_hh };

	AEVec2 diff_pos = { obj.bounding_box.min.x + obj_halfscale_x, obj.bounding_box.min.y + obj_halfscale_y };
	AEVec2Sub(&diff_pos, &boss_middle, &diff_pos);

	f32 min_distx = boss_hw + obj_halfscale_x;
	f32 min_disty = boss_hh + obj_halfscale_y;

	f32 depth_x = min_distx - fabs(diff_pos.x);

	f32 depth_y = min_disty - fabs(diff_pos.y);
	if (depth_x != 0 && depth_y != 0)
	{
		if (obj.type == GO_TYPE::GO_WALL && fabs(depth_x) < fabs(depth_y))
		{
			boss->curr_pos.x += diff_pos.x < 0 ? -depth_x : +depth_x;
		}
		else
		{
			boss->curr_pos.y += depth_y;
			boss->curr_vel.y = 0;

			if (diff_pos.y > 0)
			{
				boss->on_ground = true;
			}
		}
	}
}

/*!*****************************************************************************
  \brief
	Function to handle collision of boss with other game objects. Checks
	through all objects the boss is currently colliding with, and calls the
	appropriate function to handle each collision.

  \param obj
	a reference to a game object, assumed to be the boss game object itself.
*******************************************************************************/
void boss_collide(gameObject& obj)
{
	Boss* boss = dynamic_cast<Boss*>(&obj);
	boss->on_ground = false;
	vector<gameObject*>& colliders = obj.colliders;
	for (vector<gameObject*>::iterator i = colliders.begin(); i != colliders.end(); ++i)
	{
		gameObject& obj_col = **i;
		if (obj_col.type == GO_TYPE::GO_PLATFORM && !boss->on_ground)
		{
			AEVec2 e1{ 0,1 };
			if (AEVec2DotProduct(&boss->curr_vel, &e1) < 0)
			{
				adjust_boss_movement(obj, obj_col);
			}
		}

		if (obj_col.type == GO_TYPE::GO_WALL)
		{
			adjust_boss_movement(obj, obj_col);
		}

		if (obj_col.type == GO_TYPE::GO_PLAYERATTACKCOLLIDER)
		{
			if (!boss->is_invul)
			{
				bool ignore{ false };
				for (gameObject* x : boss->ignorecolliders)
				{
					if (x == &obj_col)
					{
						ignore = true;
						break;
					}
				}
				if (ignore) continue;

				Attack_Collider* attack_collider = dynamic_cast<Attack_Collider*>(&obj_col);
				s32 dmg_taken = attack_collider->damage;
				dmg_taken = boss->is_vul ? dmg_taken : dmg_taken / 2;
				boss->take_damage(dmg_taken);
				particleSystem* pParticleSystem = create_damaged_particles(boss);
				if (boss->is_vul)
				{
					AEVec2 push = attack_collider->flinch_scale;
					push.x = boss->curr_pos.x >= attack_collider->owner->curr_pos.x ? push.x : -push.x;
					AEVec2Scale(&push, &push, boss->flinch_resistance);
					if (!boss->is_attacking) AEVec2Add(&boss->acceleration, &boss->acceleration, &push);
					boss->on_ground = false;
					pParticleSystem->lifetime_range[0] = pParticleSystem->lifetime_range[1] = 0.5f;
					pParticleSystem->color_range[0] = pParticleSystem->color_range[1] = { 1.0f, 0.0f, 0.7f, 1.0f };
					pParticleSystem->scale_range[0] = { 10.0f, 10.0f };
					pParticleSystem->scale_range[1] = { 20.0f, 20.0f };
					pParticleSystem->speed_range[0] = 300.0f;
					pParticleSystem->speed_range[1] = 500.0f;
				}
				boss->ignorecolliders.push_back(attack_collider);
			}
		}
	}
}

/*!*****************************************************************************
  \brief
	If the boss is currently attacking, call the attack function of the current
	attack. If the duration of the attack is over, sets the cooldown of the
	boss and remove the attack collider from the vector of game objects.
*******************************************************************************/
void Boss::attack()
{
	if (is_attacking)
	{
		if (attack_timer <= 0)
		{
			cooldown_timer = curr_Attack->cooldown_timer;
			is_attacking = false;
			curr_Attack = nullptr;
			setup = false;
			remove_attack_collider(this);
		}

		else
		{
			if (!curr_Attack->attack)
				return;
			curr_Attack->attack(*this);
			attack_timer -= G_DELTATIME;
		}
	}
}

/*!*****************************************************************************
  \brief
	Sets the boss to be invulnerable for a duration.

  \param timer
	the duration the boss should be invulnerable
*******************************************************************************/
void Boss::start_invul(f32 timer)
{
	is_invul = true;
	invul_timer = timer;
}

/*!*****************************************************************************
  \brief
	Sets the boss to not be invulnerable if the duration is over
*******************************************************************************/
void Boss::invul()
{
	if (is_invul)
	{
		if (invul_timer > 0.0f)
		{
			invul_timer -= G_DELTATIME;
		}

		else
		{
			is_invul = false;
			invul_timer = 0;
		}
	}
}

/*!*****************************************************************************
  \brief
	Sets the boss to be vulnerable for a duration, and creates particles to
	indicate it.

  \param timer
	the duration the boss should be vulnerable
*******************************************************************************/
void Boss::start_vul(f32 timer)
{
	// play sound
	is_vul = true;
	vul_timer = timer;
	particleSystem* pParticleSystem = create_particlesystem(this);
	pParticleSystem->max_particles = 1;
	pParticleSystem->lifetime = 0.1f;
	pParticleSystem->lifetime_range[0] = pParticleSystem->lifetime_range[1] = 1.5f;
	pParticleSystem->scale_range[0] = pParticleSystem->scale_range[1] = { 100.0f, 100.0f };
	pParticleSystem->speed_range[0] = pParticleSystem->speed_range[1] = 0.0f;
	pParticleSystem->behaviour = boss_vulnerable;
	pParticleSystem->obj_sprite->texture = pTexVulnerable;
}

/*!*****************************************************************************
  \brief
	Sets the boss to not be vulnerable if the duration is over
*******************************************************************************/
void Boss::vul()
{
	if (is_vul)
	{
		if (vul_timer > 0)
		{
			vul_timer -= G_DELTATIME;
			obj_sprite->tint = { 0.0f, 0.0f, 1.0f, 1.0f };
		}

		else if (vul_timer < 0)
		{
			is_vul = false;
			vul_timer = 0;
			obj_sprite->tint = { 1.0f, 1.0f, 1.0f, 1.0f };
		}
	}
}

/*!*****************************************************************************
  \brief
	Reduces the health of the boss based on the damage taken. If the boss is
	currently weak, set the boss to be vulnerable.

  \param damage
	the damage taken by the boss
*******************************************************************************/
void Boss::take_damage(s32 damage)
{
	health -= damage;

	if (vul_timer)
	{
		Camera::shake(20.0f, 0.1f);
		hit_strong_sound->play_sound();
	}
	else
	{
		Camera::shake(5.0f, 0.1f);
		hit_weak_sound->play_sound();
	}

	if (is_weak)
	{
		weak_sound->play_sound();
		Camera::look_at(&curr_pos, 0.5f);
		Camera::temp_zoom(1.5f, 0.5f);
		temp_scale_timer(0.3f, 0.5f);
		start_vul(3.0f);
		is_weak = false;
	}
}

/*!*****************************************************************************
  \brief
	Sets the boss to not be weak if the duration is over. If the boss is weak,
	set the tint to be blue. If not, set the tint to be normal.
*******************************************************************************/
void Boss::weak()
{
	if (is_weak)
	{
		if (!weakened)
		{
			weakened = true;
			particleSystem* pParticleSystem = create_particlesystem(this);
			pParticleSystem->max_particles = 1;
			pParticleSystem->lifetime = 0.1f;
			pParticleSystem->lifetime_range[0] = pParticleSystem->lifetime_range[1] = 1.5f;
			pParticleSystem->scale_range[0] = pParticleSystem->scale_range[1] = { 100.0f, 100.0f };
			pParticleSystem->speed_range[0] = pParticleSystem->speed_range[1] = 0.0f;
			pParticleSystem->behaviour = boss_vulnerable;
			pParticleSystem->obj_sprite->texture = pTexVulnerable2;
		}
		obj_sprite->tint = { 1.0f, 0.0f, 0.0f, 1.0f };
		if (weak_timer > 0) weak_timer -= G_DELTATIME;

		else if (weak_timer < 0)
		{
			is_weak = false;
			weakened = false;
			weak_timer = 0;
			obj_sprite->tint = { 1.0f, 1.0f, 1.0f,1.0f };
		}
	}
}