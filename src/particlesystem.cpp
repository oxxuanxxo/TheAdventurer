/*!*****************************************************************************
\file   particlesystem.cpp
\author Mohammad Hanif Koh Teck Wee
\par    DP email: mohammadhanifkoh.t\@digipen.edu
\date   March 16 2021

\brief
This file contains the implementation of the particle system as outlined in
particlesystem.h. It also contains static helper functions used in some of
the particle system functions.

The functions include:
- Particle System constructors
- clear_particles
- particle_system_load
- particleSystem::game_object_update
- Particle::game_object_update
- particleSystem::game_object_draw
- particle_system_unload
- create_particle
- delete_system
- create_particlesystem

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************************/
#include "particleSystem.h"
#include "attack.h"
#include "level_manager.h"

static AEGfxVertexList* pMeshParticles;
static AEGfxTexture* pTexParticles;

/*!*****************************************************************************
  \brief
	Constructor for particle system objects

  \param position
	The position of the particle system.

  \param size
	The size of the particle system. Determines how big of an area the particles
	can spawn from

  \param loop
	A boolean stating whether the particle system loops indefinitely. Can be
	changed anytime during the particle system's lifetime to kill the
	particle system

  \param max_particles
	The maximum number of particles that can exist at any point of time

  \param emission_rate
	The interval between each wave of particles being spawned

  \param max_distance
	The maximum distance each individual particle can be from the actual
	particle system object before being destroyed.

  \param behavior
	The function pointer to the behavior of the particles. Can be left as a
	nullptr if no special behaviour is required.

  \param color_range[2]
	An array of 2 colours, with the first element containing the minimum
	values for each colour, and the second element containing the maximum
	values.

  \param lifetime_range[2]
	An array of 2 floats, with the first element containing the minimum
	lifetime duration of particles, and the second element containing the
	maximum lifetime duration.

  \param rotation_range[2]
	An array of 2 floats, with the first element containing the minimum
	rotation value of particles, and the second element containing the maximum
	rotation value.

  \param direction_range[2]
	An array of 2 floats, with the first element containing the minimum
	direction value of particles, and the second element containing the maximum
	direction value.

  \param speed_range[2]
	An array of 2 vectors, with the first element containing the minimum
	speed of particles, and the second element containing the maximum speed.

  \param texture
	The pointer to the loaded texture to be drawn on each particle's mesh.
	Set as nullptr to draw the default mesh without any texture.
*******************************************************************************/
particleSystem::particleSystem(AEVec2 position, AEVec2 size, bool loop, u32 max_particles, f32 lifetime, f32 emission_rate,
	f32 max_distance, fp behaviour, Color color_range[2], f32 lifetime_range[2],
	AEVec2 scale_range[2], f32 rotation_range[2], f32 direction_range[2],
	f32 speed_range[2], AEGfxTexture* texture)
	: gameObject{}, particle_increment{ 1 }, loop{ loop }, target{ nullptr }, position_offset{}, max_particles{ max_particles }, lifetime{ lifetime },
	emission_rate{ emission_rate }, max_distance{ max_distance }, behaviour{ behaviour }, particles{},
	color_range{ color_range[0], color_range[1] }, lifetime_range{ lifetime_range[0], lifetime_range[1] },
	scale_range{ scale_range[0], scale_range[1] }, rotation_range{ rotation_range[0], rotation_range[1] }, timer{},
	particle_count{}, direction_range{ direction_range[0], direction_range[1] }, speed_range{ speed_range[0], speed_range[1] }
{
	type = GO_TYPE::GO_PARTICLESYSTEM;
	obj_sprite = new Sprite(texture, pMeshParticles);
	curr_pos = position;
	scale = size;
}

