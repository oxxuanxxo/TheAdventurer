/*!*****************************************************************************
\file   particle_effects.cpp
\author Mohammad Hanif Koh Teck Wee
\par    DP email: mohammadhanifkoh.t\@digipen.edu
\date   April 6 2021

\brief
This header file contains the implementation of functions to create commonly
used particle systems, as well as all behavioural functions for particles.

The functions include:
- create_falling_particles
- create_dash_particles
- create_damaged_particles
- create_ground_particles
- spear_throw_particle
- dodge_particle
- slam_particle
- spear_charge_particle
- particle_default_behaviour
- damage_taken
- boss_vulnerable
- parry_particle
- shockwave_particle
- change_wep_particle

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************************/
#include "particlesystem.h"

/*!*****************************************************************************
  \brief
	Creates a default particle system of particles falling from the top of the
	screen. Values are default, and must be changed outside of the function for
	any variations such as colour, size speed etc.

  \return
	a pointer to the created particle system
*******************************************************************************/
particleSystem* create_falling_particles()
{
	AEVec2 camPos;
	AEGfxGetCamPosition(&camPos.x, &camPos.y);
	AEVec2 vParticleSystem = { camPos.x, camPos.y + static_cast<f32>(AEGetWindowHeight()) / 2 + 50.0f };
	particleSystem* pParticleSystem = create_particlesystem(vParticleSystem);
	pParticleSystem->lifetime = 0.016f;
	pParticleSystem->emission_rate = 0.016f;
	pParticleSystem->max_particles = 50;
	pParticleSystem->lifetime_range[0] = pParticleSystem->lifetime_range[1] = 3.0f;
	pParticleSystem->scale_range[0] = pParticleSystem->scale_range[1] = { 30.0f, 30.0f };
	pParticleSystem->speed_range[0] = pParticleSystem->speed_range[1] = 500;
	pParticleSystem->direction_range[0] = 5 * PI / 4;
	pParticleSystem->direction_range[1] = 7 * PI / 4;
	pParticleSystem->rotation_range[0] = 0;
	pParticleSystem->rotation_range[0] = 2 * PI;
	pParticleSystem->particle_increment = 50;
	pParticleSystem->scale = { static_cast<f32>(AEGetWindowWidth()), 1.0f };
	pParticleSystem->set_aabb();
	return pParticleSystem;
}

/*!*****************************************************************************
  \brief
	Creates a default particle system simulating a dash. Particles are spawned
	and grows longer horizontally over a few frames. Values are default, and
	must be changed outside of the function for any variations such as colour,
	size speed etc.

  \param target
	a pointer to the gameobject that the particle system should follow.

  \return
	a pointer to the created particle system
*******************************************************************************/
particleSystem* create_dash_particles(gameObject* target)
{
	particleSystem* pParticleSystem = create_particlesystem(target);
	pParticleSystem->behaviour = dodge_particle;
	pParticleSystem->lifetime = 0.25f;
	pParticleSystem->emission_rate = 0.016f;
	pParticleSystem->color_range[0] = { 1.0f, 1.4f, 0.0f, 0.6f };
	pParticleSystem->color_range[1] = { 1.0f, 1.6f, 0.0f, 0.8f };
	pParticleSystem->lifetime_range[0] = pParticleSystem->lifetime_range[1] = 0.5f;
	pParticleSystem->scale_range[0] = { 5.0f, 3.0f };
	pParticleSystem->scale_range[1] = { 5.0f, 5.0f };
	pParticleSystem->speed_range[0] = 50;
	pParticleSystem->speed_range[1] = 100;
	pParticleSystem->direction_range[0] = pParticleSystem->direction_range[1] = 0;
	pParticleSystem->particle_increment = 1;
	pParticleSystem->scale = { 1.0f, 75.0f };
	return pParticleSystem;
}

/*!*****************************************************************************
  \brief
	Creates a default particle system simulating damage being taken. Particles
	fly a random direction upward and slowly get more transparent.Values are
	default, and must be changed outside of the function for any variations such
	as colour, size speed etc.

  \param target
	a pointer to the gameobject taking damage.

  \return
	a pointer to the created particle system
*******************************************************************************/
particleSystem* create_damaged_particles(gameObject* target)
{
	particleSystem* pParticleSystem = create_particlesystem(target);
	pParticleSystem->lifetime = 0.1f;
	pParticleSystem->lifetime_range[0] = pParticleSystem->lifetime_range[1] = 0.5f;
	pParticleSystem->scale_range[0] = { 5.0f, 5.0f };
	pParticleSystem->scale_range[1] = { 10.0f, 10.0f };
	pParticleSystem->speed_range[0] = 300.0f;
	pParticleSystem->speed_range[1] = 500.0f;
	pParticleSystem->behaviour = damage_taken;
	return pParticleSystem;
}

/*!*****************************************************************************
  \brief
	Creates a default particle system simulating ground breaking. Slightly large
	particles spawn and move upwards, with their velocity getting affected by
	gravity. Values are default, and must be changed outside of the function for
	any variations such as colour, size speed etc.

  \param target
	a pointer to the gameobject that the particle system should follow.

  \return
	a pointer to the created particle system
*******************************************************************************/
particleSystem* create_ground_particles(gameObject* target)
{
	particleSystem* pParticleSystem = create_particlesystem(target);
	pParticleSystem->lifetime = 0.016f;
	pParticleSystem->emission_rate = 0.016f;
	pParticleSystem->max_particles = 10;
	pParticleSystem->lifetime_range[0] = pParticleSystem->lifetime_range[1] = 3.0f;
	pParticleSystem->scale_range[0] = pParticleSystem->scale_range[1] = { 50.0f, 50.0f };
	pParticleSystem->speed_range[0] = pParticleSystem->speed_range[1] = 500;
	pParticleSystem->direction_range[0] = PI / 4;
	pParticleSystem->direction_range[1] = 3 * PI / 4;
	pParticleSystem->rotation_range[0] = 0;
	pParticleSystem->rotation_range[0] = 2 * PI;
	pParticleSystem->particle_increment = 10;
	pParticleSystem->behaviour = slam_particle;
	pParticleSystem->set_aabb();
	return pParticleSystem;
}

