/*!*************************************************************************
****
\file   spearattacks.cpp
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Mar 13 2021

\brief
This header file contains the implementation of the spear attack functions.

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
#include "main.h"
#include "attack.h"
#include "audio.h"
#include "player.h"
#include "level_manager.h"

static constexpr f32 BLEED_DURATION = 5.0f;
static constexpr f32 BLEED_TICK = 1.0f;
static constexpr f32 BLEED_PROC_CHANCE = 0.3f;
static constexpr f32 CHARGED_SPEAR_TIME = 2.0f;
static constexpr f32 MAX_SPEAR_HOLD_TIME = 4.0f;
static float spear_hold_duration;

static AEGfxTexture *pTex_shockwave;
static AEGfxTexture *pTex_attackcol_stab;
static AEGfxTexture *pTex_spear_throw;
static AEGfxTexture *pTex_grey_circle;
static AEGfxTexture *pTex_slash;
static AEGfxTexture *pTex_slash_2;
static AEGfxTexture *pTex_bleed_proc;
static AEGfxTexture *pTex_dust;
static AEGfxVertexList *slash_mesh;
static AEGfxVertexList *slash_2_mesh;
static AEVec2 slash_size;
static AEVec2 slash_2_size;
static Audio *spear_charge_1;
static Audio *spear_charge_2;
static Audio *spear_charge_3;
static Audio *spear_sound;
static Audio *slash_sound_1;
static Audio *slash_sound_2;
static Audio *spear_attack_sound;
static Audio *spear_stab;
static Audio *spear_dodge_sound;
static Audio *strong_zoom_sound;
static Audio *strong_throw_sound;
static Audio *plunge_land_sound;

static Animation slash_anim_1 = Animation(6, {640, 0}, 0, -128, 0.063f);
static Animation slash_anim_2 = Animation(4, {0, 0}, 0, 150, 0.175f);

Player_Attack Spear_1_1(P_Anim_State::P_SPEAR_1_1, 0.5f, 0.2f, 4, 7, &spear_attack_1, P_Anim_State::P_SPEAR_1_2, P_Anim_State::P_SPEAR_1_5, {200.0f, 300.0f});
Player_Attack Spear_1_2(P_Anim_State::P_SPEAR_1_2, 0.5f, 0.2f, 4, 7, &spear_attack_1, P_Anim_State::P_SPEAR_1_3, P_Anim_State::P_SPEAR_1_5, {200.0f, 300.0f});
Player_Attack Spear_1_3(P_Anim_State::P_SPEAR_1_3, 0.5f, 0.2f, 4, 7, &spear_attack_1, P_Anim_State::P_SPEAR_1_4, P_Anim_State::P_SPEAR_1_5, {600.0f, 600.0f});
Player_Attack Spear_1_4(P_Anim_State::P_SPEAR_1_4, 0.7f, 0.5f, 4, 20, &spear_attack_2, P_Anim_State::P_NONE, P_Anim_State::P_NONE, {600.0f, 300.0f});
Player_Attack Spear_1_5(P_Anim_State::P_SPEAR_1_5, 0.5f, 0.25f, 4, 0, &spear_dodge, P_Anim_State::P_SPEAR_1_4, P_Anim_State::P_SPEAR_2_1, {0.0f, 0.0f});
Player_Attack Spear_2_1(P_Anim_State::P_SPEAR_2_1, 0.7f, 0.3f, 4, 10, &spear_attack_3, P_Anim_State::P_SPEAR_1_5, P_Anim_State::P_SPEAR_2_2, {300.0f, 300.0f});
Player_Attack Spear_2_2(P_Anim_State::P_SPEAR_2_2, 0.7f, 0.3f, 4, 12, &spear_attack_4, P_Anim_State::P_SPEAR_1_5, P_Anim_State::P_SPEAR_2_3, {400.0f, 300.0f});
Player_Attack Spear_2_3(P_Anim_State::P_SPEAR_2_3, 0.7f, 0.3f, 4, 15, &spear_attack_5, P_Anim_State::P_NONE, P_Anim_State::P_NONE, {450.0f, 1300.0f});

Player_Attack Spear_Plunge_Atk(P_Anim_State::P_SPEAR_PLUNGE, 0.7f, 0.7f, 3, 7, &spear_plunge, P_Anim_State::P_NONE, P_Anim_State::P_NONE, {500.0f, 300.0f});
Player_Attack Spear_Jump_Atk(P_Anim_State::P_SPEAR_JUMP_ATK, 0.2f, 0.2f, 3, 3, &spear_jump_attack, P_Anim_State::P_NONE, P_Anim_State::P_NONE, {100.0f, 300.0f});
Player_Attack Spear_Throw_Setup(P_Anim_State::P_SPEAR_3_1, 1.0f, 0.7f, 0, 10, &spear_throw, P_Anim_State::P_NONE, P_Anim_State::P_NONE, {200.0f, 300.0f});

static void spear_collide(gameObject &gameObj);
static void spear_attack_collide(gameObject &gameObj);
static void proc_bleed(Enemy &enemy);
static void proc_bleed(Boss &boss);
static void bleed_particle(gameObject *obj);
void spear_charge_particle(gameObject &obj);
void spear_throw_particle(gameObject &obj);

/******************************************************************************
 * @brief 
 * 	Load the necessary textures, audio, and mesh required for the spear 
 * 	attacks.
 * 
******************************************************************************/
void spear_load()
{
	pTex_shockwave = AEGfxTextureLoad("./Sprites/Player/shockwave.png");
	AE_ASSERT_MESG(pTex_shockwave, "Failed to create Attack Collider Texture!");
	pTex_attackcol_stab = AEGfxTextureLoad("./Sprites/stab_effect.png");
	AE_ASSERT_MESG(pTex_attackcol_stab, "Failed to create Attack Collider Stab Texture!");
	pTex_spear_throw = AEGfxTextureLoad("./Sprites/weapon_spear.png");
	AE_ASSERT_MESG(pTex_spear_throw, "Failed to create Attack Collider Spear Texture!");
	pTex_grey_circle = AEGfxTextureLoad("./Sprites/grey_circle.png");
	AE_ASSERT_MESG(pTex_grey_circle, "Failed to create Attack Collider Spear Texture!");
	pTex_slash = AEGfxTextureLoad("./Sprites/sword_effect_spritesheet.png");
	AE_ASSERT_MESG(pTex_slash, "Failed to create Attack Collider Texture!");
	pTex_slash_2 = AEGfxTextureLoad("./Sprites/Effects/spear_slash.png");
	AE_ASSERT_MESG(pTex_slash, "Failed to create Spear Attack Collider 2 Texture!");
	pTex_bleed_proc = AEGfxTextureLoad("./Sprites/Status/bleed.png");
	AE_ASSERT_MESG(pTex_slash, "Failed to create Bleed Proc Texture!");
	pTex_dust = AEGfxTextureLoad("./Sprites/Particle/dust.png");
	AE_ASSERT_MESG(pTex_dust, "Failed to create Dust Texture!");

	AEVec2 slash_trans = {128.0f, 128.0f};
	AEVec2 slash_2_trans = {150.0f, 102.0f};
	slash_size = read_png_size("./Sprites/sword_effect_spritesheet.png");
	slash_mesh = create_square_mesh(slash_trans.x / slash_size.x, slash_trans.y / slash_size.y);
	slash_2_size = read_png_size("./Sprites/Effects/spear_slash.png");
	slash_2_mesh = create_square_mesh(slash_2_trans.x / slash_2_size.x, slash_2_trans.y / slash_2_size.y);

	spear_sound = new Audio("./Audio/spear_throw.wav", AUDIO_GROUP::SFX);
	spear_charge_1 = new Audio("./Audio/spear_charge_1.wav", AUDIO_GROUP::SFX);
	spear_charge_2 = new Audio("./Audio/spear_charge_2.wav", AUDIO_GROUP::SFX);
	spear_charge_3 = new Audio("./Audio/spear_charge_3.wav", AUDIO_GROUP::SFX);
	slash_sound_1 = new Audio("./Audio/slash_1.wav", AUDIO_GROUP::SFX);
	slash_sound_2 = new Audio("./Audio/slash_3.wav", AUDIO_GROUP::SFX);
	spear_attack_sound = new Audio("./Audio/hit_light.wav", AUDIO_GROUP::SFX);
	spear_stab = new Audio("./Audio/spear_hit.wav", AUDIO_GROUP::SFX);
	spear_dodge_sound = new Audio("./Audio/airdash.wav", AUDIO_GROUP::SFX);
	strong_zoom_sound = new Audio("./Audio/strong_spear_zoom.wav", AUDIO_GROUP::SFX);
	strong_throw_sound = new Audio("./Audio/strong_spear_throw.wav", AUDIO_GROUP::SFX);
	plunge_land_sound = new Audio("./Audio/plunge_land.wav", AUDIO_GROUP::SFX);
}

