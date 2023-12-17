/*!*************************************************************************
****
\file   shield.cpp
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Mar 20 2021

\brief
This header file contains the implementation of the shield function.

The functions include:
- shield_parry_response
- shield_load
- shield_init
- shield_unload
- shield_collide
- shield_up
- parry_attack

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/
#include "level_manager.h"
#include "main.h"
#include "attack.h"
#include "audio.h"
#include "shield.h"
#include "camera.h"
#include "collision.h"
#include <algorithm>

constexpr f32 UPGRADE_MULTIPLIER = 1.4f;
static AEGfxTexture* pTex_attackcol_stab;
static AEGfxTexture* pTex_shield_aura;
static AEGfxTexture* pTex_circle;
static AEGfxTexture* pTex_shockwave;
static AEGfxTexture* pTex_z_button;
static AEGfxVertexList* z_button_mesh;
static AEVec2 button_size;
static Audio* zoom_sound;
static Audio* shield_sound;
static Audio* parry_sound;
static Audio* parry_counter_sound;

static Animation z_anim = Animation(2, { 0,0 }, 0, 75, 0.03f);
Player_Attack Shield_1(P_Anim_State::P_SHIELD_1, 0.3f, 0.0f, 3, 0, &shield_up, P_Anim_State::P_NONE, P_Anim_State::P_NONE, { 0.0f, 0.0f });
Player_Attack Shield_2(P_Anim_State::P_SHIELD_2, 0.75f, 0.0f, 5, 20, &parry_attack, P_Anim_State::P_NONE, P_Anim_State::P_NONE, { 1000.0f, 500.0f });

/******************************************************************************
 * @brief Hit response on parry.
 * Creates 2 particle system. One shows the shield icon and another shows the 
 * z button prompt for the counter attack. Also plays the parry and zoom sound,
 *  slow down time to 0.3f scale, zoom by 1.3f and shake the camera temporarily
 * 
 * @param obj 
 * Reference to the game object which should be the shield collider
******************************************************************************/
void shield_parry_response(gameObject& obj)
{
	particleSystem* pParticleSystem = create_particlesystem(&obj);
	pParticleSystem->lifetime = .3f;
	pParticleSystem->scale = { 1, 1 };
	pParticleSystem->max_particles = 1;
	pParticleSystem->behaviour = parry_particle;
	pParticleSystem->obj_sprite->texture = pTex_circle;
	pParticleSystem->color_range[0] = pParticleSystem->color_range[1] = { 1.0f, 1.0f, 1.0f, 0.8f };
	pParticleSystem->lifetime_range[0] = pParticleSystem->lifetime_range[1] = 5.0f;
	pParticleSystem->scale_range[0] = pParticleSystem->scale_range[1] = { 30.0f, 30.0f };
	pParticleSystem->speed_range[0] = 0;
	pParticleSystem->speed_range[1] = 0;
	pParticleSystem->direction_range[1] = 0;
	pParticleSystem->emission_rate = .03f;

	particleSystem* button_prompt = create_particlesystem(&obj);
	button_prompt->lifetime = .3f;
	button_prompt->position_offset.y += 30.0f;
	button_prompt->scale = { 1, 1 };
	button_prompt->max_particles = 1;
	button_prompt->behaviour = damage_taken;
	button_prompt->obj_sprite->texture = pTex_z_button;
	button_prompt->obj_sprite->mesh = z_button_mesh;
	button_prompt->obj_sprite->size = button_size;
	z_anim.set_sprite(button_prompt->obj_sprite);
	button_prompt->color_range[0] = button_prompt->color_range[1] = { 1.0f, 1.0f, 1.0f, 0.8f };
	button_prompt->lifetime_range[0] = button_prompt->lifetime_range[1] = 5.0f;
	button_prompt->scale_range[0] = { 50.0f, 50.0f };
	button_prompt->scale_range[1] = { 50.0f, 50.0f };
	button_prompt->speed_range[0] = 0;
	button_prompt->speed_range[1] = 0;
	button_prompt->direction_range[1] = 0;
	button_prompt->emission_rate = .03f;
	button_prompt->set_aabb();
	zoom_sound->play_sound();
	temp_scale_timer(0.3f, 1.0f);
	Camera::temp_zoom(1.3f, 0.75f);
	parry_sound->play_sound(0.5f);
	Camera::shake(5.0f, 0.5f);
}

