/*!*************************************************************************
****
\file   enemy.cpp
\author Primary: Chua Yip Xuan					 (91.11%)
		Secondary: Siti Nursafinah Binte Sumsuri (8.89%)
\par    DP email: yipxuan.chua\@digipen.edu
				  sitinursafinah.b\@digipen.edu
\date   Mar 12 2021

\brief
	This source file outlines an implementation of a general enemy (slime), the
	melee enemy and the ranged enemy.

The source file has functions that consists of:
	- swap
	- start_invul
	- ranged_behavior
	- melee_behavior
	- invul
	- start_vul
	- vul
	- weak
	- set_aabb
	- game_object_load
	- game_object_initialize
	- game_object_update
	- game_object_draw
	- game_object_unload
	- take_damage
	- enemy_collide

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************/

#include "enemy.h"
#include "globals.h"
#include <iostream>
#include "attack.h"
#include "collectables.h"
#include "particlesystem.h"
#include "level_manager.h"

AEVec2 enemy_pos;
AEVec2 enemy_size;
AEVec2 enemy_vel;
static AEGfxVertexList* mesh_enemy = 0;
static AEGfxVertexList* pMeshHeart = 0;
static AEGfxTexture* pTex_ball;
static AEGfxTexture* pTex_temp;
static Sprite* healthbar;
static Sprite* healthFull;
static AEVec2 healthSize = { 80.0f, 15.0f };
static AEVec2 fullhealthSize = { 90.0f, 23.0f };
static Audio* slime_death_sound;
static Color bleed_tint = { 1.0f, 0.0f, 0.0f, 1.0f };
static Color default_tint = { 1.0f, 1.0f, 1.0f, 1.0f };

/*!*****************************************************************************
  \brief
	Enemy default constructor: default enemy type is a slime.
*******************************************************************************/
Enemy::Enemy() : gameObject(), dmg(1), flinch(), innerBound(), invul_timer(), is_invul(), collider{ nullptr }, curr_attack(nullptr),
cooldown_timer(), anim_state{ ENEMY_ANIM::DEFAULT }, debuff{}
{
	full_health = ENEMY_HEALTH;
	health = ENEMY_HEALTH;
	on_collide = enemy_collide;
	obj_index = 0;
	spr_index = 0;
	type = GO_TYPE::GO_MONSTER;
	scale = { ENEMY_SIZE, ENEMY_SIZE }; // TODO - READ FROM JSON FILE
	layer = ENEMY_LAYER;
	is_static = false;
	enemy_type = ENEMY_TYPE::ENEMY_SLIME;
	debuff.owner = this;
}

/*!*****************************************************************************
  \brief
	Parameterized enemy constructor.
*******************************************************************************/
Enemy::Enemy(s32 health) : Enemy()
{
	this->health = health;
	this->full_health = health;
}

/*!*****************************************************************************
  \brief
	Swap function for enemy game object with the object in parameters.
*******************************************************************************/
void Enemy::swap(Enemy& rhs)
{
	std::swap(health, rhs.health);
	std::swap(obj_index, rhs.obj_index);
	std::swap(spr_index, rhs.spr_index);
}

/*!*****************************************************************************
  \brief
	Enemy copy constructor.
*******************************************************************************/
Enemy::Enemy(const Enemy& rhs) : cooldown_timer{ rhs.cooldown_timer }, full_health{ rhs.full_health }, flinch(), innerBound(), invul_timer(), is_invul(), collider{ nullptr }, curr_attack(nullptr),
enemy_type(rhs.enemy_type), anim_state{ rhs.anim_state }, debuff{ rhs.debuff }
{
	health = rhs.health;
	obj_index = rhs.obj_index;
	spr_index = rhs.spr_index;
	dmg = rhs.dmg;
}

/*!*****************************************************************************
  \brief
	Parameterized enemy constructor.
*******************************************************************************/
Enemy::Enemy(s32 health, AEVec2 init_pos, AEVec2 scale, Sprite* spr) : Enemy()
{
	this->health = health;
	this->full_health = health;
	this->init_pos = init_pos;
	this->scale = scale;
	this->obj_sprite = spr;
}

/*!*****************************************************************************
  \brief
	Enemy assigmenet operator.
*******************************************************************************/
Enemy& Enemy::operator=(Enemy const& rhs)
{
	Enemy tmp{ rhs };
	swap(tmp);
	return *this;
}