/******************************************************************************
 * @brief 
 * 	Initialize the spear attack functions by pushing back the attacks to 
 * 	the player attack vector
 * 
 * @param attacks 
 * 	Reference to the vector of Player_Attacks
******************************************************************************/
void spear_init(vector<Player_Attack> &attacks)
{
	attacks.push_back(Spear_1_1);
	attacks.push_back(Spear_1_2);
	attacks.push_back(Spear_1_3);
	attacks.push_back(Spear_1_4);
	attacks.push_back(Spear_1_5);
	attacks.push_back(Spear_2_1);
	attacks.push_back(Spear_2_2);
	attacks.push_back(Spear_2_3);

	attacks.push_back(Spear_Jump_Atk);
	attacks.push_back(Spear_Plunge_Atk);
	attacks.push_back(Spear_Throw_Setup);
}

/******************************************************************************
 * @brief Unload assets loaded from the load function
 * 
******************************************************************************/
void spear_unload()
{
	AEGfxTextureUnload(pTex_shockwave);
	AEGfxTextureUnload(pTex_attackcol_stab);
	AEGfxTextureUnload(pTex_spear_throw);
	AEGfxTextureUnload(pTex_grey_circle);
	AEGfxTextureUnload(pTex_slash);
	AEGfxTextureUnload(pTex_slash_2);
	AEGfxTextureUnload(pTex_bleed_proc);
	AEGfxTextureUnload(pTex_dust);
	AEGfxMeshFree(slash_mesh);
	AEGfxMeshFree(slash_2_mesh);
	delete spear_sound;
	delete spear_charge_1;
	delete spear_charge_2;
	delete spear_charge_3;
	delete slash_sound_1;
	delete slash_sound_2;
	delete spear_attack_sound;
	delete spear_stab;
	delete spear_dodge_sound;
	delete strong_zoom_sound;
	delete strong_throw_sound;
	delete plunge_land_sound;
}

