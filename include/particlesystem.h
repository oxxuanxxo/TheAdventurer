/*!*****************************************************************************
\file   particlesystem.h
\author Mohammad Hanif Koh Teck Wee
\par    DP email: mohammadhanifkoh.t\@digipen.edu
\date   March 16 2021

\brief
This file outlines the implementation of the particle system, and its data
structure. It also contains the declaration of functions to create commonly used
particle systems and all behaviour functions for particles.

The functions include:
- Particle constructors
- particle_system_load
- particle_system_unload
- create_particlesystem
- damage_taken
- particle_default_behaviour
- boss_vulnerable
- create_falling_particles
- create_dash_particles
- create_damaged_particles
- create_ground_particles
- dodge_particle
- slam_particle
- spear_charge_particle
- spear_throw_particle
- parry_particle
- shockwave_particle
- change_wep_particle

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************************/
#pragma once
#include "gameobject.h"

class particleSystem;
class Particle : public gameObject
{
public:
	void game_object_update() override;

	particleSystem* parent;
	AEVec2 initial_position;
	f32 lifetime;
	f32 max_distance;
	fp behaviour;
};

class particleSystem : public gameObject
{
public:
	particleSystem(AEVec2 position, AEVec2 size, bool loop, u32 max_particles, f32 lifetime, f32 emission_rate,
		f32 max_distance, fp behaviour, Color color_range[2], f32 lifetime_range[2],
		AEVec2 scale_range[2], f32 rotation_range[2], f32 direction_range[2],
		f32 speed_range[2], AEGfxTexture* texture);

	particleSystem(gameObject* target, AEVec2 size, bool loop, u32 max_particles, f32 lifetime, f32 emission_rate,
		f32 max_distance, fp behaviour, Color color_range[2], f32 lifetime_range[2],
		AEVec2 scale_range[2], f32 rotation_range[2], f32 direction_range[2],
		f32 speed_range[2], AEGfxTexture* texture);
	void game_object_update() override;
	void game_object_draw() override;
	void create_particle();
	void delete_system();

	bool loop;
	gameObject* target;
	AEVec2 position_offset;
	u32 max_particles;
	u32 particle_count;
	u32 particle_increment;
	f32 lifetime;
	f32 timer;
	f32 emission_rate;
	f32 max_distance;
	fp behaviour;
	vector<Particle> particles;
	Color color_range[2];
	f32 lifetime_range[2];
	AEVec2 scale_range[2];
	f32 rotation_range[2];
	f32 direction_range[2];
	f32 speed_range[2];
};

void particle_system_load();
void particle_system_unload();

particleSystem* create_particlesystem(gameObject* target);
particleSystem* create_particlesystem(AEVec2 target);

particleSystem* create_falling_particles();
particleSystem* create_dash_particles(gameObject* target);
particleSystem* create_damaged_particles(gameObject* target);
particleSystem* create_ground_particles(gameObject* target);

void boss_vulnerable(gameObject& obj);
void particle_default_behaviour(gameObject& obj);
void damage_taken(gameObject& obj);
void dodge_particle(gameObject& obj);
void slam_particle(gameObject& obj);
void spear_charge_particle(gameObject& obj);
void spear_throw_particle(gameObject& obj);
void parry_particle(gameObject& obj);
void shockwave_particle(gameObject& obj);
void change_wep_particle(gameObject& obj);