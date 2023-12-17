/*!*************************************************************************
****
\file   level_manager.cpp
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Mar 11 2021

\brief
This program includes the implementation of the level manager in the game.

The functions include:
- level1_load
- level1_initialize
- level1_update
- level1_draw
- level1_free
- level1_unload

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/

#include "main.h"
#include "level_manager.h"
#include "collectables.h"
#include "player_data.h"
#include "pause.h"

static bool fullscreen = false;
Level *Level::pLevel;
Player *Level::player;
Boss *Level::boss;
gameObject *Level::boss_room_wall;
AABB Level::boss_room_camera;
Camera Level::level_cam;
vector<gameObject *> Level::v_gameObjects;
vector<Sprite *> Level::v_sprites;
vector<vector<Enemy *> *> Level::v_area_enemies;
vector<vector<gameObject *> *> Level::v_area_walls;
AEVec2 Level::level_scale;

static AEGfxVertexList *enemies_left_mesh = 0;
static AEVec2 enemies_left_pos = {490.0f, 315.0f};
static AEVec2 enemies_left_size = {240.0f, 50.0f};

static Audio *area_clear_sound;
static Audio *bg_music;
static Audio *boss_bg_music;
static Audio *boss_warn;

/*!****************************************************************************
 * @brief Construct a new Level:: Level object. Constructing the level object
 * means loading it. The static pointer of pLevel will be pointed to this
 * object and the vector of game objects and enemy areas will be reversed by
 * MAX_OBJECTS and MAX_AREAS respectively.
 *
 * The level file will then be read and parsed as a rapidjson document object.
 * Asserts will be used which doubles up as error handling and loading of
 * the necessary level objects such as meshes, textures, bgm, sprites, traps,
 * enemies, areas, boss, and camera bounds.
 *
 * Platform sprite will be set as the one loaded to m_sprites. The player init
 *  position will be set and it's data will be set by the instance of PlayerData.
 * The required system such as attacks, health drawing, particle system,
 * enemy attacks, and pause system will be loaded last.
 *
 * @param level_path
 * Path to the level file which is stored in json format.
******************************************************************************/
Level::Level(const char *level_path) : area_cam{{}, {}}, boss_init_pos{}, current_area{}, enemy_counter{}, player_init_pos{}, level{}
{
	Level::pLevel = this;
	v_gameObjects.reserve(MAX_OBJECTS);
	v_area_enemies.reserve(MAX_AREAS);
	time_load();
	FILE *fp;
	rapidjson::Document d;
	errno_t err;
	err = fopen_s(&fp, level_path, "rb");
	if (!err)
	{
		char *readBuffer = new char[FILE_BUFFER];
		rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
		d.ParseStream(is);
		level = d["Level"].GetInt();
		AE_ASSERT(d.IsObject());
		AE_ASSERT(load_meshes(d));
		AE_ASSERT(load_textures(d));
		AE_ASSERT(load_bgm(d));
		AE_ASSERT(load_sprites(d));
		AE_ASSERT(load_traps(d));
		AE_ASSERT(load_enemies(d));
		AE_ASSERT(load_area(d));
		AE_ASSERT(load_boss(d));
		AE_ASSERT(load_camera_bounds(d));
		delete[] readBuffer;
		if (fp)
			fclose(fp);
	}
	platform_spr = m_sprites.at("Platform");
	player = new Player(player_init_pos);
	enemies_left_mesh = create_square_mesh(1.0f, 1.0f, 0x80FFFFFF);
	PlayerData::instance()->set_player();
	player->game_object_load();
	attacks_load();
	boss->game_object_load();
	boss->init_pos = boss_init_pos;
	area_clear_sound = new Audio("./Audio/level_area_clear.wav", AUDIO_GROUP::SFX);
	heart_load();
	particle_system_load();
	enemyattack_load();
	pause_load();
}

