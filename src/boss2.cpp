/*!*****************************************************************************
\file   boss2.cpp
\author Primary: Chua Yip Xuan					 (98.83%)
		Secondary: Siti Nursafinah Binte Sumsuri (1.17%)
\par    DP email: yipxuan.chua\@digipen.edu
				  sitinursafinah.b\@digipen.edu
\date   April 1 2021

\brief
	This source file contains the implmentation of behaviours and attacks
	used in Boss 2.

	The source file has functions that consists of:
	- Boss2::Boss2
	- set_aabb
	- set_attack
	- game_object_load
	- game_object_initialize
	- game_object_draw
	- game_object_free
	- game_object_unload
	- plats_oncollide
	- roots_oncollide
	- fireball_oncollide

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************************/

#include "boss.h"
#include "level_manager.h"

constexpr f32 BOSS_SIZE = 250;
static AEGfxTexture* pTex_plats = nullptr;
static AEGfxTexture* pTex_fireball = nullptr;
static AEGfxTexture* pTex_roots = nullptr;
static AEGfxTexture* pTexBoss2 = nullptr;
static AEGfxVertexList* pmesh = nullptr;
static AEGfxVertexList* pMeshCollider = nullptr;

constexpr s32 BOSS2_SPR_ROW = 1;
constexpr s32 BOSS2_SPR_COL = 3;
const s8* boss2_spritesheet = "./Sprites/Boss/boss2.png";

static f32 timer_roots = 0.0f;
static u32 j = 0;

static Sprite* plats_sprite;
static Sprite* fireball_sprite;
static Sprite* roots_sprite;
void plats_oncollide(gameObject&);
void roots_oncollide(gameObject&);
void fireball_oncollide(gameObject&);

/*!*****************************************************************************
  \brief
	Default constructor for boss2
*******************************************************************************/
Boss2::Boss2() : Boss(), outerBound(), plats(), atkcol_fireballs(), atkcol_roots(), lava_traps()
{
	full_health = 200;
	health = 200;
	scale = { BOSS_SIZE, BOSS_SIZE };
}

/*!*****************************************************************************
  \brief
	Set AABB bounding box for boss2.
*******************************************************************************/
void  Boss2::set_aabb()
{
	//the projectile fireball will only hit areas outside the outerbound
	AEVec2Set(&outerBound.min, -1.0f * scale.x, -1.0f * scale.y);
	AEVec2Set(&outerBound.max, 1.0f * scale.x, 1.0f * scale.y);
	AEVec2Add(&outerBound.min, &outerBound.min, &curr_pos);
	AEVec2Add(&outerBound.max, &outerBound.max, &curr_pos);

	AEVec2Set(&bounding_box.min, -0.5f * scale.x, -0.5f * scale.y);
	AEVec2Set(&bounding_box.max, 0.5f * scale.x, 0.5f * scale.y);
	AEVec2Add(&bounding_box.min, &bounding_box.min, &curr_pos);
	AEVec2Add(&bounding_box.max, &bounding_box.max, &curr_pos);
}

/*!*****************************************************************************
  \brief
	Load all the assets needed for boss2.
*******************************************************************************/
void  Boss2::game_object_load()
{
	pTexBoss2 = AEGfxTextureLoad(boss2_spritesheet);
	pmesh = create_square_mesh(1.0f / BOSS2_SPR_COL, 1.0f / BOSS2_SPR_ROW);
	AE_ASSERT_MESG(pmesh, "Failed to create mesh boss!");
	pMeshCollider = create_square_mesh(1.0f, 1.0f);
	AE_ASSERT_MESG(pMeshCollider, "Failed to create mesh boss!");
	obj_sprite = new Sprite(pTexBoss2, pmesh, scale);
	obj_sprite->set_size(read_png_size(boss2_spritesheet));

	pTex_plats = AEGfxTextureLoad("./Sprites/plats.png");
	AE_ASSERT_MESG(pTex_plats, "Failed to create Attack Collider Texture!");
	plats_sprite = new Sprite(pTex_plats, pMeshCollider, { 100.0f, 100.0f });

	pTex_fireball = AEGfxTextureLoad("./Sprites/Boss/boss2_fireball.png");
	AE_ASSERT_MESG(pTex_fireball, "Failed to create Attack Collider Texture!");
	fireball_sprite = new Sprite(pTex_fireball, pMeshCollider, { 100.0f, 100.0f });

	pTex_roots = AEGfxTextureLoad("./Sprites/Boss/boss2_fireball.png");
	AE_ASSERT_MESG(pTex_roots, "Failed to create Attack Collider Texture!");
	roots_sprite = new Sprite(pTex_roots, pMeshCollider, { 100.0f, 100.0f });

	boss2_attacks_load();
	Animation boss_idle = Animation(3, { 0,0 }, obj_sprite, 302, 0.3f);
	animations.emplace_back(boss_idle);
}

/*!*****************************************************************************
  \brief
	Free all the assets used in boss2.
*******************************************************************************/
void  Boss2::game_object_free()
{
	obj_sprite->mesh = nullptr;
	obj_sprite->texture = nullptr;
}