/******************************************************************************
 * \brief Spawns a particle representing trigerring of bleed effect
 * 
 * 
 * \param obj 
 * 	The game object pointer representing the enemy or boss that has been 
 * inflicted with the bleed status
******************************************************************************/
static void bleed_particle(gameObject *obj)
{
	particleSystem *pParticleSystem = create_particlesystem(obj);
	pParticleSystem->lifetime = .3f;
	pParticleSystem->position_offset.y += 75.0f;
	pParticleSystem->scale = {1, 1};
	pParticleSystem->max_particles = 1;
	pParticleSystem->behaviour = change_wep_particle;
	pParticleSystem->obj_sprite->texture = pTex_bleed_proc;
	pParticleSystem->color_range[0] = {1.0f, 1.0f, 1.0f, 1.0f};
	pParticleSystem->color_range[1] = {1.0f, 1.0f, 1.0f, 1.0f};
	pParticleSystem->lifetime_range[0] = pParticleSystem->lifetime_range[1] = 5.0f;
	pParticleSystem->scale_range[0] = {30.0f, 30.0f};
	pParticleSystem->scale_range[1] = {30.0f, 30.0f};
	pParticleSystem->speed_range[0] = 0;
	pParticleSystem->speed_range[1] = 0;
	pParticleSystem->direction_range[0] = 0;
	pParticleSystem->direction_range[1] = 0;
	pParticleSystem->emission_rate = .03f;
	pParticleSystem->particle_increment = 1;
}

/******************************************************************************
 * \brief This functions activates the bleed effect on to the enemy, spawning 
 * a bleed effect particle.
 * 
 * \param enemy 
 * The Enemy object reference representing the enemy to activate the bleed 
 * effect.
******************************************************************************/
static void proc_bleed(Enemy &enemy)
{
	bleed_particle(&enemy);
	enemy.debuff.effect = Status_Effect::bleed;
	enemy.debuff.activate(BLEED_DURATION);
	enemy.debuff.tick = BLEED_TICK;
}

/******************************************************************************
 * \brief This functions activates the bleed effect on to the enemy, spawning 
 * a bleed effect particle.
 * 
 * \param boss
 * The Boss object reference representing the boss to activate the bleed effect 
******************************************************************************/
static void proc_bleed(Boss &boss)
{
	bleed_particle(&boss);
	boss.debuff.effect = Status_Effect::bleed;
	boss.debuff.activate(BLEED_DURATION);
	boss.debuff.tick = BLEED_TICK;
}

/****************************************************************************** 
 * \brief Collision function of the spear
 * Iterates through all the colliders of the spear collider. On collide with 
 * an enemy and boss, pushback the enemy pointer to the spear collider's 
 * ignorecolliders and play the spear_stab sound.
 * If the player's spear upgrade is >= 2, there will be a chance to activate 
 * the bleed to the enemy or boss. If the spear_hold_duration is >= 
 * CHARGED_SPEAR_TIME, bleed effect will activate regardless of chance.
 * 
 * 
 * \param gameObj 
******************************************************************************/
static void spear_attack_collide(gameObject &gameObj)
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
						if (player->spear_upgrade >= 2)
						{
							if (AERandFloat() <= BLEED_PROC_CHANCE || spear_hold_duration >= CHARGED_SPEAR_TIME)
								proc_bleed(*enemy);
						}
						spear_stab->play_sound();
					}
				}
			}
			else
			{
				Boss *boss = dynamic_cast<Boss *>(&obj_col);
				if (std::find(collider->ignorecolliders.begin(), collider->ignorecolliders.end(), boss) == collider->ignorecolliders.end())
				{
					collider->ignorecolliders.push_back(boss);
					if (player->spear_upgrade >= 2)
					{
						if (AERandFloat() <= BLEED_PROC_CHANCE || spear_hold_duration >= CHARGED_SPEAR_TIME)
							proc_bleed(*boss);
					}
					spear_stab->play_sound();
				}
			}
		}
	}
}