/*!****************************************************************************
 * @brief Destroy the Level:: Level object. This doubles as the unload function
 *  of the level. The audio allocated to this level will be deleted, as well as
 * the mesh for enemies left, all allocated data in m_meshes, m_textures,
 * m_sprites, v_area_enemies, v_sprites, and v_area_walls. All loaded systems
 * will de deallocated here aswell. The player and boss will be deleted last.
 *
******************************************************************************/
Level::~Level()
{
	delete boss_bg_music;
	delete bg_music;
	delete boss_warn;
	delete area_clear_sound;

	AEGfxMeshFree(enemies_left_mesh);

	for (auto &a : m_meshes)
	{
		if (a.second)
		{
			AEGfxMeshFree(a.second);
		}
	}
	m_meshes.clear();

	for (auto &a : m_textures)
	{
		AEGfxTextureUnload(a.second);
	}
	m_textures.clear();

	for (auto &a : m_sprites)
	{
		delete a.second;
	}
	m_sprites.clear();

	for (auto &a : v_area_enemies)
	{
		for (auto &b : *a)
		{
			remove_attack_collider(b);
		}
		delete a;
	}
	v_area_enemies.clear();

	for (auto &a : v_sprites)
	{
		delete a;
	}
	v_sprites.clear();

	for (auto &a : v_area_walls)
	{
		delete a;
	}
	v_area_walls.clear();
	attacks_unload();
	heart_unload();
	unload_enemy_assets();
	particle_system_unload();
	pause_unload();
	enemyattack_unload();
	traps_unload();
	player->game_object_unload();
	boss->game_object_unload();
	for (auto &a : v_gameObjects)
	{
		if (a->type == GO_TYPE::GO_MONSTER)
		{
			RangedEnemy *b = dynamic_cast<RangedEnemy *>(a);
			if (b)
			{
				delete b;
				continue;
			}

			else
			{
				MeleeEnemy1 *c = dynamic_cast<MeleeEnemy1 *>(a);
				if (c)
				{
					delete c;
					continue;
				}

				else
				{
					MeleeEnemy2 *d = dynamic_cast<MeleeEnemy2 *>(a);
					delete d;
					continue;
				}
			}
		}

		else if (a->type == GO_TYPE::GO_PLAYER)
		{
			continue;
		}

		else if (a->type == GO_TYPE::GO_BOSS)
		{
			continue;
		}

		else if (a->type == GO_TYPE::GO_PARTICLESYSTEM)
		{
			particleSystem *b = dynamic_cast<particleSystem *>(a);
			if (b->flag)
				b->delete_system();
			delete b;
			continue;
		}

		else if (a->type == GO_TYPE::GO_TRAPS)
		{
			Traps *b = dynamic_cast<Traps *>(a);
			if (b)
			{
				delete b;
				continue;
			}

			LavaTraps *c = dynamic_cast<LavaTraps *>(a);
			if (c)
			{
				delete c;
				continue;
			}

			IciclesTraps *d = dynamic_cast<IciclesTraps *>(a);
			if (d)
			{
				delete d;
				continue;
			}
		}

		else if (a->type == GO_TYPE::GO_DESTRUCTABLE)
		{
			Attack_Collider *b = dynamic_cast<Attack_Collider *>(a);
			if (b)
			{
				delete b->obj_sprite;
				delete b;
			}
			else
				delete a;
		}

		else
		{
			delete a;
		}
	}
	v_gameObjects.clear();

	delete player;
	delete boss;
}

/*!****************************************************************************
 * @brief Initializes the level.
 * Pause and time system will be initialized first before all other game
 * objects. After initializing the player, the level_cam will initialize,
 * setting the player as the default target with boundaries specified in
 * area_cam. The player and boss game object will be pushed to v_gameobjects
 * and the audio system will initialize and play the background music on loop.
 *
******************************************************************************/
void Level::init()
{
	enemy_counter = 0;
	current_area = 0;
	time_initialize();
	pause_initialize();

	pause = false;
	player->game_object_initialize();
	v_gameObjects.push_back(player);
	enemy_counter++;
	level_cam.init(*player, area_cam);
	level_cam.set_camera_boundary(area_cam.min, area_cam.max);
	boss->game_object_initialize();
	boss->flag = false; // Engine proof stuff
	v_gameObjects.push_back(boss);
	for (auto &a : *v_area_enemies[current_area])
	{
		a->flag = true;
		a->game_object_initialize();
	}
	AudioSystem::instance()->init();
	bg_music->play_sound(1.0f, true);
}