/*!*****************************************************************************
  \brief
	Enemy destructor.
*******************************************************************************/
Enemy::~Enemy()
{
	animations.clear();
}

/*!*****************************************************************************
  \brief
	Enemy general behaviour.
*******************************************************************************/
void Enemy::behaviour(gameObject& gameObj)
{
	if (on_ground)
	{
		if (gameObj.curr_pos.x > curr_pos.x && AEVec2Distance(&curr_pos, &Level::player->curr_pos) >= 50.0f) //if player pos is more than monster pos then
		{
			obj_sprite->flip_x = true;
			acceleration.x += 100 * G_DELTATIME * FPS; //move right
		}
		else if (gameObj.curr_pos.x < curr_pos.x && AEVec2Distance(&curr_pos, &Level::player->curr_pos) >= 50.0f)
		{
			obj_sprite->flip_x = false;
			acceleration.x += -100 * G_DELTATIME * FPS; // move left
		}
	}
}

/*!*****************************************************************************
  \brief
	General aabb bounding box for enemy.
*******************************************************************************/
void Enemy::set_aabb()
{
	AEVec2Set(&innerBound.min, -0.5f * scale.x, -0.5f * scale.y);
	AEVec2Set(&innerBound.max, 0.5f * scale.x, 0.5f * scale.y);
	AEVec2Add(&innerBound.min, &innerBound.min, &curr_pos);
	AEVec2Add(&innerBound.max, &innerBound.max, &curr_pos);

	AEVec2Set(&bounding_box.min, -4.0f * scale.x, -2.0f * scale.y);
	AEVec2Set(&bounding_box.max, 4.0f * scale.x, 2.0f * scale.y);
	AEVec2Add(&bounding_box.min, &bounding_box.min, &curr_pos);
	AEVec2Add(&bounding_box.max, &bounding_box.max, &curr_pos);
}

/*!*****************************************************************************
  \brief
	Ranged enemy bounding box.
*******************************************************************************/
void RangedEnemy::set_aabb()
{
	AEVec2Set(&innerBound.min, -0.5f * scale.x, -0.5f * scale.y);
	AEVec2Set(&innerBound.max, 0.5f * scale.x, 0.5f * scale.y);
	AEVec2Add(&innerBound.min, &innerBound.min, &curr_pos);
	AEVec2Add(&innerBound.max, &innerBound.max, &curr_pos);

	AEVec2Set(&bounding_box.min, -4.0f * scale.x * range, -1.0f * scale.y * range);
	AEVec2Set(&bounding_box.max, 4.0f * scale.x * range, 1.0f * scale.y * range);
	AEVec2Add(&bounding_box.min, &bounding_box.min, &curr_pos);
	AEVec2Add(&bounding_box.max, &bounding_box.max, &curr_pos);
}

/*!*****************************************************************************
  \brief
	This function loads all the assets needed for enemy.
*******************************************************************************/
void load_enemy_assets()
{
	AEGfxMeshStart();
	AEGfxTriAdd( // 0,0
		0.0f, 0.0f, 0xFFFF0000, 0.0f, 0.0f,
		1.0f, -1.0f, 0xFFFF0000, 1.0f, 1.0f,
		1.0f, 0.0f, 0xFFFF0000, 1.0f, 0.0f);

	AEGfxTriAdd(
		0.0f, 0.0f, 0xFFFF0000, 0.0f, 0.0f,
		0.0f, -1.0f, 0xFFFF0000, 0.0f, 1.0f,
		1.0f, -1.0f, 0xFFFF0000, 1.0f, 1.0f);

	pMeshHeart = AEGfxMeshEnd();
	AE_ASSERT_MESG(pMeshHeart, "Failed to create mesh heart!!");

	healthbar = new Sprite(nullptr, pMeshHeart, healthSize);
	slime_death_sound = new Audio("./Audio/slime_death.wav", AUDIO_GROUP::SFX);

	healthFull = new Sprite(nullptr, pMeshHeart, healthSize);

	pTex_ball = AEGfxTextureLoad("./Sprites/enemy_placeholder_bullet.png");
	AE_ASSERT_MESG(pTex_ball, "Failed to create attack collider texture!");

	pTex_temp = AEGfxTextureLoad("./Sprites/enemy_atkcol.png");
	AE_ASSERT_MESG(pTex_temp, "Failed to create attack collider pTex_temp!");
}