/****************************************************************************** 
 * \brief Similar spear collision codes but for thrown spears.
 * If the player's upgrade level is < 1, on collision, the spear collider flag 
 * will be set to false.
 * 
 * \param gameObj 
******************************************************************************/
static void spear_collide(gameObject &gameObj)
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
						if (player->spear_upgrade >= 2)
						{
							if (AERandFloat() <= BLEED_PROC_CHANCE || spear_hold_duration >= CHARGED_SPEAR_TIME)
								proc_bleed(*enemy);
						}
						spear_stab->play_sound();
						if (player->spear_upgrade < 1)
							collider->flag = false;
					}
				}
			}
			else
			{
				Boss *boss = dynamic_cast<Boss *>(&obj_col);
				if (std::find(collider->ignorecolliders.begin(), collider->ignorecolliders.end(), boss) == collider->ignorecolliders.end())
				{
					collider->ignorecolliders.push_back(boss);
					if (player->spear_upgrade >= 2)
					{
						if (AERandFloat() <= BLEED_PROC_CHANCE || spear_hold_duration >= CHARGED_SPEAR_TIME)
							proc_bleed(*boss);
					}
					spear_stab->play_sound();
					if (player->spear_upgrade < 1)
						collider->flag = false;
				}
			}
		}
	}
}

/****************************************************************************** 
 * \brief Perform a straight stab in front. The flinch scale will depend on the
 * player's current anim state. A spear attack sound will play
 * 
 * 
 * \param gameObj 
 * Reference to the game object which should be the sword collider
******************************************************************************/
void spear_attack_1(gameObject &gameObj)
{
	static f32 translate_x, translate_y;
	Player *player = dynamic_cast<Player *>(&gameObj);
	if (!player)
		return;

	Attack_Collider *collider = get_attack_collider(player);

	if (player->attack_setup)
	{
		collider->flag = true;
		spear_attack_sound->play_sound();
		collider->damage = Spear_1_1.damage;
		switch (player->curr_attack->state)
		{
		case (P_Anim_State::P_SPEAR_1_1):
			collider->flinch_scale = Spear_1_1.flinch_scale;
			break;
		case (P_Anim_State::P_SPEAR_1_2):
			collider->flinch_scale = Spear_1_2.flinch_scale;
			break;
		case (P_Anim_State::P_SPEAR_1_3):
			collider->flinch_scale = Spear_1_3.flinch_scale;
			break;
		}
		collider->scale = {200, 50};
		collider->obj_sprite->texture = pTex_attackcol_stab;
		collider->obj_sprite->scale = collider->scale;
		collider->obj_sprite->flip_x = player->is_facing_right ? false : true;
		collider->flag = false;
		collider->on_collide = spear_attack_collide;
		translate_x = 0.0f, translate_y = -80.0f;
		player->attack_setup = false;
	}

	if (player->attack_timer <= Spear_1_1.leeway_timer / 1.2f)
	{
		collider->curr_pos.x = player->is_facing_right ? player->curr_pos.x + 100.0f : player->curr_pos.x - 100.0f;
		collider->curr_pos.y = player->curr_pos.y - 20.0f;
		collider->flag = true;
		player->animations[player->curr_attack->state].play_anim_frame(2);
	}
	else
	{
		translate_x = 10 * G_DELTATIME * 30.0f;
		player->curr_pos.x = player->is_facing_right ? player->curr_pos.x + translate_x : player->curr_pos.x - translate_x;
		player->animations[player->curr_attack->state].play_animation(false);
	}
}

/****************************************************************************** 
 * \brief Wait for a while, dash forward, and perform a stab. The player's 
 * superarmour state will be set to true.
 * 
 * The player's animation will play depending on the attack timer.
 * 
 * \param gameObj 
 * Reference to the game object which should be the sword collider
******************************************************************************/
void spear_attack_2(gameObject &gameObj)
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
		collider->curr_pos.y = player->curr_pos.y + 70;
		collider->damage = Spear_1_4.damage;
		collider->flinch_scale = Spear_1_4.flinch_scale;
		collider->scale = {300, 50};
		collider->obj_sprite->scale = collider->scale;
		collider->obj_sprite->texture = pTex_attackcol_stab;
		collider->obj_sprite->flip_x = player->is_facing_right ? false : true;
		collider->flag = false;
		collider->on_collide = spear_attack_collide;
		translate_x = 0.0f, translate_y = -80.0f;
		player->superarmour = true;
		player->attack_setup = false;
	}

	if (player->attack_timer <= Spear_1_4.leeway_timer / 2.0f)
	{
		if (!collider->flag)
			spear_attack_sound->play_sound();
		collider->curr_pos.x = player->is_facing_right ? player->curr_pos.x + 30.0f : player->curr_pos.x - 30.0f;
		collider->curr_pos.y = player->curr_pos.y - 20.0f;
		collider->flag = true;
		player->animations[player->curr_attack->state].play_anim_frame(2);
	}
	else if (player->attack_timer <= Spear_1_4.leeway_timer / 1.3f)
	{
		translate_x = 10 * G_DELTATIME * 120.0f;
		player->curr_pos.x = player->is_facing_right ? player->curr_pos.x + translate_x : player->curr_pos.x - translate_x;
		player->animations[player->curr_attack->state].play_animation(false);
	}
	else
	{
		player->animations[player->curr_attack->state].play_anim_frame(3);
	}
}

