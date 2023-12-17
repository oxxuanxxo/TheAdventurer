/*!*****************************************************************************
\file   boss3attacks.cpp
\author Mohammad Hanif Koh Teck Wee
\par    DP email: mohammadhanifkoh.t\@digipen.edu
\date   March 27 2021

\brief
This file contains the implementation of the attacks used by boss 3 as outlined
in boss.h

The functions include:
- boss3_attacks_load
- boss3_attacks_init
- boss3_attacks_unload
- boss_sword_slash
- boss_sword_stab
- boss_sword_big_sword
- boss_ultimate
- big_sword_collide
- setup_sword
- stab_collide

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************************/
#include "boss.h"
#include "attack.h"
#include "level_manager.h"
#include "graphics.h"

enum Boss3_Anim {
	IDLE = 0,
	WALK,
	SLASH_1,
	SLASH_2,
	STAB,
	LEAP,
	SWORD_FALL,
	VULNERABLE,
	SUMMON_SWORD
};

#define SWORDS_SPEED 1000.0f

static AEGfxTexture* pTex_shockwave;
static AEGfxTexture* pTex_crate;
static AEGfxTexture* pTex_sword;
static AEGfxTexture* pTex_arrow;
static AEGfxTexture* pTex_slash;
static AEGfxTexture* pTex_dust;
static AEGfxVertexList* slash_mesh;

static AEVec2 slash_size;

static Audio* slash_sound_1;
static Audio* slash_sound_2;
static Audio* ground_crash_sound;
static Audio* slam_explosion;
static Audio* whoosh_sound;
static Audio* wall_hit_sound;

static gameObject* indicator_1{ nullptr };
static gameObject* indicator_2{ nullptr };
static gameObject* indicator_3{ nullptr };

void boss_sword_slash(gameObject& gameObj);
void boss_sword_stab(gameObject& gameObj);
void boss_sword_big_sword(gameObject& gameObj);
void boss_ultimate(gameObject& gameObj);
void big_sword_collide(gameObject& obj);
void setup_sword(Boss3* boss, u16 swordNo);
void stab_collide(gameObject&);

static Animation slash_anim_1 = Animation(6, { 640,0 }, 0, -128, 0.063f);
static Animation slash_anim_2 = Animation(6, { 640,0 }, 0, 128, 0.063f);
Boss_Attack Sword_Slash{ 1.0f, 0, 2, boss_sword_slash, 1.0f , 5.0f, {200.0f, 300.0f} };
Boss_Attack Sword_Stab{ 1.0f, 0, 2, boss_sword_stab, 1.0f , 5.0f, {1500.0f, 900.0f} };
Boss_Attack Sword_Big{ 1.0f, 0, 3, boss_sword_big_sword, 1.0f , 5.0f, {1500.0f, 900.0f} };
Boss_Attack Ultimate{ 1.0f, 0, 1, boss_ultimate, 1.0f , 5.0f, {1500.0f, 900.0f} };

/*!*****************************************************************************
  \brief
	Loads all assets and allocates memory needed for the attacks used by boss
	3. Called once in the Boss 3 game_object_load function.
*******************************************************************************/
void boss3_attacks_load()
{
	pTex_shockwave = AEGfxTextureLoad("./Sprites/Player/shockwave.png");
	AE_ASSERT_MESG(pTex_shockwave, "Failed to create Attack Collider Texture!");
	pTex_crate = AEGfxTextureLoad("./Sprites/stab_effect.png");
	AE_ASSERT_MESG(pTex_crate, "Failed to create Attack Collider Texture!");
	pTex_sword = AEGfxTextureLoad("./Sprites/weapon_sword.png");
	AE_ASSERT_MESG(pTex_sword, "Failed to create Attack Collider Texture!");
	pTex_arrow = AEGfxTextureLoad("./Sprites/arrow_indicator.png");
	AE_ASSERT_MESG(pTex_arrow, "Failed to create Attack Collider Texture!");
	pTex_slash = AEGfxTextureLoad("./Sprites/sword_effect_spritesheet.png");
	AE_ASSERT_MESG(pTex_slash, "Failed to create Attack Collider Texture!");
	pTex_dust = AEGfxTextureLoad("./Sprites/Particle/dust.png");
	AE_ASSERT_MESG(pTex_dust, "Failed to create Dust Texture!");
	AEVec2 trans = { 128.0f,128.0f };
	slash_size = read_png_size("./Sprites/sword_effect_spritesheet.png");
	slash_mesh = create_square_mesh(trans.x / slash_size.x, trans.y / slash_size.y);

	slash_sound_1 = new Audio("./Audio/slash_1.wav", AUDIO_GROUP::SFX);
	slash_sound_2 = new Audio("./Audio/slash_2.wav", AUDIO_GROUP::SFX);
	ground_crash_sound = new Audio("./Audio/crash_1.wav", AUDIO_GROUP::SFX);
	slam_explosion = new Audio("./Audio/explosion.wav", AUDIO_GROUP::SFX);
	whoosh_sound = new Audio("./Audio/airdash.wav", AUDIO_GROUP::SFX);
	wall_hit_sound = new Audio("./Audio/boss3_wall_hit.wav", AUDIO_GROUP::SFX);

	indicator_1 = push_object(GO_TYPE::GO_ATTACKINDICATOR, nullptr);
	indicator_1->flag = false;
	indicator_2 = push_object(GO_TYPE::GO_ATTACKINDICATOR, nullptr);
	indicator_2->flag = false;
	indicator_3 = push_object(GO_TYPE::GO_ATTACKINDICATOR, nullptr);
	indicator_3->flag = false;
}

/*!*****************************************************************************
  \brief
	Adds all the attacks into Boss 3's vector of attacks. Called once in Boss 3
	game_object_init function.

  \param attacks
	A reference to the vector to add the attacks into.
*******************************************************************************/
void boss3_attacks_init(vector<Boss_Attack>& attacks)
{
	attacks.push_back(Sword_Slash);
	attacks.push_back(Sword_Stab);
	attacks.push_back(Sword_Big);
	attacks.push_back(Ultimate);
}

