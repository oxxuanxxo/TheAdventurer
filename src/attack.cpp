/*!*****************************************************************************
\file   attack.cpp
\author Mohammad Hanif Koh Teck Wee
\par    DP email: mohammadhanifkoh.t\@digipen.edu
\date   Feb 21 2021

\brief
This file contains the implementation of the shared functions used across many
of the different types of attacks as outlined in attack.h

The functions include:
- Attack::Attack
- Player_Attack::Player_Attack
- Boss_Attack::Boss_Attack
- Attack_Collider::Attack_Collider
- Player_Attack::operator==
- attacks_load
- attacks_init
- push_object
- get_attack_collider
- remove_attack_collider
- attacks_unload
- Attack_Collider::set_aabb

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************************/
#include "attack.h"
#include "player.h"
#include "graphics.h"
#include "camera.h"
#include "boss.h"
#include "traps.h"
#include "level_manager.h"

static AEGfxVertexList* pMesh_attackcollider;
static Camera* level_cam;

/*!*****************************************************************************
  \brief
	Constructor for the Attack class

  \param leeway_timer
	The leeway timer of the attack, depicting the duration of the attacks, or in
	the player's case, how long the player has to input the next attack of the
	combo.

  \param anim_index
	The index of the gameobject's animations array that contains the specific
	animation for the attack.

  \param damage
	The damage the attack should do

  \param attack
	The function pointer to the function corresponding to the attack

  \param flinch_scale
	The flinch factor of the attack.
*******************************************************************************/
Attack::Attack(f32 leeway_timer, u32 anim_index, s32 damage, fp attack, AEVec2 flinch_scale)
	: setup{ false },
	leeway_timer{ leeway_timer },
	anim_index{ anim_index },
	damage{ damage },
	attack{ attack },
	flinch_scale{ flinch_scale }{};

/*!*****************************************************************************
  \brief
	Constructor for the Player_Attack class

  \param state
	The state of the player when using the attack

  \param leeway_timer
	How long the player has to input the next attack of the combo.

  \param min_duration
	How long the player must wait minimally before being able to input the next
	attack of the combo.

  \param anim_index
	The index of the gameobject's animations array that contains the specific
	animation for the attack.

  \param damage
	The damage the attack should do

  \param attack
	The function pointer to the function corresponding to the attack

  \param z
	The state the player will be in when he uses the "next z attack". Used
	to search for which attack will be used next in the combo.

  \param x
	The state the player will be in when he uses the "next x attack". Used
	to search for which attack will be used next in the combo.

  \param flinch_scale
	The flinch factor of the attack.
*******************************************************************************/
Player_Attack::Player_Attack(P_Anim_State state, f32 leeway_timer, f32 min_duration, u32 anim_index,
	s32 damage, fp attack, P_Anim_State z, P_Anim_State x, AEVec2 flinch_scale)
	: Attack{ leeway_timer, anim_index, damage, attack, flinch_scale },
	state{ state },
	min_duration{ min_duration },
	next_z{ z },
	next_x{ x }{};

/*!*****************************************************************************
  \brief
	Constructor for the Boss_Attack class

  \param leeway_timer
	The leeway timer of the attack, depicting the duration of the attacks, or in
	the player's case, how long the player has to input the next attack of the
	combo.

  \param anim_index
	The index of the gameobject's animations array that contains the specific
	animation for the attack.

  \param damage
	The damage the attack should do

  \param attack
	The function pointer to the function corresponding to the attack

  \param weak_duration
	How long the Boss will be weak after using the attack.

  \param cooldown_timer
	How long the Boss must wait before he can attack again after using this
	attack

  \param flinch_scale
	The flinch factor of the attack.
*******************************************************************************/
Boss_Attack::Boss_Attack(f32 leeway_timer, u32 anim_index,
	s32 damage, fp attack, f32 weak_duration, f32 cooldown_timer, AEVec2 flinch_scale)
	: Attack{ leeway_timer, anim_index, damage, attack, flinch_scale },
	weak_duration{ weak_duration }, cooldown_timer{ cooldown_timer }{}

/*!*****************************************************************************
  \brief
	Constructor for the Attack Collider class

  \param pos
	The position of the attack collider

  \param sprite
	A pointer to the Sprite of the attack collider

  \param layer
	The layer that the attack collider should be in

  \param scale
	The size of the attack collider

  \param type
	The type of the attack collider (e.g. Player attack collider, Boss attack
	collider)

  \bool is_static
	Whether the attack collider is static or not
*******************************************************************************/
Attack_Collider::Attack_Collider(AEVec2 pos, Sprite* sprite, u32 layer,
	AEVec2 scale, GO_TYPE type, bool is_static)
	: gameObject{ pos, sprite, layer, scale, type, is_static },
	damage{ 0 }, owner{ nullptr }, flinch_scale{ 0.0f } {};

/*!*****************************************************************************
  \brief
	Checks if the Player Attack is equal to the inputted Player Attack

  \param rhs
	The Player Attack to be compared to.
*******************************************************************************/
bool Player_Attack::operator==(Player_Attack const& rhs)
{
	return state == rhs.state;
}

/*!*****************************************************************************
  \brief
	Loads all assets and allocates memory for objects used in attack functions.
	calls the sword_load, spear_load and shield_load functions. Called once in
	the player load function.
*******************************************************************************/
void attacks_load()
{
	sword_load();
	spear_load();
	shield_load();
	pMesh_attackcollider = create_square_mesh(1.0f, 1.0f, 0xFFFFFFFF);
	AE_ASSERT_MESG(pMesh_attackcollider, "Failed to create Attack Collider Mesh!!");
}

