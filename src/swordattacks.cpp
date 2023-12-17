/*!*************************************************************************
****
\file   swordattacks.cpp
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Mar 13 2021

\brief
This header file contains the implementation of the sword attack functions.

The functions include:
- sword_load
- sword_init
- sword_unload
- sword_collide
- sword_attack_1
- sword_attack_2
- sword_attack_3
- sword_attack_4
- sword_attack_5
- sword_attack_6
- sword_plunge
- sword_jump_attack
- slam_particle

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/
#include "attack.h"
#include "shield.h"
#include "audio.h"
#include "player.h"
#include "camera.h"
#include "level_manager.h"

static constexpr f32 UPGRADED_MULTIPLIER = 2.0f;

static AEGfxTexture *pTex_shockwave;
static AEGfxTexture *pTex_slash;
static AEGfxTexture *pTex_stab;
static AEGfxTexture *pTex_dust;
static AEGfxVertexList *slash_mesh;

static AEVec2 slash_size;

static Audio *slash_sound_1;
static Audio *slash_sound_2;
static Audio *sword_hit_sound;
static Audio *ground_crash_sound;
static Audio *jump_atk_sword;
static Audio *shield_charge_sound;
static Audio *plunge_land_sound;

static Animation slash_anim_1 = Animation(6, {640, 0}, 0, -128, 0.063f);
static Animation slash_anim_2 = Animation(6, {640, 0}, 0, 128, 0.063f);
Player_Attack Sword_1_1(P_Anim_State::P_SWORD_1_1, 0.38f, 0.15f, 1, 4, &sword_attack_1, P_Anim_State::P_SWORD_1_2, P_Anim_State::P_NONE, {300.0f, 300.0f});
Player_Attack Sword_1_2(P_Anim_State::P_SWORD_1_2, 0.38f, 0.15f, 2, 6, &sword_attack_2, P_Anim_State::P_SWORD_1_3, P_Anim_State::P_NONE, {400.0f, 300.0f});
Player_Attack Sword_1_3(P_Anim_State::P_SWORD_1_3, 0.7f, 0.5f, 1, 15, &sword_attack_3, P_Anim_State::P_SWORD_1_4, P_Anim_State::P_NONE, {700.0f, 700.0f});
Player_Attack Sword_1_4(P_Anim_State::P_SWORD_1_4, 1.0, 0.75f, 3, 20, &sword_attack_4, P_Anim_State::P_NONE, P_Anim_State::P_NONE, {800.0f, 1000.0f});
Player_Attack Sword_2_1(P_Anim_State::P_SWORD_2_1, 0.75f, 0.45f, 3, 10, &sword_attack_5, P_Anim_State::P_SWORD_2_4, P_Anim_State::P_NONE, {700.0, 600.0f});
Player_Attack Sword_2_4(P_Anim_State::P_SWORD_2_4, 0.5f, 0.15f, 3, 5, &sword_attack_6, P_Anim_State::P_SWORD_2_5, P_Anim_State::P_NONE, {300.0, 300.0f});
Player_Attack Sword_2_5(P_Anim_State::P_SWORD_2_5, 0.5f, 0.15f, 3, 7, &sword_attack_6, P_Anim_State::P_SWORD_2_6, P_Anim_State::P_NONE, {300.0, 300.0f});
Player_Attack Sword_2_6(P_Anim_State::P_SWORD_2_6, 0.5f, 0.15f, 3, 8, &sword_attack_6, P_Anim_State::P_NONE, P_Anim_State::P_NONE, {300.0, 300.0f});
Player_Attack Sword_Plunge_Atk(P_Anim_State::P_SWORD_PLUNGE, 0.7f, 0.7f, 3, 10, &sword_plunge, P_Anim_State::P_NONE, P_Anim_State::P_NONE, {500.0f, 300.0f});
Player_Attack Sword_Jump_Atk(P_Anim_State::P_SWORD_JUMP_ATK, 0.3f, 0, 3, 5, &sword_jump_attack, P_Anim_State::P_NONE, P_Anim_State::P_NONE, {200.0f, 300.0f});

/******************************************************************************
 * @brief 
 * 	Load the necessary textures, audio, and mesh required for the sword 
 * 	attacks.
 * 
******************************************************************************/
void sword_load()
{
	pTex_shockwave = AEGfxTextureLoad("./Sprites/Player/shockwave.png");
	AE_ASSERT_MESG(pTex_shockwave, "Failed to create Attack Collider Texture!");
	pTex_slash = AEGfxTextureLoad("./Sprites/sword_effect_spritesheet.png");
	AE_ASSERT_MESG(pTex_slash, "Failed to create Attack Collider Texture!");
	pTex_stab = AEGfxTextureLoad("./Sprites/stab_effect.png");
	AE_ASSERT_MESG(pTex_stab, "Failed to create Sword Stab Texture!");
	pTex_dust = AEGfxTextureLoad("./Sprites/Particle/dust.png");
	AE_ASSERT_MESG(pTex_dust, "Failed to create Dust Texture!");
	AEVec2 trans = {128.0f, 128.0f};
	slash_size = read_png_size("./Sprites/sword_effect_spritesheet.png");
	slash_mesh = create_square_mesh(trans.x / slash_size.x, trans.y / slash_size.y);
	slash_sound_1 = new Audio("./Audio/slash_1.wav", AUDIO_GROUP::SFX);
	slash_sound_2 = new Audio("./Audio/slash_2.wav", AUDIO_GROUP::SFX);
	sword_hit_sound = new Audio("./Audio/sword_hit.wav", AUDIO_GROUP::SFX);
	ground_crash_sound = new Audio("./Audio/crash_1.wav", AUDIO_GROUP::SFX);
	jump_atk_sword = new Audio("./Audio/jump_atk_sword.wav", AUDIO_GROUP::SFX);
	shield_charge_sound = new Audio("./Audio/shield_bash.wav", AUDIO_GROUP::SFX);
	plunge_land_sound = new Audio("./Audio/plunge_land.wav", AUDIO_GROUP::SFX);
}