/****************************************************************************** 
 * \brief Dash backwards, playing the spear_dodge_sound and spawning a 
 * particle system to represent fast moving backwards.
 * 
 * \param gameObj 
 * Reference to the game object, which should be the player
******************************************************************************/
void spear_dodge(gameObject &gameObj)
{
	static f32 translate_x;
	Player *player = dynamic_cast<Player *>(&gameObj);
	if (!player)
		return;

	Attack_Collider *collider = get_attack_collider(player);

	if (player->attack_setup)
	{
		collider->flag = false;
		translate_x = 0.0f;
		spear_dodge_sound->play_sound();
		particleSystem *pParticleSystem = create_dash_particles(player);
		pParticleSystem->direction_range[0] = pParticleSystem->direction_range[1] = player->is_facing_right ? 0 : PI;
		player->superarmour = true;
		player->attack_setup = false;
	}

	if (player->attack_timer > Spear_1_4.leeway_timer / 2.0f)
	{
		translate_x = 10 * G_DELTATIME * 120.0f;
		player->curr_pos.x = player->is_facing_right ? player->curr_pos.x - translate_x : player->curr_pos.x + translate_x;
		player->animations[P_Anim_State::P_SPEAR_1_4].play_anim_frame(3);
	}
}

/*!
 * \brief Move forward and slash with the spear
 * 
 * The collider for this attack will have a slashing animation.
 * 
 * The player's superarmour will be set to true to prevent interruption
 * 
 * \param gameObj 
 * 	Reference to the game object, which should be the player.
 */
void spear_attack_3(gameObject &gameObj)
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
		collider->curr_pos.y = player->curr_pos.y;
		collider->damage = Spear_2_1.damage;
		collider->flinch_scale = Spear_2_1.flinch_scale;
		collider->scale = {150, 150};
		collider->obj_sprite->texture = pTex_slash;
		collider->obj_sprite->mesh = slash_mesh;
		collider->obj_sprite->size = slash_size;
		collider->obj_sprite->flip_x = player->is_facing_right ? false : true;
		collider->flag = false;
		collider->on_collide = spear_attack_collide;
		slash_sound_2->play_sound();
		translate_x = 0.0f, translate_y = -80.0f;
		player->animations[player->curr_attack->state].reset();
		player->superarmour = true;
		slash_anim_1.set_sprite(collider->obj_sprite);
		slash_anim_1.reset();
		player->attack_setup = false;
	}

	collider->curr_pos.x = player->is_facing_right ? player->curr_pos.x + 75 : player->curr_pos.x - 75;
	collider->curr_pos.y = player->curr_pos.y;
	collider->flag = true;
	slash_anim_1.play_animation(false, 2.0f);

	if (player->attack_timer <= Spear_2_1.leeway_timer / 1.6f)
	{

		player->animations[player->curr_attack->state].play_anim_frame(4);
	}
	else
	{
		translate_x = 10 * G_DELTATIME * 30.0f;
		player->curr_pos.x = player->is_facing_right ? player->curr_pos.x + translate_x : player->curr_pos.x - translate_x;
		player->animations[player->curr_attack->state].play_animation(false);
	}
}

/*!*************************************************************************
****
 * \brief The player slashes backwards, kicking the enemies. The collider 
 * position will be relatively near the player, with a different texture and 
 * animation.
 * 
 * The slash sound will play out and the player will move forward slightly
 * 
 * \param gameObj 
****************************************************************************
***/
void spear_attack_4(gameObject &gameObj)
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
		collider->curr_pos.y = player->curr_pos.y;
		collider->damage = Spear_2_2.damage;
		collider->flinch_scale = Spear_2_2.flinch_scale;
		collider->scale = {150, 150};
		collider->obj_sprite->texture = pTex_slash_2;
		collider->obj_sprite->mesh = slash_2_mesh;
		collider->obj_sprite->size = slash_2_size;
		collider->obj_sprite->flip_x = player->is_facing_right ? false : true;
		collider->flag = true;
		collider->on_collide = spear_attack_collide;
		slash_sound_1->play_sound();
		translate_x = 0.0f, translate_y = -80.0f;
		player->superarmour = true;
		player->attack_setup = false;
		slash_anim_2.set_sprite(collider->obj_sprite);
		slash_anim_2.reset();
	}

	collider->curr_pos.x = player->is_facing_right ? player->curr_pos.x : player->curr_pos.x;
	collider->curr_pos.y = player->curr_pos.y;
	collider->flag = true;
	slash_anim_2.play_animation();

	if (player->attack_timer <= Spear_2_2.leeway_timer / 1.6f)
	{
		player->animations[player->curr_attack->state].play_anim_frame(2);
	}
	else
	{
		translate_x = 10 * G_DELTATIME * 30.0f;
		player->curr_pos.x = player->is_facing_right ? player->curr_pos.x + translate_x : player->curr_pos.x - translate_x;
		player->animations[player->curr_attack->state].play_animation();
	}
}