/*!*****************************************************************************
  \brief
	Unloads all assets and deallocates all memory used in Boss 3 attacks. Called
	once in Boss 3 game_object_unload function.
*******************************************************************************/
void boss3_attacks_unload()
{
	delete indicator_1->obj_sprite;
	delete indicator_2->obj_sprite;
	delete indicator_3->obj_sprite;
	delete slash_sound_1;
	delete slash_sound_2;
	delete ground_crash_sound;
	delete whoosh_sound;
	delete slam_explosion;
	delete wall_hit_sound;
	AEGfxTextureUnload(pTex_shockwave);
	AEGfxTextureUnload(pTex_sword);
	AEGfxTextureUnload(pTex_crate);
	AEGfxTextureUnload(pTex_arrow);
	AEGfxTextureUnload(pTex_slash);
	AEGfxTextureUnload(pTex_dust);
	AEGfxMeshFree(slash_mesh);
}

/*!*****************************************************************************
  \brief
	The slashing combo attack of boss 3. The boss slashes 3 times in front of
	him, and then performs a short leap in the direction of the player. Deals
	damage to the player if the player is close to the boss when he lands. If
	enraged, the 3 swords will then appear at the left, top and right of the
	screen and shoot towards the player. If the player touches the swords, he
	will get damaged.

  \param gameObj
	A reference to the boss game object.
*******************************************************************************/
void boss_sword_slash(gameObject& gameObj)
{
	AEVec2 camPos;
	AEGfxGetCamPosition(&camPos.x, &camPos.y);
	static AEGfxVertexList* orig_mesh;
	static AEVec2 orig_size;
	static f32 translate_x, translate_y;
	Boss3* boss = dynamic_cast<Boss3*> (&gameObj);
	if (!boss) return;

	Attack_Collider* collider = get_attack_collider(boss);

	if (!boss->setup)
	{
		// PUT ATTACK SETUPS HERE ( E.G. COLLIDER SPAWNS WHERE, ANY COUNTDOWNS ETC. )
		collider = dynamic_cast<Attack_Collider*>(push_object(GO_TYPE::GO_BOSSATTACKCOLLIDER, boss));
		orig_mesh = collider->obj_sprite->mesh;
		orig_size = collider->obj_sprite->size;
		collider->flag = true;
		collider->curr_pos.x = boss->is_facing_right ? boss->curr_pos.x + 30 : boss->curr_pos.x - 30;
		collider->curr_pos.y = boss->curr_pos.y + 80;
		collider->obj_sprite->flip_x = boss->is_facing_right ? false : true;
		collider->damage = Sword_Slash.damage;
		collider->flinch_scale = Sword_Slash.flinch_scale;
		collider->obj_sprite->texture = pTex_slash;
		collider->obj_sprite->mesh = slash_mesh;
		collider->obj_sprite->size = slash_size;
		collider->scale = { 200, 150 };
		translate_x = 30.0f, translate_y = -80.0f;
		boss->attack_phase = 1;
		boss->attack_timer = 1.0f;
		boss->setup = true;
		slash_anim_1.set_sprite(collider->obj_sprite);
		slash_anim_1.reset();
		slash_sound_1->play_sound();
		slash_anim_2.set_sprite(collider->obj_sprite);
		slash_anim_2.reset();
		boss->animations[SLASH_1].reset();
		boss->animations[SLASH_2].reset();
		boss->animations[LEAP].reset();
	}

	if (boss->attack_phase == 1)
	{
		set_sword_positions(*boss);
		collider->curr_pos.x = boss->curr_pos.x + (boss->is_facing_right ? translate_x : -translate_x);
		collider->curr_pos.y = boss->curr_pos.y;
		boss->animations[SLASH_1].play_animation(false);
		slash_anim_1.play_animation(false);
		if (boss->attack_timer <= 0.5f)
		{
			collider->flag = true;
			boss->attack_timer = 1.0f;
			++boss->attack_phase;
			slash_anim_1.reset();
			slash_sound_2->play_sound();
			boss->animations[SLASH_1].reset();
		}
	}

	if (boss->attack_phase == 2)
	{
		set_sword_positions(*boss);
		collider->curr_pos.x = boss->curr_pos.x + (boss->is_facing_right ? translate_x : -translate_x);
		collider->curr_pos.y = boss->curr_pos.y;
		boss->animations[SLASH_2].play_animation(false);
		slash_anim_2.play_animation(false);
		if (boss->attack_timer <= 0.5f)
		{
			collider->flag = true;
			boss->attack_timer = 1.0f;
			++boss->attack_phase;
			slash_anim_2.reset();
			slash_sound_1->play_sound();
			boss->animations[SLASH_2].reset();
		}
	}

	if (boss->attack_phase == 3)
	{
		set_sword_positions(*boss);
		collider->curr_pos.x = boss->curr_pos.x + (boss->is_facing_right ? translate_x : -translate_x);
		collider->curr_pos.y = boss->curr_pos.y;
		boss->animations[SLASH_1].play_animation(false);
		slash_anim_1.play_animation(false);

		if (boss->attack_timer <= 0.5f)
		{
			slash_anim_1.reset();
			boss->animations[SLASH_1].reset();
			collider->flag = false;
			collider->obj_sprite->texture = pTex_shockwave;
			collider->obj_sprite->mesh = orig_mesh;
			collider->obj_sprite->size = orig_size;
			collider->curr_pos.x = boss->is_facing_right ? boss->curr_pos.x + 30 : boss->curr_pos.x - 30;
			collider->curr_pos.y = boss->curr_pos.y - 80;
			collider->scale = { 300, 50 };
			boss->is_facing_right = Level::player->curr_pos.x > boss->curr_pos.x;
			translate_x = 0.0f, translate_y = -80.0f;
			boss->acceleration.y = 900;
			boss->acceleration.x = boss->is_facing_right ? 300.0f : -300.0f;
			boss->attack_timer = 1.5f;
			boss->on_ground = false;
			++boss->attack_phase;
		}
	}

	if (boss->attack_phase == 4)
	{
		set_sword_positions(*boss);
		boss->animations[LEAP].play_animation(false);
		if (boss->on_ground)
		{
			boss->animations[LEAP].play_anim_frame(2);
			collider->curr_pos.x = boss->curr_pos.x;
			collider->curr_pos.y = boss->curr_pos.y - 50.0f;
			collider->flag = true;
			Camera::shake(50, 0.3f);
			boss->attack_timer = 2.0f;
			boss->curr_vel.x = 0;
			particleSystem* pParticleSystem = create_particlesystem(boss);
			pParticleSystem->behaviour = slam_particle;
			pParticleSystem->obj_sprite->texture = pTex_dust;
			pParticleSystem->lifetime = 0.25f;
			pParticleSystem->emission_rate = 0.02f;
			pParticleSystem->max_particles = 25;
			pParticleSystem->color_range[0] = pParticleSystem->color_range[1] = { 1.0f, 1.0f, 1.0f, 1.0f };
			pParticleSystem->lifetime_range[0] = pParticleSystem->lifetime_range[1] = 1.5f;
			pParticleSystem->scale_range[0] = { 30.0f, 30.0f };
			pParticleSystem->scale_range[1] = { 60.0f, 60.0f };
			pParticleSystem->speed_range[0] = 200;
			pParticleSystem->speed_range[1] = 200;
			pParticleSystem->direction_range[0] = PI / 4;
			pParticleSystem->direction_range[1] = PI / 2 + PI / 4;
			pParticleSystem->particle_increment = 25;
			pParticleSystem->scale.x = collider->scale.x;
			pParticleSystem->set_aabb();
			pParticleSystem->position_offset.y -= 35.0f;
			ground_crash_sound->play_sound();
			slash_sound_2->play_sound();
			if (boss->enraged)
			{
				boss->swords[0]->obj_sprite->rot = PI;
				boss->swords[1]->obj_sprite->rot = 0;
				boss->swords[2]->obj_sprite->rot = PI / 2;

				AEVec2Zero(&boss->swords[0]->curr_vel);
				AEVec2Zero(&boss->swords[1]->curr_vel);
				AEVec2Zero(&boss->swords[2]->curr_vel);
				++boss->attack_phase;
			}
			else boss->attack_phase = 9;
		}
	}

	if (boss->attack_phase == 5)
	{
		setup_sword(boss, 0);
		setup_sword(boss, 1);
		setup_sword(boss, 2);

		indicator_1->curr_pos = { camPos.x, boss->swords[0]->curr_pos.y };
		indicator_1->scale = { static_cast<f32>(AEGetWindowWidth()), boss->swords[0]->scale.y };
		indicator_1->flag = true;
		indicator_1->obj_sprite->tint = { 1.0f, 0.0f, 0.0f, 0.25f };

		if (boss->attack_timer <= 1.0f)
		{
			boss->swords[0]->curr_vel.x = SWORDS_SPEED;
			if (boss->swords[0]->type != GO_TYPE::GO_ENEMYATTACKCOLLIDER)
				slash_sound_1->play_sound();
			boss->swords[0]->type = GO_TYPE::GO_ENEMYATTACKCOLLIDER;
			boss->attack_timer = 2.0f;
			indicator_1->flag = false;
			++boss->attack_phase;
		}

		else if (boss->attack_timer <= 1.5f)
		{
			collider->flag = false;
		}
	}

	if (boss->attack_phase == 6)
	{
		setup_sword(boss, 1);
		setup_sword(boss, 2);

		indicator_1->curr_pos = { camPos.x, boss->swords[1]->curr_pos.y };
		indicator_1->scale = { static_cast<f32>(AEGetWindowWidth()), boss->swords[1]->scale.y };
		indicator_1->flag = true;
		indicator_1->obj_sprite->tint = { 1.0f, 0.0f, 0.0f, 0.25f };

		if (boss->attack_timer <= 1.0f)
		{
			boss->swords[1]->curr_vel.x = -SWORDS_SPEED;
			if (boss->swords[1]->type != GO_TYPE::GO_ENEMYATTACKCOLLIDER)
				slash_sound_1->play_sound();
			boss->swords[1]->type = GO_TYPE::GO_ENEMYATTACKCOLLIDER;
			boss->attack_timer = 2.0f;
			indicator_1->flag = false;
			++boss->attack_phase;
		}
	}

	if (boss->attack_phase == 7)
	{
		setup_sword(boss, 2);

		indicator_1->curr_pos = { boss->swords[2]->curr_pos.x, camPos.y };
		indicator_1->scale = { boss->swords[2]->scale.y, static_cast<f32>(AEGetWindowHeight()) };
		indicator_1->flag = true;
		indicator_1->obj_sprite->tint = { 1.0f, 0.0f, 0.0f, 0.25f };

		if (boss->attack_timer <= 1.0f)
		{
			boss->swords[2]->curr_vel.y = -SWORDS_SPEED;
			if (boss->swords[2]->type != GO_TYPE::GO_ENEMYATTACKCOLLIDER)
				slash_sound_1->play_sound();
			boss->swords[2]->type = GO_TYPE::GO_ENEMYATTACKCOLLIDER;
			boss->attack_timer = 2.0f;
			indicator_1->flag = false;
			++boss->attack_phase;
		}
	}

	if (boss->attack_phase == 8 && boss->attack_timer <= 0.5f) ++boss->attack_phase;

	if (boss->attack_phase == 9)
	{
		boss->is_weak = true;
		boss->weak_timer = 1.5f;
		boss->swords[0]->type = GO_TYPE::GO_PROJECTILE;
		boss->swords[1]->type = GO_TYPE::GO_PROJECTILE;
		boss->swords[2]->type = GO_TYPE::GO_PROJECTILE;

		if (boss->attack_timer <= 1.5f)
		{
			collider->flag = false;
		}
	}
}

