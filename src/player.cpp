/*!*****************************************************************************
\file   player.cpp
\author Primary: Cruz Rolly Matthew Capiral			(60.04%)
		Secondary: Mohammad Hanif Koh Teck Wee		(35.13%)
		Secondary: Siti Nursafinah Binte Sumsuri	(4.83%)
\par    DP email: cruzrolly.m\@digipen.edu
				  mohammadhanifkoh.t\@digipen.edu
				  sitinursafinah.b\@digipen.edu
\date   Feb 1 2021

\brief
This header file contains the implementation of the player class as outlined in
player.h

The functions include:
	- Player Constructors
	- Player Operator=
	- Player Destructor
	- Player Game Object Load
	- Player Game Object Init
	- Player Game Object Update
	- Player Game Object Draw
	- Player Game Object Free
	- Player Game Object Unload
	- hearts
	- h_healths
	- change_weapon
	- clear_ignore_colliders
	- setup_attack
	- combo
	- move
	- adjust_player_movement
	- start_invul
	- invul
	- take_damage
	- player_collide

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************************/
#include "main.h"
#include "player.h"
#include "enemy.h"
#include "traps.h"
#include "shield.h"
#include "collectables.h"
#include "camera.h"
#include "audio.h"
#include <iostream>
#include "graphics.h"
#include "game_states.h"
#include "particlesystem.h"
#include "level_manager.h"

constexpr f32 P_VEL_MOVEMENT = 500.0f;
constexpr f32 P_JUMP_FORCE = 1100.0f;
constexpr f32 AIRDASH_DISTANCE = 200.0f;
constexpr s32 P_SPRITE_ROW_SWR = 9;
constexpr s32 P_SPRITE_ROW_SPR = 9;
constexpr s32 P_SPRITE_COL_SWR = 9;
constexpr s32 P_SPRITE_COL_SPR = 9;
const s8* sword_sprite_sheet = "./Sprites/Player/sword_movement.png";
const s8* spear_sprite_sheet = "./Sprites/Player/spear_movement.png";
const s8* spear_icon_path = "./Sprites/icon_spear.png";
const s8* sword_icon_path = "./Sprites/icon_sword_shield.png";
static Audio* airdash_sound;
static Audio* hurt_sound;
static Audio* switch_sound;
static Audio* hurt_sound2;
static Audio* guarded_sound;
static f32 heart_shake_interval = 0.0f;
static f32 heart_shake_timer;

AEVec2 playerPos = { 0.0f, 0.0f };
AEVec2 playerSize = { 75.0f, 75.0f };
AEVec2 playerVel = { 0.0f, 0.0f };
AEVec2 frame_swr_size, tex_spear_size;
AEGfxVertexList* pMesh_Sword = 0;
AEGfxVertexList* pMesh_Spear = 0;
AEGfxTexture* pTex_Sword;
AEGfxTexture* pTex_Spear;
AEGfxTexture* pTex_Spear_Icon;
AEGfxTexture* pTex_Sword_Icon;

float combat_timer = 0.0f;
float combat_leeway = 0.5f;
float airdash_Counter = 0.0f;
float GRAVITY = -50.0f;

AEGfxTexture* fullheart;
AEGfxTexture* halfheart;
static Sprite* fullheart_sprite;
static Sprite* halfheart_sprite;
static AEGfxVertexList* pMeshHeart = 0;
static AEGfxVertexList* pMeshHeart2 = 0;
AEVec2 heartSize = { 50.0f, 50.0f };

/*!
 * \brief Construct a new Player:: Player object
 *
 *
 */
Player::Player()
	: gameObject(), sword_upgrade{ 0 }, spear_upgrade{ 0 }, airdashed{ false }, attack_setup{ false },
	curr_weapon{ false }, is_invul{ false }, parry{ false }, superarmour{ false }, attack_timer(), curr_attack(), flinch(), invul_timer(),
	is_facing_right(), shield_timer{}
{
	health = PLAYER_HEALTH;
	full_health = PLAYER_HEALTH;
	on_collide = player_collide;
	obj_index = 0;
	spr_index = 0;
	type = GO_TYPE::GO_PLAYER;
	scale = { PLAYER_SIZE, PLAYER_SIZE };
	layer = PLAYER_LAYER;
	is_static = false;
	curr_attack = nullptr;
	slide = false;
}

/*!
 * \brief Construct a new Player:: Player object with a set health
 *
 * \param health
 * Health value to set to the player
 */
Player::Player(s32 health) : Player()
{
	this->health = health;
	this->full_health = health;
}

/*!
 * \brief Construct a new Player:: Player object with a set init_pos
 *
 *
 * \param pos
 * Initial position for the player
 */
Player::Player(AEVec2 pos) : Player()
{
	init_pos = pos;
}

/*!
 * \brief Swap values of the player
 *
 * \param rhs
 * Rhs player reference to swap values with
 */
void Player::swap(Player& rhs)
{
	std::swap(health, rhs.health);
	std::swap(slide, rhs.slide);
	std::swap(on_ground, rhs.on_ground);
	std::swap(obj_index, rhs.obj_index);
	std::swap(spr_index, rhs.spr_index);
}

/*!
 * \brief Construct a new Player:: Player object via copy constructor
 *
 * \param rhs
 * Rhs player reference to copy values from
 */
Player::Player(const Player& rhs)
	: spear_upgrade{ rhs.spear_upgrade }, sword_upgrade(rhs.sword_upgrade), health{ rhs.health }, full_health{ rhs.full_health }, airdashed(), attack_setup(),
	attack_timer(), shield_timer{ rhs.shield_timer }, curr_attack(), curr_weapon(), flinch(), parry(), superarmour(), invul_timer(),
	is_facing_right(), is_invul{}
{
	slide = rhs.slide;
	on_ground = rhs.on_ground;
	obj_index = rhs.obj_index;
	spr_index = rhs.spr_index;
}

/*!
 * \brief Assignment operator overload of the player
 *
 *
 * \param rhs
 * 	Rhs player reference to copy values from
 * \return Player&
 * Reference to the player
 */
Player& Player::operator=(Player const& rhs)
{
	Player tmp{ rhs };
	swap(tmp);
	return *this;
}

/*!
 * \brief Destroy the Player:: Player object
 *
 *
 */
Player::~Player()
{
}

/*!
 * \brief Override of gameObject's set_aabb(), changing the values of x and y
 * to fit the hitbox of the player.
 *
 */