/*!*****************************************************************************
  \brief
	Constructor for particle system objects

  \param target
	A pointer to the game object that the particle system should follow.

  \param size
	The size of the particle system. Determines how big of an area the particles
	can spawn from

  \param loop
	A boolean stating whether the particle system loops indefinitely. Can be
	changed anytime during the particle system's lifetime to kill the
	particle system

  \param max_particles
	The maximum number of particles that can exist at any point of time

  \param emission_rate
	The interval between each wave of particles being spawned

  \param max_distance
	The maximum distance each individual particle can be from the actual
	particle system object before being destroyed.

  \param behavior
	The function pointer to the behavior of the particles. Can be left as a
	nullptr if no special behaviour is required.

  \param color_range[2]
	An array of 2 colours, with the first element containing the minimum
	values for each colour, and the second element containing the maximum
	values.

  \param lifetime_range[2]
	An array of 2 floats, with the first element containing the minimum
	lifetime duration of particles, and the second element containing the
	maximum lifetime duration.

  \param rotation_range[2]
	An array of 2 floats, with the first element containing the minimum
	rotation value of particles, and the second element containing the maximum
	rotation value.

  \param direction_range[2]
	An array of 2 floats, with the first element containing the minimum
	direction value of particles, and the second element containing the maximum
	direction value.

  \param speed_range[2]
	An array of 2 vectors, with the first element containing the minimum
	speed of particles, and the second element containing the maximum speed.

  \param texture
	The pointer to the loaded texture to be drawn on each particle's mesh.
	Set as nullptr to draw the default mesh without any texture.
*******************************************************************************/
particleSystem::particleSystem(gameObject* target, AEVec2 size, bool loop, u32 max_particles, f32 lifetime, f32 emission_rate,
	f32 max_distance, fp behaviour, Color color_range[2], f32 lifetime_range[2],
	AEVec2 scale_range[2], f32 rotation_range[2], f32 direction_range[2],
	f32 speed_range[2], AEGfxTexture* texture)
	: gameObject{}, particle_increment{ 1 }, loop{ loop }, target{ target }, position_offset{}, max_particles{ max_particles }, lifetime{ lifetime },
	emission_rate{ emission_rate }, max_distance{ max_distance }, behaviour{ behaviour }, particles{},
	color_range{ color_range[0], color_range[1] }, lifetime_range{ lifetime_range[0], lifetime_range[1] },
	scale_range{ scale_range[0], scale_range[1] }, rotation_range{ rotation_range[0], rotation_range[1] }, timer{},
	particle_count{}, direction_range{ direction_range[0], direction_range[1] }, speed_range{ speed_range[0], speed_range[1] }
{
	type = GO_TYPE::GO_PARTICLESYSTEM;
	obj_sprite = new Sprite(texture, pMeshParticles);
	curr_pos = target->curr_pos;
	scale = size;
}

/*!*****************************************************************************
  \brief
	Clears all existing particles from within a particle system, and deallocates
	all memory used by the particles.

  \param pSystem
	A reference to the particle system to be cleared.
*******************************************************************************/
void clear_particles(particleSystem& pSystem)
{
	for (Particle& pParticle : pSystem.particles)
	{
		delete pParticle.obj_sprite;
	}
	pSystem.particles.clear();
}

/*!*****************************************************************************
  \brief
	Loads all assets and allocates memory to be used by particle systems. Called
	once in the load function of all levels.
*******************************************************************************/
void particle_system_load()
{
	pMeshParticles = create_square_mesh(1.0f, 1.0f, 0xFFFFFFFF);
	AE_ASSERT_MESG(pMeshParticles, "Failed to create mesh boss!");
}

/*!*****************************************************************************
  \brief
	Game Object Update function of Particle Systems class. Handles the creation
	of particles, updates the position of the particle system if necessary, and
	handles the destruction of the particle system.
*******************************************************************************/
void particleSystem::game_object_update()
{
	if (target)
	{
		if (target->flag)
		{
			AEVec2Add(&curr_pos, &target->curr_pos, &position_offset);
		}

		else
		{
			loop = false;
			lifetime = -1;
		}
	}

	timer += G_DELTATIME;

	if (lifetime >= 0 && timer >= emission_rate)
	{
		for (u32 i{ 0 }; i < particle_increment; ++i)
			if (particle_count < max_particles)
			{
				create_particle();
			}
	}

	for (Particle& pParticle : particles)
	{
		if (pParticle.flag)
			pParticle.game_object_update();
	}

	if (!loop)
	{
		lifetime -= G_DELTATIME;
		if (lifetime <= 0 && !particle_count)
		{
			this->delete_system();
		}
	}
}

