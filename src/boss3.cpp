/*!*****************************************************************************
\file   boss3.cpp
\author Primary: Mohammad Hanif Koh Teck Wee	 (99.04%)
		Secondary: Siti Nursafinah Binte Sumsuri (0.96%)
\par    DP email: mohammadhanifkoh.t\@digipen.edu
				  sitinursafinah.b\@digipen.edu
\date   Mar 27 2021

\brief
This file contains the implementation of functions used in Boss 3.

The functions include:
- Boss3::Boss3
- set_aabb
- set_attack
- game_object_load
- game_object_initialize
- game_object_draw
- game_object_free
- game_object_unload
- take_damage
- set_sword_positions
- state_machine_1
- state_machine_2

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************************/
#include "boss.h"
#include "main.h"
#include "attack.h"
#include "level_manager.h"
#include "fmod.hpp"

#define SLASH_ATTACK boss_attacks[0];
#define STAB_ATTACK boss_attacks[1];
#define BIG_SWORD_ATTACK boss_attacks[2];
#define ULTIMATE_ATTACK boss_attacks[3];

constexpr s32 BOSS_HEALTH = 300;
constexpr AEVec2 BOSS_SIZE = { 150.0f, 150.0f };

static Audio* sword_summon;
static Audio* hit_weak_sound;
static Audio* hit_strong_sound;
static Audio* weak_sound;

constexpr s32 BOSS3_SPR_ROW = 3;
constexpr s32 BOSS3_SPR_COL = 9;
const s8* boss3_spritesheet = "./Sprites/Boss/boss3.png";

static AEGfxTexture* pTexBoss = nullptr;
static AEGfxTexture* pTexSword = nullptr;
static AEGfxTexture* pTexShield = nullptr;
static AEGfxVertexList* pMeshBoss = nullptr;

static void state_machine_1(Boss& boss);
static void state_machine_2(Boss& boss);

/*!*****************************************************************************
  \brief
	Default constructor for Boss3 class.
*******************************************************************************/
Boss3::Boss3() :
	Boss{}, enraged{}, anim_state{}, swords{}
{
	full_health = BOSS_HEALTH;
	health = BOSS_HEALTH;
	scale = BOSS_SIZE;
	is_facing_right = true;
}

/*!*****************************************************************************
  \brief
	Sets a specific aabb for the boss.
*******************************************************************************/
void Boss3::set_aabb()
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
void Boss3::set_attack()
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
	Loads all assets and allocates memory required by Boss 3. Called once in
	level manager load function.
*******************************************************************************/
void Boss3::game_object_load()
{
	pTexBoss = AEGfxTextureLoad(boss3_spritesheet);
	pTexSword = AEGfxTextureLoad("./Sprites/weapon_sword.png");
	pTexShield = AEGfxTextureLoad("./Sprites/shield_aura.png");
	pMeshBoss = create_square_mesh(1.0f / BOSS3_SPR_COL, 1.0f / BOSS3_SPR_ROW, 0xFFFFFFFF);
	AE_ASSERT_MESG(pMeshBoss, "Failed to create mesh enemy!");
	obj_sprite = new Sprite(pTexBoss, pMeshBoss, scale);
	obj_sprite->set_size(read_png_size(boss3_spritesheet));
	sword_summon = new Audio("./Audio/mage_attack.wav", AUDIO_GROUP::SFX);
	weak_sound = new Audio("./Audio/zoom1.wav", AUDIO_GROUP::SFX);
	hit_weak_sound = new Audio("./Audio/boss_hurt_weak.wav", AUDIO_GROUP::SFX);
	hit_strong_sound = new Audio("./Audio/boss_hurt_strong.wav", AUDIO_GROUP::SFX);

	boss3_attacks_load();

	Animation boss_idle = Animation(1, { 0, 350.0f }, obj_sprite, 370);
	Animation boss_walk = Animation(6, { 370.0f, 350.0f }, obj_sprite, 370, 0.2f);
	Animation boss_slash_1 = Animation(4, { 0, 0 }, obj_sprite, 370, 0.08f);
	Animation boss_slash_2 = boss_slash_1;
	boss_slash_2.reverse_anims();
	Animation boss_stab = Animation(2, { 1480.0f, 0 }, obj_sprite, 370, 0.1f);
	Animation boss_leap = Animation(3, { 2220.0f, 0 }, obj_sprite, 370, 0.5f);
	Animation boss_sword_fall = Animation(2, { 0.0f, 700.0f }, obj_sprite, 370, 0.03f);
	Animation boss_vuln = Animation(3, { 740.0f, 700.0f }, obj_sprite, 370, 0.03f);
	Animation boss_summon_sword = Animation(2, { 370.0f, 0.0f }, obj_sprite, 370, 0.03f);
	animations.push_back(boss_idle);
	animations.push_back(boss_walk);
	animations.push_back(boss_slash_1);
	animations.push_back(boss_slash_2);
	animations.push_back(boss_stab);
	animations.push_back(boss_leap);
	animations.push_back(boss_sword_fall);
	animations.push_back(boss_vuln);
	animations.push_back(boss_summon_sword);
}