void Player::set_aabb()
{
	AEVec2Set(&bounding_box.min, -0.28f * scale.x, -0.5f * scale.y);
	AEVec2Set(&bounding_box.max, 0.22f * scale.x, 0.45f * scale.y);
	AEVec2Add(&bounding_box.min, &bounding_box.min, &curr_pos);
	AEVec2Add(&bounding_box.max, &bounding_box.max, &curr_pos);
}

/*!
 * \brief Load the assets required for the player such as textures, animations,
 * audio, and sprites for heart UI
 */
void Player::game_object_load()
{
	f32 sword_trans_x, sword_trans_y, spear_trans_x, spear_trans_y;
	pTex_Sword = AEGfxTextureLoad(sword_sprite_sheet);
	AE_ASSERT_MESG(pTex_Sword, "Failed to create sword texture!!");
	frame_swr_size = read_png_size(sword_sprite_sheet);

	pTex_Spear = AEGfxTextureLoad(spear_sprite_sheet);
	AE_ASSERT_MESG(pTex_Sword, "Failed to create spear texture!!");
	tex_spear_size = read_png_size(spear_sprite_sheet);

	pTex_Spear_Icon = AEGfxTextureLoad(spear_icon_path);
	AE_ASSERT_MESG(pTex_Spear_Icon, "Failed to create spear icon texture!!");

	pTex_Sword_Icon = AEGfxTextureLoad(sword_icon_path);
	AE_ASSERT_MESG(pTex_Sword_Icon, "Failed to create sword icon texture!!");

	fullheart = AEGfxTextureLoad("./Sprites/fullheart.png");
	AE_ASSERT_MESG(fullheart, "Failed to create test_texture4!!");

	halfheart = AEGfxTextureLoad("./Sprites/halfheart.png");
	AE_ASSERT_MESG(halfheart, "Failed to create test_texture5!!");

	sword_trans_x = frame_swr_size.x / P_SPRITE_COL_SWR / frame_swr_size.x;
	sword_trans_y = frame_swr_size.y / P_SPRITE_ROW_SWR / frame_swr_size.y;
	spear_trans_x = tex_spear_size.x / P_SPRITE_COL_SPR / tex_spear_size.x;
	spear_trans_y = tex_spear_size.y / P_SPRITE_ROW_SPR / tex_spear_size.y;

	pMesh_Sword = create_square_mesh(1.0f / P_SPRITE_COL_SWR, 1.0f / P_SPRITE_ROW_SWR);

	pMesh_Spear = create_square_mesh(spear_trans_x, spear_trans_y);

	airdash_sound = new Audio("./Audio/airdash.wav", AUDIO_GROUP::SFX);
	hurt_sound = new Audio("./Audio/player_hit.wav", AUDIO_GROUP::SFX);
	hurt_sound2 = new Audio("./Audio/hurt.wav", AUDIO_GROUP::SFX);
	switch_sound = new Audio("./Audio/wep_switch.wav", AUDIO_GROUP::SFX);
	guarded_sound = new Audio("./Audio/guard_hurt.wav", AUDIO_GROUP::SFX);

	obj_sprite = new Sprite(pTex_Sword, pMesh_Sword, scale);
	obj_sprite->set_size(read_png_size(sword_sprite_sheet));
	Animation idle = Animation(1, { 0.0f, 0.0f }, obj_sprite, 125, 0.1f);
	AEVec2 wep_slash_offsets[4] = { {0.0f, 204.0f}, {500.0f, 204.0f}, {125.0f, 204.0f}, {250.0f, 204.0f} };
	Animation wep_slash1 = Animation(obj_sprite, wep_slash_offsets, 4, 0.08f);
	Animation wep_slash2 = wep_slash1;
	wep_slash2.reverse_anims();
	Animation wep_slash3 = Animation(4, { 0.0f, 306.0f }, obj_sprite, 125, 0.4f);
	Animation shielding = Animation(8, { 125.0f, 612.0f }, obj_sprite, 125);
	Animation shield_shove = Animation(6, { 0.0f, 510.0f }, obj_sprite, 125, 0.4f);
	AEVec2 parry_stab_offsets[2] = { {0.0f, 306.0f}, {125.0f, 204.0f} };
	Animation parry_stab = Animation(obj_sprite, parry_stab_offsets, 2);
	Animation run = Animation(8, { 125.0f, 0 }, obj_sprite, 125, 0.08f);
	Animation jump = Animation(3, { 125.0f, 102.0f }, obj_sprite, 125, 0.02f);
	Animation sword_jump_atk = Animation(3, { 125.0f, 714.0f }, obj_sprite, 125, 0.02f);
	Animation sword_jump_plunge = Animation(3, { 0, 816.0f }, obj_sprite, 125, 0.16f);

	Animation spear_stab1 = Animation(3, { 125.0f, 612.0f }, obj_sprite, 125, 0.4f);
	spear_stab1.pushback_offset({ 250.0f, 510.0f });
	Animation spear_slash1 = Animation(5, { 125.0f, 204.0f }, obj_sprite, 125, 0.05f);
	Animation spear_slash2 = Animation(3, { 125.0f, 306.0f }, obj_sprite, 125, 0.1f);
	Animation spear_kick = Animation(3, { 0.0f, 408.0f }, obj_sprite, 125, 0.05f);
	Animation spear_throw = Animation(4, { 125.0f, 714.0f }, obj_sprite, 125, 0.05f);
	Animation spear_jump_slash = Animation(3, { 125.0f, 816.0f }, obj_sprite, 125, 0.06f);
	Animation spear_plunge = Animation(5, { 125.0f, 816.0f }, obj_sprite, 125, 0.08f);

	animations.insert({ P_Anim_State::P_NONE, idle });
	animations.insert({ P_Anim_State::P_SWORD_RUN, run });
	animations.insert({ P_Anim_State::P_SWORD_JUMP, jump });
	animations.insert({ P_Anim_State::P_SWORD_1_1, wep_slash1 });
	animations.insert({ P_Anim_State::P_SWORD_1_2, wep_slash2 });
	animations.insert({ P_Anim_State::P_SWORD_1_3, wep_slash1 });
	animations.insert({ P_Anim_State::P_SWORD_1_4, wep_slash3 });
	animations.insert({ P_Anim_State::P_SWORD_2_1, shield_shove });
	animations.insert({ P_Anim_State::P_SHIELD_1, shielding });
	animations.insert({ P_Anim_State::P_SWORD_JUMP_ATK, sword_jump_atk });
	animations.insert({ P_Anim_State::P_SWORD_PLUNGE, sword_jump_plunge });

	animations.insert({ P_Anim_State::P_SPEAR_RUN, run });
	animations.insert({ P_Anim_State::P_SPEAR_JUMP, jump });
	animations.insert({ P_Anim_State::P_SPEAR_JUMP_ATK, spear_jump_slash });
	animations.insert({ P_Anim_State::P_SPEAR_PLUNGE, spear_plunge });
	animations.insert({ P_Anim_State::P_SPEAR_1_1, spear_stab1 });
	animations.insert({ P_Anim_State::P_SPEAR_1_2, spear_stab1 });
	animations.insert({ P_Anim_State::P_SPEAR_1_3, spear_stab1 });
	animations.insert({ P_Anim_State::P_SPEAR_1_4, spear_stab1 });
	animations.insert({ P_Anim_State::P_SPEAR_2_1, spear_slash1 });
	animations.insert({ P_Anim_State::P_SPEAR_2_2, spear_slash2 });
	animations.insert({ P_Anim_State::P_SPEAR_2_3, spear_kick });
	animations.insert({ P_Anim_State::P_SPEAR_3_1, spear_throw });

	animations.insert({ P_Anim_State::P_SHIELD_2, parry_stab });

	pMeshHeart = create_square_mesh(1.0f, 1.0f);
	AE_ASSERT_MESG(pMeshHeart, "Failed to create mesh heart!!");

	fullheart_sprite = new Sprite(fullheart, pMeshHeart, heartSize);
	fullheart_sprite->set_size(read_png_size("./Sprites/fullheart.png"));

	halfheart_sprite = new Sprite(halfheart, pMeshHeart, heartSize);
	halfheart_sprite->set_size(read_png_size("./Sprites/halfheart.png"));
}

