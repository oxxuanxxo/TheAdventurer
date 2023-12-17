/*!*****************************************************************************
\file   attack.h
\author Mohammad Hanif Koh Teck Wee
\par    DP email: mohammadhanifkoh.t\@digipen.edu
\date   Feb 21 2021

\brief
This header file outlines the Attack class, the subclasses for Player, Boss and
Enemy Attacks, and also the implementation of all attacks.

The functions include:
- load functions for sword, spear and shield attacks
- init functions for sword, spear and shield attacks
- unload functions for sword, spear and shield attacks
- push_object
- get_attack_collider
- remove_attack_collider
- 8 sword attack functions
- 9 spear attack functions
- load functions for boss 1, 2 and 3 attacks
- init functions for boss 1, 2 and 3 attacks
- unload functions for boss 1, 2 and 3 attacks
- load function for enemy attacks
- init function for enemy attacks
- unload function for enemy attacks
- crate_collide
- shield_collide

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************************/
#pragma once
#include "main.h"
#include "gameobject.h"

class Attack
{
public:
	Attack(Attack const& rhs) = default;
	Attack(f32 leeway_timer, u32 anim_index, s32 damage, fp attack, AEVec2 flinch_scale);
public:
	bool setup;
	f32 leeway_timer;
	u32 anim_index;
	s32 damage;
	fp attack;
	AEVec2 flinch_scale;
};

class Player_Attack : public Attack
{
public:
	Player_Attack(Player_Attack const& rhs) = default;
	Player_Attack(P_Anim_State state, f32 leeway_timer, f32 min_duration, u32 anim_index,
		s32 damage, fp attack, P_Anim_State z, P_Anim_State x, AEVec2 flinch_scale);
	bool operator==(Player_Attack const&);
public:
	f32 min_duration;
	P_Anim_State state;
	P_Anim_State next_z;
	P_Anim_State next_x;
};

class Enemy_Attack : public Attack
{
public:
	Enemy_Attack(f32 leeway_timer, u32 anim_index, s32 damage, fp attack, AEVec2 flinch_scale);
};

class Boss_Attack : public Attack
{
public:
	Boss_Attack(f32 leeway_timer, u32 anim_index,
		s32 damage, fp attack, f32 weak_duration, f32 cooldown_timer, AEVec2 flinch_scale);
public:
	f32 weak_duration;
	f32 cooldown_timer;
};

class Attack_Collider : public gameObject
{
public:
	Attack_Collider(AEVec2 pos, Sprite* sprite, u32 layer,
		AEVec2 scale, GO_TYPE type, bool is_static);
	void set_aabb() override;
public:
	gameObject* owner;
	s32 damage;
	AEVec2 flinch_scale;
};

class Shield : public Attack_Collider
{
};

void sword_load();
void spear_load();
void shield_load();
void attacks_load();
void sword_init(vector<Player_Attack>&);
void spear_init(vector<Player_Attack>&);
void shield_init(vector<Player_Attack>&);
void attacks_init(vector<Player_Attack>&, vector<Player_Attack>&);
void sword_unload();
void spear_unload();
void shield_unload();
void attacks_unload();

gameObject* push_object(GO_TYPE type, gameObject* owner);
Attack_Collider* get_attack_collider(gameObject* owner);
void remove_attack_collider(gameObject* owner);

void sword_attack_1(gameObject&);
void sword_attack_2(gameObject&);
void sword_attack_3(gameObject&);
void sword_attack_4(gameObject&);
void sword_attack_5(gameObject&);
void sword_attack_6(gameObject&);
void sword_jump_attack(gameObject&);
void sword_plunge(gameObject&);

void spear_attack_1(gameObject&);
void spear_attack_2(gameObject&);
void spear_attack_3(gameObject&);
void spear_attack_4(gameObject&);
void spear_attack_5(gameObject&);
void spear_dodge(gameObject&);
void spear_jump_attack(gameObject&);
void spear_plunge(gameObject&);
void spear_throw(gameObject&);

void boss1_attacks_load();
void boss1_attacks_init(vector<Boss_Attack>&);
void boss1_attacks_unload();

void boss2_attacks_load();
void boss2_attacks_init(vector<Boss_Attack>&);
void boss2_attacks_unload();

void boss3_attacks_load();
void boss3_attacks_init(vector<Boss_Attack>&);
void boss3_attacks_unload();

void enemyattack_load();
void enemy_attack_init(gameObject&);
void enemyattack_unload();

void crate_collide(gameObject&);
void shield_collide(gameObject&);