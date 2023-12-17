/*!*****************************************************************************
\file   enemy.h
\author Chua Yip Xuan
\par    DP email: yipxuan.chua\@digipen.edu
\date   Mar 12 2021

\brief
	This header file outlines an implementation of a general enemy (slime), the 
	melee enemy and the ranged enemy.

The header file has functions that consists of:
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
*******************************************************************************/
#pragma once
#include "main.h"
#include "gameobject.h"
#include "attack.h"
#include "status_effect.h"

constexpr u32 ENEMY_LAYER = 3;
constexpr s32 ENEMY_HEALTH = 10;
constexpr s32 FPS = 60;
constexpr f32 ENEMY_SIZE = 75.0f;

enum class ENEMY_TYPE
{
	ENEMY_SLIME,
	ENEMY_SKELETON_MELEE,
	ENEMY_SKELETON_RANGED
};

enum class ENEMY_ANIM
{
	DEFAULT,
	ATTACK,
	WALK,
	HURT
};

class Enemy : public gameObject {
private:
	f32 invul_timer;
	void swap(Enemy&);
	void start_invul();
	void invul();
public:
	f32 cooldown_timer;
	s32 full_health;
	s32 health;
	u32 obj_index;
	u32 spr_index;
	s32 dmg; // touch damage: TODO - Move this to monster attack
	bool is_invul;
	bool flinch;
	AABB innerBound;
	Attack_Collider* collider;
	Enemy_Attack* curr_attack;
	ENEMY_TYPE enemy_type;
	ENEMY_ANIM anim_state;
	Effect debuff;
	
	Enemy();
	Enemy(s32 health);
	Enemy(const Enemy&);
	Enemy(s32 health, AEVec2 init_pos, AEVec2 scale, Sprite* spr);
	Enemy& operator=(Enemy const&);
	~Enemy();

	virtual void behaviour(gameObject&);
	void f_healths();
	void full_bar();
	void set_aabb() override;
	void take_damage(s32);

	virtual void game_object_load() override;
	virtual void game_object_initialize() override;
	virtual void game_object_update() override;
	virtual void game_object_draw() override;
	virtual void game_object_free() override;
	virtual void game_object_unload() override;
};

class RangedEnemy : public Enemy
{
	f32 range;
public:
	RangedEnemy();
	RangedEnemy(s32 health, AEVec2 init_pos, AEVec2 scale, Sprite* spr, f32 range = 1.0f);
	void set_aabb() override;
	void behaviour(gameObject& gameObj) override;

	virtual void game_object_load() override;
	virtual void game_object_draw() override;
};

class MeleeEnemy1 : public Enemy
{
public:
	MeleeEnemy1();
	MeleeEnemy1(s32 health, AEVec2 init_pos, AEVec2 scale, Sprite* sprite);
};

class MeleeEnemy2 : public Enemy
{
public:
	MeleeEnemy2();
	MeleeEnemy2(s32 health, AEVec2 init_pos, AEVec2 scale, Sprite* sprite);
	void behaviour(gameObject& gameObj) override;

	virtual void game_object_load() override;
	virtual void game_object_draw() override;
};

/*!*****************************************************************************
  \brief
	This function is the on collide function for the enemy, for different
	collision of game object types
*******************************************************************************/
void enemy_collide(gameObject& obj);
/*!*****************************************************************************
  \brief
	This function loads all the assets needed for enemy.
*******************************************************************************/
void load_enemy_assets();
/*!*****************************************************************************
  \brief
	This function unloads all the assets used to implement the enemy.
*******************************************************************************/
void unload_enemy_assets();