/******************************************************************************
 * @brief 
 * 	Initialize the sword attack functions by pushing back the attacks to 
 * 	the player attack vector
 * 
 * @param attacks 
 * 	Reference to the vector of Player_Attacks
******************************************************************************/
void sword_init(vector<Player_Attack> &attacks)
{
	attacks.push_back(Sword_1_1);
	attacks.push_back(Sword_1_2);
	attacks.push_back(Sword_1_3);
	attacks.push_back(Sword_1_4);
	attacks.push_back(Sword_2_1);
	attacks.push_back(Sword_2_4);
	attacks.push_back(Sword_2_5);
	attacks.push_back(Sword_2_6);
	attacks.push_back(Sword_Jump_Atk);
	attacks.push_back(Sword_Plunge_Atk);
}

/******************************************************************************
 * @brief Unload assets loaded from the load function
 * 
******************************************************************************/
void sword_unload()
{
	AEGfxTextureUnload(pTex_shockwave);
	AEGfxTextureUnload(pTex_slash);
	AEGfxTextureUnload(pTex_stab);
	AEGfxTextureUnload(pTex_dust);
	AEGfxMeshFree(slash_mesh);
	delete slash_sound_1;
	delete slash_sound_2;
	delete sword_hit_sound;
	delete ground_crash_sound;
	delete jump_atk_sword;
	delete shield_charge_sound;
	delete plunge_land_sound;
}