/*!*****************************************************************************
  \brief
	The stabbing attack for Boss 3. After a short duration, it charges quickly
	in the direction of the player. If it hits the player, it stabs the player
	one more time. If enraged, the 3 swords will then appear at the left, top
	and right of the screen and shoot towards the player. If the player touches
	the swords, he will get damaged. If the initial charge does not hit the
	player and the boss hits the wall instead, the boss will not perform the
	additional stab. However, if the boss is enraged, all 3 swords will target
	the player and shoot towards the player at the same time.

  \param gameObj
	A reference to the boss game object.
*******************************************************************************/
void boss_sword_stab(gameObject& gameObj)
{
	AEVec2 camPos;
	AEGfxGetCamPosition(&camPos.x, &camPos.y);

	static f32 translate_x, translate_y;
	static particleSystem* pIndicatorParticle{ nullptr };

	Boss3* boss = dynamic_cast<Boss3*>(&gameObj);
	if (!boss)
		return;

	Attack_Collider* collider = get_attack_collider(boss);

	if (!boss->setup)
	{
		// PUT ATTACK SETUPS HERE ( E.G. COLLIDER SPAWNS WHERE, ANY COUNTDOWNS ETC. )
		collider = dynamic_cast<Attack_Collider*>(push_object(GO_TYPE::GO_BOSSATTACKCOLLIDER, boss));
		collider->flag = true;
		collider->curr_pos.x = boss->is_facing_right ? boss->curr_pos.x + 30 : boss->curr_pos.x - 30;
		collider->curr_pos.y = boss->curr_pos.y + 80;
		collider->damage = Sword_Stab.damage;
		collider->flinch_scale = { 200,300 };;
		collider->scale = { 200, 100 };
		collider->obj_sprite->scale = collider->scale;
		collider->obj_sprite->texture = pTex_crate;
		collider->obj_sprite->flip_x = boss->is_facing_right ? false : true;
		collider->on_collide = stab_collide;
		collider->flag = false;
		translate_x = 0.0f, translate_y = -80.0f;
		pIndicatorParticle = nullptr;
		boss->animations[STAB].reset();
		boss->animations[SUMMON_SWORD].reset();
		boss->attack_phase = 1;
		boss->attack_timer = 3.5f;
		boss->is_facing_right = Level::player->curr_pos.x > boss->curr_pos.x;
		boss->setup = true;
	}

	if (boss->attack_phase == 1)
	{
		set_sword_positions(*boss);
		if (!pIndicatorParticle)
		{
			pIndicatorParticle = create_particlesystem({ boss->curr_pos });
			pIndicatorParticle->loop = true;
			pIndicatorParticle->obj_sprite->flip_x = !boss->is_facing_right;
			pIndicatorParticle->scale = { 1, 1 };
			pIndicatorParticle->max_particles = 4;
			pIndicatorParticle->behaviour = nullptr;
			pIndicatorParticle->obj_sprite->texture = pTex_arrow;
			pIndicatorParticle->color_range[0] = { 1.0f, 1.0f, 1.0f, 0.5f };
			pIndicatorParticle->color_range[1] = { 1.0f, 1.0f, 1.0f, 0.5f };
			pIndicatorParticle->lifetime_range[0] = pIndicatorParticle->lifetime_range[1] = 1.0f;
			pIndicatorParticle->scale_range[0] = pIndicatorParticle->scale_range[1] = { 150.0f, boss->scale.y };
			pIndicatorParticle->speed_range[0] = pIndicatorParticle->speed_range[1] = 1000.0f;
			pIndicatorParticle->direction_range[0] = pIndicatorParticle->direction_range[1] = !boss->is_facing_right ? PI : 0;
			pIndicatorParticle->emission_rate = 0.25f;
		}

		if (!indicator_1->flag)
		{
			indicator_1->obj_sprite->tint = { 1.0f, 1.0f, 0.0f, 0.4f };
			indicator_1->curr_pos = { camPos.x, boss->curr_pos.y };
			indicator_1->obj_sprite->rot = 0;
			indicator_1->scale = { static_cast<f32>(AEGetWindowWidth()) * 2, collider->scale.y };
			indicator_1->flag = true;
			boss->animations[STAB].play_anim_frame(0);
		}

		indicator_1->obj_sprite->tint.g -= 1.0f / 120.0f;

		if (boss->attack_timer <= 1.5f)
		{
			if (!collider->flag)
				whoosh_sound->play_sound();
			collider->flag = true;
			translate_x = 20 * G_DELTATIME * 120.0f;
			collider->curr_pos.x = boss->is_facing_right ? boss->curr_pos.x + 30 : boss->curr_pos.x - 30;
			collider->curr_pos.y = boss->curr_pos.y;
			boss->animations[STAB].play_anim_frame(1);
			boss->curr_pos.x = boss->is_facing_right ? boss->curr_pos.x + translate_x : boss->curr_pos.x - translate_x;
		}
	}

	if (boss->attack_phase == 2)
	{
		boss->animations[STAB].play_anim_frame(0);
		if (boss->attack_timer <= 0.5f)
		{
			++boss->attack_phase;
		}

		else if (boss->attack_timer <= 0.8f)
		{
			collider->damage = 0;
		}

		else if (boss->attack_timer <= 1.0f)
		{
			collider->flag = true;
			boss->animations[STAB].play_anim_frame(1);
		}

		else
		{
			if (pIndicatorParticle)
			{
				pIndicatorParticle->delete_system();
				pIndicatorParticle = nullptr;
			}
			indicator_1->flag = false;
		}
	}

	if (boss->attack_phase == 3)
	{
		set_sword_positions(*boss);
		if (boss->attack_timer <= 0.5f)
		{
			collider->flag = false;
			boss->attack_timer = 2.0f;
			boss->is_facing_right = Level::player->curr_pos.x > boss->curr_pos.x;

			if (boss->enraged)
			{
				boss->animations[SUMMON_SWORD].play_anim_frame(0);
				AEVec2Zero(&boss->swords[0]->curr_vel);
				AEVec2Zero(&boss->swords[1]->curr_vel);
				AEVec2Zero(&boss->swords[2]->curr_vel);

				boss->swords[0]->obj_sprite->rot = PI;
				boss->swords[1]->obj_sprite->rot = 0;
				boss->swords[2]->obj_sprite->rot = PI / 2;
				++boss->attack_phase;
			}
			else boss->attack_phase = 8;
		}
	}

	if (boss->attack_phase == 4)
	{
		setup_sword(boss, 0);
		setup_sword(boss, 1);
		setup_sword(boss, 2);
		boss->swords[1]->obj_sprite->rot += 0.1f;
		boss->swords[2]->obj_sprite->rot += 0.1f;

		indicator_1->curr_pos = { camPos.x, boss->swords[0]->curr_pos.y };
		indicator_1->scale = { static_cast<f32>(AEGetWindowWidth()), boss->swords[0]->scale.y };
		indicator_1->flag = true;
		indicator_1->obj_sprite->tint = { 1.0f, 0.0f, 0.0f, 0.25f };

		if (boss->attack_timer <= 1.0f)
		{
			if (boss->swords[0]->type != GO_TYPE::GO_ENEMYATTACKCOLLIDER)
				slash_sound_1->play_sound();
			boss->animations[SUMMON_SWORD].play_anim_frame(1);
			boss->swords[0]->type = GO_TYPE::GO_ENEMYATTACKCOLLIDER;
			boss->swords[0]->curr_vel.x = SWORDS_SPEED;
			boss->attack_timer = 2.0f;
			indicator_1->flag = false;
			boss->swords[1]->obj_sprite->rot = 0;
			++boss->attack_phase;
		}

		else if (boss->attack_timer <= 1.5f)
		{
			collider->flag = false;
		}
	}

	if (boss->attack_phase == 5)
	{
		setup_sword(boss, 1);
		setup_sword(boss, 2);
		boss->swords[2]->obj_sprite->rot += 0.1f;

		indicator_1->curr_pos = { camPos.x, boss->swords[1]->curr_pos.y };
		indicator_1->scale = { static_cast<f32>(AEGetWindowWidth()), boss->swords[1]->scale.y };
		indicator_1->flag = true;
		indicator_1->obj_sprite->tint = { 1.0f, 0.0f, 0.0f, 0.25f };

		if (boss->attack_timer <= 1.0f)
		{
			if (boss->swords[1]->type != GO_TYPE::GO_ENEMYATTACKCOLLIDER)
				slash_sound_1->play_sound();
			boss->swords[1]->type = GO_TYPE::GO_ENEMYATTACKCOLLIDER;
			boss->swords[1]->curr_vel.x = -SWORDS_SPEED;
			boss->attack_timer = 2.0f;
			indicator_1->flag = false;
			boss->swords[2]->obj_sprite->rot = PI / 2;
			++boss->attack_phase;
		}
	}

	if (boss->attack_phase == 6)
	{
		setup_sword(boss, 2);

		indicator_1->curr_pos = { boss->swords[2]->curr_pos.x, camPos.y };
		indicator_1->scale = { boss->swords[2]->scale.y, static_cast<f32>(AEGetWindowHeight()) };
		indicator_1->flag = true;
		indicator_1->obj_sprite->tint = { 1.0f, 0.0f, 0.0f, 0.25f };

		if (boss->attack_timer <= 1.0f)
		{
			if (boss->swords[2]->type != GO_TYPE::GO_ENEMYATTACKCOLLIDER)
				slash_sound_1->play_sound();
			boss->swords[2]->type = GO_TYPE::GO_ENEMYATTACKCOLLIDER;
			boss->swords[2]->curr_vel.y = -SWORDS_SPEED;
			boss->attack_timer = 2.0f;
			indicator_1->flag = false;
			++boss->attack_phase;
		}
	}

	if (boss->attack_phase == 7 && boss->attack_timer <= 0.5f) ++boss->attack_phase;

	if (boss->attack_phase == 8)
	{
		if (pIndicatorParticle)
		{
			pIndicatorParticle->delete_system();
			pIndicatorParticle = nullptr;
		}

		boss->is_weak = true;
		boss->weak_timer = 1.5f;
		boss->swords[0]->type = GO_TYPE::GO_PROJECTILE;
		boss->swords[1]->type = GO_TYPE::GO_PROJECTILE;
		boss->swords[2]->type = GO_TYPE::GO_PROJECTILE;
	}

	else if (boss->attack_phase == 9)
	{
		if (pIndicatorParticle)
		{
			pIndicatorParticle->delete_system();
			pIndicatorParticle = nullptr;
		}

		AEVec2 toAngle;
		AEVec2Sub(&toAngle, &Level::player->curr_pos, &boss->swords[0]->curr_pos);
		indicator_1->obj_sprite->rot = AEVec2toRad(&toAngle);
		indicator_1->obj_sprite->tint = { 1.0f,0.0f, 0.0f, 0.25f };
		indicator_1->scale = { static_cast<f32>(AEGetWindowWidth()) * 2, boss->swords[0]->scale.y };
		indicator_1->curr_pos = boss->swords[0]->curr_pos;
		indicator_1->flag = true;

		AEVec2Sub(&toAngle, &Level::player->curr_pos, &boss->swords[1]->curr_pos);
		indicator_2->obj_sprite->rot = AEVec2toRad(&toAngle);
		indicator_2->obj_sprite->tint = { 1.0f,0.0f, 0.0f, 0.25f };
		indicator_2->scale = { static_cast<f32>(AEGetWindowWidth()) * 2, boss->swords[1]->scale.y };
		indicator_2->curr_pos = boss->swords[1]->curr_pos;
		indicator_2->flag = true;

		AEVec2Sub(&toAngle, &Level::player->curr_pos, &boss->swords[2]->curr_pos);
		indicator_3->obj_sprite->rot = AEVec2toRad(&toAngle);
		indicator_3->obj_sprite->tint = { 1.0f,0.0f, 0.0f, 0.25f };
		indicator_3->scale = { static_cast<f32>(AEGetWindowWidth()) * 2, boss->swords[2]->scale.y };
		indicator_3->curr_pos = boss->swords[2]->curr_pos;
		indicator_3->flag = true;

		if (boss->attack_timer <= 1.0f)
		{
			for (int i{ 0 }; i < NUM_SWORDS; ++i)
			{
				if (boss->swords[i]->type != GO_TYPE::GO_ENEMYATTACKCOLLIDER)
					slash_sound_2->play_sound();
				boss->swords[i]->obj_sprite->flip_x = false;
				boss->swords[i]->type = GO_TYPE::GO_ENEMYATTACKCOLLIDER;
				AEVec2Sub(&boss->swords[i]->curr_vel, &Level::player->curr_pos, &boss->swords[i]->curr_pos);
				AEVec2Normalize(&boss->swords[i]->curr_vel, &boss->swords[i]->curr_vel);
				AEVec2Scale(&boss->swords[i]->curr_vel, &boss->swords[i]->curr_vel, SWORDS_SPEED);
				boss->swords[i]->obj_sprite->rot = AEVec2toRad(&boss->swords[i]->curr_vel) + PI;
			}
			boss->attack_timer = 2.0f;
			++boss->attack_phase;
		}
	}

	else if (boss->attack_phase == 10)
	{
		if (boss->attack_timer <= 0.5f)
		{
			boss->attack_phase = 8;
		}

		else if (boss->attack_timer <= 1.0f)
		{
			indicator_1->flag = false;
			indicator_2->flag = false;
			indicator_3->flag = false;
		}
	}
}