/*!****************************************************************************
 * @brief Update function of the level
 * Checks if the current area is the same as the size of v_area_enemies and
 * that boss health is <= 0. This is the win condition for the level. The first
 *  condition is to check if the player completed all areas and boss health
 * <= 0 means that the boss has been defeated.
 *
 * If current are is <= size of v_area_enemies, it will count the number of
 * enemies alive in the current area. If the enemy counter is 0, it means that
 *  all enemies have been defeated. This will destroy all the walls in the
 * current area, shown via a particle system and playing of the
 * area_clear_sound. The next area's enemies will then be initialized and
 * the maximum bound for the camera will be extended to the wall of the next
 * area.
 *
******************************************************************************/
void Level::update()
{
	if (current_area == v_area_enemies.size() && boss->health <= 0)
	{
		win_level();
	}

	if (current_area < v_area_enemies.size())
	{
		int temp_count_enemy = 0;
		for (auto &a : *v_area_enemies[current_area])
		{
			if (a->flag)
				++temp_count_enemy;
		}
		enemy_counter = temp_count_enemy;
		if (enemy_counter == 0)
		{
			area_clear_sound->play_sound();
			for (auto &a : *v_area_walls[current_area])
			{
				particleSystem *pParticleSystem = create_particlesystem(a);
				pParticleSystem->color_range[0] = {0.3f, 0.3f, 0.3f, 0.6f};
				pParticleSystem->color_range[1] = {0.4f, 0.4f, 0.4f, 0.8f};
				pParticleSystem->scale_range[0] = {75.0f, 75.0f};
				pParticleSystem->scale_range[1] = {100.0f, 100.0f};
				pParticleSystem->lifetime_range[0] = 1;
				pParticleSystem->lifetime_range[1] = 3;
				pParticleSystem->rotation_range[0] = 0;
				pParticleSystem->rotation_range[1] = 2 * PI;
				pParticleSystem->direction_range[0] = 0;
				pParticleSystem->direction_range[1] = 2 * PI;
				pParticleSystem->speed_range[0] = 100.0f;
				pParticleSystem->speed_range[1] = 300.0f;
				pParticleSystem->scale = a->scale;
				pParticleSystem->max_particles = 30;
				pParticleSystem->emission_rate = 0.016f;
				pParticleSystem->lifetime = 0.1f;
				pParticleSystem->particle_increment = 30;
				pParticleSystem->set_aabb();
				pParticleSystem->game_object_update();
				a->flag = false;
			}
			++current_area;
			if (current_area < v_area_enemies.size())
			{
				level_cam.set_max_boundary(v_area_walls[current_area]->at(0)->curr_pos);
				for (auto &a : *v_area_enemies[current_area])
				{
					a->flag = true;
					a->game_object_initialize();
				}
			}
			else
			{
				level_cam.set_camera_boundary(area_cam.min, boss_room_camera.max);
			}
		}
	}
	if (AEInputCheckTriggered(AEVK_ESCAPE) && !transitioning)
	{
		pause = true;
		AudioSystem::instance()->toggle_pause_group(AUDIO_GROUP::STAGE_BGM);
		AudioSystem::instance()->toggle_pause_group(AUDIO_GROUP::BOSS_BGM);
		AudioSystem::instance()->toggle_pause_group(AUDIO_GROUP::SFX);
	}
}

/*!****************************************************************************
 * @brief Draw function of the level. Drawing of every game object of the
 * level is not done here to allow more flexibility in unique situations.
 *
 * The enemy counter will be printed on screen as well as the pause menu if
 * its flag is true. Transition will be drawn here too if ever the game state
 * transitions to another.
******************************************************************************/
void Level::draw()
{
	char strBuffer[100];
	memset(strBuffer, 0, 100 * sizeof(char));
	if (enemy_counter)
	{
		draw_non_relative(nullptr, enemies_left_mesh, enemies_left_pos, {0, 0}, {1.0f, 1.0f, 1.0f, 1.0f}, 1.0f, enemies_left_size);
		sprintf_s(strBuffer, "Enemies Left:  %d", enemy_counter);
		AEGfxPrint(bold_font, strBuffer, 0.6f, 0.85f, 1.0f, 0, 0, 0);
	}
	if (pause)
		pause_draw();

	transition();
}

/*!****************************************************************************
 * @brief Call game_object_free at every game objects in the level. Frees the
 * level_cam, time system, and pause system as well and sets the pointer of
 * platform_spr to nullptr.
 *
******************************************************************************/
void Level::free()
{
	for (size_t i{0}; i < v_gameObjects.size(); ++i)
	{
		gameObject *obj = v_gameObjects[i];
		obj->game_object_free();
	}
	level_cam.free();
	time_free();
	pause_free();
	platform_spr = nullptr;
}