/******************************************************************************
 * @brief Collision function of the sword
 * Iterates through all the colliders of the sword collider. On collide with 
 * an enemy and boss, pushback the enemy pointer to the sword collider's 
 * ignorecolliders and play the sword_hit_sound. Perform additional check for 
 * the enemy as the initial collision is on the enemy's outer bound hence 
 * another collision check is required for collision between the inner bound 
 * and the sword collider.
 * This sword collider will destroy enemy attack colliders.
 * 
 * @param obj 
 * Reference to the game object which should be the sword collider
******************************************************************************/
void sword_collide(gameObject &gameObj)
{
	Attack_Collider *collider = dynamic_cast<Attack_Collider *>(&gameObj);
	Player *player = dynamic_cast<Player *>(collider->owner);

	vector<gameObject *> &colliders = collider->colliders;
	for (vector<gameObject *>::iterator i = colliders.begin(); i != colliders.end(); ++i)
	{
		gameObject &obj_col = **i;
		if (obj_col.type == GO_TYPE::GO_MONSTER || obj_col.type == GO_TYPE::GO_BOSS)
		{
			if (obj_col.type == GO_TYPE::GO_MONSTER)
			{
				Enemy *enemy = dynamic_cast<Enemy *>(&obj_col);
				if (rtr_collide(collider->bounding_box, collider->curr_vel, enemy->innerBound, enemy->curr_vel))
				{
					if (std::find(collider->ignorecolliders.begin(), collider->ignorecolliders.end(), enemy) == collider->ignorecolliders.end())
					{
						collider->ignorecolliders.push_back(enemy);
						sword_hit_sound->play_sound();
					}
				}
			}
			else
			{
				Boss *boss = dynamic_cast<Boss *>(&obj_col);
				if (std::find(collider->ignorecolliders.begin(), collider->ignorecolliders.end(), boss) == collider->ignorecolliders.end())
				{
					collider->ignorecolliders.push_back(boss);
					sword_hit_sound->play_sound();
				}
			}
		}
		if (obj_col.type == GO_TYPE::GO_ENEMYATTACKCOLLIDER)
		{
			Attack_Collider *collider = dynamic_cast<Attack_Collider *>(&obj_col);
			if (collider->owner)
			{
				obj_col.flag = false;
			}
		}
	}
}

/*!*************************************************************************
****
  \brief
	A straight slash in front of the player.

	The collider for this attack will have a slashing animation. Hence, it
	will reset at every attack_setup and have its sprite set to the current
	collider sprite pointer.

	The attack animation will play out to the player according to its
	relevant animation state.

  \param gameObj
	Reference to the game object, which should be the player
****************************************************************************
***/
void sword_attack_1(gameObject &gameObj)
{
	Player *player = dynamic_cast<Player *>(&gameObj);
	if (!player)
		return;

	Attack_Collider *collider = get_attack_collider(player);
	if (player->attack_setup)
	{
		collider->flag = true;
		collider->curr_pos.x = player->is_facing_right ? player->curr_pos.x + 30.0f : player->curr_pos.x - 30.0f;
		collider->curr_pos.y = player->curr_pos.y;
		collider->damage = Sword_1_1.damage;
		collider->flinch_scale = Sword_1_1.flinch_scale;
		collider->obj_sprite->texture = pTex_slash;
		collider->obj_sprite->mesh = slash_mesh;
		collider->obj_sprite->size = slash_size;
		collider->obj_sprite->flip_x = player->is_facing_right ? false : true;
		collider->scale = {200, 150};
		collider->on_collide = sword_collide;
		slash_anim_1.set_sprite(collider->obj_sprite);
		slash_anim_1.reset();
		slash_sound_1->play_sound();
		player->attack_setup = false;
	}

	if (player->attack_timer)
	{
		slash_anim_1.play_animation(false);
	}

	player->animations[player->curr_attack->state].play_animation();
}

/*!*************************************************************************
  \brief
	A straight slash in front of the player.

	It has the same attack logic as the previous attack, only with a 
	different animation.

	The attack animation will play out to the player according to its
	relevant animation state.

  \param gameObj
	Reference to the game object, which should be the player
***************************************************************************/
void sword_attack_2(gameObject &gameObj)
{
	Player *player = dynamic_cast<Player *>(&gameObj);
	if (!player)
		return;

	Attack_Collider *collider = get_attack_collider(player);

	if (player->attack_setup)
	{
		collider->flag = true;
		collider->curr_pos.x = player->is_facing_right ? player->curr_pos.x + 30 : player->curr_pos.x - 30;
		collider->curr_pos.y = player->curr_pos.y;
		collider->damage = Sword_1_2.damage;
		collider->flinch_scale = Sword_1_2.flinch_scale;
		collider->obj_sprite->texture = pTex_slash;
		collider->obj_sprite->mesh = slash_mesh;
		collider->obj_sprite->size = slash_size;
		collider->obj_sprite->flip_x = player->is_facing_right ? false : true;
		collider->scale = {200, 150};
		collider->on_collide = sword_collide;
		slash_anim_2.set_sprite(collider->obj_sprite);
		slash_anim_2.reset();
		slash_sound_2->play_sound();
		player->attack_setup = false;
	}

	if (player->attack_timer)
	{
		slash_anim_2.play_animation();
	}
	player->animations[player->curr_attack->state].play_animation();
}

