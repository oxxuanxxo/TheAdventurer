/*!*****************************************************************************
\file   player.h
\author Cruz Rolly Matthew Capiral, Mohammad Hanif Koh Teck Wee, Siti
		Nursafinah Binte Sumsuri
\par    DP email: cruzrolly.m\@digipen.edu, mohammadhanifkoh.t@digipen.edu,
		sitinursafinah.b@digipen.edu
\date   Mar 12 2021

\brief
This header file outlines an implementation of the player object in the game
engine

The functions include:
- h_healths
- change_weapon
- combo
- move
- start_invul
- invul
- swap
- set_aabb
- Draw_Obj
- game_object_load
- game_object_initialize
- game_object_update
- game_object_draw
- game_object_unload
- take_damage
- player_collide
- hearts
- start_invul
- take_damage
- setup_attack

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************************/
#pragma once
#include "main.h"
#include "gameobject.h"
#include "AEEngine.h"
#include "timer.h"
#include "attack.h"

constexpr u32 PLAYER_LAYER = 4;
constexpr s32 PLAYER_HEALTH = 20;
constexpr f32 PLAYER_SIZE = 125.0f;

class Player : public gameObject
{
private:
	void h_healths(); // saf
	void change_weapon(); // mat
	void combo(); // 75 hanif, 25 mat
	void move(); // hanif
	void invul(); // mat
	void swap(Player& rhs); // mat

public:
	u8 sword_upgrade;
	u8 spear_upgrade;
	s32 health;
	s32 full_health;
	bool slide;
	bool curr_weapon;
	bool airdashed;
	bool is_facing_right;
	bool attack_setup;
	bool is_invul;
	bool flinch;
	bool parry;
	bool superarmour;
	f32 invul_timer;
	u32 obj_index;
	u32 spr_index;
	f32 attack_timer;
	f32 shield_timer;
	map<P_Anim_State, Animation> animations;
	vector<Player_Attack> sword_attacks;
	vector<Player_Attack> spear_attacks;
	Player_Attack* curr_attack;
	Player(); // matt
	Player(s32 health); // matt
	Player(AEVec2 pos); // matt
	Player(const Player&); // matt
	Player& operator=(Player const&); // matt
	~Player(); // matt
	void set_aabb() override; // matt
	void game_object_load() override; // matt
	void game_object_initialize() override; // matt
	void game_object_update() override; // FFA
	void game_object_draw() override; // matt
	void game_object_free() override; // matt
	void game_object_unload() override; // matt
	void start_invul(); // matt
	void take_damage(s32); // matt
	void setup_attack(Player_Attack* pAtk); // hanif
};

void player_collide(gameObject& obj); // 50 hanif 50 matt
bool hearts(Player const& p); // saf