/*!*****************************************************************************
  \brief
	Game Object Update function of Particle class. If any behaviour is set for
	Particles, it is carried out here. The function then updates the position
	of the particle and checks if the lifetime of the individual particle is
	over.
*******************************************************************************/
void Particle::game_object_update()
{
	if (behaviour)
	{
		behaviour(*this);
	}

	AEVec2 dt_vel;
	AEVec2Scale(&dt_vel, &curr_vel, G_DELTATIME);
	AEVec2Add(&curr_pos, &dt_vel, &curr_pos);

	lifetime -= G_DELTATIME;
	if (lifetime <= 0 || obj_sprite->tint.a <= 0
		|| (max_distance && AEVec2Distance(&curr_pos, &initial_position) > max_distance))
	{
		flag = false;
		--parent->particle_count;
	}
}

/*!*****************************************************************************
  \brief
	Game Object Draw function of Particle Systems class. Iterates through the
	vector of particles and draws each individual particle.
*******************************************************************************/
void particleSystem::game_object_draw()
{
	for (Particle& pParticle : particles)
	{
		pParticle.game_object_draw();
	}
}

/*!*****************************************************************************
  \brief
	Unloads all assets and deallocates all memory used by particle systems.
	Called once in the unload functions of each level.
*******************************************************************************/
void particle_system_unload()
{
	AEGfxMeshFree(pMeshParticles);
}

/*!*****************************************************************************
  \brief
	Creates a new particle. Randomizes a value for each data member of the new
	particle based on the ranges set by the particle system. It then emplaces
	the new particle into the particle system's vector of particles.
*******************************************************************************/
void particleSystem::create_particle()
{
	Color color = {
				AEWrap(color_range[0].r + (color_range[1].r - color_range[0].r) * AERandFloat(), color_range[0].r, color_range[1].r),
				AEWrap(color_range[0].g + (color_range[1].g - color_range[0].g) * AERandFloat(), color_range[0].g, color_range[1].g),
				AEWrap(color_range[0].b + (color_range[1].b - color_range[0].b) * AERandFloat(), color_range[0].b, color_range[1].b),
				AEWrap(color_range[0].a + (color_range[1].a - color_range[0].a) * AERandFloat(), color_range[0].a, color_range[1].a)
	};

	float lifespan = AEWrap(lifetime_range[0] + (lifetime_range[1] - lifetime_range[0]) * AERandFloat(), lifetime_range[0], lifetime_range[1]);
	float scale_x = AEWrap(scale_range[0].x + (scale_range[1].x - scale_range[0].x) * AERandFloat(), scale_range[0].x, scale_range[1].x);
	float scale_y = AEWrap(scale_range[0].y + (scale_range[1].y - scale_range[0].y) * AERandFloat(), scale_range[0].y, scale_range[1].y);

	float rotation = AEWrap(rotation_range[0] + (rotation_range[1] - rotation_range[0]) * AERandFloat(), rotation_range[0], rotation_range[1]);
	float direction = AEWrap(direction_range[0] + (direction_range[1] - direction_range[0]) * AERandFloat(), direction_range[0], direction_range[1]);
	float speed = AEWrap(speed_range[0] + (speed_range[1] - speed_range[0]) * AERandFloat(), speed_range[0], speed_range[1]);

	AEVec2 vel;
	AEVec2FromAngle(&vel, direction);
	AEVec2Scale(&vel, &vel, speed);

	Particle pParticle = Particle();
	pParticle.obj_sprite = new Sprite;
	pParticle.obj_sprite->tint = color;
	pParticle.obj_sprite->mesh = obj_sprite->mesh ? obj_sprite->mesh : pMeshParticles;
	pParticle.obj_sprite->texture = obj_sprite->texture;
	pParticle.obj_sprite->size = obj_sprite->size;
	pParticle.obj_sprite->rot = rotation;
	pParticle.obj_sprite->flip_x = obj_sprite->flip_x;
	pParticle.obj_sprite->flip_y = obj_sprite->flip_y;
	pParticle.initial_position.x = AERandFloat() * scale.x + bounding_box.min.x + position_offset.x;
	pParticle.initial_position.y = AERandFloat() * scale.y + bounding_box.min.y + position_offset.y;
	pParticle.curr_pos = pParticle.initial_position;
	pParticle.curr_vel = vel;
	pParticle.scale = { scale_x, scale_y };
	pParticle.lifetime = lifespan;
	pParticle.behaviour = behaviour;
	pParticle.parent = this;
	pParticle.max_distance = max_distance;
	pParticle.type = GO_TYPE::GO_PARTICLE;

	particles.emplace_back(pParticle);
	++particle_count;
	timer = 0;
}