/*!*****************************************************************************
  \brief
	The Big Sword attack of Boss 3. After a brief duration, a giang sword is
	summoned from the sky and lands on the ground, damaging the player if it
	hits the player. After landing, it rotates and slashes in the player's
	direction. If enraged, the 3 swords will then appear at the left, top and
	right of the screen and shoot towards the player. If the player touches the
	swords, he will get damaged.

  \param gameObj
	A reference to the boss game object.
*******************************************************************************/
void boss_sword_big_sword(gameObject& gameObj)
{
	AEVec2 camPos;
	AEGfxGetCamPosition(&camPos.x, &camPos.y);
	static f32 rotation;

	Boss3* boss = dynamic_cast<Boss3*> (&gameObj);
	if (!boss) return;

	Attack_Collider* collider = get_attack_collider(boss);

	if (!boss->setup)
	{
		// PUT ATTACK SETUPS HERE ( E.G. COLLIDER SPAWNS WHERE, ANY COUNTDOWNS ETC. )
		collider = dynamic_cast<Attack_Collider*>(push_object(GO_TYPE::GO_BOSSATTACKCOLLIDER, boss));
		collider->flag = true;
		collider->curr_pos.x = boss->curr_pos.x;
		collider->curr_pos.y = camPos.y + static_cast<f32>(AEGetWindowHeight()) / 2 + 225;
		collider->scale = { 450.0f, 150.0f };
		collider->damage = Sword_Big.damage;
		collider->flinch_scale = Sword_Big.flinch_scale;
		collider->obj_sprite->texture = pTex_sword;
		collider->obj_sprite->rot = PI / 2;
		collider->on_collide = big_sword_collide;
		indicator_1->flag = true;
		indicator_1->obj_sprite->texture = nullptr;
		indicator_1->curr_pos = { boss->curr_pos.x, boss->curr_pos.y - boss->scale.y / 2 };
		indicator_1->scale = { collider->scale.y, 20.0f };
		indicator_1->obj_sprite->tint = { 1.0f, 1.0f, 0.0f, 1.0f };
		boss->animations[SWORD_FALL].reset();
		boss->animations[SWORD_FALL].play_anim_frame(0);
		boss->attack_phase = 1;
		boss->attack_timer = 3.0f;
		boss->setup = true;
	}

	if (boss->attack_phase == 1)
	{
		indicator_1->obj_sprite->tint.g -= 1.0f / 150.0f;
		if (collider->on_ground)
		{
			boss->attack_timer = 1.0f;
			++boss->attack_phase;
			indicator_1->flag = false;
			ground_crash_sound->play_sound();
			boss->animations[SWORD_FALL].play_anim_frame(1);
		}

		else if (boss->attack_timer <= 1.0f)
		{
			collider->curr_vel.y = -1500.0f;
		}
	}

	if (boss->attack_phase == 2)
	{
		if (boss->attack_timer <= 0.5f)
		{
			boss->is_facing_right = Level::player->curr_pos.x > boss->curr_pos.x;
			rotation = boss->is_facing_right ? -3 * PI / 2 / 30 : 3 * PI / 2 / 30;
			indicator_1->curr_pos.x = collider->curr_pos.x + (boss->is_facing_right ? collider->scale.x / 2 : -collider->scale.x / 2);
			indicator_1->curr_pos.y += collider->scale.y;
			indicator_1->scale.x = collider->scale.x;
			indicator_1->scale.y = collider->scale.y * 2;
			indicator_1->obj_sprite->tint.g = 1.0f;
			indicator_1->obj_sprite->tint.a = 0.5f;
			indicator_1->flag = true;
			boss->attack_timer = 2.0f;
			++boss->attack_phase;
		}

		else if (boss->attack_timer <= 0.8f)
		{
			collider->damage = 0;
		}
	}

	if (boss->attack_phase == 3)
	{
		indicator_1->obj_sprite->tint.g -= 1.0f / 60.0f;
		if (boss->attack_timer <= 1.0f)
		{
			boss->attack_timer = 1.0f;
			++boss->attack_phase;
			whoosh_sound->play_sound();
		}
	}

	if (boss->attack_phase == 4)
	{
		collider->obj_sprite->rot += rotation;
		AEVec2 pivot;
		AEVec2FromAngle(&pivot, collider->obj_sprite->rot);
		AEVec2Scale(&pivot, &pivot, collider->scale.x / 2);
		AEVec2Sub(&pivot, &collider->curr_pos, &pivot);
		AEVec2RotateFromPivot(&collider->curr_pos, &collider->curr_pos, &pivot, -rotation);
		if (boss->attack_timer <= 0.5f)
		{
			slam_explosion->play_sound();
			collider->damage = Sword_Big.damage;
			AEVec2Zero(&collider->curr_vel);
			Camera::shake(10.0f, 0.5f);
			indicator_1->flag = false;
			boss->attack_timer = 2.0f;

			particleSystem* pParticleSystem = create_ground_particles(collider);
			pParticleSystem->color_range[0] = pParticleSystem->color_range[1] = { 0.68f, 0.85f, 0.9f, 0.8f };
			pParticleSystem->scale.x = collider->scale.x / 2;
			++boss->attack_phase;
		}

		else if (boss->attack_timer <= 0.85f)
		{
			collider->curr_vel.y = -1000.0f;
		}
	}

	if (boss->attack_phase == 5)
	{
		if (boss->attack_timer <= 1.0f)
		{
			collider->flag = false;

			if (boss->enraged)
			{
				boss->swords[0]->obj_sprite->rot = PI;
				boss->swords[1]->obj_sprite->rot = 0;
				boss->swords[2]->obj_sprite->rot = PI / 2;

				AEVec2Zero(&boss->swords[0]->curr_vel);
				AEVec2Zero(&boss->swords[1]->curr_vel);
				AEVec2Zero(&boss->swords[2]->curr_vel);
				boss->attack_timer = 2.0f;
				++boss->attack_phase;
			}
			else boss->attack_phase = 10;
		}

		else if (boss->attack_timer <= 1.8f)
		{
			collider->damage = 0;
		}
	}

	if (boss->attack_phase == 6)
	{
		setup_sword(boss, 0);
		setup_sword(boss, 1);
		setup_sword(boss, 2);

		indicator_1->curr_pos = { camPos.x, boss->swords[0]->curr_pos.y };
		indicator_1->scale = { static_cast<f32>(AEGetWindowWidth()), boss->swords[0]->scale.y };
		indicator_1->flag = true;
		indicator_1->obj_sprite->tint = { 1.0f, 0.0f, 0.0f, 0.25f };

		if (boss->attack_timer <= 1.0f)
		{
			if (boss->swords[0]->type != GO_TYPE::GO_ENEMYATTACKCOLLIDER)
				slash_sound_1->play_sound();
			boss->swords[0]->type = GO_TYPE::GO_ENEMYATTACKCOLLIDER;
			boss->swords[0]->curr_vel.x = SWORDS_SPEED;
			boss->attack_timer = 2.0f;
			indicator_1->flag = false;
			++boss->attack_phase;
		}
	}

	if (boss->attack_phase == 7)
	{
		setup_sword(boss, 1);
		setup_sword(boss, 2);

		indicator_1->curr_pos = { camPos.x, boss->swords[1]->curr_pos.y };
		indicator_1->scale = { static_cast<f32>(AEGetWindowWidth()), boss->swords[1]->scale.y };
		indicator_1->flag = true;
		indicator_1->obj_sprite->tint = { 1.0f, 0.0f, 0.0f, 0.25f };

		if (boss->attack_timer <= 1.0f)
		{
			if (boss->swords[1]->type != GO_TYPE::GO_ENEMYATTACKCOLLIDER)
				slash_sound_1->play_sound();
			boss->swords[1]->type = GO_TYPE::GO_ENEMYATTACKCOLLIDER;
			boss->swords[1]->curr_vel.x = -SWORDS_SPEED;
			boss->attack_timer = 2.0f;
			indicator_1->flag = false;
			++boss->attack_phase;
		}
	}

	if (boss->attack_phase == 8)
	{
		setup_sword(boss, 2);
		indicator_1->curr_pos = { boss->swords[2]->curr_pos.x, camPos.y };
		indicator_1->scale = { boss->swords[2]->scale.y, static_cast<f32>(AEGetWindowHeight()) };
		indicator_1->flag = true;
		indicator_1->obj_sprite->tint = { 1.0f, 0.0f, 0.0f, 0.25f };

		if (boss->attack_timer <= 1.0f)
		{
			if (boss->swords[2]->type != GO_TYPE::GO_ENEMYATTACKCOLLIDER)
				slash_sound_1->play_sound();
			boss->swords[2]->type = GO_TYPE::GO_ENEMYATTACKCOLLIDER;
			boss->swords[2]->curr_vel.y = -SWORDS_SPEED;
			boss->attack_timer = 2.0f;
			indicator_1->flag = false;
			++boss->attack_phase;
		}
	}

	if (boss->attack_phase == 9 && boss->attack_timer <= 0.5f) ++boss->attack_phase;

	if (boss->attack_phase == 10)
	{
		boss->is_weak = true;
		boss->weak_timer = 1.5f;
		boss->swords[0]->type = GO_TYPE::GO_PROJECTILE;
		boss->swords[1]->type = GO_TYPE::GO_PROJECTILE;
		boss->swords[2]->type = GO_TYPE::GO_PROJECTILE;
	}
}