/*!****************************************************************************
 * @brief Load all the meshes in the rapidjson document. In the json file,
 * check for the "RectMesh" array. Then, iterate through the array. In this
 * game, all meshes are made up of 2 triangles. Hence, in each iteration, 2
 * triangle meshes will be found and each member such as name, colour values
 * (c), x, y, translation values will be searched and parsed. These values will
 * be used to insert into the map m_meshes, with mesh_name as the key.
 *
 * @param d
 * Rapidjson doument
 * @return true
 * Meshes have been loaded successfully
 * @return false
 * Meshes have not been loaded successfully
******************************************************************************/
bool Level::load_meshes(const rapidjson::Document &d)
{
	const rapidjson::Value &rect_mesh = d["RectMesh"];
	for (auto &itr : rect_mesh.GetArray())
	{
		string mesh_name = itr.FindMember("Name")->value.GetString();
		string mesh1 = itr.FindMember("HalfTriMesh1")->value.GetString();
		string mesh2 = itr.FindMember("HalfTriMesh2")->value.GetString();
		AEGfxMeshStart();
		const rapidjson::Value &tri_mesh = d["TriMesh"];
		for (auto &itr : tri_mesh.GetArray())
		{
			std::string tri_name = itr.FindMember("Name")->value.GetString();
			if (tri_name == mesh1 || tri_name == mesh2)
			{
				u32 c0, c1, c2;
				f32 x0, x1, x2, y0, y1, y2, tu0, tu1, tu2, tv0, tv1, tv2;
				c0 = itr.FindMember("c0")->value.GetUint();
				c1 = itr.FindMember("c1")->value.GetUint();
				c2 = itr.FindMember("c2")->value.GetUint();
				x0 = itr.FindMember("x0")->value.GetFloat();
				x1 = itr.FindMember("x1")->value.GetFloat();
				x2 = itr.FindMember("x2")->value.GetFloat();
				y0 = itr.FindMember("y0")->value.GetFloat();
				y1 = itr.FindMember("y1")->value.GetFloat();
				y2 = itr.FindMember("y2")->value.GetFloat();
				tu0 = itr.FindMember("tu0")->value.GetFloat();
				tu1 = itr.FindMember("tu1")->value.GetFloat();
				tu2 = itr.FindMember("tu2")->value.GetFloat();
				tv0 = itr.FindMember("tv0")->value.GetFloat();
				tv1 = itr.FindMember("tv1")->value.GetFloat();
				tv2 = itr.FindMember("tv2")->value.GetFloat();
				AEGfxTriAdd(x0, y0, c0, tu0, tv0,
							x1, y1, c1, tu1, tv1,
							x2, y2, c2, tu2, tv2);
			}
		}
		m_meshes.insert({mesh_name, AEGfxMeshEnd()});
	}
	return 1;
}

/*!****************************************************************************
 * @brief Load all the textures in the rapidjson document.
 * In the json file, check for the "Texture" array. Each and every textures
 * found will be loaded and inserted in m_textures.
 *
 * @param d
 * Rapidjson document
 * @return true
 * Textures loaded successfully
 * @return false
 * Textures did not load successfully
******************************************************************************/
bool Level::load_textures(const rapidjson::Document &d)
{
	const rapidjson::Value &texture = d["Texture"];
	for (auto &itr : texture.GetArray())
	{
		string tex_name = itr.FindMember("Name")->value.GetString();
		const s8 *path = itr.FindMember("Filepath")->value.GetString();
		m_textures.insert({tex_name, AEGfxTextureLoad(path)});
	}
	return 1;
}

/*!****************************************************************************
 * @brief Load all the sprites in the rapidjson document.
 * In the json file, check for the "Sprite" array. The sprite will be added to
 *  the map m_sprites with its texture and mesh. If the sprite has no "Mesh"
 * member, it means that it has its own mesh, specified by TranslateX,
 * TranslateY, and Color. This creation of mesh will also be added to m_meshes.
 *
 * @param d
 * Rapidjson document
 * @return true
 * Sprites loaded successfully
 * @return false
 * Sprites did not load successfully
******************************************************************************/
bool Level::load_sprites(const rapidjson::Document &d)
{
	const rapidjson::Value &sprite = d["Sprite"];
	const rapidjson::Value &texture = d["Texture"];
	for (auto &spr : sprite.GetArray())
	{
		string spr_name = spr.FindMember("Name")->value.GetString();
		string tex;
		AEVec2 size = {1.0f, 1.0f};
		AEGfxTexture *spr_tex = nullptr;
		AEGfxVertexList *spr_mesh = nullptr;
		if (spr.HasMember("Texture"))
		{
			tex = spr.FindMember("Texture")->value.GetString();
			spr_tex = m_textures.at(tex);
			size = read_png_size(m_textures.at(tex)->mpName);
		}
		if (spr.HasMember("Mesh"))
		{
			string mesh = spr.FindMember("Mesh")->value.GetString();
			spr_mesh = m_meshes.at(mesh);
			int i = 0x75757575;
		}
		else
		{
			f32 trans_x = 1.0f, trans_y = 1.0f;
			s32 color;
			trans_x = spr.FindMember("TranslateX")->value.GetFloat() / size.x;
			trans_y = spr.FindMember("TranslateY")->value.GetFloat() / size.y;
			color = spr.FindMember("Color")->value.GetInt();
			spr_mesh = create_square_mesh(trans_x, trans_y, color);
			AE_ASSERT_MESG(spr_mesh, "Failed to create mesh!");
			m_meshes.insert({spr_name + tex, spr_mesh});
		}
		Sprite *s = new Sprite(spr_tex, spr_mesh);
		s->set_size(size);
		m_sprites.insert({spr_name, s});
	}
	return 1;
}