/*!*****************************************************************************
  \brief
	Unload all the assets in boss 2.
*******************************************************************************/
void  Boss2::game_object_unload()
{
	delete obj_sprite;
	delete fireball_sprite;
	delete plats_sprite;
	delete roots_sprite;
	AEGfxTextureUnload(pTexBoss2);
	AEGfxTextureUnload(pTex_plats);
	AEGfxTextureUnload(pTex_fireball);
	AEGfxTextureUnload(pTex_roots);
	AEGfxMeshFree(pmesh);
	AEGfxMeshFree(pMeshCollider);
	boss2_attacks_unload();
	boss_attacks.clear();
	boss_attacks.~vector();
}

/*!*****************************************************************************
  \brief
	Initialize all the assets and variables of boss2.
*******************************************************************************/
void  Boss2::game_object_initialize()
{
	curr_pos = init_pos;
	health = 200;
	boss2_attacks_init(boss_attacks);

	for (int i{ 0 }; i < 7; ++i)
	{
		plats[i] = new gameObject();
		plats[i]->obj_sprite = plats_sprite;
		plats[i]->obj_sprite->texture = pTex_plats;
		plats[i]->type = GO_TYPE::GO_DESTRUCTABLE;
		plats[i]->scale = { 250.0f, 50.0f };
		plats[i]->on_collide = plats_oncollide;
		plats[i]->flag = false;
		Level::v_gameObjects.emplace_back(plats[i]);
	}
	plats[0]->scale = { 100.0f, 50.0f };
	plats[1]->scale = { 100.0f, 50.0f };

	plats[0]->curr_pos = { this->curr_pos.x - 550.0f, this->curr_pos.y + 80.0f };
	plats[1]->curr_pos = { this->curr_pos.x + 550.0f, this->curr_pos.y + 80.0f };
	plats[2]->curr_pos = { this->curr_pos.x - 375.0f, this->curr_pos.y + 200.0f };
	plats[3]->curr_pos = { this->curr_pos.x + 375.0f, this->curr_pos.y + 200.0f };
	plats[4]->curr_pos = { this->curr_pos.x - 200.0f, this->curr_pos.y + 300.0f };
	plats[5]->curr_pos = { this->curr_pos.x + 200.0f, this->curr_pos.y + 300.0f };
	plats[6]->curr_pos = { this->curr_pos.x , this->curr_pos.y + 400.0f };

	for (int i{ 0 }; i < 10; ++i)
	{
		atkcol_fireballs[i] = dynamic_cast<Attack_Collider*> (push_object(GO_TYPE::GO_ENEMYATTACKCOLLIDER, nullptr));
		delete atkcol_fireballs[i]->obj_sprite;
		atkcol_fireballs[i]->obj_sprite = fireball_sprite;
		atkcol_fireballs[i]->obj_sprite->texture = pTex_fireball;
		atkcol_fireballs[i]->type = GO_TYPE::GO_ENEMYATTACKCOLLIDER;
		atkcol_fireballs[i]->scale = { 50.0f, 50.0f };
		atkcol_fireballs[i]->damage = 1;
		atkcol_fireballs[i]->curr_vel = { 0.0f, 0.0f };
		atkcol_fireballs[i]->acceleration = { 0.0f, 0.0f };
		atkcol_fireballs[i]->on_collide = fireball_oncollide;
		atkcol_fireballs[i]->flag = false;
	}

	for (int i{ 0 }; i < 2; ++i)
	{
		atkcol_roots[i] = dynamic_cast<Attack_Collider*> (push_object(GO_TYPE::GO_ENEMYATTACKCOLLIDER, nullptr));
		delete atkcol_roots[i]->obj_sprite;
		atkcol_roots[i]->obj_sprite = roots_sprite;
		atkcol_roots[i]->obj_sprite->texture = pTex_roots;
		atkcol_roots[i]->type = GO_TYPE::GO_ENEMYATTACKCOLLIDER;
		atkcol_roots[i]->scale = { 10.0f, 10.0f };
		atkcol_roots[i]->damage = 3;
		atkcol_roots[i]->curr_pos = { this->curr_pos.x, this->curr_pos.y - 50.0f };
		atkcol_roots[i]->curr_vel = { 0.0f, 0.0f };
		atkcol_roots[i]->on_collide = roots_oncollide;
		atkcol_roots[i]->acceleration = { 0.0f, 0.0f };
		atkcol_roots[i]->flag = false;
	}

	for (int i{ 0 }; i < 10; ++i)
	{
		lava_traps[i] = dynamic_cast<Traps*> (push_object(GO_TYPE::GO_TRAPS, nullptr));
		lava_traps[i]->type = GO_TYPE::GO_TRAPS;
		lava_traps[i]->traps_type = TRAPS_TYPE::TRAPS_FIRE;
		lava_traps[i]->scale = { 50.0f, 50.0f };
		lava_traps[i]->curr_pos = { 0.0f, 0.0f };
		lava_traps[i]->curr_vel = { 0.0f, 0.0f };
		//lava_traps[i]->on_collide = roots_oncollide;
		lava_traps[i]->acceleration = { 0.0f, 0.0f };
		lava_traps[i]->damage = 1;
		lava_traps[i]->flag = false;
	}
}