/*!*****************************************************************************
  \brief
	Initializes some data members of Boss 3, and pushes attacks into Boss 3
	vector. Also sets up the 3 swords used when Boss 3 is enraged
*******************************************************************************/
void Boss3::game_object_initialize()
{
	health = BOSS_HEALTH;
	curr_pos = init_pos;
	boss3_attacks_init(boss_attacks);

	for (int i{ 0 }; i < NUM_SWORDS; ++i)
	{
		swords[i] = dynamic_cast<Attack_Collider*>(push_object(GO_TYPE::GO_ENEMYATTACKCOLLIDER, nullptr));
		swords[i]->type = GO_TYPE::GO_PROJECTILE;
		swords[i]->obj_sprite->texture = pTexSword;
		swords[i]->scale.y = 50.0f;
		swords[i]->scale = { 120.0f, 40.0f };
		swords[i]->damage = 2;
		swords[i]->layer = 6;
		swords[i]->obj_sprite->rot = PI;
		swords[i]->flag = false;
	}
}

/*!*****************************************************************************
  \brief
	Update function for Boss1 object, overriding the default game object update
	function. Sets a new velocity of Boss 3 based on where the player is. Also
	updates all timers according to delta time if necessary
*******************************************************************************/
void Boss3::game_object_update()
{
	debuff.update();
	if (!enraged)
	{
		if ((health * 2) <= full_health)
		{
			sword_summon->play_sound();
			for (int i{ 0 }; i < NUM_SWORDS; ++i)
			{
				particleSystem* psys = create_damaged_particles(swords[i]);
				swords[i]->flag = true;
			}
			enraged = true;
		}
	}

	set_attack();
	attack();

	if (cooldown_timer > 0)
	{
		if (enraged)
		{
			set_sword_positions(*this);
		}

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
	Draw function for Boss3 object, overriding the default game object draw
	function. Draws the hp of the boss and the boss sprite itself.
*******************************************************************************/
void Boss3::game_object_draw()
{
	if (health > 0)
	{
		full_bar();
		f_health();
	}
	if (!curr_Attack && curr_vel.x)
		animations[1].play_animation();
	if (!curr_Attack && !curr_vel.x && !is_vul)
		animations[0].play_animation();
	if (is_vul)
		animations[7].play_animation(false);
	obj_sprite->flip_x = is_facing_right ? false : true;
	draw(obj_sprite, curr_pos, scale);
}

/*!*****************************************************************************
  \brief
	Free function for Boss3 object, overriding the default game object free
	function. removes all attack colliders belonging to the boss from the vector
	of game objects.
*******************************************************************************/
void Boss3::game_object_free()
{
	obj_sprite->mesh = nullptr;
	obj_sprite->texture = nullptr;
	delete obj_sprite;
	for (int i{ 0 }; i < NUM_SWORDS; ++i)
	{
		delete swords[i]->obj_sprite;
	}
}

/*!*****************************************************************************
  \brief
	Unloads all assets and dellocates memory required by Boss 3. Called once in
	level manager unload function.
*******************************************************************************/
void Boss3::game_object_unload()
{
	AEGfxTextureUnload(pTexBoss);
	AEGfxTextureUnload(pTexSword);
	AEGfxTextureUnload(pTexShield);
	AEGfxMeshFree(pMeshBoss);
	delete sword_summon;
	delete hit_weak_sound;
	delete hit_strong_sound;
	delete weak_sound;

	boss3_attacks_unload();
	boss_attacks.clear();
	boss_attacks.~vector();
}

/*!*****************************************************************************
  \brief
	Reduces the health of the boss based on the damage taken. If the boss is
	currently weak, set the boss to be vulnerable. Boss 3 overrides this
	function as it only takes damage when vulnerable. Creates a particle if
	player attempts to attack the boss when not vulnerable.

  \param damage
	the damage taken by the boss
*******************************************************************************/
void Boss3::take_damage(s32 damage)
{
	if (is_vul)
	{
		health -= damage;
	}

	else
	{
		particleSystem* pParticlesystem = create_particlesystem(this);
		pParticlesystem->obj_sprite->texture = pTexShield;
		pParticlesystem->max_particles = 1;
		pParticlesystem->scale_range[0] = pParticlesystem->scale_range[1] = { 100.0f, 150.0f };
		pParticlesystem->speed_range[0] = pParticlesystem->speed_range[1] = 0.0f;
		pParticlesystem->position_offset.x = Level::player->curr_pos.x > curr_pos.x ? 50.0f : -50.0f;
		AEVec2Add(&pParticlesystem->curr_pos, &pParticlesystem->position_offset, &pParticlesystem->curr_pos);
		pParticlesystem->set_aabb();
		pParticlesystem->lifetime = 0.05f;
		pParticlesystem->behaviour = damage_taken;
		pParticlesystem->obj_sprite->flip_x = Level::player->curr_pos.x < curr_pos.x;
	}

	if (health <= 0)
	{
		gAEGameStateNext = GS_WIN;
	}

	if (vul_timer)
	{
		Camera::shake(20.0f, 0.1f);
		hit_strong_sound->play_sound();
	}

	if (is_weak)
	{
		weak_sound->play_sound();
		Camera::look_at(&curr_pos, 0.5f);
		Camera::temp_zoom(1.5f, 0.5f);
		temp_scale_timer(0.3f, 0.5f);
		start_vul(3.0f);
		attack_timer = -1.0f;
		is_weak = false;
	}
}

/*!*****************************************************************************
  \brief
	Sets the position of each of Boss 3's swords to be behind the boss. Called
	every frame so the swords will follow the boss around.

  \param damage
	the damage taken by the boss
*******************************************************************************/
void set_sword_positions(Boss3& boss)
{
	for (int i{ 0 }; i < NUM_SWORDS; ++i)
	{
		AEVec2 offset = { 0, 0 };
		offset.x = boss.is_facing_right ? -150.0f : 150.0f;
		AEMtx33 mtxRot;
		f32 rotation = boss.is_facing_right ? -i * PI / 4 : i * PI / 4;
		AEMtx33Rot(&mtxRot, rotation);
		AEMtx33MultVec(&offset, &mtxRot, &offset);
		AEVec2Add(&boss.swords[i]->curr_pos, &boss.curr_pos, &offset);
		boss.swords[i]->obj_sprite->flip_x = !boss.is_facing_right;
		boss.swords[i]->obj_sprite->rot = rotation / 2 + PI;
	}
}

/*!*****************************************************************************
  \brief
	Selects which attack to use. Uses one attack if the player is far enough
	from the boss, another attack if the player is closer to the boss, and the
	third if the player is even closer.

  \param boss
	A reference to the boss 3 object.
*******************************************************************************/
void state_machine_1(Boss& boss)
{
	if (fabs(Level::player->curr_pos.x - boss.curr_pos.x) > 200.0f)
	{
		boss.curr_Attack = &boss.STAB_ATTACK;
	}

	else if (fabs(Level::player->curr_pos.x - boss.curr_pos.x) <= 100.0f)
	{
		boss.curr_Attack = &boss.BIG_SWORD_ATTACK;
	}

	else
	{
		boss.curr_Attack = &boss.SLASH_ATTACK;
	}
}

/*!*****************************************************************************
  \brief
	Selects which attack to use. Uses one attack if the player is far enough
	from the boss, another attack if the player is closer to the boss, and the
	third if the player is even closer. The fourth attack is used at a random
	chance, and is more likely to be used the lower health the boss has.

  \param boss
	A reference to the boss 3 object.
*******************************************************************************/
void state_machine_2(Boss& boss)
{
	f32 ultimate_chance = static_cast<f32>(boss.health) / static_cast<f32>(boss.full_health);
	f32 roll = AERandFloat();

	if (roll > ultimate_chance)
	{
		boss.curr_Attack = &boss.ULTIMATE_ATTACK;
	}

	else if (fabs(Level::player->curr_pos.x - boss.curr_pos.x) > 200.0f)
	{
		boss.curr_Attack = &boss.STAB_ATTACK;
	}

	else if (fabs(Level::player->curr_pos.x - boss.curr_pos.x) <= 100.0f)
	{
		boss.curr_Attack = &boss.BIG_SWORD_ATTACK;
	}

	else
	{
		boss.curr_Attack = &boss.SLASH_ATTACK;
	}
}