/*!
 * \brief Initialize the player with sword weapon, curr_pos at init_pos, facing
 * right, idle state and initialize the attacks.
 *
 */
void Player::game_object_initialize()
{
	curr_pos = init_pos;
	curr_weapon = false;
	airdashed = false;
	is_facing_right = true;
	state = P_Anim_State::P_SWORD_IDLE;
	on_collide = player_collide;
	attacks_init(sword_attacks, spear_attacks);
}

/*!
 * \brief Update the player's game object, calling functions such as
 * change_weapon, invulnerability, combo system, movement. The heart UI timer is
 *  processed here as well.
 *
 */
void Player::game_object_update()
{
	change_weapon();
	invul();
	combo();
	move();
	heart_shake_timer -= G_DELTATIME;
	heart_shake_interval -= G_DELTATIME;
	if (health <= 6)
	{
		heart_shake_timer = 0.5f;
	}
	on_ground = false;
}

/*!
 * \brief Draw the health UI. Afterwards, the sprite will flip if the player is
 * not facing right. Depending on the appropriate state, play its respective
 * animation. Afterwards, tint the player half-transparent if it is on idle
 * animation state and is invulnerable. Will also tint if shielding or parrying.
 * Lastly, draw the sprite.
 */
void Player::game_object_draw()
{
	h_healths();

	obj_sprite->flip_x = is_facing_right ? false : true;

	if (curr_weapon)
	{
		switch (state)
		{
		case P_Anim_State::P_SPEAR_IDLE:
		case P_Anim_State::P_SPEAR_FLINCH:
			animations[P_Anim_State::P_NONE].reset();
			animations[P_Anim_State::P_NONE].play_animation();
			break;
		case P_Anim_State::P_SPEAR_AIRDASH:
		case P_Anim_State::P_SPEAR_JUMP:
			animations[P_Anim_State::P_SPEAR_JUMP].play_animation(false);
			break;
		case P_Anim_State::P_SPEAR_RUN:
			if (curr_vel.x != 0)
				animations[P_Anim_State::P_SPEAR_RUN].play_animation();
			else
			{
				animations[P_Anim_State::P_SPEAR_RUN].reset();
				animations[P_Anim_State::P_NONE].reset();
				animations[P_Anim_State::P_NONE].play_animation();
			}
			break;
		default:
			break;
		}
	}
	else
	{
		switch (state)
		{
		case P_Anim_State::P_SWORD_IDLE:
		case P_Anim_State::P_SWORD_FLINCH:
			animations[P_Anim_State::P_NONE].reset();
			animations[P_Anim_State::P_NONE].play_animation();
			break;
		case P_Anim_State::P_SWORD_AIRDASH:
		case P_Anim_State::P_SWORD_JUMP:
			animations[P_Anim_State::P_SWORD_JUMP].play_animation(false);
			break;
		case P_Anim_State::P_SWORD_RUN:
			if (curr_vel.x != 0)
				animations[P_Anim_State::P_SWORD_RUN].play_animation();
			else
			{
				animations[P_Anim_State::P_SWORD_RUN].reset();
				animations[P_Anim_State::P_NONE].reset();
				animations[P_Anim_State::P_NONE].play_animation();
			}
			break;
		default:
			break;
		}
	}

	if (is_invul && state == P_Anim_State::P_SWORD_IDLE)
	{
		obj_sprite->tint = { 0.5f, 0.5f, 0.5f, 0.5f };
	}
	else
	{
		obj_sprite->tint = { 1.0f, 1.0f, 1.0f, 1.0f };
	}
	if (shield_timer && shield_timer < PARRY_TIME)
		obj_sprite->tint = { 0.4f, 1.0f, 0.8f, 1.0f };
	else if (shield_timer > PARRY_TIME)
		obj_sprite->tint = { 0.2f, 0.8f, 0.8f, 1.0f };
	draw(obj_sprite, curr_pos, scale);
}
void Player::game_object_free()
{
	while (get_attack_collider(this))
	{
		remove_attack_collider(this);
	}
	delete obj_sprite;
	delete fullheart_sprite;
	delete halfheart_sprite;
	sword_attacks.clear();
	spear_attacks.clear();
	sword_attacks.~vector();
	spear_attacks.~vector();
	ignorecolliders.clear();
	ignorecolliders.~vector();
}

/*!
 * \brief Unload player assets that were loaded in the load function.
 */
void Player::game_object_unload()
{
	animations.clear();
	AEGfxMeshFree(pMesh_Sword);
	AEGfxMeshFree(pMesh_Spear);
	AEGfxMeshFree(pMeshHeart);
	AEGfxTextureUnload(pTex_Sword);
	AEGfxTextureUnload(pTex_Sword_Icon);
	AEGfxTextureUnload(pTex_Spear);
	AEGfxTextureUnload(pTex_Spear_Icon);
	AEGfxTextureUnload(fullheart);
	AEGfxTextureUnload(halfheart);
	delete airdash_sound;
	delete hurt_sound;
	delete hurt_sound2;
	delete switch_sound;
	delete guarded_sound;
}