/*!*************************************************************************
  \brief
	Pause for a while and perform a straight slash in front of the player.

	This attack makes the player invulnerable so that the attack is safe to 
	use.

	The attack animation will play out to the player according to its
	relevant animation state.

  \param gameObj
	Reference to the game object, which should be the player
***************************************************************************/
void sword_attack_3(gameObject &gameObj)
{
	static f32 translate_x, translate_y;
	Player *player = dynamic_cast<Player *>(&gameObj);
	if (!player)
		return;

	Attack_Collider *collider = get_attack_collider(player);

	if (player->attack_setup)
	{
		collider->flag = false;
		collider->curr_pos.x = player->is_facing_right ? player->curr_pos.x + 30 : player->curr_pos.x - 30;
		collider->curr_pos.y = player->curr_pos.y;
		collider->damage = Sword_1_3.damage;
		collider->flinch_scale = Sword_1_3.flinch_scale;
		collider->obj_sprite->texture = pTex_slash;
		collider->obj_sprite->mesh = slash_mesh;
		collider->obj_sprite->size = slash_size;
		collider->obj_sprite->flip_x = player->is_facing_right ? false : true;
		collider->scale = {300, 200};
		collider->on_collide = sword_collide;
		slash_anim_1.set_sprite(collider->obj_sprite);
		slash_anim_1.reset();
		player->start_invul();
		player->superarmour = true;
		player->attack_setup = false;
	}

	if (player->attack_timer <= Sword_1_3.leeway_timer / 1.6f)
	{
		if (!collider->flag)
			slash_sound_1->play_sound();
		collider->curr_pos.x = player->is_facing_right ? player->curr_pos.x + 30 : player->curr_pos.x - 30;
		collider->curr_pos.y = player->curr_pos.y;
		collider->flag = true;
		slash_anim_1.play_animation();
		player->animations[player->curr_attack->state].play_anim_frame(3);
	}
	else if (player->attack_timer <= Sword_1_3.leeway_timer / 1.3f)
	{
		translate_x = 10 * G_DELTATIME * 120.0f;
		player->curr_pos.x = player->is_facing_right ? player->curr_pos.x + translate_x : player->curr_pos.x - translate_x;
		player->animations[player->curr_attack->state].play_anim_frame(1);
	}
	else
	{
		player->animations[P_Anim_State::P_SWORD_1_4].play_anim_frame(0);
	}
}

/*!*************************************************************************
  \brief
	Pause for a while and then leap forward and perform a slash on the 
	floor.

	This attack sets the superarmour to true to prevent interruption of 
	attack.

	If the player's sword upgrade is 3 or more, the attack's hitbox will be
	 scaled by 2.

	The collider will be false at first. Once on ground, the collider's 
	flag will be set to true and a particle system representing slamming of 
	the ground will be created. Also, the camera will shake temporarily 
	and a ground crash sound will be played

	The attack animation will play out to the player according to its
	relevant animation state.

  \param gameObj
	Reference to the game object, which should be the player
***************************************************************************/
void sword_attack_4(gameObject &gameObj)
{
	Player *player = dynamic_cast<Player *>(&gameObj);
	if (!player)
		return;
	static bool part_1, landed;
	Attack_Collider *collider = get_attack_collider(player);

	if (player->attack_setup)
	{
		collider->flag = false;
		collider->curr_pos.x = player->is_facing_right ? player->curr_pos.x + 30 : player->curr_pos.x - 30;
		collider->curr_pos.y = player->curr_pos.y;
		collider->scale = {200, 50};
		collider->damage = Sword_1_4.damage;
		collider->flinch_scale = Sword_1_4.flinch_scale;
		collider->obj_sprite->texture = pTex_shockwave;
		collider->on_collide = sword_collide;
		player->attack_setup = false;
		player->superarmour = true;
		part_1 = true;
		landed = false;
		if (player->sword_upgrade >= 3)
		{
			collider->scale = {collider->scale.x * 2, collider->scale.y * 2};
		}
		collider->obj_sprite->scale = collider->scale;
	}

	if (player->attack_timer < Sword_1_4.leeway_timer - 0.2f && part_1)
	{
		part_1 = false;
		player->on_ground = false;
		player->acceleration.y = 1000;
	}
	else if (player->attack_timer && !player->on_ground)
	{
		player->acceleration.x = player->is_facing_right ? 500.0f : -500.0f;
		collider->curr_pos.x = player->curr_pos.x;
		collider->curr_pos.y = player->curr_pos.y - 50.0f;
		if (player->attack_timer < 0.4)
		{
			player->animations[player->curr_attack->state].play_anim_frame(1);
			player->attack_timer = 0.3f;
		}
		else
		{
			player->animations[player->curr_attack->state].play_anim_frame(2);
		}
	}
	else if (player->attack_timer && player->on_ground && !part_1)
	{
		collider->flag = true;
		if (!landed)
		{
			particleSystem *pParticleSystem = create_particlesystem(player);
			pParticleSystem->behaviour = slam_particle;
			pParticleSystem->obj_sprite->texture = pTex_dust;
			pParticleSystem->lifetime = 0.25f;
			pParticleSystem->emission_rate = 0.02f;
			pParticleSystem->max_particles = 25;
			pParticleSystem->color_range[0] = pParticleSystem->color_range[1] = {1.0f, 1.0f, 1.0f, 1.0f};
			pParticleSystem->lifetime_range[0] = pParticleSystem->lifetime_range[1] = 1.5f;
			pParticleSystem->scale_range[0] = {30.0f, 30.0f};
			pParticleSystem->scale_range[1] = {60.0f, 60.0f};
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
		}
		landed = true;
		player->animations[player->curr_attack->state].play_anim_frame(3);
		Camera::shake(10, 0.1f);
	}
	else
	{
		player->animations[player->curr_attack->state].play_anim_frame(0);
	}
}