/******************************************************************************
 * @brief Load the necessary textures, audio, and mesh required for the shield 
 * functions.
 * 
******************************************************************************/
void shield_load()
{
	pTex_attackcol_stab = AEGfxTextureLoad("./Sprites/stab_effect.png");
	AE_ASSERT_MESG(pTex_attackcol_stab, "Failed to create Attack Collider Stab Texture!");
	pTex_shield_aura = AEGfxTextureLoad("./Sprites/shield_aura.png");
	AE_ASSERT_MESG(pTex_shield_aura, "Failed to create Shield Aura Texture!");
	pTex_circle = AEGfxTextureLoad("./Sprites/weapon_shield.png");
	AE_ASSERT_MESG(pTex_circle, "Failed to create Circle Grey Texture!");
	pTex_shockwave = AEGfxTextureLoad("./Sprites/white_donut2.png");
	AE_ASSERT_MESG(pTex_shockwave, "Failed to create Donut Texture!");
	pTex_z_button = AEGfxTextureLoad("./UI/z_button_sheet.png");
	AE_ASSERT_MESG(pTex_z_button, "Failed to create Z prompt Texture!");
	AEVec2 trans = { 75.0f, 75.0f };
	button_size = read_png_size("./UI/z_button_sheet.png");
	z_button_mesh = create_square_mesh(trans.x / button_size.x, trans.y / button_size.y);

	zoom_sound = new Audio("./Audio/zoom2.wav", AUDIO_GROUP::SFX);
	shield_sound = new Audio("./Audio/shieldup.wav", AUDIO_GROUP::SFX);
	parry_sound = new Audio("./Audio/parry.wav", AUDIO_GROUP::SFX);
	parry_counter_sound = new Audio("./Audio/parry_counter.wav", AUDIO_GROUP::SFX);
}

/******************************************************************************
 * @brief Initialize the shield functions by pushing back the attacks to 
 * the player attack vector
 * 
 * @param attacks 
 * Reference to the vector of Player_Attacks
******************************************************************************/
void shield_init(vector<Player_Attack>& attacks)
{
	attacks.push_back(Shield_1);
	attacks.push_back(Shield_2);
}

/******************************************************************************
 * @brief Unload assets loaded from the load function
 * 
******************************************************************************/
void shield_unload()
{
	AEGfxTextureUnload(pTex_attackcol_stab);
	AEGfxTextureUnload(pTex_shield_aura);
	AEGfxTextureUnload(pTex_circle);
	AEGfxTextureUnload(pTex_shockwave);
	AEGfxTextureUnload(pTex_z_button);
	AEGfxMeshFree(z_button_mesh);
	delete zoom_sound;
	delete shield_sound;
	delete parry_sound;
	delete parry_counter_sound;
}

/******************************************************************************
 * @brief Collision function of the shield
 * Iterates through all the colliders of the shield collider. On collide with 
 * an enemy attack collider, boss attack collider, and monster that is not a
 * RangedEnemy, check if the shield timer is less than PARRY_TIME. If it is, 
 * it indicates a parry, calling the shield_parry_response and focus the camera
 *  to the player. Destroys enemy attack collider.
 * 
 * @param obj 
 * Reference to the game object which should be the shield collider
******************************************************************************/
void shield_collide(gameObject& obj)
{
	Attack_Collider* collider = dynamic_cast<Attack_Collider*>(&obj);
	Player* player = dynamic_cast<Player*>(collider->owner);
	vector<gameObject*>& colliders = obj.colliders;
	for (vector<gameObject*>::iterator i = colliders.begin(); i != colliders.end(); ++i)
	{
		gameObject& obj_col = **i;
		if (obj_col.type == GO_TYPE::GO_ENEMYATTACKCOLLIDER || obj_col.type == GO_TYPE::GO_BOSSATTACKCOLLIDER)
		{
			if (player->shield_timer < PARRY_TIME)
			{
				if (!player->parry)
				{
					Camera::look_at(&player->curr_pos, 0.75f);
					shield_parry_response(obj);
				}
				player->parry = true;
			}
			else
				player->parry = false;
			if (obj_col.type == GO_TYPE::GO_ENEMYATTACKCOLLIDER)
			{
				Attack_Collider* collider = dynamic_cast<Attack_Collider*>(&obj_col);
				if (collider->owner)
				{
					obj_col.flag = false;
				}
			}
		}
		if (obj_col.type == GO_TYPE::GO_MONSTER)
		{
			Enemy* mob = dynamic_cast<Enemy*>(&obj_col);
			if (!dynamic_cast<RangedEnemy*>(mob))
			{
				if (rtr_collide(obj.bounding_box, obj.curr_vel, mob->innerBound, mob->curr_vel))
				{
					if (player->shield_timer < PARRY_TIME)
					{
						if (!player->parry)
						{
							Camera::look_at(&player->curr_pos, 0.75f);
							shield_parry_response(obj);
						}
						player->parry = true;
					}
					else
						player->parry = false;
				}
			}
		}
	}
}

