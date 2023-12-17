/*!*****************************************************************************
\file   boss1attacks.cpp
\author Mohammad Hanif Koh Teck Wee
\par    DP email: mohammadhanifkoh.t\@digipen.edu
\date   March 16 2021

\brief
This file contains the implementation of the attacks used by boss 1 as outlined
in boss.h

The functions include:
- boss1_attacks_load
- boss1_attacks_init
- boss1_attacks_unload
- boss_jump
- crate_collide
- boss_boomerang
- boss_slash
- boss_slam

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************************/
#include "boss.h"
#include "audio.h"
#include "attack.h"
#include "level_manager.h"

enum Boss1_Anim {
	IDLE = 0,
	WALK,
	LEAP,
	LAND,
	SLASH,
	SLAM,
	TACKLE,
	BOOMERANG
};

static AEGfxTexture* pTex_crate;
static AEGfxTexture* pTex_boomerang;
static AEGfxTexture* pTex_slam;
static AEGfxTexture* pTex_slash;
static AEGfxTexture* pTex_arrow;
static AEGfxTexture* pTex_cut;
static Audio* explosion;
static Audio* charge_sound;
static Audio* crate_create_sound;

static gameObject* indicator_1{ nullptr };
static gameObject* indicator_2{ nullptr };
static gameObject* indicator_3{ nullptr };

void boss_jump(gameObject&);
void boss_boomerang(gameObject&);
void boss_slash(gameObject& gameObj);
void boss_slam(gameObject& gameObj);

static Boss_Attack jump{ 1.0f, 0, 4, boss_jump, 1.0f , 5.0f, {200.0f, 300.0f} };
static Boss_Attack boomerang{ 1.0f, 0, 2, boss_boomerang, 1.0f , 3.0f, {200.0f, 300.0f} };
static Boss_Attack slash{ 1.0f, 0, 1, boss_slash, 1.0f , 3.0f, {200.0f, 300.0f} };
static Boss_Attack slam{ 1.0f, 0, 3, boss_slam, 1.0f , 4.0f, {200.0f, 300.0f} };

/*!*****************************************************************************
  \brief
	Loads all assets and allocates memory needed for the attacks used by boss
	1. Called once in the Boss 1 game_object_load function.
*******************************************************************************/
void boss1_attacks_load()
{
	pTex_crate = AEGfxTextureLoad("./Sprites/crate.png");
	AE_ASSERT_MESG(pTex_crate, "Failed to create Attack Collider Texture!");
	pTex_boomerang = AEGfxTextureLoad("./Sprites/Axe.png");
	AE_ASSERT_MESG(pTex_boomerang, "Failed to create Axe Texture!");
	pTex_slam = AEGfxTextureLoad("./Sprites/slam.png");
	AE_ASSERT_MESG(pTex_slam, "Failed to create Attack Collider Texture!");
	pTex_slash = AEGfxTextureLoad("./Sprites/slash.png");
	AE_ASSERT_MESG(pTex_slash, "Failed to create Attack Collider Texture!");
	pTex_arrow = AEGfxTextureLoad("./Sprites/arrow_indicator.png");
	AE_ASSERT_MESG(pTex_arrow, "Failed to create Attack Collider Texture!");
	pTex_cut = AEGfxTextureLoad("./Sprites/test_spritesheet.png");
	AE_ASSERT_MESG(pTex_cut, "Failed to create Attack Collider Texture!");

	explosion = new Audio("./Audio/explosion.wav", AUDIO_GROUP::SFX);
	charge_sound = new Audio("./Audio/charge_impact.wav", AUDIO_GROUP::SFX);
	crate_create_sound = new Audio("./Audio/boss1_crate.wav", AUDIO_GROUP::SFX);

	indicator_1 = push_object(GO_TYPE::GO_ATTACKINDICATOR, nullptr);
	indicator_1->flag = false;
	indicator_2 = push_object(GO_TYPE::GO_ATTACKINDICATOR, nullptr);
	indicator_2->flag = false;
	indicator_3 = push_object(GO_TYPE::GO_ATTACKINDICATOR, nullptr);
	indicator_3->flag = false;
}