/*!*************************************************************************
  \brief
	Pause for a while and then charge forward

	This attack sets the superarmour to true to prevent interruption of
	attack.

	After a period of time, the collider will be set to true and a shield 
	bash sound will be played.

	The attack animation will play out to the player according to its
	relevant animation state.

	The damage will be multiplied if the player's sword upgrade is >= 2

  \param gameObj
	Reference to the game object, which should be the player
***************************************************************************/
void sword_attack_5(gameObject &gameObj)
{
	static f32 translate_x, translate_y;
	Player *player = dynamic_cast<Player *>(&gameObj);
	if (!player)
		return;

	Attack_Collider *collider = get_attack_collider(player);

	if (player->attack_setup)
	{
		collider->flag = true;
		collider->curr_pos.x = player->is_facing_right ? player->curr_pos.x + 30 : player->curr_pos.x - 30;
		collider->curr_pos.y = player->curr_pos.y + 80;
		collider->damage = player->sword_upgrade >= 2 ? static_cast<s32>(Sword_2_1.damage * UPGRADED_MULTIPLIER) : Sword_2_1.damage;
		collider->flinch_scale = Sword_2_1.flinch_scale;
		collider->scale = {200.0f, 300.0f};
		collider->obj_sprite->scale = collider->scale;
		collider->obj_sprite->texture = nullptr;
		collider->obj_sprite->transparency = 0.0f;
		collider->obj_sprite->flip_x = player->is_facing_right ? false : true;
		collider->flag = false;
		collider->on_collide = sword_collide;
		translate_x = 0.0f, translate_y = -80.0f;
		player->start_invul();
		player->superarmour = true;
		player->attack_setup = false;
	}

	if (player->attack_timer <= Sword_2_1.leeway_timer / 2.0f)
	{
		if (!collider->flag)
		{
			shield_charge_sound->play_sound();
			particleSystem *particles = create_damaged_particles(collider);
			particles->scale_range[0] = {10.0f, 10.0f};
			particles->scale_range[1] = {30.0f, 30.0f};
			particles->emission_rate = 0.02f;
			particles->set_aabb();
			particles->position_offset.y -= 50.0f;
			particles->max_particles = 25;
			particles->particle_increment = 25;
			particles->color_range[0] = particles->color_range[1] = {0.0f, 0.88f, 1.0f, 1.0f};
		}

		collider->curr_pos.x = player->is_facing_right ? player->curr_pos.x + 30 : player->curr_pos.x - 30;
		collider->curr_pos.y = player->curr_pos.y + 75.0f;
		collider->flag = true;
		player->animations[player->curr_attack->state].play_anim_frame(2);
	}
	else if (player->attack_timer <= Sword_2_1.leeway_timer / 1.3f)
	{
		translate_x = 10 * G_DELTATIME * 120.0f;
		player->curr_pos.x = player->is_facing_right ? player->curr_pos.x + translate_x : player->curr_pos.x - translate_x;
		player->animations[player->curr_attack->state].play_anim_frame(1);
	}
	else
	{
		player->animations[P_Anim_State::P_SWORD_1_4].play_anim_frame(0);
	}
}