/*!*****************************************************************************
  \brief
	Function is to check whether the player's health is even or odd number
*******************************************************************************/
bool hearts(Player const& p)
{
	int health = p.health;
	return health % 2;
}

/*!*****************************************************************************
  \brief
	Function is to draw the health of the player to its correspending number
	of hearts
*******************************************************************************/
void Player::h_healths()
{
	AEVec2 camPos;
	AEGfxGetCamPosition(&camPos.x, &camPos.y);
	AEVec2 coordinates = { -610, 310 };
	if (heart_shake_timer > 0)
	{
		coordinates.x += 5.0f * AERandFloat();
		coordinates.x -= 5.0f * AERandFloat();
		coordinates.y += 5.0f * AERandFloat();
		coordinates.y -= 5.0f * AERandFloat();
		heart_shake_timer -= G_DT;
	}

	int offset = 50;

	if (health <= 0)
	{
		transition(0.5f, GS_GAMEOVER);
	}
	else if (hearts(*this) == false && health != 0)
	{
		for (int i = 0; i < (health / 2); ++i)
		{
			coordinates.x += offset;

			draw_non_relative(fullheart_sprite, coordinates, heartSize);
		}
	}

	else if (hearts(*this) == true && health != 0)
	{
		for (int i = 0; i < ((health - 1) / 2); ++i)
		{
			coordinates.x += offset;
			draw_non_relative(fullheart_sprite, coordinates, heartSize);
		}

		coordinates.x += offset - 12;
		draw_non_relative(halfheart_sprite, coordinates, { heartSize.x / 2, heartSize.y });
	}
}

/*!
 * \brief Change the player's current weapon.
 * If the player state is idle, run, or jump in either weapon, spawn the
 * particle system that will show the changed weapon icon and change the
 * sprite's mesh, texture, and size.
 *
 */
void Player::change_weapon()
{
	if (AEInputCheckTriggered(AEVK_V))
	{
		if (state == P_Anim_State::P_SWORD_IDLE || state == P_Anim_State::P_SPEAR_IDLE ||
			state == P_Anim_State::P_SWORD_RUN || state == P_Anim_State::P_SPEAR_RUN ||
			state == P_Anim_State::P_SWORD_JUMP || state == P_Anim_State::P_SPEAR_JUMP)
		{
			particleSystem* pParticleSystem = create_particlesystem(this);
			pParticleSystem->lifetime = .3f;
			pParticleSystem->position_offset.y += 75.0f;
			pParticleSystem->scale = { 1, 1 };
			pParticleSystem->max_particles = 1;
			pParticleSystem->behaviour = change_wep_particle;
			pParticleSystem->obj_sprite->texture = curr_weapon ? pTex_Sword_Icon : pTex_Spear_Icon;
			pParticleSystem->color_range[0] = { 1.0f, 1.0f, 1.0f, 1.0f };
			pParticleSystem->color_range[1] = { 1.0f, 1.0f, 1.0f, 1.0f };
			pParticleSystem->lifetime_range[0] = pParticleSystem->lifetime_range[1] = 5.0f;
			pParticleSystem->scale_range[0] = { 30.0f, 30.0f };
			pParticleSystem->scale_range[1] = { 30.0f, 30.0f };
			pParticleSystem->speed_range[0] = 0;
			pParticleSystem->speed_range[1] = 0;
			pParticleSystem->direction_range[0] = 0;
			pParticleSystem->direction_range[1] = 0;
			pParticleSystem->emission_rate = .03f;
			pParticleSystem->particle_increment = 1;
			switch_sound->play_sound();
			curr_weapon = !curr_weapon;
			if (curr_weapon)
			{
				obj_sprite->mesh = pMesh_Spear;
				obj_sprite->texture = pTex_Spear;
				obj_sprite->size = tex_spear_size;
			}
			else
			{
				obj_sprite->mesh = pMesh_Sword;
				obj_sprite->texture = pTex_Sword;
				obj_sprite->size = frame_swr_size;
			}

			if (state == P_Anim_State::P_SWORD_IDLE)
				state = P_Anim_State::P_SPEAR_IDLE;
			else if (state == P_Anim_State::P_SPEAR_IDLE)
				state = P_Anim_State::P_SWORD_IDLE;
			else if (state == P_Anim_State::P_SWORD_RUN)
				state = P_Anim_State::P_SPEAR_RUN;
			else if (state == P_Anim_State::P_SPEAR_RUN)
				state = P_Anim_State::P_SWORD_RUN;
			else if (state == P_Anim_State::P_SWORD_JUMP)
				state = P_Anim_State::P_SPEAR_JUMP;
			else if (state == P_Anim_State::P_SPEAR_JUMP)
				state = P_Anim_State::P_SWORD_JUMP;
		}
	}
}

/*!*****************************************************************************
  \brief
	Clears the ignore_colliders vector of all active enemies within the current
	area. Called in between each attack so that enemies will take damage from
	the incoming attack
*******************************************************************************/
void clear_ignore_colliders()
{
	if (Level::boss->flag)
	{
		Level::boss->ignorecolliders.clear();
	}

	else if (Level::pLevel->enemy_counter)
	{
		for (Enemy* pEnemy : *Level::v_area_enemies[Level::pLevel->current_area])
		{
			if (pEnemy->flag)
			{
				pEnemy->ignorecolliders.clear();
			}
		}
	}
}

/*!*****************************************************************************
  \brief
	Sets up the new attack the player is using by pushing an attack_collider
	into the vector of game objects, clearing the ignore_colliders vector in
	the active enemies and also setting the values of certain variables in the
	player according to the attack being used.

  \param pAtk
	A pointer to the attack being used by the player.
*******************************************************************************/
void Player::setup_attack(Player_Attack* pAtk)
{
	push_object(GO_TYPE::GO_PLAYERATTACKCOLLIDER, this);
	clear_ignore_colliders();
	curr_attack = pAtk;
	attack_setup = true;
	state = curr_attack->state;
	attack_timer = curr_attack->leeway_timer;
}