/*!*****************************************************************************
  \brief
	Adds all the attacks into Boss 1's vector of attacks. Called once in Boss 1
	game_object_init function.

  \param attacks
	A reference to the vector to add the attacks into.
*******************************************************************************/
void boss1_attacks_init(vector<Boss_Attack>& attacks)
{
	attacks.push_back(jump);
	attacks.push_back(boomerang);
	attacks.push_back(slash);
	attacks.push_back(slam);
}

/*!*****************************************************************************
  \brief
	Unloads all assets and deallocates all memory used in Boss 1 attacks. Called
	once in Boss 1 game_object_unload function.
*******************************************************************************/
void boss1_attacks_unload()
{
	delete indicator_1->obj_sprite;
	delete indicator_2->obj_sprite;
	delete indicator_3->obj_sprite;
	delete explosion;
	delete charge_sound;
	delete crate_create_sound;
	AEGfxTextureUnload(pTex_boomerang);
	AEGfxTextureUnload(pTex_slam);
	AEGfxTextureUnload(pTex_slash);
	AEGfxTextureUnload(pTex_crate);
	AEGfxTextureUnload(pTex_arrow);
	AEGfxTextureUnload(pTex_cut);
}

/*!*****************************************************************************
  \brief
	The jumping attack for Boss 1. The boss will leap towards the edge of the
	screen, depending on which side the player is relative to the boss. If the
	player is near the boss when it lands, the player will receive damage. After
	landing, 3 crates then drop from the sky and damage the player if it hits
	the player. Upon touching the ground, the crates become a platform which
	the player can stand on. After a brief duration, the boss will then charge
	across the map, dealing damage if it touches the player. To avoid this,
	players must stand on the crates and jump off of them at the right time.
	All crates will be destroyed by the boss when the boss touches them.

  \param gameObj
	A reference to the boss game object.
*******************************************************************************/
void boss_jump(gameObject& gameObj)
{
	static int num_crates;
	static f32 alpha_increment;
	static particleSystem* pParticleSystem{ nullptr };

	Boss* boss = dynamic_cast<Boss*> (&gameObj);
	if (!boss) return;
	if (!Level::player) return;
	Attack_Collider* collider = get_attack_collider(boss);

	if (!boss->setup)
	{
		num_crates = 0;
		collider = dynamic_cast<Attack_Collider*>(push_object(GO_TYPE::GO_BOSSATTACKCOLLIDER, boss));
		collider->obj_sprite->texture = pTex_cut;
		collider->damage = jump.damage;
		collider->flag = false;
		boss->is_facing_right = Level::player->curr_pos.x > boss->curr_pos.x;
		pParticleSystem = nullptr;
		indicator_1->flag = false;
		indicator_2->flag = false;
		indicator_3->flag = false;
		indicator_1->obj_sprite->texture = nullptr;
		indicator_2->obj_sprite->texture = nullptr;
		indicator_3->obj_sprite->texture = nullptr;
		indicator_1->scale = { 100.0f, 1500.0f };
		indicator_2->scale = { 100.0f, 1500.0f };
		indicator_3->scale = { 100.0f, 1500.0f };
		indicator_1->obj_sprite->tint = { 1.0f, 0.0f, 0.0f, 0.5f };
		indicator_2->obj_sprite->tint = { 1.0f, 0.0f, 0.0f, 0.5f };
		indicator_3->obj_sprite->tint = { 1.0f, 0.0f, 0.0f, 0.5f };
		boss->attack_phase = 1;
		boss->curr_vel.y = 1500;
		boss->curr_vel.x = boss->is_facing_right ?
			(Level::boss_room_camera.max.x - 100.0f - boss->curr_pos.x) :
			(Level::boss_room_camera.min.x + 100.0f - boss->curr_pos.x);
		boss->on_ground = false;
		boss->attack_timer = 3.0f;
		alpha_increment = 0.025f;
		boss->setup = true;
		boss->animations[LEAP].reset();
		boss->animations[LAND].reset();
		boss->animations[TACKLE].reset();
	}

	if (boss->attack_phase == 1)
	{
		if (boss->on_ground)
		{
			collider->flag = true;
			collider->curr_pos = boss->curr_pos;
			collider->curr_pos.x += boss->is_facing_right ? -25.0f : 25.0f;
			collider->curr_pos.y -= 50.0f;
			collider->scale = { 300.0f, 100.0f };
			Camera::shake(10.0f, 0.5f);
			indicator_1->curr_pos = { Level::boss_room_camera.min.x + 250.0f, Level::boss_room_camera.min.y + 500.0f };
			indicator_2->curr_pos = { indicator_1->curr_pos.x + 350.0f,Level::boss_room_camera.min.y + 500.0f };
			indicator_3->curr_pos = { indicator_2->curr_pos.x + 350.0f, Level::boss_room_camera.min.y + 500.0f };
			indicator_1->flag = true;
			indicator_2->flag = true;
			indicator_3->flag = true;
			boss->animations[LAND].play_animation(false);

			if (!pParticleSystem)
			{
				pParticleSystem = create_ground_particles(collider);
				pParticleSystem->color_range[0] = pParticleSystem->color_range[1] = { 0.95f, 0.64f, 0.38f, 0.8f };
				pParticleSystem->position_offset.y = -collider->scale.y / 2;
			}

			if (boss->attack_phase == 1 && boss->attack_timer <= 1.0f)
			{
				remove_attack_collider(boss);
				boss->curr_vel.x = 0;
				boss->attack_timer = 1.5f;
				crate_create_sound->play_sound();

				pParticleSystem = nullptr;
				++boss->attack_phase;
			}
		}
		else
			boss->animations[LEAP].play_animation(false);
	}

	if (boss->attack_phase == 2)
	{
		if (num_crates < 3)
		{
			collider = dynamic_cast<Attack_Collider*>(push_object(GO_TYPE::GO_BOSSATTACKCOLLIDER, boss));
			collider->damage = jump.damage;
			collider->obj_sprite->texture = pTex_crate;
			collider->obj_sprite->rot = PI / 4;
			collider->curr_pos = { Level::boss_room_camera.min.x + 250.0f + num_crates * 350.0f, 1200.0f };
			collider->acceleration.y = LEVEL_GRAVITY;
			collider->scale = { 100.0f, 100.0f };
			collider->on_collide = crate_collide;
			++num_crates;
		}

		else
		{
			boss->attack_timer = 3.25f;
			++boss->attack_phase;
		}
	}

	if (boss->attack_phase == 3)
	{
		if (boss->attack_timer <= 0.25f)
		{
			boss->is_facing_right = Level::player->curr_pos.x > boss->curr_pos.x;
			boss->curr_vel.x = boss->is_facing_right ? 1000.0f : -1000.0f;
			boss->attack_timer = 1.5f;
			collider = dynamic_cast<Attack_Collider*>(push_object(GO_TYPE::GO_BOSSATTACKCOLLIDER, boss));
			collider->damage = jump.damage;
			collider->obj_sprite->texture = nullptr;
			collider->obj_sprite->tint.a = 0.0f;
			collider->obj_sprite->flip_x = !boss->is_facing_right;
			collider->curr_pos = boss->curr_pos;
			collider->scale = { 250.0f, 300.0f };
			++boss->attack_phase;
		}

		else if (boss->attack_timer <= 2.25f)
		{
			AEVec2 camPos;
			AEGfxGetCamPosition(&camPos.x, &camPos.y);
			boss->animations[TACKLE].play_anim_frame(0);
			boss->is_facing_right = Level::player->curr_pos.x > boss->curr_pos.x;
			if (indicator_1->flag == false)
			{
				indicator_1->obj_sprite->tint = { 1.0f, 1.0f, 0.0f, 0.4f };
				indicator_1->curr_pos = { camPos.x, boss->curr_pos.y };
				indicator_1->scale = { static_cast<f32>(AEGetWindowWidth()), boss->scale.y };
				indicator_1->flag = true;
			}

			indicator_1->obj_sprite->tint.g -= 1.0f / 120.0f;
		}

		else if (boss->attack_timer <= 2.5f)
		{
			indicator_1->flag = false;
			indicator_2->flag = false;
			indicator_3->flag = false;
			if (!pParticleSystem)
			{
				pParticleSystem = create_particlesystem({ boss->curr_pos });
				pParticleSystem->loop = true;
				pParticleSystem->obj_sprite->flip_x = boss->is_facing_right;
				pParticleSystem->scale = { 1, 1 };
				pParticleSystem->max_particles = 4;
				pParticleSystem->behaviour = nullptr;
				pParticleSystem->obj_sprite->texture = pTex_arrow;
				pParticleSystem->color_range[0] = { 1.0f, 1.0f, 1.0f, 0.5f };
				pParticleSystem->color_range[1] = { 1.0f, 1.0f, 1.0f, 0.5f };
				pParticleSystem->lifetime_range[0] = pParticleSystem->lifetime_range[1] = 1.0f;
				pParticleSystem->scale_range[0] = pParticleSystem->scale_range[1] = { 150.0f, boss->scale.y };
				pParticleSystem->speed_range[0] = pParticleSystem->speed_range[1] = 1000.0f;
				pParticleSystem->direction_range[0] = pParticleSystem->direction_range[1] = boss->is_facing_right ? PI : 0;
				pParticleSystem->emission_rate = 0.25f;
			}
		}
	}

	if (boss->attack_phase == 4)
	{
		boss->animations[TACKLE].play_animation();
		if (indicator_1->flag)
		{
			indicator_1->flag = false;
			pParticleSystem->delete_system();
			pParticleSystem = nullptr;
		}
		collider->curr_pos = { boss->curr_pos.x, boss->curr_pos.y - 30.0f };

		if (boss->attack_timer <= 0.5f)
		{
			boss->attack_timer = jump.weak_duration;
			++boss->attack_phase;
			pParticleSystem = nullptr;
		}
	}

	if (boss->attack_phase == 5)
	{
		if (!boss->is_weak)
			charge_sound->play_sound();
		boss->curr_vel.x = 0;
		Camera::shake(10.0f, 0.5f);
		boss->is_weak = true;
		boss->weak_timer = jump.weak_duration;

		if (!pParticleSystem)
		{
			pParticleSystem = create_falling_particles();
			pParticleSystem->color_range[0] = pParticleSystem->color_range[1] = { 0.95f, 0.64f, 0.38f, 0.8f };
		}

		boss->attack_timer = -1.0f;
	}
}