/*!*****************************************************************************
  \brief
	The ultimate attack of Boss 3. Keeps shooting swords towards the player
	every second for 10 seconds. Will only be used if the boss is enraged.

  \param gameObj
	A reference to the boss game object.
*******************************************************************************/
void boss_ultimate(gameObject& gameObj)
{
	AEVec2 camPos;
	AEGfxGetCamPosition(&camPos.x, &camPos.y);
	f32 windowWidth = static_cast<f32>(AEGetWindowWidth());
	f32 windowHeight = static_cast<f32>(AEGetWindowHeight());

	static f32 timer;
	static u16 curr_sword;

	Boss3* boss = dynamic_cast<Boss3*> (&gameObj);
	if (!boss) return;

	if (!boss->setup)
	{
		boss->attack_phase = 1;
		boss->attack_timer = 10.5f;
		timer = 0;
		curr_sword = 0;
		setup_sword(boss, 0);
		boss->swords[0]->obj_sprite->rot = PI;
		setup_sword(boss, 1);
		boss->swords[1]->obj_sprite->rot = 0;
		setup_sword(boss, 2);
		boss->swords[2]->obj_sprite->rot = PI / 2;
		boss->setup = true;
		boss->animations[SUMMON_SWORD].reset();
		boss->animations[SUMMON_SWORD].play_anim_frame(1);
	}

	if (boss->attack_phase == 1)
	{
		timer += G_DELTATIME;

		AEVec2 toAngle;
		AEVec2Sub(&toAngle, &Level::player->curr_pos, &boss->swords[curr_sword]->curr_pos);
		indicator_1->obj_sprite->rot = AEVec2toRad(&toAngle);
		indicator_1->obj_sprite->tint = { 1.0f,0.0f, 0.0f, 0.25f };
		indicator_1->scale = { static_cast<f32>(AEGetWindowWidth()) * 2, boss->swords[curr_sword]->scale.y };
		indicator_1->curr_pos = boss->swords[curr_sword]->curr_pos;
		indicator_1->flag = true;

		for (int i{ 0 }; i < NUM_SWORDS; ++i)
		{
			if (boss->swords[i]->curr_pos.x > camPos.x + windowWidth / 2 + boss->swords[i]->scale.x / 2 ||
				boss->swords[i]->curr_pos.x < camPos.x - windowWidth / 2 - boss->swords[i]->scale.x / 2 ||
				boss->swords[i]->curr_pos.y > camPos.y + windowHeight / 2 + boss->swords[i]->scale.y / 2 ||
				boss->swords[i]->curr_pos.y < camPos.y - windowHeight / 2 - boss->swords[i]->scale.y / 2)
			{
				AEVec2Zero(&boss->swords[i]->curr_vel);
				boss->swords[i]->type = GO_TYPE::GO_PROJECTILE;
				f32 rand = AERandFloat();
				if (i == 0)
				{
					boss->swords[i]->curr_pos.x = camPos.x - windowWidth / 2 + 100.0f;
					boss->swords[i]->curr_pos.y =
						AEWrap(camPos.y + rand * windowHeight - windowHeight,
							camPos.y - windowHeight / 2 + 100.0f,
							camPos.y + windowHeight / 2 - 100.0f);
				}

				else if (i == 1)
				{
					boss->swords[i]->curr_pos.x = camPos.x + windowWidth / 2 - 100.0f;
					boss->swords[i]->curr_pos.y =
						AEWrap(camPos.y + rand * windowHeight - windowHeight,
							camPos.y - windowHeight / 2 + 100.0f,
							camPos.y + windowHeight / 2 - 100.0f);
				}

				if (i == 2)
				{
					boss->swords[i]->curr_pos.x =
						AEWrap(camPos.x + rand * windowWidth - windowWidth,
							camPos.x - windowWidth / 2 + 100.0f,
							camPos.x + windowWidth / 2 - 100.0f);
					boss->swords[i]->curr_pos.y = camPos.y + windowHeight / 2 - 100.0f;
				}
			}

			if (boss->swords[i]->type == GO_TYPE::GO_PROJECTILE)
			{
				AEVec2 toAngle;
				AEVec2Sub(&toAngle, &Level::player->curr_pos, &boss->swords[i]->curr_pos);
				boss->swords[i]->obj_sprite->rot = AEVec2toRad(&toAngle) + PI;
			}
		}

		if (boss->attack_timer <= 0.5f)
		{
			boss->attack_timer = 2.0f;
			++boss->attack_phase;
		}

		else if (timer >= 1.0f)
		{
			if (boss->swords[curr_sword]->type == GO_TYPE::GO_PROJECTILE)
			{
				slash_sound_1->play_sound();
				boss->swords[curr_sword]->type = GO_TYPE::GO_ENEMYATTACKCOLLIDER;
				AEVec2Sub(&boss->swords[curr_sword]->curr_vel, &Level::player->curr_pos, &boss->swords[curr_sword]->curr_pos);
				AEVec2Normalize(&boss->swords[curr_sword]->curr_vel, &boss->swords[curr_sword]->curr_vel);
				AEVec2Scale(&boss->swords[curr_sword]->curr_vel, &boss->swords[curr_sword]->curr_vel, SWORDS_SPEED);
				timer = 0.0f;
			}
			++curr_sword;
			if (curr_sword > 2) curr_sword = 0;
		}
	}

	if (boss->attack_phase == 2)
	{
		set_sword_positions(*boss);
		AEVec2Zero(&boss->swords[0]->curr_vel);
		AEVec2Zero(&boss->swords[1]->curr_vel);
		AEVec2Zero(&boss->swords[2]->curr_vel);
		indicator_1->flag = false;
		boss->is_weak = true;
		boss->weak_timer = 1.5f;
		boss->swords[0]->type = GO_TYPE::GO_PROJECTILE;
		boss->swords[1]->type = GO_TYPE::GO_PROJECTILE;
		boss->swords[2]->type = GO_TYPE::GO_PROJECTILE;
	}
}