/*!*****************************************************************************
  \brief
	The combo function of the player. When the player inputs an attack key, the
	function will first check if the player is in a state where he can attack.
	It will then check if the player is currently attacking. If the player is
	not attacking currently, it will set the player's current attack to the
	first attack in any combo corresponding to the input entered by the player.
	If the player is currently attacking, it will check whether the minimum
	duration of the attack has passed. If it has passed, the function will then
	set the current attack of the player to the new attack. If not, it will
	ignore the input.

	If the player has a current attack, the function will then execute the
	attack and update the attack timer of the player. When the attack timer
	reaches 0, it will reset the state of the player, remove the attack collider
	from the vector of game objects, and set the current attack of the player
	to be a nullptr.
*******************************************************************************/
void Player::combo()
{
	if (AEInputCheckTriggered(AEVK_Z) &&
		state != P_Anim_State::P_SWORD_AIRDASH &&
		state != P_Anim_State::P_SPEAR_AIRDASH &&
		state != P_Anim_State::P_SWORD_FLINCH &&
		state != P_Anim_State::P_SPEAR_FLINCH)
	{
		if (!curr_weapon)
		{
			if (state == P_Anim_State::P_SWORD_IDLE || state == P_Anim_State::P_SWORD_RUN || state == P_Anim_State::P_SWORD_JUMP)
			{
				for (vector<Player_Attack>::iterator i = sword_attacks.begin(); i != sword_attacks.end(); ++i)
				{
					if (i->state == P_Anim_State::P_SWORD_1_1 && state != P_Anim_State::P_SWORD_JUMP)
					{
						setup_attack(&*i);
						break;
					}
					if (i->state == P_Anim_State::P_SWORD_JUMP_ATK)
					{
						setup_attack(&*i);
						break;
					}
				}
			}

			else
			{
				for (vector<Player_Attack>::iterator i = sword_attacks.begin(); i != sword_attacks.end(); ++i)
				{
					if (curr_attack && curr_attack->next_z != P_Anim_State::P_NONE &&
						i->state == curr_attack->next_z &&
						curr_attack->leeway_timer - attack_timer > curr_attack->min_duration)
					{
						if (curr_attack)
							animations[curr_attack->state].reset();

						setup_attack(&*i);
						remove_attack_collider(this);
						break;
					}
				}
			}
		}

		else
		{
			if (state == P_Anim_State::P_SPEAR_IDLE || state == P_Anim_State::P_SPEAR_RUN || state == P_Anim_State::P_SPEAR_JUMP)
			{
				for (vector<Player_Attack>::iterator i = spear_attacks.begin(); i != spear_attacks.end(); ++i)
				{
					if (i->state == P_Anim_State::P_SPEAR_1_1 && state != P_Anim_State::P_SPEAR_JUMP)
					{
						setup_attack(&*i);
						break;
					}
					if (i->state == P_Anim_State::P_SPEAR_JUMP_ATK)
					{
						setup_attack(&*i);
						break;
					}
				}
			}

			else
			{
				for (vector<Player_Attack>::iterator i = spear_attacks.begin(); i != spear_attacks.end(); ++i)
				{
					if (curr_attack && curr_attack->next_z != P_Anim_State::P_NONE &&
						i->state == curr_attack->next_z &&
						curr_attack->leeway_timer - attack_timer > curr_attack->min_duration)
					{
						setup_attack(&*i);
						remove_attack_collider(this);
						break;
					}
				}
			}
		}
	}

	else if (AEInputCheckTriggered(AEVK_X) &&
		state != P_Anim_State::P_SWORD_AIRDASH &&
		state != P_Anim_State::P_SPEAR_AIRDASH &&
		state != P_Anim_State::P_SWORD_FLINCH &&
		state != P_Anim_State::P_SPEAR_FLINCH)
	{
		if (!curr_weapon)
		{
			if (state == P_Anim_State::P_SWORD_IDLE || state == P_Anim_State::P_SWORD_RUN || state == P_Anim_State::P_SWORD_JUMP)
			{
				for (vector<Player_Attack>::iterator i = sword_attacks.begin(); i != sword_attacks.end(); ++i)
				{
					if (i->state == P_Anim_State::P_SWORD_2_1 && state != P_Anim_State::P_SWORD_JUMP)
					{
						setup_attack(&*i);
						break;
					}
					if (i->state == P_Anim_State::P_SWORD_PLUNGE)
					{
						setup_attack(&*i);
						break;
					}
				}
			}

			else
			{
				for (vector<Player_Attack>::iterator i = sword_attacks.begin(); i != sword_attacks.end(); ++i)
				{
					if (curr_attack && curr_attack->next_x != P_Anim_State::P_NONE &&
						i->state == curr_attack->next_x &&
						curr_attack->leeway_timer - attack_timer > curr_attack->min_duration)
					{
						setup_attack(&*i);
						remove_attack_collider(this);
						break;
					}
				}
			}
		}

		else
		{
			if (state == P_Anim_State::P_SPEAR_IDLE || state == P_Anim_State::P_SPEAR_RUN || state == P_Anim_State::P_SPEAR_JUMP)
			{
				for (vector<Player_Attack>::iterator i = spear_attacks.begin(); i != spear_attacks.end(); ++i)
				{
					if (i->state == P_Anim_State::P_SPEAR_2_1 && state != P_Anim_State::P_SPEAR_JUMP)
					{
						setup_attack(&*i);
						break;
					}
					if (i->state == P_Anim_State::P_SPEAR_PLUNGE)
					{
						setup_attack(&*i);
						break;
					}
				}
			}

			else
			{
				for (vector<Player_Attack>::iterator i = spear_attacks.begin(); i != spear_attacks.end(); ++i)
				{
					if (curr_attack && curr_attack->next_x != P_Anim_State::P_NONE &&
						i->state == curr_attack->next_x &&
						curr_attack->leeway_timer - attack_timer > curr_attack->min_duration)
					{
						setup_attack(&*i);
						remove_attack_collider(this);
						break;
					}
				}
			}
		}
	}

	else if (AEInputCheckTriggered(AEVK_C) &&
		state != P_Anim_State::P_SWORD_AIRDASH &&
		state != P_Anim_State::P_SPEAR_AIRDASH &&
		state != P_Anim_State::P_SWORD_FLINCH &&
		state != P_Anim_State::P_SPEAR_FLINCH)
	{
		if (!curr_weapon)
		{
			if (state == P_Anim_State::P_SWORD_IDLE || state == P_Anim_State::P_SWORD_RUN)
			{
				for (vector<Player_Attack>::iterator i = sword_attacks.begin(); i != sword_attacks.end(); ++i)
				{
					if (i->state == P_Anim_State::P_SHIELD_1)
					{
						setup_attack(&*i);
						break;
					}
				}
			}

			else
			{
				for (vector<Player_Attack>::iterator i = sword_attacks.begin(); i != sword_attacks.end(); ++i)
				{
					if (curr_attack && curr_attack->next_x != P_Anim_State::P_NONE &&
						i->state == curr_attack->next_x &&
						curr_attack->leeway_timer - attack_timer > curr_attack->min_duration)
					{
						setup_attack(&*i);
						remove_attack_collider(this);
						break;
					}
				}
			}
		}

		else
		{
			if (state == P_Anim_State::P_SPEAR_IDLE || state == P_Anim_State::P_SPEAR_RUN)
			{
				for (vector<Player_Attack>::iterator i = spear_attacks.begin(); i != spear_attacks.end(); ++i)
				{
					if (i->state == P_Anim_State::P_SPEAR_3_1)
					{
						setup_attack(&*i);
						break;
					}
				}
			}

			else
			{
				for (vector<Player_Attack>::iterator i = spear_attacks.begin(); i != spear_attacks.end(); ++i)
				{
					if (curr_attack && curr_attack->next_x != P_Anim_State::P_NONE &&
						i->state == curr_attack->next_x &&
						curr_attack->leeway_timer - attack_timer > curr_attack->min_duration)
					{
						setup_attack(&*i);
						remove_attack_collider(this);
						break;
					}
				}
			}
		}
	}

	if (curr_attack && curr_attack->attack)
		curr_attack->attack(*this);

	if (attack_timer > 0)
	{
		attack_timer -= G_DELTATIME;
	}

	else if (attack_timer < 0)
	{
		attack_timer = 0;
		if (curr_attack)
			animations[curr_attack->state].reset();
		curr_attack = nullptr;
		shield_timer = 0;
		remove_attack_collider(this);
		parry = false;
		superarmour = false;
		if (!curr_weapon)
			state = on_ground ? P_Anim_State::P_SWORD_IDLE : P_Anim_State::P_SWORD_JUMP;
		else
			state = on_ground ? P_Anim_State::P_SPEAR_IDLE : P_Anim_State::P_SPEAR_JUMP;
	}
}

