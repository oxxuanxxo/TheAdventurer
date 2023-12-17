/*!*************************************************************************
****
\file   gameobject.h
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Mar 12 2021

\brief
This header file outlines an implementation of a game object in the game
engine

The functions include:
- swap
- set_aabb
- Draw_Obj
- game_object_load
- game_object_initialize
- game_object_update
- game_object_draw
- game_object_unload
- collision_update
- cmp_layer

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/
#pragma once
#include "graphics.h"
#include "animation.h"
#include "collision.h"
#include "game_enums.h"

using namespace std;
class gameObject;
using fp = void (*)(gameObject&);

#define ANIMS_MAX 50

constexpr u32 LAYER_PLATFORM = 2;

class gameObject
{
private:
	void swap(gameObject& rhs);
public:
	gameObject(); //ctor
	gameObject(AEVec2 pos,
		Sprite* sprite,
		u32 layer,
		AEVec2 scale,
		GO_TYPE type,
		bool is_static);
	gameObject(const gameObject&);
	gameObject& operator=(const gameObject&);
	~gameObject() = default; //dtor
	bool flag;
	AEVec2 curr_pos;
	AEVec2 init_pos;
	AABB bounding_box;
	vector<Animation> animations;
	vector<gameObject*> colliders; // a list of gameobject that is colliding with this gameobject
	vector<gameObject*> ignorecolliders; // a list of colliding game objects to ignore
	AEVec2 scale;
	AEVec2 curr_vel;
	AEVec2 acceleration;
	fp on_collide;
	Sprite* obj_sprite;
	u32 layer; // rendering of graphics
	P_Anim_State state; //move this to player.h
	GO_TYPE type;
	bool is_colliding;
	bool is_static;
	bool on_ground;
	void Draw_Obj();
	virtual void set_aabb();
	virtual void game_object_load();
	virtual void game_object_initialize();
	virtual void game_object_update();
	virtual void game_object_draw();
	virtual void game_object_free();
	virtual void game_object_unload();
};

void collision_update(vector<gameObject*>& v_obj);
bool cmp_layer(gameObject* const& lhs, gameObject* const& rhs);