/*!*************************************************************************
 * \brief Perform a kick upwards, vaulting the player.
 * The player's superarmour will be set to true 
 * The flinch scale of the attack will decay based on how long the attack has
 * been activated.
 * 
 * \param gameObj 
 * Reference to the game object, which should be the player
****************************************************************************/
void spear_attack_5(gameObject &gameObj)
{
	static f32 translate_x, translate_y;
	static bool leaped;
	Player *player = dynamic_cast<Player *>(&gameObj);
	if (!player)
		return;

	Attack_Collider *collider = get_attack_collider(player);

	if (player->attack_setup)
	{
		leaped = false;
		collider->flag = false;
		collider->curr_pos.x = player->is_facing_right ? player->curr_pos.x + 30 : player->curr_pos.x - 30;
		collider->curr_pos.y = player->curr_pos.y;
		collider->damage = Spear_2_3.damage;
		collider->flinch_scale = Spear_2_3.flinch_scale;
		collider->scale = {150, 150};
		collider->obj_sprite->texture = nullptr;
		collider->obj_sprite->scale = collider->scale;
		collider->obj_sprite->flip_x = player->is_facing_right ? false : true;
		collider->obj_sprite->rot = player->is_facing_right ? PI / 6 : -PI / 6;
		collider->obj_sprite->transparency = 0.0f;
		collider->flag = false;
		collider->on_collide = spear_attack_collide;
		player->superarmour = true;
		translate_x = 0.0f, translate_y = -80.0f;
		player->attack_setup = false;
	}

	if (player->attack_timer <= Spear_2_3.leeway_timer - 0.10f)
	{
		if (!leaped)
		{
			player->on_ground = false;
			collider->flag = true;
			player->acceleration = {300.0f, 1100.0f};
			player->acceleration.x = player->is_facing_right ? 300.0f : -300.0f;
			leaped = true;
		}
		translate_x = 10 * G_DELTATIME * 30.0f;
		translate_y = 10 * G_DELTATIME * 30.0f;
		player->curr_pos.x = player->is_facing_right ? player->curr_pos.x + translate_x : player->curr_pos.x - translate_x;
		collider->curr_pos.x = player->is_facing_right ? player->curr_pos.x + 75.0f : player->curr_pos.x - 75.0f;
		collider->curr_pos.y = player->curr_pos.y;
		collider->flinch_scale.y -= 1300.0f * G_DELTATIME;
		collider->flinch_scale.x -= 600.0f * G_DELTATIME;
		AEClamp(collider->flinch_scale.x, 0, Spear_2_3.flinch_scale.x);
		AEClamp(collider->flinch_scale.y, 0, Spear_2_3.flinch_scale.y);
		player->animations[player->curr_attack->state].play_animation(false);
	}
}