/*!*************************************************************************
  \brief
	Stab the front.

	Play the jump attack sound as the stabbing sound.
	The rotation of the attack collider will depend on the animation state
	 which should represent the player stabbing from low, medium to high 
	angle.

	The attack animation will play out to the player according to its
	relevant animation state.

  \param gameObj
	Reference to the game object, which should be the player
***************************************************************************/
void sword_attack_6(gameObject &gameObj)
{
	static f32 translate_x, translate_y;
	Player *player = dynamic_cast<Player *>(&gameObj);
	if (!player)
		return;

	Attack_Collider *collider = get_attack_collider(player);

	if (player->attack_setup)
	{
		collider->flag = true;
		collider->curr_pos.x = player->is_facing_right ? player->curr_pos.x + 75 : player->curr_pos.x - 75;
		collider->curr_pos.y = player->curr_pos.y;
		collider->damage = player->sword_upgrade >= 2 ? static_cast<s32>(Sword_2_4.damage * UPGRADED_MULTIPLIER) : Sword_2_4.damage;
		collider->flinch_scale = Sword_2_4.flinch_scale;
		collider->scale = {150, 50};
		collider->obj_sprite->texture = pTex_stab;
		collider->obj_sprite->scale = collider->scale;
		collider->obj_sprite->flip_x = player->is_facing_right ? false : true;
		collider->flag = false;
		collider->on_collide = sword_collide;
		jump_atk_sword->play_sound();
		translate_x = 0.0f, translate_y = -80.0f;
		player->attack_setup = false;
	}

	if (player->attack_timer <= Sword_2_4.leeway_timer / 1.2f)
	{
		collider->curr_pos.x = player->is_facing_right ? player->curr_pos.x + 75 : player->curr_pos.x - 75;
		collider->curr_pos.y = player->curr_pos.y;
		collider->flag = true;
		AEVec2 pivot = {player->is_facing_right ? collider->curr_pos.x - collider->scale.x / 2 : collider->curr_pos.x + collider->scale.x / 2,
						collider->curr_pos.y};
		if (!player->is_facing_right)
			pivot.x = collider->curr_pos.x + collider->scale.x / 2;
		switch (player->state)
		{
		case P_Anim_State::P_SWORD_2_4:
		{
			collider->obj_sprite->rot = player->is_facing_right ? -PI / 8 : PI / 8;
			AEVec2RotateFromPivot(&collider->curr_pos, &collider->curr_pos, &pivot, collider->obj_sprite->rot);
			break;
		}
		case P_Anim_State::P_SWORD_2_6:
		{
			collider->obj_sprite->rot = player->is_facing_right ? PI / 8 : -PI / 8;
			AEVec2RotateFromPivot(&collider->curr_pos, &collider->curr_pos, &pivot, collider->obj_sprite->rot);
			break;
		}
		default:
			collider->obj_sprite->rot = 0;
			break;
		}
		player->animations[P_Anim_State::P_SWORD_2_1].play_anim_frame(5);
	}
	else
	{
		translate_x = 10 * G_DELTATIME * 30.0f;
		player->curr_pos.x = player->is_facing_right ? player->curr_pos.x + translate_x : player->curr_pos.x - translate_x;
		player->animations[P_Anim_State::P_SWORD_2_1].play_anim_frame(4);
	}
}