/*!*****************************************************************************
  \brief
	The oncollide function for the crates. Changes type to destructable if it
	touches the floor, and gets destroyed if it touches the boss.

  \param gameObj
	A reference to the gameobject calling the function, assumed to be a crate
	used specifically in the jump attack of boss 1.
*******************************************************************************/
void crate_collide(gameObject& obj)
{
	Attack_Collider* collider = dynamic_cast<Attack_Collider*>(&obj);
	vector<gameObject*>& colliders = obj.colliders;
	for (vector<gameObject*>::iterator i = colliders.begin(); i != colliders.end(); ++i)
	{
		gameObject& obj_col = **i;
		if (obj_col.type == GO_TYPE::GO_PLATFORM)
		{
			AEVec2Zero(&collider->curr_vel);
			collider->curr_pos.y = obj_col.bounding_box.max.y + collider->scale.y / 2;
			collider->obj_sprite->rot = 0.0f;
			collider->type = GO_TYPE::GO_DESTRUCTABLE;
		}

		if (obj_col.type == GO_TYPE::GO_BOSSATTACKCOLLIDER)
		{
			collider->flag = false;
		}
	}
}

/*!*****************************************************************************
  \brief
	The boomerang attack for boss 1. Shoots a boomerang in the direction of the
	player. After reaching a certain distance, the boomerang lowers and travels
	back to the boss. The boomerang starts too high to hit the player, and the
	player may avoid the boomerang by jumping over it when the boomerang is
	returning to the boss.

  \param gameObj
	A reference to the boss game object.
*******************************************************************************/
void boss_boomerang(gameObject& gameObj)
{
	Boss* boss = dynamic_cast<Boss*> (&gameObj);
	if (!boss) return;
	if (!Level::player) return;
	Attack_Collider* collider = get_attack_collider(boss);

	if (!boss->setup)
	{
		boss->attack_phase = 1;
		collider = dynamic_cast<Attack_Collider*>(push_object(GO_TYPE::GO_BOSSATTACKCOLLIDER, boss));
		boss->is_facing_right = Level::player->curr_pos.x > boss->curr_pos.x;
		collider->obj_sprite->texture = pTex_boomerang;
		collider->damage = boomerang.damage;
		collider->curr_pos.x = boss->curr_pos.x;
		collider->curr_pos.y = boss->curr_pos.y + 150.0f;
		collider->curr_vel.x = boss->is_facing_right ? 25 * 60.0f : -25 * 60.0f;
		collider->acceleration.x = boss->is_facing_right ? -20.0f * 60.0f * G_DELTATIME : 20.0f * 60.0f * G_DELTATIME;
		boss->attack_timer = 3.0f;
		boss->setup = true;
		boss->animations[BOOMERANG].reset();
	}

	if (boss->attack_phase == 1)
	{
		boss->animations[BOOMERANG].play_animation(false);
		collider->obj_sprite->rot += 25.0f * G_DELTATIME;
		if (fabs(collider->curr_vel.x) <= 600.0f && fabs(collider->curr_vel.x) >= 15.0f)
		{
			collider->curr_pos.y -= 3.0f * 60.0f * G_DELTATIME;
		}

		if (boss->attack_timer <= 1.5f && fabs(collider->curr_pos.x - boss->curr_pos.x) <= 10.0f) boss->attack_timer = -1.0f;
	}
}