/*!*****************************************************************************
  \brief
	This function loads all the assets for the class enemy
*******************************************************************************/
void Enemy::game_object_load()
{
	Animation slime_ani = Animation(3, { 0.0f, 0.0f }, obj_sprite, 205, 0.1f);
	Animation melee_ani = Animation(4, { 00.0f, 0.0f }, obj_sprite, 302, 0.3f);
	if (this->enemy_type == ENEMY_TYPE::ENEMY_SLIME)
	{
		animations.push_back(slime_ani);
	}
	if (this->enemy_type == ENEMY_TYPE::ENEMY_SKELETON_MELEE)
	{
		animations.push_back(melee_ani);
	}
}

/*!*****************************************************************************
  \brief
	This function initializes the enemy.
*******************************************************************************/
void Enemy::game_object_initialize()
{
	curr_pos = init_pos;
	collider = dynamic_cast<Attack_Collider*>(push_object(GO_TYPE::GO_ENEMYATTACKCOLLIDER, this));
	enemy_attack_init(*this);
}

/*!*****************************************************************************
  \brief
	This function updates the enemy variables.
*******************************************************************************/
void Enemy::game_object_update()
{
	debuff.update();
	if (cooldown_timer > 0) cooldown_timer -= G_DELTATIME;
	if (health <= 0)
	{
		bounding_box.min = { 0,0 };
		bounding_box.max = { 0,0 };
		particleSystem* pSystem = create_damaged_particles(this);
		pSystem->particle_increment = 15;
		pSystem->direction_range[0] = 0;
		pSystem->direction_range[1] = 2.0f * PI;
		pSystem->game_object_update();
		Camera::shake(10, 0.1f);
		flag = false;
		slime_death_sound->play_sound();
		remove_attack_collider(this);
		spawn_heart(*this);
	}
	acceleration.y += LEVEL_GRAVITY;
	AEVec2Add(&curr_vel, &curr_vel, &acceleration);
	AEVec2Zero(&acceleration);

	AEVec2 dt_vel;
	AEVec2Scale(&dt_vel, &curr_vel, G_DELTATIME);
	AEVec2Add(&curr_pos, &curr_pos, &dt_vel);

	if (on_ground)
	{
		curr_vel.x = 0.0f;
	}
	if (anim_state == ENEMY_ANIM::WALK)
		anim_state = ENEMY_ANIM::DEFAULT;
	invul();
}

/*!*****************************************************************************
  \brief
	This function draws the enemy and healthbar.
*******************************************************************************/
void Enemy::game_object_draw()
{
	if (health > 0)
	{
		full_bar();
		f_healths();
	}
	obj_sprite->tint = debuff.activated ? bleed_tint : default_tint;
	if (this->enemy_type == ENEMY_TYPE::ENEMY_SLIME)
	{
		animations[0].play_animation();
	}
	draw(obj_sprite, curr_pos, scale);
}

/*!*****************************************************************************
  \brief
	This function frees all the assets used to implement the enemy.
*******************************************************************************/
void Enemy::game_object_free()
{
	ignorecolliders.clear();
	ignorecolliders.~vector();
}

/*!*****************************************************************************
  \brief
	This function unloads all the assets used to implement the enemy.
*******************************************************************************/
void Enemy::game_object_unload()
{
	animations.clear();
}

/*!*****************************************************************************
  \brief
	This function unloads all the assets used to implement the enemy.
*******************************************************************************/
void unload_enemy_assets()
{
	AEGfxTextureUnload(pTex_ball);
	AEGfxTextureUnload(pTex_temp);
	AEGfxMeshFree(pMeshHeart);
	delete healthFull;
	delete healthbar;
	delete slime_death_sound;
}