/*!*****************************************************************************
  \brief
	The movement function of the player. Function checks if the player is in
	a state where he can move, and checks for movement inputs by the player.

	If the player inputs left or right movement, the movement is added to the
	acceleration of the player.

	If the player inputs to jump, the function will check if the player is on
	the floor. If it is, it will add the jump force to the velocity of the
	player. If not, it will ignore the input.

	If the player inputs to airdash, the function will check if the player has
	already airdashed. If not, it will check if the player is in the air. If
	the player is in the air, gravity will be ignored and a large x velocity
	will be added to the acceleration of the player.

	After checking for all movement inputs, the function then adds the
	acceleration to the velocity of the player, and calculates and updates the
	new position of the player. It then zeros the acceleration to set up for the
	movement calculation in the next frame.
*******************************************************************************/
void Player::move()
{
	AEVec2 rel_pos = get_relative_pos(curr_pos);

	if (state == P_Anim_State::P_SWORD_RUN || state == P_Anim_State::P_SWORD_JUMP ||
		state == P_Anim_State::P_SPEAR_RUN || state == P_Anim_State::P_SPEAR_JUMP ||
		state == P_Anim_State::P_SWORD_IDLE || state == P_Anim_State::P_SPEAR_IDLE)
	{
		if ((AEInputCheckTriggered(AEVK_UP) || AEInputCheckTriggered(AEVK_SPACE)) && on_ground)
		{
			curr_vel.y = P_JUMP_FORCE;
			on_ground = false;
			animations[P_Anim_State::P_SWORD_JUMP].reset();
			animations[P_Anim_State::P_SPEAR_JUMP].reset();
			if (!curr_weapon)
				state = P_Anim_State::P_SWORD_JUMP;
			else
				state = P_Anim_State::P_SPEAR_JUMP;
		}
		if (AEInputCheckCurr(AEVK_LEFT))
		{
			acceleration.x += -P_VEL_MOVEMENT;
			if (!curr_weapon && on_ground)
				state = P_Anim_State::P_SWORD_RUN;
			else if (on_ground)
				state = P_Anim_State::P_SPEAR_RUN;
			is_facing_right = false;
		}
		if (AEInputCheckCurr(AEVK_RIGHT))
		{
			acceleration.x += P_VEL_MOVEMENT;
			if (!curr_weapon && on_ground)
				state = P_Anim_State::P_SWORD_RUN;
			else if (on_ground)
				state = P_Anim_State::P_SPEAR_RUN;
			is_facing_right = true;
		}

		if ((AEInputCheckReleased(AEVK_LEFT) || AEInputCheckReleased(AEVK_RIGHT)) && on_ground)
		{
			if (!curr_weapon)
				state = P_Anim_State::P_SWORD_IDLE;
			else
				state = P_Anim_State::P_SPEAR_IDLE;
		}

		if (AEInputCheckTriggered(AEVK_D) && !airdashed && !on_ground)
		{
			airdash_sound->play_sound(0.3f);
			particleSystem* pParticleSystem = create_dash_particles(this);
			pParticleSystem->direction_range[0] = pParticleSystem->direction_range[1] = is_facing_right ? PI : 0;

			airdashed = true;
			if (is_facing_right)
			{
				curr_vel.x = 2 * P_VEL_MOVEMENT;
				curr_vel.y = 0;
			}
			else
			{
				airdashed = true;
				curr_vel.x = 2 * -P_VEL_MOVEMENT;
				curr_vel.y = 0;
			}
			airdash_Counter = AIRDASH_DISTANCE;
			if (!curr_weapon)
				state = P_Anim_State::P_SWORD_AIRDASH;
			else
				state = P_Anim_State::P_SPEAR_AIRDASH;
		}
	}

	if (state != P_Anim_State::P_SPEAR_AIRDASH && state != P_Anim_State::P_SWORD_AIRDASH)
	{
		acceleration.y += GRAVITY;
	}

	if (state != P_Anim_State::P_SWORD_AIRDASH && state != P_Anim_State::P_SPEAR_AIRDASH &&
		state != P_Anim_State::P_SWORD_FLINCH)
	{
		if (slide)
		{
			acceleration.x *= static_cast<f32>(G_DELTATIME);
			AEVec2Add(&curr_vel, &curr_vel, &acceleration);
			curr_vel.x *= 0.98f;
		}
		else
		{
			curr_vel.x = acceleration.x * 60 * G_DELTATIME;
			curr_vel.y += acceleration.y * 60 * G_DELTATIME;
		}
	}

	if (state != P_Anim_State::P_SWORD_FLINCH)
	{
		AEVec2Zero(&acceleration);
	}
	else if (on_ground)
	{
		acceleration.x = 0;
	}
	AEVec2 dt_vel;
	AEVec2Scale(&dt_vel, &curr_vel, static_cast<f32>(G_DELTATIME));
	AEVec2Add(&curr_pos, &curr_pos, &dt_vel);

	if (airdash_Counter)
		airdash_Counter -= fabs(dt_vel.x);
	if (airdash_Counter < 0)
	{
		airdash_Counter = 0;
		if (!curr_weapon)
			state = P_Anim_State::P_SWORD_JUMP;
		else
			state = P_Anim_State::P_SPEAR_JUMP;
	}
}