/*!*****************************************************************************
  \brief
	Pushes all attacks into the player's sword and spear vectors. Called once
	in the player init function.
*******************************************************************************/
void attacks_init(vector<Player_Attack>& v_sword_attacks, vector<Player_Attack>& v_spear_attacks)
{
	sword_init(v_sword_attacks);
	spear_init(v_spear_attacks);
	shield_init(v_sword_attacks);
}

/*!*****************************************************************************
  \brief
	Pushes an object into the vector of game objects, depending on the type
	inputted by the user.

  \param type
	The type of the object to push in

  \param owner
	The owner of the game object to be pushed. Input nullptr if not applicable.

  \return
	A pointer to the game object pushed into the vector.
*******************************************************************************/
gameObject* push_object(GO_TYPE type, gameObject* owner)
{
	if (type == GO_TYPE::GO_BOSSATTACKCOLLIDER || type == GO_TYPE::GO_PLAYERATTACKCOLLIDER ||
		type == GO_TYPE::GO_ENEMYATTACKCOLLIDER)
	{
		Sprite* attack_collider_spr;
		Attack_Collider* attack_collider;
		attack_collider_spr = new Sprite(nullptr, pMesh_attackcollider, { 75.0f, 75.0f });

		attack_collider = new Attack_Collider({ -20.0f, 150.0f }, attack_collider_spr, 9, { 75.0f,75.0f }, type, true);
		attack_collider->owner = owner;
		attack_collider->is_static = false;
		(Level::v_gameObjects).emplace_back(attack_collider);
		return attack_collider;
	}
	else if (type == GO_TYPE::GO_TRAPS)
	{
		Traps* traps;
		Sprite* traps_sprite = new Sprite(nullptr, pMesh_attackcollider, { 50.0f, 50.0f });
		traps = new Traps({ -20.0f, 150.0f }, { 50.0f,50.0f }, TRAPS_TYPE::TRAPS_SPIKE, traps_sprite);
		traps->layer = 9;
		traps->type = type;
		traps->is_static = true;
		(Level::v_gameObjects).emplace_back(traps);
		return traps;
	}

	else
	{
		Sprite* obj_spr;
		gameObject* obj;
		obj_spr = new Sprite(nullptr, pMesh_attackcollider, { 75.0f, 75.0f });

		obj = new gameObject({ -20.0f, 150.0f }, obj_spr, 9, { 75.0f,75.0f }, type, true);
		(Level::v_gameObjects).push_back(obj);
		return obj;
	}
}

/*!*****************************************************************************
  \brief
	Searches for an existing attack collider from within the vector of game
	objects. Returns nullptr if not found.

  \param owner
	The gameobject that owns the attack collider to be searched for.

  \return
	A pointer to the attack collider found. Returns nullptr if not found.
*******************************************************************************/
Attack_Collider* get_attack_collider(gameObject* owner)
{
	for (gameObject* i : Level::v_gameObjects)
	{
		if (i->type == GO_TYPE::GO_PLAYERATTACKCOLLIDER ||
			i->type == GO_TYPE::GO_BOSSATTACKCOLLIDER ||
			i->type == GO_TYPE::GO_ENEMYATTACKCOLLIDER)
		{
			Attack_Collider* collider = dynamic_cast<Attack_Collider*>(i);
			if (collider->owner == owner) return collider;
		}
	}
	return nullptr;
}

/*!*****************************************************************************
  \brief
	Removes an attack collider from the vector of game objects.

  \param ower
	The owner of the attack collider to be removed
*******************************************************************************/
void remove_attack_collider(gameObject* owner)
{
	if (gameObject* collider = get_attack_collider(owner))
	{
		(Level::v_gameObjects).erase(std::remove((Level::v_gameObjects).begin(), (Level::v_gameObjects).end(),
			collider), (Level::v_gameObjects).end());
		delete collider->obj_sprite;
		delete collider;
		collider = nullptr;
	}
}

/*!*****************************************************************************
  \brief
	Unloads all assets and deallocates memory used in attack functions. Calls
	the sword_unload, spear_unload and shield_unload functions. Called once in the
	player unload function.
*******************************************************************************/
void attacks_unload()
{
	spear_unload();
	sword_unload();
	shield_unload();
	AEGfxMeshFree(pMesh_attackcollider);
}

/*!*****************************************************************************
  \brief
	Sets the aabb of the attack collider. As some attack colliders may rotate,
	this function takes in the minimum x and y, and the maximum x and y to be
	set as the new bounding box's min and max respectively.
*******************************************************************************/
void Attack_Collider::set_aabb()
{
	AEMtx33 mtxRotation;
	AEMtx33Rot(&mtxRotation, obj_sprite->rot);
	AEVec2 min, max;

	AEVec2Set(&min, -0.5f * scale.x, -0.5f * scale.y);
	AEVec2Set(&max, 0.5f * scale.x, 0.5f * scale.y);
	AEMtx33MultVec(&min, &mtxRotation, &min);
	AEMtx33MultVec(&max, &mtxRotation, &max);
	AEVec2Add(&min, &min, &curr_pos);
	AEVec2Add(&max, &max, &curr_pos);
	bounding_box.min.x = AEMin(min.x, max.x);
	bounding_box.min.y = AEMin(min.y, max.y);
	bounding_box.max.x = AEMax(min.x, max.x);
	bounding_box.max.y = AEMax(min.y, max.y);
}