/*!****************************************************************************
 * @brief Load the background music for the game.
 * Create a new Audio object for bg_music, boss_bg_music and boss_warn using
 * the values in the members "StageBGM", "BossBGM", and "BossWarn" respectively.
 * 
 * @param d 
 * Rapidjson document
 * @return true 
 * Loaded music successfully
 * @return false 
 * Music not loaded successfully
******************************************************************************/
bool Level::load_bgm(const rapidjson::Document &d)
{
	const rapidjson::Value &stage_bgm = d["StageBGM"];
	const rapidjson::Value &boss_bgm = d["BossBGM"];
	const rapidjson::Value &boss_warning = d["BossWarn"];
	bg_music = new Audio(stage_bgm.GetString(), AUDIO_GROUP::STAGE_BGM);
	boss_bg_music = new Audio(boss_bgm.GetString(), AUDIO_GROUP::BOSS_BGM);
	boss_warn = new Audio(boss_warning.GetString(), AUDIO_GROUP::SFX);
	return 1;
}

/*!****************************************************************************
 * @brief Load the enemies required for the level.
 * In the json file, search for the "Enemy" array and "Areas" member.
 * Memory will be reserved for each area in v_area_enemies.
 * In each enemy member in the array, created a new enemy using its sprite,
 * position, scale, health, area, and type values. As each enemy should have a
 * unique sprite, a new sprite object will be created using the copy
 * constructor of Sprite. The constructor of the enemy differs from the type
 *  of enemy.
 * 
 * @param d 
 * Rapidjson document
 * @return true 
 * Enemies loaded successfully
 * @return false 
 * Enemies not loaded successfully
******************************************************************************/
bool Level::load_enemies(const rapidjson::Document &d)
{
	const rapidjson::Value &enemy = d["Enemy"];
	int areas = d["Areas"].GetInt();
	v_area_enemies.reserve(areas);
	for (int i{0}; i < areas; ++i)
	{
		vector<Enemy *> *v_area = new vector<Enemy *>;
		v_area->reserve(MAX_AREA_ENEMY);
		v_area_enemies.push_back(v_area);
	}
	for (auto &e : enemy.GetArray())
	{
		string spr_name = e.FindMember("Sprite")->value.GetString();
		float pos_x = e.FindMember("PosX")->value.GetFloat();
		float pos_y = e.FindMember("PosY")->value.GetFloat();
		float scale_x = e.FindMember("ScaleX")->value.GetFloat();
		float scale_y = e.FindMember("ScaleY")->value.GetFloat();
		int health = e.FindMember("Health")->value.GetInt();
		int area = e.FindMember("Area")->value.GetInt();
		Sprite *enemy_sprite = new Sprite(*m_sprites.at(spr_name));
		string enemy_type = e.FindMember("Type")->value.GetString();
		Enemy *enemy_obj = 0;
		if (enemy_type == "Melee")
		{
			enemy_obj = new MeleeEnemy2(health, {pos_x, pos_y}, {scale_x, scale_y}, enemy_sprite);
		}
		else if (enemy_type == "Ranged")
		{
			f32 range;
			if (e.HasMember("Range"))
				range = e.FindMember("Range")->value.GetFloat();
			else
				range = 1.0f;
			enemy_obj = new RangedEnemy(health, {pos_x, pos_y}, {scale_x, scale_y}, enemy_sprite, range);
		}
		else
		{
			enemy_obj = new MeleeEnemy1(health, {pos_x, pos_y}, {scale_x, scale_y}, enemy_sprite);
		}
		enemy_obj->game_object_load();
		enemy_obj->flag = false;
		v_sprites.push_back(enemy_sprite);
		v_gameObjects.push_back(enemy_obj);
		v_area_enemies[area]->push_back(enemy_obj);
	}
	load_enemy_assets();
	return true;
}