/*!
 * \brief Adjust the player's movement based on which side it is colliding to
 * If obj is a wall, push the player by the horizontal depth.
 *
 * If the player is near the top and velocity is going downwards, the player
 * will be forced on the ground, disabling airdash, zeroing the velocity and
 * remove attack colliders
 *
 * \param plyr
 * Reference to the game object representing the player
 * \param obj
 * Reference to the game object representing the collided object
 */
void adjust_player_movement(gameObject& plyr, const gameObject& obj)
{
	Player* player = dynamic_cast<Player*>(&plyr);
	AEVec2 player_currentpos = player->curr_pos;
	AEVec2 diff_pos = obj.curr_pos;
	AEVec2Sub(&diff_pos, &player->curr_pos, &diff_pos);

	f32 player_width = (player->bounding_box.max.x - player->bounding_box.min.x) / 2;
	f32 player_hh = (player->bounding_box.max.y - player->bounding_box.min.y) / 2;
	f32 obj_halfscale_x = (obj.bounding_box.max.x - obj.bounding_box.min.x) / 2;
	f32 obj_halfscale_y = (obj.bounding_box.max.y - obj.bounding_box.min.y) / 2;

	f32 min_distx = player_width + obj_halfscale_x;
	f32 min_disty = player_hh + obj_halfscale_y;

	f32 depth_x = min_distx - fabs(diff_pos.x);

	f32 depth_y = min_disty - fabs(diff_pos.y);
	if (depth_x != 0 && depth_y != 0)
	{
		if (obj.type == GO_TYPE::GO_WALL && fabs(depth_x) < fabs(depth_y))
		{
			player->curr_pos.x += diff_pos.x < 0 ? -depth_x : +depth_x;
		}
		if (fabs(depth_y) < fabs(depth_x))
		{
			if (diff_pos.y > 0 && fabs(depth_y) <= 30.0f)
			{
				player->curr_pos.y += depth_y;
				player->curr_vel.y = 0;
				player->on_ground = true;
				player->airdashed = false;
				switch (player->state)
				{
				case P_Anim_State::P_SWORD_JUMP:
				case P_Anim_State::P_SWORD_JUMP_ATK:
					player->state = P_Anim_State::P_SWORD_IDLE;
					remove_attack_collider(player);
					break;
				case P_Anim_State::P_SPEAR_JUMP_ATK:
				case P_Anim_State::P_SPEAR_JUMP:
					player->state = P_Anim_State::P_SPEAR_IDLE;
					remove_attack_collider(player);
					break;
				}
			}
		}
	}
	else if (!depth_y && diff_pos.y > 0)
	{
		player->curr_vel.y = 0;
		player->airdashed = false;
		player->on_ground = true;
	}
}

/*!
 * \brief Sets in_vul to true
 *
 *
 */
void Player::start_invul()
{
	is_invul = true;
}

/*!
 * \brief Invulnerability time of the player.
 *
 * If the player state is flinch on either weapon, its state will be set back to
 *  the idle state.
 * Increment the invul_timer by delta time and if the timer is >= 1, undo
 *  is_invul and set invul_timer to 0
 *
 */
void Player::invul()
{
	if (is_invul)
	{
		if (invul_timer < 1.0f)
		{
			invul_timer += G_DELTATIME;
		}

		if (state == P_Anim_State::P_SWORD_FLINCH || state == P_Anim_State::P_SPEAR_FLINCH)
		{
			state = static_cast<P_Anim_State>((static_cast<int>(state) - 4));
		}

		else if (invul_timer >= 1)
		{
			is_invul = !is_invul;
			invul_timer = 0;
		}
	}
}

/*!
 * \brief Get player to take damage
 * If god mode is set, health will not increment
 * If the player is on flinch state, a sound will play. If the player is
 * shielding, another sound will play.
 * The player will set to invul and the camera will shake for hit response.
 * Additionally, a particle system will spawn representing the player taking
 * damage.
 *
 * \param damage
 */
void Player::take_damage(s32 damage)
{
	if (!god_mode)
		health -= damage;
	heart_shake_timer = 0.5f;
	if (state == P_Anim_State::P_SWORD_FLINCH || state == P_Anim_State::P_SPEAR_FLINCH)
		hurt_sound->play_sound();
	if (shield_timer)
		guarded_sound->play_sound();
	else
		hurt_sound2->play_sound();
	start_invul();
	Camera::shake(4.0f, 0.3f);
	particleSystem* pParticleSystem = create_damaged_particles(this);
}

/*!
 * \brief Collision function for the player.
 * Iterate through all the colliders in the player.
 * If the collider is a platform or a destructable and the player is not on
 * ground, check if the player is moving downwards and then adjust player
 * movement.
 * Same goes for the wall but did not need to dot product.
 *
 * If the object type is a monster, check for the default enemy. If the type is
 * the default enemy, check collision with player and enemy inner bound and then
 *  take damage if collided.
 *
 * Same check for the boss attack collider, enemy attack collider, and trpas,
 * but without the need to check for inner bound.
 *
 * Same check for the collectable but on collision, check if full health. If
 * full health, shake the hearts to indicate full health. Add health and play
 * heart pick up sound.
 *
 * \param obj
 * Game object reference, which should be the player
 */