/*!*****************************************************************************
  \brief
	Particle behaviour for the spear throw particle effects. Particles increase
	in size both horizontally and vertically, and slowly lose transparency.
	Particle grows much faster vertically.

  \param obj
	a reference to each individual particle.
*******************************************************************************/
void spear_throw_particle(gameObject& obj)
{
	obj.scale.y += 2.0f;
	obj.scale.x += 0.16f;
	obj.obj_sprite->tint.a -= 0.05f;
}

/*!*****************************************************************************
  \brief
	Particle behaviour for dashing effects. Particles grow a random size
	vertically, and slowly lose transparency.

  \param obj
	a reference to each individual particle.
*******************************************************************************/
void dodge_particle(gameObject& obj)
{
	obj.scale.x += G_DELTATIME * AERandFloat() * 100.0f;
	obj.obj_sprite->tint.a -= 0.03f;
}

/*!*****************************************************************************
  \brief
	Particle behaviour for any floor-slamming related particle systems. A
	downward force is added to the velocity of all particles every frame, and
	particles slowly lose transparency.

  \param obj
	a reference to each individual particle.
*******************************************************************************/
void slam_particle(gameObject& obj)
{
	obj.curr_vel.y -= 500 * G_DELTATIME;
	obj.obj_sprite->tint.a -= 0.01f;
}

/*!*****************************************************************************
  \brief
	Particle behaviour for the spear charging animation. Particles move toward
	the center of the particle system (adjusted to the tip of the spear), while
	slowly getting smaller and losing transparency.

  \param obj
	a reference to each individual particle.
*******************************************************************************/
void spear_charge_particle(gameObject& obj)
{
	Particle* part = dynamic_cast<Particle*>(&obj);
	AEVec2 dir = { part->parent->curr_pos.x - part->curr_pos.x, part->parent->curr_pos.y - part->curr_pos.y };
	AEVec2Normalize(&dir, &dir);
	AEVec2Scale(&obj.curr_vel, &dir, AEVec2Length(&obj.curr_vel));
	obj.scale.x -= G_DELTATIME * 75.0f;
	obj.scale.y -= G_DELTATIME * 75.0f;
	obj.obj_sprite->tint.a -= 0.03f;
}

/*!*****************************************************************************
  \brief
	Default particle behaviour. Particles lose transparency very slowly over
	time.

  \param obj
	a reference to each individual particle.
*******************************************************************************/
void particle_default_behaviour(gameObject& obj)
{
	obj.obj_sprite->tint.a -= 0.01f;
}

/*!*****************************************************************************
  \brief
	Particle behaviour for damage taken particles. Particles lose transparency
	over time.

  \param obj
	a reference to each individual particle.
*******************************************************************************/
void damage_taken(gameObject& obj)
{
	obj.obj_sprite->tint.a -= 0.05f;
}

/*!*****************************************************************************
  \brief
	Particle behaviour for when boss gets vulnerable. Particles grow bigger and
	lose transparency over time.

  \param obj
	a reference to each individual particle.
*******************************************************************************/
void boss_vulnerable(gameObject& obj)
{
	obj.obj_sprite->tint.a -= 0.02f;
	obj.scale.x += 7;
	obj.scale.y += 7;
}

/*!*****************************************************************************
  \brief
	Particle behaviour for Parry animation. Particles grow bigger and lose
	transparency over time. Particle also follow the position of the particle
	system (adjusted to be above the player).

  \param obj
	a reference to each individual particle.
*******************************************************************************/
void parry_particle(gameObject& obj)
{
	Particle* pParticle = dynamic_cast<Particle*>(&obj);
	pParticle->curr_pos = pParticle->parent->curr_pos;
	pParticle->scale.y += 30.0f * G_DELTATIME * 90;
	pParticle->scale.x += 30.0f * G_DELTATIME * 90;
	pParticle->obj_sprite->tint.a -= 0.03f * G_DELTATIME * 90;
}

/*!*****************************************************************************
  \brief
	Particle behaviour for shockwave effects. Particles grow bigger over time.
	Particle also follow the position of the particle system (adjusted to be
	above the player).

  \param obj
	a reference to each individual particle.
*******************************************************************************/
void shockwave_particle(gameObject& obj)
{
	Particle* pParticle = dynamic_cast<Particle*>(&obj);
	pParticle->curr_pos = pParticle->parent->curr_pos;
	pParticle->scale.y += 30.0f * G_DELTATIME * 900;
	pParticle->scale.x += 30.0f * G_DELTATIME * 900;
	//pParticle->obj_sprite->tint.a -= 0.03f * G_DELTATIME * 90;
}

/*!*****************************************************************************
  \brief
	Particle behaviour for weapon changing  animation. Particles grow bigger and
	lose transparency over time. Particle also follow the position of the
	particle system (adjusted to be above the player).

  \param obj
	a reference to each individual particle.
*******************************************************************************/
void change_wep_particle(gameObject& obj)
{
	Particle* pParticle = dynamic_cast<Particle*>(&obj);
	pParticle->curr_pos = pParticle->parent->curr_pos;
	pParticle->scale.y += 2.0f;
	pParticle->scale.x += 2.0f;
	pParticle->obj_sprite->tint.a -= 0.05f;
}