/*!*************************************************************************
  \brief
	Perform a jump attack while on air.

	Slash the air while levitating for a very short while.

	Play the jump attack sound as well.

	The attack animation will play out to the player according to its
	relevant animation state.

  \param gameObj
	Reference to the game object, which should be the player
***************************************************************************/
void spear_jump_attack(gameObject &gameObj)
{
	Player *player = dynamic_cast<Player *>(&gameObj);
	if (!player)
		return;
	if (player->state != Spear_Jump_Atk.state)
		return;

	Attack_Collider *collider = get_attack_collider(player);

	if (player->attack_setup)
	{
		slash_sound_1->play_sound();
		collider->flag = true;
		collider->damage = Spear_Jump_Atk.damage;
		collider->flinch_scale = Spear_Jump_Atk.flinch_scale;
		collider->obj_sprite->texture = pTex_slash;
		collider->obj_sprite->mesh = slash_mesh;
		collider->obj_sprite->size = slash_size;
		collider->obj_sprite->flip_x = player->is_facing_right ? false : true;
		collider->scale = {200, 200};
		slash_anim_1.set_sprite(collider->obj_sprite);
		slash_anim_1.reset();
		player->attack_setup = false;
	}

	if (player->attack_timer)
	{
		collider->curr_pos.x = player->is_facing_right ? player->curr_pos.x : player->curr_pos.x;
		collider->curr_pos.y = player->curr_pos.y;
		player->curr_vel.y = 0;
		slash_anim_1.play_animation(false);
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
void spear_plunge(gameObject &gameObj)
{
	Player *player = dynamic_cast<Player *>(&gameObj);
	if (!player)
		return;
	if (player->state != Spear_Plunge_Atk.state)
		return;

	Attack_Collider *collider = get_attack_collider(player);
	static bool part_1;

	if (player->attack_setup)
	{
		collider->flag = false;
		part_1 = false;
		collider->damage = Spear_Plunge_Atk.damage;
		collider->flinch_scale = Spear_Plunge_Atk.flinch_scale;
		collider->obj_sprite->texture = pTex_shockwave;
		collider->obj_sprite->flip_x = player->is_facing_right ? false : true;
		collider->scale = {200, 200};
		player->attack_setup = false;
		slash_sound_1->play_sound();
	}

	if (player->attack_timer <= Spear_Plunge_Atk.leeway_timer - 0.15f && !player->on_ground)
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
		collider->curr_pos.x = player->is_facing_right ? player->curr_pos.x : player->curr_pos.x;
		collider->curr_pos.y = player->curr_pos.y;
		player->curr_vel.y = 0;
	}

	if (player->attack_timer && player->on_ground)
	{
		if (!collider->flag)
		{
			particleSystem *pParticleSystem = create_particlesystem(player);
			pParticleSystem->behaviour = slam_particle;
			pParticleSystem->obj_sprite->texture = pTex_dust;
			pParticleSystem->lifetime = 0.25f;
			pParticleSystem->emission_rate = 0.02f;
			pParticleSystem->max_particles = 25;
			pParticleSystem->color_range[0] = pParticleSystem->color_range[1] = {1.0f, 1.0f, 1.0f, 1.0f};
			pParticleSystem->lifetime_range[0] = pParticleSystem->lifetime_range[1] = 1.5f;
			pParticleSystem->scale_range[0] = {20.0f, 20.0f};
			pParticleSystem->scale_range[1] = {20.0f, 20.0f};
			pParticleSystem->speed_range[0] = pParticleSystem->speed_range[1] = 100;
			pParticleSystem->direction_range[0] = PI / 4;
			pParticleSystem->direction_range[1] = PI / 2 + PI / 4;
			pParticleSystem->particle_increment = 25;
			pParticleSystem->scale = {300.0f, 1.0f};
			pParticleSystem->set_aabb();
			pParticleSystem->position_offset.y -= 35.0f;
			collider->flag = true;
			plunge_land_sound->play_sound();
		}
		collider->curr_pos.x = player->is_facing_right ? player->curr_pos.x : player->curr_pos.x;
		collider->curr_pos.y = player->curr_pos.y - 35.0f;
		collider->scale = {350, 75};
	}

	player->animations[player->curr_attack->state].play_animation(false);
}

/*!*************************************************************************
 * \brief Performs a spear throw
 * The player's superarmour state will be true for the entire attack 
 * duration.
 * 
 * Holding the C key allows the player to charge the attack further. The 
 * charge level is represented by the particle system. On CHARGED_SPEAR_TIME,
 *  the particle will switch colour to a darker green and moves faster. On 
 * the max charge time, the colour will switch to purple. 
 * 
 * Once the key is let go, the collider will be set to true and the velocity
 *  will increment per frame. The damage of the spear throw will be 
 * multiplied by how long the player held the C key, clamped at 4s. 
 * 
 * If the player lets go of the C key after the max duration and if the 
 * spear upgrade is >=3, a special attack will play out where the camera 
 * zooms and focuses on the player. After a while, the collider will be set
 *  to true and while the velocity will not change, the collider's size will
 *  be extremely big to represent a throw so strong that it breaks the sound
 *  barrier.
 * 
 * The spear flying will be accompanied by spawning of a particle system.
 * 
 * \param gameObj 
 * Reference to the game object, which should be the player
***************************************************************************/
void spear_throw(gameObject &gameObj)
{
	Player *player = dynamic_cast<Player *>(&gameObj);
	static particleSystem *charge_particle;
	static bool spear_thrown;
	static bool super_power;
	static int charge_level;
	static bool attack_over;
	if (!player)
		return;
	if (player->state != Spear_Throw_Setup.state)
		return;

	Attack_Collider *collider = get_attack_collider(player);

	if (player->attack_setup)
	{
		spear_thrown = false;
		spear_hold_duration = 0.0f;
		super_power = false;
		attack_over = false;
		charge_level = 1;
		charge_particle = create_particlesystem(player);
		charge_particle->behaviour = spear_charge_particle;
		charge_particle->lifetime = 0.25f;
		charge_particle->obj_sprite->texture = pTex_grey_circle;
		charge_particle->emission_rate = 0.036f;
		charge_particle->color_range[0] = charge_particle->color_range[1] = {0.5f, 1.0f, 0.53f, 1.0f};
		charge_particle->lifetime_range[0] = charge_particle->lifetime_range[1] = 0.5f;
		charge_particle->scale_range[0] = charge_particle->scale_range[1] = {20.0f, 20.0f};
		charge_particle->speed_range[0] = 50;
		charge_particle->speed_range[1] = 100;
		charge_particle->direction_range[0] = charge_particle->direction_range[1] = player->is_facing_right ? 0 : PI;
		charge_particle->particle_increment = 3;
		charge_particle->scale = {100.0f, 100.0f};
		collider->flag = false;
		collider->curr_pos.x = player->is_facing_right ? player->curr_pos.x + 80 : player->curr_pos.x - 80;
		collider->curr_pos.y = player->curr_pos.y;
		collider->damage = Spear_Throw_Setup.damage;
		collider->flinch_scale = Spear_Throw_Setup.flinch_scale;
		collider->obj_sprite->texture = pTex_spear_throw;
		collider->scale = {200.0f, 75.0f};
		collider->is_static = false;
		collider->curr_vel.x = player->is_facing_right ? 25 * 60.0f : -25 * 60.0f;
		collider->obj_sprite->flip_x = player->is_facing_right ? false : true;
		collider->on_collide = spear_collide;
		player->superarmour = true;
		player->attack_setup = false;
		spear_charge_1->play_sound();
	}

	if (player->attack_timer)
	{
		if (!spear_thrown)
		{
			if (AEInputCheckReleased(AEVK_C))
			{
				spear_thrown = true;
				charge_particle->delete_system();
				collider->damage = static_cast<s32>(Spear_Throw_Setup.damage * spear_hold_duration);
				if (spear_hold_duration == MAX_SPEAR_HOLD_TIME && player->spear_upgrade >= 3)
				{
					Camera::temp_zoom(1.4f, 1.3f);
					Camera::look_at(&Level::player->curr_pos, 1.3f);
					temp_scale_timer(0.3f, 1.3f);
					strong_zoom_sound->play_sound();
					collider->scale = {static_cast<f32>(AEGetWindowWidth()) * 2, 100.0f};
					collider->obj_sprite->texture = pTex_attackcol_stab;
					collider->obj_sprite->tint = {1.0f, 0.0f, 1.0f, 1.0f};
					collider->flinch_scale = {500.0f, 300.0f};
					collider->curr_vel.x = 0;
					super_power = true;
					player->animations[player->curr_attack->state].play_anim_frame(0);
				}
			}
			else if (AEInputCheckCurr(AEVK_C))
			{
				spear_thrown = false;
				charge_particle->lifetime = 0.25f;
				spear_hold_duration = AEClamp(spear_hold_duration + G_DELTATIME, 0.5f, MAX_SPEAR_HOLD_TIME);
				if (spear_hold_duration >= CHARGED_SPEAR_TIME)
				{
					if (charge_level == 1)
					{
						spear_charge_2->play_sound();
						++charge_level;
					}
					charge_particle->scale_range[0] = charge_particle->scale_range[1] = {35.0f, 35.0f};
					if (spear_hold_duration == MAX_SPEAR_HOLD_TIME)
					{
						if (charge_level == 2)
						{
							spear_charge_3->play_sound();
							++charge_level;
						}
						charge_particle->color_range[0] = charge_particle->color_range[1] = {1.0f, 0.0f, 1.0f, 1.0f};
					}
					else
						charge_particle->color_range[0] = charge_particle->color_range[1] = {0.0f, 1.0f, 0.066f, 1.0f};
					charge_particle->scale = {150.0f, 150.0f};
					charge_particle->speed_range[0] = charge_particle->speed_range[1] = 300.0f;
				}
				player->attack_timer = Spear_Throw_Setup.leeway_timer;
				player->animations[player->curr_attack->state].play_anim_frame(0);
			}
		}
		else if (!super_power)
			player->animations[player->curr_attack->state].play_animation(false);

		if (super_power && player->attack_timer < Spear_Throw_Setup.leeway_timer - 0.4f)
		{
			if (!collider->flag)
			{
				particleSystem *pParticleSystem = create_particlesystem(collider);
				pParticleSystem->lifetime = 2.0f;
				pParticleSystem->behaviour = spear_throw_particle;
				pParticleSystem->obj_sprite->texture = pTex_grey_circle;
				pParticleSystem->color_range[0] = {1.0f, 1.0f, 1.0f, 1.0f};
				pParticleSystem->color_range[1] = {1.0f, 1.0f, 1.0f, 1.0f};
				pParticleSystem->lifetime_range[0] = pParticleSystem->lifetime_range[1] = 5.0f;
				pParticleSystem->scale_range[0] = {30.0f, 60.0f};
				pParticleSystem->scale_range[1] = {30.0f, 60.0f};
				pParticleSystem->speed_range[0] = 75.0f;
				pParticleSystem->speed_range[1] = 75.0f;
				pParticleSystem->direction_range[0] = 0;
				pParticleSystem->direction_range[1] = 0;
				pParticleSystem->emission_rate = 0.06f;
				pParticleSystem->scale = {static_cast<f32>(AEGetWindowWidth()) * 2, 0.0f};
				pParticleSystem->max_particles = 15;
				pParticleSystem->particle_increment = 15;
				pParticleSystem->set_aabb();
				strong_throw_sound->play_sound();
			}
			collider->flag = true;
			player->animations[player->curr_attack->state].play_anim_frame(3);
		}
		else if (!super_power && !attack_over && player->animations[player->curr_attack->state].at_last_frame())
		{
			if (!collider->flag)
			{
				particleSystem *pParticleSystem = create_particlesystem(collider);
				pParticleSystem->lifetime = 5.1f;
				pParticleSystem->behaviour = spear_throw_particle;
				pParticleSystem->obj_sprite->texture = pTex_grey_circle;
				pParticleSystem->color_range[0] = {1.0f, 1.0f, 1.0f, 1.0f};
				pParticleSystem->color_range[1] = {1.0f, 1.0f, 1.0f, 1.0f};
				pParticleSystem->lifetime_range[0] = pParticleSystem->lifetime_range[1] = 5.0f;
				pParticleSystem->scale_range[0] = {30.0f, 30.0f};
				pParticleSystem->scale_range[1] = {30.0f, 30.0f};
				pParticleSystem->speed_range[0] = 75.0f;
				pParticleSystem->speed_range[1] = 75.0f;
				pParticleSystem->direction_range[0] = 0;
				pParticleSystem->direction_range[1] = 0;
				pParticleSystem->emission_rate = 0.06f;
				pParticleSystem->particle_increment = 1;
				spear_sound->play_sound();
			}
			collider->flag = true;
			attack_over = true;
		}
	}
}