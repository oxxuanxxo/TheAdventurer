/*!*****************************************************************************
\file   boss.h
\author Primary: Mohammad Hanif Koh Teck Wee	 (96.94%)
		Secondary: Siti Nursafinah Binte Sumsuri (3.06%)
\par    DP email: mohammadhanifkoh.t\@digipen.edu
				  sitinursafinah.b\@digipen.edu
\date   Mar 4 2021

\brief
This header file outlines the Boss data structures, as well as the
implementation of the bosses.

The functions include:
	- start_invul
	- invul
	- start_vul
	- vul
	- weak
	- f_health
	- full_bar
	- set_aabb for Boss 1, 2 and 3
	- game_object_load for Boss 1, 2 and 3
	- game_object_initialize
	- game_object_update
	- game_object_draw
	- game_object_unload
	- take_damage
	- boss_collide
	- attack
	- set_attack
	- set_sword_positions
	- load_boss_assets
	- unload_boss_assets

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************************/
#pragma once

#include "gameobject.h"
#include "status_effect.h"
#include "attack.h"
#include "camera.h"
#include "player.h"
#include "traps.h"

constexpr int NUM_SWORDS = 3;

class Boss : public gameObject
{
protected:
	void start_invul(f32 timer);
	void invul();
	void start_vul(f32 timer);
	void vul();
	void weak();
public:
	Boss();
	virtual void set_attack() = 0;
	void attack();
	void f_health();
	void full_bar();
	virtual void take_damage(s32);

public:
	s32 full_health;
	s32 health;
	s32 attack_phase;
	bool setup;
	bool is_invul;
	bool is_vul;
	bool is_weak;
	bool is_attacking;
	bool is_facing_right;
	f32 invul_timer;
	f32 vul_timer;
	f32 weak_timer;
	f32 cooldown_timer;
	f32 attack_timer;
	f32 flinch_resistance;
	Effect debuff;
	Boss_Attack* curr_Attack;
	vector<Boss_Attack> boss_attacks;
};

class Boss1 :public Boss
{
public:
	Boss1();
	void set_aabb() override;
	void game_object_load() override;
	void game_object_initialize() override;
	void game_object_update() override;
	void game_object_draw() override;
	void game_object_free() override;
	void game_object_unload() override;
	void set_attack() override;
};

class Boss2 : public Boss
{
public:
	AABB outerBound;
	gameObject* plats[7];
	Traps* lava_traps[10];
	Attack_Collider* atkcol_fireballs[10];
	Attack_Collider* atkcol_roots[2];
public:
	Boss2();
	void set_aabb() override;
	void game_object_load() override;
	void game_object_initialize() override;
	void game_object_update() override;
	void game_object_draw() override;
	void game_object_free() override;
	void game_object_unload() override;
	void set_attack() override;
};

class Boss3 :public Boss
{
public:
	Boss3();
	void set_aabb() override;
	void game_object_load() override;
	void game_object_initialize() override;
	void game_object_update() override;
	void game_object_draw() override;
	void game_object_free() override;
	void game_object_unload() override;
	void set_attack() override;
	void take_damage(s32) override;
public:
	bool enraged;
	s32 anim_state;
	Attack_Collider* swords[NUM_SWORDS];
};

void boss_collide(gameObject& obj);
void get_Player(Player*);
void set_sword_positions(Boss3& boss);
void load_boss_assets();
void unload_boss_assets();