/******************************************************************************
 * @brief Attack function representing shield up for the player. 
 * On attack set up, set the collider variables, reset animations, and play 
 * the shield up sound.
 * In general, this attack is defensive, allowing the player to shield against
 *  enemy attacks. While holding the attack button (C), the shield_timer of 
 * the player increments based on the global scaled deltatime. Of parry, 
 * the z key is unlocked to P_SHIELD_2 attack and plays a different animation.
 * It also makes the player invulnerable to attacks.
 * 
 * @param gameObj 
 * Reference to the game object which should be the player
******************************************************************************/
void shield_up(gameObject& gameObj)
{
	static f32 translate_x, translate_y;
	Player* player = dynamic_cast<Player*> (&gameObj);
	if (!player) return;

	Attack_Collider* collider = get_attack_collider(player);

	if (player->attack_setup)
	{
		collider->flag = true;
		collider->curr_pos.x = player->is_facing_right ? player->curr_pos.x + 30 : player->curr_pos.x - 30;
		collider->curr_pos.y = player->curr_pos.y;
		collider->damage = Shield_1.damage;
		collider->flinch_scale = Shield_1.flinch_scale;
		collider->scale = { 120, 100 };
		collider->obj_sprite->texture = pTex_shield_aura;
		collider->obj_sprite->scale = collider->scale;
		collider->obj_sprite->flip_x = player->is_facing_right ? false : true;
		collider->on_collide = shield_collide;
		z_anim.reset();
		shield_sound->play_sound();
		translate_x = 0.0f, translate_y = -80.0f;
		player->animations[player->curr_attack->state].reset();
		player->attack_setup = false;
	}

	if (player->attack_timer)
	{
		collider->curr_pos.x = player->is_facing_right ? player->curr_pos.x + 30 : player->curr_pos.x - 30;
		collider->curr_pos.y = player->curr_pos.y;
		player->shield_timer += G_DELTATIME;
		z_anim.play_animation();
		if (AEInputCheckCurr(AEVK_C))
		{
			player->attack_timer = Shield_1.leeway_timer;
		}
		if (Shield_1.leeway_timer / 1.3)
		{
			player->obj_sprite->tint = { 0.0, 0.5, 1.0f, 1.0f };
		}
		if (player->parry)
		{
			player->curr_attack->next_z = player->parry ? P_Anim_State::P_SHIELD_2 : P_Anim_State::P_NONE;
			player->start_invul();
			player->animations[P_Anim_State::P_SWORD_1_4].play_anim_frame(0);
		}
		else
			player->animations[player->curr_attack->state].play_animation(false);
	}
}

/******************************************************************************
 * @brief Attack function representing the parry attack for the player. 
 * On attack set up, set the collider variables, disable parry attack on the 
 * shield attack and turn the player invulnerable to damage.
 * After a certain time, the collider will be set to true and the 
 * parry_counter_sound will play and the particle system will spawn which 
 * represents a single shockwave.
 * 
 * @param gameObj 
******************************************************************************/
void parry_attack(gameObject& gameObj)
{
	Player* player = dynamic_cast<Player*> (&gameObj);
	if (!player) return;
	player->start_invul();
	Attack_Collider* collider = get_attack_collider(player);

	if (player->attack_setup)
	{
		collider->curr_pos.x = player->is_facing_right ? player->curr_pos.x + 30 : player->curr_pos.x - 30;
		collider->curr_pos.y = player->curr_pos.y;
		collider->damage = player->sword_upgrade >= 1 ? static_cast<s32>(Shield_2.damage * UPGRADE_MULTIPLIER) : Shield_2.damage;
		collider->flinch_scale = Shield_2.flinch_scale;
		collider->scale = { 300, 50 };
		collider->obj_sprite->texture = pTex_attackcol_stab;
		collider->obj_sprite->scale = collider->scale;
		collider->obj_sprite->flip_x = player->is_facing_right ? false : true;
		collider->flag = false;
		(player->curr_attack - 1)->next_z = P_Anim_State::P_NONE;
		player->start_invul();
		player->attack_setup = false;
	}

	if (player->attack_timer <= 0.2f)
	{
		player->animations[player->curr_attack->state].play_anim_frame(0);
		collider->flag = false;
	}
	else if (player->attack_timer <= Shield_2.leeway_timer / 1.3f)
	{
		collider->curr_pos.x = player->is_facing_right ? player->curr_pos.x + 100 : player->curr_pos.x - 100;
		collider->curr_pos.y = player->curr_pos.y;
		if (!collider->flag)
		{
			parry_counter_sound->play_sound();
			particleSystem* pParticleSystem = create_particlesystem(player);
			pParticleSystem->lifetime = .3f;
			pParticleSystem->scale = { 1, 1 };
			pParticleSystem->max_particles = 1;
			pParticleSystem->behaviour = shockwave_particle;
			pParticleSystem->obj_sprite->texture = pTex_shockwave;
			pParticleSystem->color_range[0] = { 1.0f, 1.0f, 1.0f, 0.5f };
			pParticleSystem->color_range[1] = { 1.0f, 1.0f, 1.0f, 0.5f };
			pParticleSystem->lifetime_range[0] = pParticleSystem->lifetime_range[1] = .75f;
			pParticleSystem->scale_range[0] = { 30.0f, 30.0f };
			pParticleSystem->scale_range[1] = { 30.0f, 30.0f };
			pParticleSystem->speed_range[0] = 0;
			pParticleSystem->speed_range[1] = 0;
			pParticleSystem->direction_range[0] = 0;
			pParticleSystem->direction_range[1] = 0;
			pParticleSystem->emission_rate = .03f;
		}
		collider->flag = true;
		collider->scale = { 300, 50 };
		player->animations[player->curr_attack->state].play_anim_frame(1);
	}
	else
	{
		player->animations[player->curr_attack->state].play_anim_frame(0);
	}
}