/*!*************************************************************************
  \brief
	Perform a jump attack while on air.

	Stab the air for a short while, playing the jump attack

	Play the jump attack sound as the stabbing sound.

	The attack animation will play out to the player according to its
	relevant animation state.

  \param gameObj
	Reference to the game object, which should be the player
***************************************************************************/
void sword_jump_attack(gameObject &gameObj)
{
	Player *player = dynamic_cast<Player *>(&gameObj);
	if (!player)
		return;
	if (player->state != Sword_Jump_Atk.state)
		return;

	Attack_Collider *collider = get_attack_collider(player);

	if (player->attack_setup)
	{
		collider->flag = true;
		collider->curr_pos.x = player->is_facing_right ? player->curr_pos.x + 80 : player->curr_pos.x - 80;
		collider->curr_pos.y = player->curr_pos.y;
		collider->damage = Sword_Jump_Atk.damage;
		collider->flinch_scale = Sword_Jump_Atk.flinch_scale;
		collider->obj_sprite->texture = pTex_stab;
		collider->obj_sprite->flip_x = player->is_facing_right ? false : true;
		jump_atk_sword->play_sound();
		player->animations[player->curr_attack->state].reset();
		player->attack_setup = false;
	}

	if (player->attack_timer)
	{
		collider->curr_pos.x = player->is_facing_right ? player->curr_pos.x + 80 : player->curr_pos.x - 80;
		collider->curr_pos.y = player->curr_pos.y;
	}

	player->animations[player->curr_attack->state].play_animation(false);
}

/*!*************************************************************************
 * \brief
 * 	Perform a plunging attack.
 * 
 * 	The player will stay on air for a short while. Afterwards, its 
 * acceleration will be added to push the player down. Once on ground, the 
 * collider will be set to true and another animation will play. A particle 
 * system will be created resembling dust formed from slamming the floor
 * 
 * \param gameObj 
 * Reference to the game object, which should be the player
***************************************************************************/
void sword_plunge(gameObject &gameObj)
{
	Player *player = dynamic_cast<Player *>(&gameObj);
	if (!player)
		return;
	if (player->state != Sword_Plunge_Atk.state)
		return;

	Attack_Collider *collider = get_attack_collider(player);
	static bool part_1;

	if (player->attack_setup)
	{
		collider->flag = false;
		collider->scale = {200.0f, 75.0f};
		part_1 = false;
		collider->damage = Sword_Plunge_Atk.damage;
		collider->flinch_scale = Sword_Plunge_Atk.flinch_scale;
		collider->obj_sprite->texture = pTex_shockwave;
		collider->obj_sprite->flip_x = player->is_facing_right ? false : true;
		player->attack_setup = false;
		slash_sound_1->play_sound();
	}

	if (player->attack_timer <= Sword_Plunge_Atk.leeway_timer - 0.15f && !player->on_ground)
	{
		if (!part_1)
		{
			part_1 = true;
			player->acceleration.y -= 300.0f;
			player->superarmour = true;
		}
		player->attack_timer += G_DELTATIME;
	}
	else
	{
		collider->curr_pos.x = player->curr_pos.x;
		collider->curr_pos.y = player->curr_pos.y;
		player->curr_vel.y = 0;
	}

	if (player->attack_timer && player->on_ground)
	{
		if (!collider->flag)
		{
			plunge_land_sound->play_sound();
			particleSystem *pParticleSystem = create_particlesystem(player);
			pParticleSystem->behaviour = slam_particle;
			pParticleSystem->obj_sprite->texture = pTex_dust;
			pParticleSystem->lifetime = 0.25f;
			pParticleSystem->emission_rate = 0.02f;
			pParticleSystem->max_particles = 25;
			pParticleSystem->color_range[0] = pParticleSystem->color_range[1] = {1.0f, 1.0f, 1.0f, 1.0f};
			pParticleSystem->lifetime_range[0] = pParticleSystem->lifetime_range[1] = 1.5f;
			pParticleSystem->scale_range[0] = {30.0f, 30.0f};
			pParticleSystem->scale_range[1] = {60.0f, 60.0f};
			pParticleSystem->speed_range[0] = pParticleSystem->speed_range[1] = 100;
			pParticleSystem->direction_range[0] = PI / 4;
			pParticleSystem->direction_range[1] = PI / 2 + PI / 4;
			pParticleSystem->particle_increment = 25;
			pParticleSystem->scale = {150.0f, 1.0f};
			pParticleSystem->set_aabb();
			pParticleSystem->position_offset.y -= 35.0f;
			collider->flag = true;
		}
		collider->curr_pos.x = player->curr_pos.x;
		collider->curr_pos.y = player->curr_pos.y - 35.0f;
	}

	player->animations[player->curr_attack->state].play_animation(false);
}