/*!*****************************************************************************
  \brief
	The oncollide function for the big sword. When it collides with the platform
	it will create particles and shake the screen, and also stop moving.

  \param obj
	A reference to the big sword game object.
*******************************************************************************/
void big_sword_collide(gameObject& obj)
{
	particleSystem* pParticleSystem = nullptr;

	Attack_Collider* collider = dynamic_cast<Attack_Collider*>(&obj);
	vector<gameObject*>& colliders = obj.colliders;
	for (vector<gameObject*>::iterator i = colliders.begin(); i != colliders.end(); ++i)
	{
		gameObject& obj_col = **i;
		if (obj_col.type == GO_TYPE::GO_PLATFORM)
		{
			AEVec2Zero(&collider->curr_vel);
			Camera::shake(5.0f, 0.5f);
			collider->on_collide = nullptr;
			collider->on_ground = true;

			if (!pParticleSystem)
			{
				pParticleSystem = create_ground_particles(collider);
				if (pParticleSystem)
				{
					pParticleSystem->color_range[0] = pParticleSystem->color_range[1] = { 0.68f, 0.85f, 0.9f, 0.8f };
					pParticleSystem->position_offset.y = -collider->scale.x / 2;
				}
			}
		}
	}
}

/*!*****************************************************************************
  \brief
	Sets the positions of the sword to its respective positions on the screen
	to be shot towards the player.

  \param boss
	A pointer to the boss 3 object.

  \param swordNo
	The index of the sword to set up
*******************************************************************************/
void setup_sword(Boss3* boss, u16 swordNo)
{
	AEVec2 camPos;
	AEGfxGetCamPosition(&camPos.x, &camPos.y);

	switch (swordNo)
	{
	case 0:
	{
		boss->swords[0]->curr_pos.x = camPos.x - static_cast<f32>(AEGetWindowWidth()) / 2 + 100.0f;
		boss->swords[0]->curr_pos.y = Level::player->curr_pos.y;
		boss->swords[0]->obj_sprite->flip_x = false;
		break;
	}

	case 1:
	{
		boss->swords[1]->curr_pos.x = camPos.x + static_cast<f32>(AEGetWindowWidth()) / 2 - 100.0f;
		boss->swords[1]->curr_pos.y = Level::player->curr_pos.y;
		boss->swords[1]->obj_sprite->flip_x = false;
		break;
	}

	case 2:

	{
		boss->swords[2]->curr_pos.x = Level::player->curr_pos.x;
		boss->swords[2]->curr_pos.y = camPos.y + static_cast<f32>(AEGetWindowHeight()) / 2 - 100.0f;
		boss->swords[2]->obj_sprite->flip_x = false;
	}
	}
}

