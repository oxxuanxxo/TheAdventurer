/*!*************************************************************************
****
\file   graphics.h
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Mar 16 2021

\brief
This header file outlines an implementation of the level system in the
game

The functions include:
- win_level
- load_player_data
- load_meshes
- load_textures
- load_bgm
- load_sprites
- load_enemies
- load_area
- load_player_init
- load_boss
- load_camera_bounds
- add_platform
- clear_area
- level_objects_update
- objects_draw
- spawner_collide

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/
#pragma once
#include "main.h"
#include "rapidjson.h"
#include "document.h"
#include "filereadstream.h"
#include "gameobject.h"
#include "mainmenu.h"
#include "player.h"
#include "camera.h"
#include "attack.h"
#include "enemy.h"
#include "boss.h"
#include "particlesystem.h"
#include "traps.h"

class Level
{
public:
	map<std::string, AEGfxVertexList*> m_meshes;
	map<std::string, AEGfxTexture*> m_textures;
	map<std::string, Sprite*> m_sprites;
	AEVec2 player_init_pos;
	AEVec2 boss_init_pos;
	Sprite* platform_spr;
	static Level* pLevel;
	static Player* player;
	static Boss* boss;
	static gameObject* boss_room_wall;
	AABB area_cam;
	static AABB boss_room_camera;
	static vector<gameObject*> v_gameObjects;
	static vector<Sprite*> v_sprites;
	static vector<vector<Enemy*>*> v_area_enemies;
	static vector<vector<gameObject*>*> v_area_walls;
	s32 enemy_counter;
	s32 current_area;
	s32 level;
	static Camera level_cam;
	static AEVec2 level_scale;
public:
	Level(const char* level_path);	// Level_load
	~Level();						// Level_unload
	void init();					// Level_init
	void update();					// Level_update
	void draw();					// Level_draw
	void free();					// Level_free
	void win_level();
	bool load_meshes(const rapidjson::Document& d);
	bool load_textures(const rapidjson::Document& d);
	bool load_bgm(const rapidjson::Document& d);
	bool load_sprites(const rapidjson::Document& d);
	bool load_enemies(const rapidjson::Document& d);
	bool load_area(const rapidjson::Document& d);
	bool load_traps(const rapidjson::Document& d);
	bool load_boss(const rapidjson::Document& d);
	bool load_camera_bounds(const rapidjson::Document& d);
	void add_platform(AEVec2 const& pos);
	void clear_area(s32 const);
};

constexpr u32 MAX_OBJECTS = 1024;
constexpr u32 MAX_ENEMIES = 100;
constexpr u32 MAX_AREAS = 10;
constexpr u32 MAX_AREA_WALLS = 20;
constexpr s32 FILE_BUFFER = 65536;
constexpr u32 MAX_AREA_ENEMY = 25;

void level_objects_update(std::vector<gameObject*>& v_obj);
void objects_draw(std::vector<gameObject*>& v_obj);
void spawner_collide(gameObject& obj);