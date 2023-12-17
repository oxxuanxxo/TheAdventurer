/*!*************************************************************************
****
\file   game_enums.h
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Mar 21 2021

\brief
This header file outlines an enumeration classes required in the game.

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/
#pragma once

enum class GO_TYPE {
	GO_PLAYER,
	GO_MONSTER,
	GO_BOSS,
	GO_UI,
	GO_PLATFORM,
	GO_WALL,
	GO_DESTRUCTABLE,
	GO_COLLECTABLES,
	GO_PROJECTILE,
	GO_PLAYERATTACKCOLLIDER,
	GO_ENEMYATTACKCOLLIDER,
	GO_BOSSATTACKCOLLIDER,
	GO_BOSS_SPAWNER,
	GO_TRAPS,
	GO_PARTICLE,
	GO_PARTICLESYSTEM,
	GO_ATTACKINDICATOR
};

enum class STATUS_TYPE
{
	BLEED
};

enum class P_Anim_State : char
{
	P_NONE,
	P_SWORD_IDLE,
	P_SWORD_RUN,
	P_SWORD_JUMP,
	P_SWORD_AIRDASH,
	P_SWORD_FLINCH,
	P_SPEAR_IDLE,
	P_SPEAR_RUN,
	P_SPEAR_JUMP,
	P_SPEAR_AIRDASH,
	P_SPEAR_FLINCH,
	P_SWORD_1_1,
	P_SWORD_1_2,
	P_SWORD_1_3,
	P_SWORD_1_4,
	P_SWORD_2_1,
	P_SWORD_2_2,
	P_SWORD_2_3,
	P_SWORD_2_4,
	P_SWORD_2_5,
	P_SWORD_2_6,
	P_SWORD_JUMP_ATK,
	P_SWORD_PLUNGE,
	P_SHIELD_1,
	P_SHIELD_2,
	P_SPEAR_1_1,
	P_SPEAR_1_2,
	P_SPEAR_1_3,
	P_SPEAR_1_4,
	P_SPEAR_1_5,
	P_SPEAR_2_1,
	P_SPEAR_2_2,
	P_SPEAR_2_3,
	P_SPEAR_2_4,
	P_SPEAR_2_5,
	P_SPEAR_3_1,
	P_SPEAR_3_2,
	P_SPEAR_JUMP_ATK,
	P_SPEAR_PLUNGE
};