/*!*****************************************************************************
  \brief
	The on collide function for the stab attack collider. If it collides with
	the player, setup the next phase of the boss's attack and increment the
	boss attack phase. If it collides with the wall, shake the screen, create
	particles and set the boss's attack phase according to whether it is enraged

  \param obj
	A reference to the stab attack collider.
*******************************************************************************/
void stab_collide(gameObject& obj)
{
	Attack_Collider* collider = dynamic_cast<Attack_Collider*>(&obj);
	vector<gameObject*>& colliders = obj.colliders;
	for (vector<gameObject*>::iterator i = colliders.begin(); i != colliders.end(); ++i)
	{
		gameObject& obj_col = **i;
		if (obj_col.type == GO_TYPE::GO_WALL)
		{
			Boss3* boss = dynamic_cast<Boss3*>(collider->owner);
			Camera::shake(10.0f, 0.5f);
			collider->on_collide = nullptr;
			collider->flag = false;
			if (boss->enraged)
			{
				boss->attack_timer = 2.0f;
				boss->attack_phase = 9;
			}
			else
			{
				indicator_1->flag = false;
				boss->attack_phase = 8;
			}

			wall_hit_sound->play_sound();
			particleSystem* pParticleSystem = create_falling_particles();
			pParticleSystem->color_range[0] = pParticleSystem->color_range[1] = { 0.68f, 0.85f, 0.9f, 0.8f };
		}

		else if (obj_col.type == GO_TYPE::GO_PLAYER)
		{
			Boss3* boss = dynamic_cast<Boss3*>(collider->owner);
			collider->curr_pos.x = boss->is_facing_right ? boss->curr_pos.x + 30 : boss->curr_pos.x - 30;
			collider->curr_pos.y = boss->curr_pos.y;
			collider->scale = { 300, 50 };
			collider->obj_sprite->scale = collider->scale;
			collider->obj_sprite->flip_x = boss->is_facing_right ? false : true;
			collider->flinch_scale = Sword_Stab.flinch_scale;
			collider->flag = false;
			collider->on_collide = nullptr;
			boss->attack_timer = 2.0f;
			++boss->attack_phase;
		}
	}
}