/*!*****************************************************************************
  \brief
	This function adjust the enemy movement for collision and checks for empty
	platforms.
*******************************************************************************/
void adjust_enemy_movement(gameObject& e_obj, const gameObject& obj)
{
	Enemy* enemy = dynamic_cast<Enemy*>(&e_obj);
	AEVec2 diff_pos = obj.curr_pos;
	AEVec2Sub(&diff_pos, &enemy->curr_pos, &diff_pos);

	f32 enemy_width = (enemy->innerBound.max.x - enemy->innerBound.min.x) / 2;
	f32 enemy_hh = (enemy->innerBound.max.y - enemy->innerBound.min.y) / 2;
	f32 obj_halfscale_x = (obj.bounding_box.max.x - obj.bounding_box.min.x) / 2;
	f32 obj_halfscale_y = (obj.bounding_box.max.y - obj.bounding_box.min.y) / 2;

	f32 min_distx = enemy_width + obj_halfscale_x;
	f32 min_disty = enemy_hh + obj_halfscale_y;
	f32 depth_x = min_distx - fabs(diff_pos.x);
	f32 depth_y = min_disty - fabs(diff_pos.y);

	if (depth_x != 0 && depth_y != 0)
	{
		if (obj.type == GO_TYPE::GO_WALL && fabs(depth_x) < fabs(depth_y))
		{
			enemy->curr_pos.x += diff_pos.x < 0 ? -depth_x : +depth_x;
		}

		if (fabs(depth_y) < fabs(depth_x))
		{
			if (diff_pos.y > 0)
			{
				enemy->curr_pos.y += depth_y;
				enemy->curr_vel.y = 0;
				enemy->on_ground = true;
			}
		}
	}
	else if (!depth_y && diff_pos.y > 0)
	{
		enemy->curr_vel.y = 0;
		enemy->on_ground = true;
	}
}

/*!*****************************************************************************
  \brief
	This function starts the invulnerable state of the enemy.
*******************************************************************************/
void Enemy::start_invul()
{
	is_invul = true;
}

/*!*****************************************************************************
  \brief
	This function sets the vulnerability behaviour of the enemy.
*******************************************************************************/
void Enemy::invul()
{
	if (is_invul)
	{
		if (invul_timer < 0.7f)
		{
			invul_timer += G_DELTATIME;
		}
		else
		{
			is_invul = !is_invul;
			invul_timer = 0;
		}
	}
}

/*!*****************************************************************************
  \brief
	If enemy takes damage, this function will reduce enemy health according to
	the damage taken.
*******************************************************************************/
void Enemy::take_damage(s32 damage)
{
	health -= damage;
	anim_state = ENEMY_ANIM::HURT;
	particleSystem* pParticleSystem = create_damaged_particles(this);
	pParticleSystem->color_range[0] = pParticleSystem->color_range[1] = { 0.0f, 1.0f, 0.2f, 1.0f };
}

/*!*****************************************************************************
  \brief
	This function is the on collide function for the enemy, for different
	collision of game object types
*******************************************************************************/
void enemy_collide(gameObject& obj)
{
	Enemy* enemy = dynamic_cast<Enemy*>(&obj);
	vector<gameObject*>& colliders = obj.colliders;
	for (vector<gameObject*>::iterator i = colliders.begin(); i != colliders.end(); ++i)
	{
		gameObject& obj_col = **i;
		if (obj_col.type == GO_TYPE::GO_PLATFORM
			&& rtr_collide(obj_col.bounding_box, obj_col.curr_vel, enemy->innerBound, enemy->curr_vel))
		{
			AEVec2 e1{ 0,1 };
			if (AEVec2DotProduct(&enemy->curr_vel, &e1) < 0)
			{
				adjust_enemy_movement(obj, obj_col);
			}
		}
		else if (obj_col.type == GO_TYPE::GO_WALL
			&& rtr_collide(obj_col.bounding_box, obj_col.curr_vel, enemy->innerBound, enemy->curr_vel))
		{
			adjust_enemy_movement(obj, obj_col);
		}

		if (obj_col.type == GO_TYPE::GO_PLAYER)
		{
			enemy->behaviour(obj_col);
		}

		if (obj_col.type == GO_TYPE::GO_PLAYERATTACKCOLLIDER)
		{
			Attack_Collider* attack_collider = dynamic_cast<Attack_Collider*>(&obj_col);
			if (!enemy->is_invul && rtr_collide(enemy->innerBound, enemy->curr_vel, attack_collider->bounding_box, attack_collider->curr_vel))
			{
				bool ignore{ false };
				for (gameObject* x : enemy->ignorecolliders)
				{
					if (x == &obj_col)
					{
						ignore = true;
						break;
					}
				}
				if (ignore) continue;

				AEVec2 push = attack_collider->flinch_scale;
				push.x = enemy->curr_pos.x >= attack_collider->owner->curr_pos.x ? push.x : -push.x;
				enemy->take_damage(attack_collider->damage);
				enemy->ignorecolliders.push_back(attack_collider);
				enemy->on_ground = false;
				AEVec2Add(&enemy->acceleration, &enemy->acceleration, &push);
			}
		}

		if (obj_col.type == GO_TYPE::GO_TRAPS)
		{
			Traps* trap = dynamic_cast<Traps*>(&obj_col);
			if (rtr_collide(enemy->innerBound, enemy->curr_vel, trap->bounding_box, trap->curr_vel) && trap->traps_type == TRAPS_TYPE::TRAPS_KILLZONE)
			{
				enemy->flag = false;
			}
		}
	}
}