/*!*****************************************************************************
  \brief
	Clears all particles, deallocates memory of the sprite of the particle
	system, and sets the particle system to be inactive.
*******************************************************************************/
void particleSystem::delete_system()
{
	clear_particles(*this);
	delete obj_sprite;
	flag = false;
}

/*!*****************************************************************************
  \brief
	Searches for an existing particle system in the vector of game objects to be
	reused. Returns a nullptr if nothing is found.

  \param v_gameObj
	A pointer to the vector of game objects

  \return
	A pointer to the particle system found.
*******************************************************************************/
gameObject* get_particlesystem(vector<gameObject*>* v_gameObj)
{
	for (gameObject* i : *v_gameObj)
	{
		if (i->type == GO_TYPE::GO_PARTICLESYSTEM && i->flag == false) return i;
	}
	return nullptr;
}

/*!*****************************************************************************
  \brief
	Creates a default particle system that will follow the game object inputted
	by the user. First it searches for any inactive particle systems in the
	vector of game objects. If one exists, it will overwrite the existing
	inactive particle system. If there are no existing inactive particle
	systems, it will then create a new particle system and push it into the
	vector of game objects.

  \param target
	A pointer to the game object the particle system should follow

  \return
	A pointer to the created particle system.
*******************************************************************************/
particleSystem* create_particlesystem(gameObject* target)
{
	Color colors[2]{ { 1.0f, 1.0f, 1.0f, 1.0f }, {1.0f, 1.0f, 1.0f, 1.0f } };
	AEVec2 scales[2]{ {20.0f, 20.0f}, {20.0f, 20.0f} };
	f32 lifetimes[2]{ 1.0f, 1.0f };
	f32 rotations[2]{ 0, 0 };
	f32 directions[2]{ 0, PI };
	f32 speeds[2]{ 300.0f, 600.0f };
	particleSystem* pParticleSystem = dynamic_cast<particleSystem*>(get_particlesystem(&Level::v_gameObjects));
	if (pParticleSystem)
	{
		*pParticleSystem = { target, { 0, 0 }, false, 10U, 5.0f, 0.016f, 0.0f, particle_default_behaviour, colors, lifetimes, scales, rotations, directions, speeds, nullptr };
		pParticleSystem->set_aabb();
	}
	else
	{
		pParticleSystem = new particleSystem{ target, { 0, 0 }, false, 10U, 5.0f, 0.016f, 0.0f, particle_default_behaviour, colors, lifetimes, scales, rotations, directions, speeds, nullptr };
		pParticleSystem->set_aabb();
		Level::v_gameObjects.emplace_back(pParticleSystem);
	}
	return pParticleSystem;
}

/*!*****************************************************************************
  \brief
	Creates a default particle system at the position inputted by the user.
	First it searches for any inactive particle systems in the vector of game
	objects. If one exists, it will overwrite the existing inactive particle
	system. If there are no existing inactive particle systems, it will then
	create a new particle system and push it into the vector of game objects.

  \param target
	The position of the particle system

  \return
	A pointer to the created particle system.
*******************************************************************************/
particleSystem* create_particlesystem(AEVec2 target)
{
	Color colors[2]{ { 1.0f, 1.0f, 1.0f, 1.0f }, {1.0f, 1.0f, 1.0f, 1.0f } };
	AEVec2 scales[2]{ {20.0f, 20.0f}, {20.0f, 20.0f} };
	f32 lifetimes[2]{ 1.0f, 1.0f };
	f32 rotations[2]{ 0, 0 };
	f32 directions[2]{ 0, PI };
	f32 speeds[2]{ 300.0f, 600.0f };
	particleSystem* pParticleSystem = dynamic_cast<particleSystem*>(get_particlesystem(&Level::v_gameObjects));
	if (pParticleSystem)
	{
		*pParticleSystem = { target, { 0, 0 }, false, 10U, 5.0f, 0.016f, 0.0f, particle_default_behaviour, colors, lifetimes, scales, rotations, directions, speeds, nullptr };
		pParticleSystem->set_aabb();
	}
	else
	{
		pParticleSystem = new particleSystem{ target, { 0, 0 }, false, 10U, 5.0f, 0.016f, 0.0f, particle_default_behaviour, colors, lifetimes, scales, rotations, directions, speeds, nullptr };
		pParticleSystem->set_aabb();
		Level::v_gameObjects.push_back(pParticleSystem);
	}
	return pParticleSystem;
}