/*!****************************************************************************
 * @brief Load the area walls in the level
 * Iterate through the object "Walls" and load the game object based on its 
 * X, Y and scale values. If it has a sprite, the object will be created with
 *  that sprite. Pointer will be pushed to v_gameObjects and v_area_walls.
 * 
 * @param d 
 * Rapidjson document
 * @return true 
 * Area walls loaded successfully
 * @return false 
 * Area walls not loaded successfully
******************************************************************************/
bool Level::load_area(const rapidjson::Document &d)
{
	const rapidjson::Value &sprite = d["Sprite"];
	const rapidjson::Value &wall = d["Walls"];
	int areas = d["Areas"].GetInt();
	v_area_walls.reserve(areas);
	for (int i{0}; i < areas; ++i)
	{
		vector<gameObject *> *v_area = new vector<gameObject *>;
		v_area->reserve(MAX_AREA_WALLS);
		v_area_walls.push_back(v_area);
	}
	for (auto &a : wall.GetArray())
	{
		f32 x = a.FindMember("X")->value.GetFloat();
		f32 y = a.FindMember("Y")->value.GetFloat();
		f32 scale_x = a.FindMember("ScaleX")->value.GetFloat();
		f32 scale_y = a.FindMember("ScaleY")->value.GetFloat();
		AEVec2 scale = {scale_x, scale_y};
		gameObject *g_obj;
		if (a.HasMember("Sprite"))
		{
			string plat_sprite = a.FindMember("Sprite")->value.GetString();
			g_obj = new gameObject({x, y}, m_sprites.at(plat_sprite), LAYER_PLATFORM, scale, GO_TYPE::GO_WALL, true);
		}
		else
		{
			g_obj = new gameObject({x, y}, nullptr, LAYER_PLATFORM, scale, GO_TYPE::GO_WALL, true);
		}
		v_gameObjects.push_back(g_obj);
		if (a.HasMember("Area"))
		{
			int area = a.FindMember("Area")->value.GetInt();
			v_area_walls[area]->push_back(g_obj);
		}
	}
	return true;
}

/*!****************************************************************************
 * @brief Load all the traps from the rapidjson document
 * In the array of "Traps", create traps based on the X, Y, scale, type, and 
 * sprite member values. The traps are constructed differently based on the 
 * type of the trap.
 * 
 * @param d 
 * Rapidjson document
 * @return true 
 * Traps loaded successfully
 * @return false
 * Trans not loaded successfully 
******************************************************************************/
bool Level::load_traps(const rapidjson::Document &d)
{
	const rapidjson::Value &trap = d["Traps"];
	traps_load();
	if (trap.IsNull())
		return false;
	for (auto &tr : trap.GetArray())
	{
		f32 x = tr.FindMember("X")->value.GetFloat();
		f32 y = tr.FindMember("Y")->value.GetFloat();
		f32 scale_x = tr.FindMember("ScaleX")->value.GetFloat();
		f32 scale_y = tr.FindMember("ScaleY")->value.GetFloat();
		AEVec2 scale = {scale_x, scale_y};
		string s = tr.FindMember("Type")->value.GetString();
		string sprite_str = tr.FindMember("Sprite")->value.GetString();
		Sprite *trap_spr = new Sprite(*m_sprites.at(sprite_str));
		v_sprites.emplace_back(trap_spr);
		if (s == "Spike")
		{
			SpikeTraps *spike = new SpikeTraps({x, y}, scale, TRAPS_TYPE::TRAPS_SPIKE, trap_spr);
			spike->game_object_initialize();
			v_gameObjects.push_back(spike);
		}
		else if (s == "Icicle")
		{
			IciclesTraps *icicle = new IciclesTraps({x, y}, scale, TRAPS_TYPE::TRAPS_ICICLES, trap_spr);
			icicle->game_object_initialize();
			v_gameObjects.push_back(icicle);
		}
		else if (s == "Fire")
		{
			LavaTraps *lava = new LavaTraps({x, y}, scale, TRAPS_TYPE::TRAPS_FIRE, trap_spr);
			lava->game_object_initialize();
			v_gameObjects.push_back(lava);
		}
		else
		{
			Traps *trap = new Traps({x, y}, scale, TRAPS_TYPE::TRAPS_KILLZONE, trap_spr);
			trap->game_object_initialize();
			v_gameObjects.push_back(trap);
		}
	}
	return true;
}