/*!*****************************************************************************
  \brief
	This is the melee enemy default constructor. (slime)
*******************************************************************************/
MeleeEnemy1::MeleeEnemy1() : Enemy()
{
	enemy_type = ENEMY_TYPE::ENEMY_SLIME;
}

/*!*****************************************************************************
  \brief
	This is the melee enemy parameterized constructor. (slime)
*******************************************************************************/
MeleeEnemy1::MeleeEnemy1(s32 health, AEVec2 init_pos, AEVec2 scale, Sprite* sprite)
	: Enemy(health, init_pos, scale, sprite)
{
	enemy_type = ENEMY_TYPE::ENEMY_SLIME;
}

/*!*****************************************************************************
  \brief
	This is the melee enemy default constructor. (melee skeleton)
*******************************************************************************/
MeleeEnemy2::MeleeEnemy2() : Enemy()
{
	enemy_type = ENEMY_TYPE::ENEMY_SKELETON_MELEE;
}

/*!*****************************************************************************
  \brief
	This is the melee enemy parameterized constructor. (melee skeleton)
*******************************************************************************/
MeleeEnemy2::MeleeEnemy2(s32 health, AEVec2 init_pos, AEVec2 scale, Sprite* sprite)
	: Enemy(health, init_pos, scale, sprite)
{
	enemy_type = ENEMY_TYPE::ENEMY_SKELETON_MELEE;
}

/*!*****************************************************************************
  \brief
	This function overrrides the normal behaviour of the enemy, this function
	implements the behaviour of the melee enemy.
*******************************************************************************/
void MeleeEnemy2::behaviour(gameObject& gameObj)
{
	collider->obj_sprite->texture = pTex_temp;
	if (on_ground)
	{
		if (gameObj.curr_pos.x > curr_pos.x) //if player pos is more than monster pos then
		{
			obj_sprite->flip_x = true;
			if (AEVec2Distance(&curr_pos, &Level::player->curr_pos) >= 100.0f)
			{
				acceleration.x += 100 * G_DELTATIME * FPS;
				anim_state = ENEMY_ANIM::WALK;
			}
			if (cooldown_timer <= 0)
			{
				curr_attack->attack(*this);
			}
			else if (cooldown_timer < 2.7f)
			{
				collider->flag = false;
			}
		}
		else
		{
			obj_sprite->flip_x = false;
			if (AEVec2Distance(&curr_pos, &Level::player->curr_pos) >= 100.0f)
			{
				acceleration.x -= 100 * G_DELTATIME * FPS;
				anim_state = ENEMY_ANIM::WALK;
			}

			if (cooldown_timer <= 0)
			{
				curr_attack->attack(*this);
			}
			else if (cooldown_timer < 2.7f)
			{
				collider->flag = false;
			}
		}
	}
}

/*!*****************************************************************************
  \brief
	Function is to make the enemy shoot out a ball that has damage
*******************************************************************************/
void RangedEnemy::behaviour(gameObject& gameObj)
{
	collider->obj_sprite->texture = pTex_ball;
	if (on_ground)
	{
		if (cooldown_timer <= 0)
		{
			curr_attack->attack(*this);
		}
	}
}
/*!*****************************************************************************
  \brief
	Function is to draw the total health bar of the enemy
*******************************************************************************/
void Enemy::full_bar()
{
	AEVec2 coordinatesFull = { curr_pos.x - healthSize.x / 2 - 5, curr_pos.y + scale.y / 2 + 18.5f };

	draw(0, healthFull->mesh, coordinatesFull, { 0,0 }, { 0.0f, 1.0f, 1.0f,1.0f }, 1.0f, { fullhealthSize }, 0);
}
/*!*****************************************************************************
  \brief
	Function is to draw the corresponding health length of the enemy
*******************************************************************************/
void Enemy::f_healths()
{
	AEVec2 coordinates = { curr_pos.x - healthSize.x / 2, curr_pos.y + scale.y / 2 + 15 };

	f32 current = static_cast<f32> (health) / static_cast<f32> (full_health) * healthSize.x;

	draw(0, healthbar->mesh, coordinates, { 0,0 }, { 1.0f, 0.0f, 0.0f,1.0f }, 1.0f, { current, healthSize.y }, 0);
}