void player_collide(gameObject& obj)
{
	Player* player = dynamic_cast<Player*>(&obj);
	player->on_ground = false;
	vector<gameObject*>& colliders = obj.colliders;
	for (vector<gameObject*>::iterator i = colliders.begin(); i != colliders.end(); ++i)
	{
		gameObject& obj_col = **i;
		if ((obj_col.type == GO_TYPE::GO_PLATFORM || obj_col.type == GO_TYPE::GO_DESTRUCTABLE) && !player->on_ground)
		{
			AEVec2 e1{ 0, 1 };
			if (AEVec2DotProduct(&player->curr_vel, &e1) < 0)
			{
				adjust_player_movement(obj, obj_col);
			}
		}

		else if (obj_col.type == GO_TYPE::GO_WALL)
		{
			adjust_player_movement(obj, obj_col);
		}

		else if (obj_col.type == GO_TYPE::GO_MONSTER)
		{
			if (dynamic_cast<RangedEnemy*>(&obj_col))
				continue;

			if (dynamic_cast<MeleeEnemy2*>(&obj_col))
				continue;

			Enemy* enemy = dynamic_cast<Enemy*>(&obj_col);
			if (!player->is_invul && rtr_collide(player->bounding_box, player->curr_vel, enemy->innerBound, enemy->curr_vel))
			{
				AEVec2 push = { 200, 300 };
				AEVec2 player_currentpos = player->curr_pos;
				AEVec2 diff_pos = enemy->curr_pos;
				AEVec2Sub(&diff_pos, &player->curr_pos, &diff_pos);

				f32 player_width = (player->bounding_box.max.x - player->bounding_box.min.x) / 2;
				f32 player_hh = (player->bounding_box.max.y - player->bounding_box.min.y) / 2;
				f32 obj_halfscale_x = enemy->scale.x / 2;
				f32 obj_halfscale_y = enemy->scale.y / 2;

				f32 min_distx = player_width + obj_halfscale_x;
				f32 min_disty = player_hh + obj_halfscale_y;

				f32 depth_x = diff_pos.x > 0 ? min_distx - diff_pos.x : -min_distx - diff_pos.x;
				f32 depth_y = diff_pos.y > 0 ? min_disty - diff_pos.y : -min_disty - diff_pos.y;
				if (depth_x != 0 && depth_y != 0)
				{
					if (fabs(depth_x) < fabs(depth_y))
					{
						if (depth_x < 0)
							push.x = -push.x;
					}
				}
				if (player->shield_timer)
				{
					if (player->shield_timer > PARRY_TIME)
						player->take_damage((enemy->dmg + 2 - 1) / 2); // round up
				}
				else
				{
					player->take_damage(enemy->dmg);
					if (!player->superarmour)
					{
						player->on_ground = false;
						player->curr_attack = nullptr;
						player->acceleration = push;
						player->state = P_Anim_State::P_SWORD_FLINCH;
					}
				}
			}
		}
		else if (obj_col.type == GO_TYPE::GO_BOSSATTACKCOLLIDER)
		{
			if (!player->is_invul)
			{
				if (!player->on_ground)
				{
					player->curr_vel.y = 0;
				}
				Attack_Collider* collider = dynamic_cast<Attack_Collider*>(&obj_col);
				if (collider->damage)
				{
					if (player->shield_timer)
					{
						if (player->shield_timer > PARRY_TIME)
							player->take_damage((collider->damage + 2 - 1) / 2); // round up
					}
					else
					{
						player->take_damage(collider->damage);
						if (!player->superarmour)
						{
							player->on_ground = false;
							player->curr_attack = nullptr;
							player->acceleration = collider->flinch_scale;
							player->state = P_Anim_State::P_SWORD_FLINCH;
						}
					}
				}
			}
		}

		else if (obj_col.type == GO_TYPE::GO_ENEMYATTACKCOLLIDER)
		{
			if (!player->is_invul)
			{
				Attack_Collider* collider = dynamic_cast<Attack_Collider*>(&obj_col);
				player->take_damage(collider->damage);
				if (collider->owner)
				{
					collider->flag = false;
				}
			}
		}

		else if (obj_col.type == GO_TYPE::GO_COLLECTABLES)
		{
			if (player->health == PLAYER_HEALTH)
			{
				if (heart_shake_interval <= 0.0f)
				{
					heart_shake_timer = 0.5f;
					heart_shake_interval = 2.0f;
				}
				continue;
			}

			player->health = static_cast<s32>(AEClamp(static_cast<f32>(player->health + 2), 0, PLAYER_HEALTH));
			Heart::pick_up_sound->play_sound();
			obj_col.flag = false;
		}

		else if (obj_col.type == GO_TYPE::GO_TRAPS)
		{
			Traps* traps = dynamic_cast<Traps*>(&obj_col);

			if (traps->traps_type == TRAPS_TYPE::TRAPS_KILLZONE)
			{
				if (rtr_collide(player->bounding_box, player->curr_vel, traps->bounding_box, traps->curr_vel))
				{
					player->take_damage(30);
				}
			}
			if (traps->traps_type == TRAPS_TYPE::TRAPS_SPIKE || traps->traps_type == TRAPS_TYPE::TRAPS_FIRE || traps->traps_type == TRAPS_TYPE::TRAPS_ICICLES)
			{
				if (!player->is_invul && rtr_collide(player->bounding_box, player->curr_vel, traps->bounding_box, traps->curr_vel))
				{
					AEVec2 push = { 200, 300 };
					// TODO - STANDARDISE THIS INTO A FUNCTION FOR MODULARITY
					AEVec2 player_currentpos = player->curr_pos;
					AEVec2 diff_pos = traps->curr_pos;
					AEVec2Sub(&diff_pos, &player->curr_pos, &diff_pos);

					f32 player_width = (player->bounding_box.max.x - player->bounding_box.min.x) / 2;
					f32 player_hh = (player->bounding_box.max.y - player->bounding_box.min.y) / 2;
					f32 obj_halfscale_x = traps->scale.x / 2;
					f32 obj_halfscale_y = traps->scale.y / 2;

					f32 min_distx = player_width + obj_halfscale_x;
					f32 min_disty = player_hh + obj_halfscale_y;

					f32 depth_x = diff_pos.x > 0 ? min_distx - diff_pos.x : -min_distx - diff_pos.x;
					f32 depth_y = diff_pos.y > 0 ? min_disty - diff_pos.y : -min_disty - diff_pos.y;
					if (depth_x != 0 && depth_y != 0)
					{
						if (fabs(depth_x) < fabs(depth_y))
						{
							if (depth_x < 0)
								push.x = -push.x;
						}
					}
					if (player->shield_timer)
					{
						player->take_damage((traps->damage + 2 - 1) / 2); // round up
					}
					else
					{
						player->state = P_Anim_State::P_SWORD_FLINCH;
						player->take_damage(traps->damage);
						player->on_ground = false;
						player->curr_attack = nullptr;
						if (!player->superarmour)
							player->acceleration = push;
					}
				}
			}
		}
	}
}