/*!****************************************************************************
 * @brief Load the logic for spawning the boss of the level from the rapidjson 
 * document. In this game. The boss battle will start on collision of a 
 * boss spawner trigger. The scale and position of this trigger is stored in 
 * "BossTrigger" member object. A boss room wall is created with its values 
 * stored in "BossRoomWall". The boss room's camera bound will be read from 
 * "BossRoomCamBound". The boss's initialized position is also stored in
 *  "BossInit".
 * 
 * @param d 
 * Rapidjson document
 * @return true 
 * Boss room loaded successfully
 * @return false 
 * Boss room not loaded successfully
******************************************************************************/
bool Level::load_boss(const rapidjson::Document &d)
{
	const rapidjson::Value &boss_init = d["BossInit"];
	const rapidjson::Value &boss_trigger = d["BossTrigger"];
	const rapidjson::Value &boss_wall = d["BossRoomWall"];
	const rapidjson::Value &boss_bound = d["BossRoomCamBound"];
	if (boss_init.IsNull())
		return false;
	f32 boss_init_x = boss_init.FindMember("X")->value.GetFloat();
	f32 boss_init_y = boss_init.FindMember("Y")->value.GetFloat();
	f32 trigger_x = boss_trigger.FindMember("X")->value.GetFloat();
	f32 trigger_y = boss_trigger.FindMember("Y")->value.GetFloat();
	f32 scale_x = boss_trigger.FindMember("ScaleX")->value.GetFloat();
	f32 scale_y = boss_trigger.FindMember("ScaleY")->value.GetFloat();
	f32 boss_room_x = boss_wall.FindMember("X")->value.GetFloat();
	f32 boss_room_y = boss_wall.FindMember("Y")->value.GetFloat();
	f32 boss_room_scalex = boss_wall.FindMember("ScaleX")->value.GetFloat();
	f32 boss_room_scaley = boss_wall.FindMember("ScaleY")->value.GetFloat();
	f32 cam_min_x = boss_bound.FindMember("MinX")->value.GetFloat();
	f32 cam_min_y = boss_bound.FindMember("MinY")->value.GetFloat();
	f32 cam_max_x = boss_bound.FindMember("MaxX")->value.GetFloat();
	f32 cam_max_y = boss_bound.FindMember("MaxY")->value.GetFloat();
	boss_init_pos = {boss_init_x, boss_init_y};
	AEVec2 scale = {scale_x, scale_y};
	AEVec2 boss_room_scale = {boss_room_scalex, boss_room_scaley};
	AEVec2 min = {cam_min_x, cam_min_y};
	AEVec2 max = {cam_max_x, cam_max_y};
	gameObject *g_obj = new gameObject({trigger_x, trigger_y}, nullptr, LAYER_PLATFORM, scale, GO_TYPE::GO_BOSS_SPAWNER, true);
	boss_room_wall = new gameObject({boss_room_x, boss_room_y}, nullptr, LAYER_PLATFORM, boss_room_scale, GO_TYPE::GO_WALL, true);
	boss_room_wall->flag = false;
	boss_room_camera.min = min;
	boss_room_camera.max = max;
	g_obj->on_collide = spawner_collide;
	v_gameObjects.push_back(g_obj);
	v_gameObjects.push_back(boss_room_wall);
	return true;
}

/*!****************************************************************************
 * @brief Load the level's camera boundary using the values stored in 
 * "CameraBoundary" member object of the rapidjson document
 * 
 * @param d 
 * Rapidjson document
 * @return true 
 * Camera bounds loaded successfully
 * @return false 
 * Camera bounds not loaded successfully
******************************************************************************/
bool Level::load_camera_bounds(const rapidjson::Document &d)
{
	const rapidjson::Value &boundary = d["CameraBoundary"];
	f32 min_x = boundary.FindMember("MinX")->value.GetFloat();
	f32 min_y = boundary.FindMember("MinY")->value.GetFloat();
	f32 max_x = boundary.FindMember("MaxX")->value.GetFloat();
	f32 max_y = boundary.FindMember("MaxY")->value.GetFloat();
	AEVec2 min = {min_x, min_y};
	AEVec2 max = {max_x, max_y};
	area_cam.min = min;
	area_cam.max = max;
	return true;
}