/*!*****************************************************************************
  \brief
	Update the boss game objects and other objects in boss2.
*******************************************************************************/
void  Boss2::game_object_update()
{
	debuff.update();
	set_attack();
	attack();

	acceleration.y += LEVEL_GRAVITY * G_DELTATIME * 60;
	AEVec2Add(&curr_vel, &curr_vel, &acceleration);
	AEVec2Zero(&acceleration);
	AEVec2 dt_vel;
	AEVec2Scale(&dt_vel, &curr_vel, G_DELTATIME);
	AEVec2Add(&curr_pos, &curr_pos, &dt_vel);

	vul();
	invul();
	weak();
}

/*!*****************************************************************************
  \brief
	Draw the boss2 and healthbar.
*******************************************************************************/
void  Boss2::game_object_draw()
{
	if (health > 0)
	{
		full_bar();
		f_health();
	}
	if (!curr_Attack)
		animations[0].play_animation();
	obj_sprite->flip_x = is_facing_right ? false : true;
	draw(obj_sprite, curr_pos, scale);
}

/*!*****************************************************************************
  \brief
	Set the attacks for boss 2, boss2 attacks are sequential.
	boss_attacks[0]: linear fireball
	boss_attacks[1]: projectile fireball
	boss_attacks[2]: roots attack
*******************************************************************************/
void  Boss2::set_attack()
{
	if (!is_vul)
	{
		if (cooldown_timer > 0)
			cooldown_timer -= G_DELTATIME;

		else if (!is_attacking)
		{
			static int phase = 1;
			timer_roots = 0.0f;
			j = 0;

			if (phase == 1)
			{
				this->curr_Attack = &boss_attacks[2];
			}
			if (phase == 2)
			{
				this->curr_Attack = &boss_attacks[0];
			}
			if (phase == 3)
			{
				this->curr_Attack = &boss_attacks[1];
				phase = 0;
			}
			++phase;
			is_attacking = true;
			attack_timer = curr_Attack->leeway_timer;
			setup = false;
			attack_phase = 0;
			curr_vel.x = 0.0f;
		}
	}
}

/*!*****************************************************************************
  \brief
	On collide function for the temporary platforms that will be used in boss2
*******************************************************************************/
void plats_oncollide(gameObject& plats)
{
	vector<gameObject*>& colliders = plats.colliders;
	for (vector<gameObject*>::iterator i{ colliders.begin() }; i < colliders.end(); ++i)
	{
		gameObject& obj_col = **i;
		if (obj_col.type == GO_TYPE::GO_ENEMYATTACKCOLLIDER)
		{
			plats.flag = false;
		}
	}
}

/*!*****************************************************************************
  \brief
	On collide function for the fireball attack colliders that will be used in
	boss2
*******************************************************************************/
void fireball_oncollide(gameObject& fireball)
{
	vector<gameObject*>& colliders = fireball.colliders;
	for (vector<gameObject*>::iterator i{ colliders.begin() }; i < colliders.end(); ++i)
	{
		gameObject& obj_col = **i;
		if (obj_col.type == GO_TYPE::GO_DESTRUCTABLE)
		{
			fireball.flag = false;
		}
	}
}

/*!*****************************************************************************
  \brief
	On collide function for the roots attack colliders that will be used in boss2
*******************************************************************************/
void roots_oncollide(gameObject& roots)
{
	vector<gameObject*>& colliders = roots.colliders;
	for (vector<gameObject*>::iterator i{ colliders.begin() }; i < colliders.end(); ++i)
	{
		gameObject& obj_col = **i;
		if (obj_col.type == GO_TYPE::GO_PLATFORM)
		{
			if ((timer_roots > 1.0f) && (j < 9))
			{
				Boss2* boss2 = dynamic_cast<Boss2*> (Level::boss);
				boss2->lava_traps[j]->traps_type = TRAPS_TYPE::TRAPS_FIRE;
				boss2->lava_traps[j]->flag = true;
				boss2->lava_traps[j]->curr_pos.x = boss2->atkcol_roots[0]->curr_pos.x;
				boss2->lava_traps[j]->curr_pos.y = obj_col.bounding_box.max.y + boss2->lava_traps[j]->scale.y / 2;

				boss2->lava_traps[j + 1]->traps_type = TRAPS_TYPE::TRAPS_FIRE;
				boss2->lava_traps[j + 1]->flag = true;
				boss2->lava_traps[j + 1]->curr_pos.x = boss2->atkcol_roots[1]->curr_pos.x;
				boss2->lava_traps[j + 1]->curr_pos.y = obj_col.bounding_box.max.y + boss2->lava_traps[j + 1]->scale.y / 2;

				timer_roots = 0.0f;
				j += 2;
			}
			timer_roots += G_DELTATIME;
		}
	}
}