/*!*****************************************************************************
  \brief
	The slashing attack for boss 1. After a short duration, the boss slashes the
	ground in front of it, dealing damage to the player if the player is within
	range.

  \param gameObj
	A reference to the boss game object.
*******************************************************************************/
void boss_slash(gameObject& gameObj)
{
	static particleSystem* pParticleSystem;
	static f32 anim_time;
	Boss* boss = dynamic_cast<Boss*> (&gameObj);
	if (!boss) return;
	if (!Level::player) return;
	Attack_Collider* collider = get_attack_collider(boss);

	if (!boss->setup)
	{
		anim_time = 0;
		boss->attack_phase = 1;
		boss->is_facing_right = Level::player->curr_pos.x > boss->curr_pos.x;
		collider = dynamic_cast<Attack_Collider*>(push_object(GO_TYPE::GO_BOSSATTACKCOLLIDER, boss));
		collider->flag = false;
		collider->obj_sprite->texture = pTex_slash;
		collider->damage = slash.damage;
		collider->curr_pos.x = boss->is_facing_right ? boss->curr_pos.x + 150.0f : boss->curr_pos.x - 150.0f;
		collider->curr_pos.y = boss->curr_pos.y + 15.0f;
		collider->scale = { 250.0f, 300.0f };
		indicator_1->flag = true;
		indicator_1->obj_sprite->texture = nullptr;
		indicator_1->obj_sprite->tint = { 1.0f, 1.0f, 0.0f, 1.0f };
		indicator_1->curr_pos.x = collider->curr_pos.x;
		indicator_1->curr_pos.y = collider->curr_pos.y - collider->scale.y / 2;
		indicator_1->scale = { 250.0f, 10.0f };
		boss->attack_timer = 2.0f;
		pParticleSystem = nullptr;
		boss->setup = true;
		boss->animations[SLASH].reset();
	}

	if (boss->attack_phase == 1)
	{
		indicator_1->obj_sprite->tint.g -= 1.0f / 90.0f;
		anim_time += G_DELTATIME;
		AEClamp(anim_time, 0, 4);
		boss->animations[SLASH].play_anim_frame(static_cast<u8>(anim_time + 0.5f));

		if (boss->attack_timer <= 0.25f)
		{
			boss->animations[SLASH].play_anim_frame(4);
			collider->damage = 0;
		}

		else if (boss->attack_timer <= 0.5f)
		{
			boss->animations[SLASH].play_anim_frame(4);
			if (!collider->flag)
				explosion->play_sound();

			if (!pParticleSystem)
			{
				pParticleSystem = create_ground_particles(collider);
				pParticleSystem->color_range[0] = pParticleSystem->color_range[1] = { 0.95f, 0.64f, 0.38f, 0.8f };
				pParticleSystem->position_offset.y = -collider->scale.y / 2;
			}

			collider->flag = true;
			indicator_1->flag = false;
		}
	}
}