/*!****************************************************************************
 * @brief Add a platform to the level based on the position.
 * 
 * @param pos 
 * Coordinate of the platform to place
******************************************************************************/
void Level::add_platform(AEVec2 const &pos)
{
	if (!(m_sprites.find("Platform") == m_sprites.end()))
		v_gameObjects.push_back(new gameObject({pos.x, pos.y}, m_sprites.at("Platform"), LAYER_PLATFORM, level_scale, GO_TYPE::GO_PLATFORM, true));
	else
		v_gameObjects.push_back(new gameObject({pos.x, pos.y}, nullptr, LAYER_PLATFORM, level_scale, GO_TYPE::GO_PLATFORM, true));
}

/*!****************************************************************************
 * @brief Clears the specified area, including all its enemies and walls.
 * 
 * @param area 
 * Area to clear
******************************************************************************/
void Level::clear_area(s32 const area)
{
	area_clear_sound->play_sound();
	for (auto &a : *v_area_walls[area])
	{
		a->flag = false;
		Color colors[2]{{0.3f, 0.3f, 0.3f, 0.6f}, {0.4f, 0.4f, 0.4f, 0.8f}};
		AEVec2 scales[2]{{10.0f, 10.0f}, {30.0f, 30.0f}};
		f32 lifetimes[2]{1, 3};
		f32 rotations[2]{0, 2 * PI};
		f32 directions[2]{0, 2 * PI};
		f32 speeds[2]{100.0f, 300.0f};
		particleSystem *pParticleSystem = new particleSystem(a, a->scale, false, 100U, 0.1f, 0.016f,
															 0.0f, particle_default_behaviour, colors, lifetimes, scales, rotations, directions, speeds, nullptr);
		pParticleSystem->particle_increment = 300;
		pParticleSystem->set_aabb();
		Level::v_gameObjects.push_back(pParticleSystem);
	}
	for (auto &a : *v_area_enemies[area])
	{
		a->flag = false;
	}
	current_area++;
	if (current_area < v_area_enemies.size())
	{
		level_cam.set_max_boundary(v_area_walls[current_area]->at(0)->curr_pos);
		for (auto &a : *v_area_enemies[current_area])
		{
			a->flag = true;
			a->game_object_initialize();
		}
	}
	else
	{
		level_cam.set_camera_boundary(area_cam.min, boss_room_camera.max);
	}
}

/*!****************************************************************************
 * @brief Collision logic for the boss spawner
 * If the player collides with the boss spawner, the boss bgm and boss_warn 
 * sound will play out and crossfades the stage_bgm and boss_bgm audio group.
 * 
 * 
 * @param obj 
 * Reference to the boss spawner game object
******************************************************************************/
void spawner_collide(gameObject &obj)
{
	for (auto &i : obj.colliders)
	{
		if (i->type == GO_TYPE::GO_PLAYER)
		{
			boss_warn->play_sound();
			boss_bg_music->play_sound(1.0f, true);
			AudioSystem::instance()->crossfade(AUDIO_GROUP::STAGE_BGM, AUDIO_GROUP::BOSS_BGM);
			Level::boss->flag = true;
			Camera::shake(4.0f, 2.0f);
			// set camera boundary
			Level::level_cam.set_camera_boundary(Level::boss_room_camera.min, Level::boss_room_camera.max);
			// set boss_room wall to true
			Level::boss_room_wall->flag = true;
			// remove spawner
			obj.flag = false;
		}
	}
}

/*!****************************************************************************
 * @brief Wins the level
 * Transition to the win game state and call playerdata's level_complete and 
 * save data.
 * 
******************************************************************************/
void Level::win_level()
{
	PlayerData::instance()->level_complete(level);
	PlayerData::instance()->save_data("./Data/player.json");
	transition(0.5f, GS_WIN);
}

/*!****************************************************************************
 * @brief Update all the gameobjects in the specified vector and set its
 * AABB boundary for collision check
 * 
 * @param v_obj 
 * Reference to the vector of gameobject pointers
******************************************************************************/
void level_objects_update(std::vector<gameObject *> &v_obj)
{
	for (std::vector<gameObject *>::iterator i = v_obj.begin(); i != v_obj.end(); ++i)
	{
		if ((*i)->flag)
		{
			(*i)->game_object_update();
			(*i)->set_aabb();
		}
	}
}

/*!****************************************************************************
 * @brief Draw all the gameobjects in the specified vector if the flag is true
 * 
 * @param v_obj 
 * Reference to the vector of gameobject pointers
******************************************************************************/
void objects_draw(std::vector<gameObject *> &v_obj)
{
	for (vector<gameObject *>::iterator go = v_obj.begin(); go != v_obj.end(); ++go)
	{
		if ((*go)->flag)
		{
			(*go)->game_object_draw();
		}
	}
}