/*!*****************************************************************************
  \brief
	The slam attack for boss 1. After a brief duration, 3 pillars will erupt
	from the ground one after another, damaging the player if it touches the
	player. The first pillar takes longer to appear, but each subsequent pillar
	appears after a shorter duration.

  \param gameObj
	A reference to the boss game object.
*******************************************************************************/
void boss_slam(gameObject& gameObj)
{
	static f32 x_offset;
	static bool explode;
	static particleSystem* pParticleSystem;
	static f32 anim_time;

	Boss* boss = dynamic_cast<Boss*> (&gameObj);
	if (!boss) return;
	if (!Level::player) return;
	Attack_Collider* collider = get_attack_collider(boss);

	if (!boss->setup)
	{
		anim_time = 0;
		boss->attack_phase = 1;
		collider = dynamic_cast<Attack_Collider*>(push_object(GO_TYPE::GO_BOSSATTACKCOLLIDER, boss));
		boss->is_facing_right = Level::player->curr_pos.x > boss->curr_pos.x;
		collider->flag = false;
		collider->obj_sprite->texture = pTex_slam;
		collider->damage = slam.damage;
		x_offset = boss->is_facing_right ? 200.0f : -200.0f;
		collider->curr_pos.x = boss->curr_pos.x + x_offset;
		collider->curr_pos.y = boss->curr_pos.y + 80.0f;
		collider->scale = { 150.0f, 400.0f };
		indicator_1->flag = true;
		indicator_1->obj_sprite->texture = nullptr;
		indicator_1->obj_sprite->tint = { 1.0f, 1.0f, 0.0f, 1.0f };
		indicator_1->curr_pos.x = collider->curr_pos.x;
		indicator_1->curr_pos.y = collider->curr_pos.y - collider->scale.y / 2;
		indicator_1->scale = { 150.0f, 7.5f };
		boss->attack_timer = 2.0f;
		explode = false;
		pParticleSystem = nullptr;
		boss->setup = true;
		boss->animations[SLAM].reset();
	}

	if (boss->attack_phase >= 1 && boss->attack_phase <= 3)
	{
		anim_time += G_DELTATIME;
		u8 frame_time = static_cast<u8>(AEClamp((anim_time + 0.5f) * 2.0f, 0, 4));
		boss->animations[SLASH].play_anim_frame(frame_time);
		indicator_1->obj_sprite->tint.g -= 1.0f / 60.0f;

		if (boss->attack_timer <= 0.5f)
		{
			collider->damage = 0;
		}

		else if (boss->attack_timer <= 0.75f)
		{
			collider->flag = false;
			collider->curr_pos.x += x_offset;
			boss->attack_timer = 1.5f;
			explode = false;
			pParticleSystem = nullptr;
			++boss->attack_phase;
		}

		else if (boss->attack_timer <= 1.0f)
		{
			if (!collider->flag && boss->attack_phase < 3)
			{
				indicator_1->curr_pos.x += x_offset;
				indicator_1->obj_sprite->tint = { 1.0f, 1.0f, 0.0f, 1.0f };
			}

			collider->damage = slam.damage;
			collider->flag = true;
			Camera::shake(10.0f, 0.2f);
			if (!explode)
				explosion->play_sound();
			explode = true;

			if (!pParticleSystem)
			{
				pParticleSystem = create_particlesystem(collider->curr_pos);
				pParticleSystem->lifetime = 0.5f;
				pParticleSystem->emission_rate = 0.016f;
				pParticleSystem->max_particles = 20;
				pParticleSystem->lifetime_range[0] = pParticleSystem->lifetime_range[1] = 0.5f;
				pParticleSystem->scale_range[0] = pParticleSystem->scale_range[1] = { 10.0f, 10.0f };
				pParticleSystem->speed_range[0] = pParticleSystem->speed_range[1] = 800;
				pParticleSystem->direction_range[0] = 3 * PI / 8;
				pParticleSystem->direction_range[1] = 5 * PI / 8;
				pParticleSystem->rotation_range[0] = 0;
				pParticleSystem->rotation_range[0] = 2 * PI;
				pParticleSystem->particle_increment = 1;
				pParticleSystem->color_range[0] = pParticleSystem->color_range[1] = { 1.0f, 0.0f, 0.0f, 0.8f };
				pParticleSystem->set_aabb();
				pParticleSystem->position_offset.y = -collider->scale.y / 2;
			}
		}
	}

	if (boss->attack_phase == 4)
	{
		indicator_1->flag = false;
		boss->is_weak = true;
		boss->weak_timer = slam.weak_duration;
		boss->attack_timer = -1.0f;
	}
}