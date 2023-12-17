/*!*************************************************************************
****
\file   status_effect.cpp
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Apr 5 2021

\brief
This header file outlines an implementation of the status effect class.

The functions include:
- update
- activate
- bleed

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/
#include "status_effect.h"
#include "enemy.h"
#include "boss.h"

constexpr int BLEED_DMG = 5;

/******************************************************************************
 * @brief Construct a new Effect:: Effect object
 * 
******************************************************************************/
Effect::Effect() : type{ STATUS_TYPE::BLEED }, timer{ 0 }, expiry{ 0 }, tick{ 0 }, tick_timer{ 0 },
owner{ nullptr }, activated{ false }, effect{ nullptr }{}

/******************************************************************************
 * @brief Update function for the effect
 * Applies the effect to the owner at every tick if it is not expired. If 
 * expired, deactivate the effect.
 * 
******************************************************************************/
void Effect::update()
{
	if (activated && timer < expiry)
	{
		if (tick_timer > tick)
		{
			if(effect)
				effect(*owner);
			tick_timer = 0;
		}
		tick_timer += G_DELTATIME;
		timer += G_DELTATIME;
		if (timer >= expiry)
		{
			timer = 0;
			activated = false;
		}
	}
}

/******************************************************************************
 * @brief Activate the effect, setting its expiry by duration and timer to 0
 * 
 * @param duration 
 * How long the effect will last
******************************************************************************/
void Effect::activate(f32 duration)
{
	activated = true;
	expiry = duration;
	timer = 0;
}


namespace Status_Effect
{
	/**************************************************************************
	 * @brief Bleed status effect. Try to dynamic cast the game object and 
	 * call its take_damage function.
	 * 
	 * @param obj 
	 * Either a boss or an enemy game object
	 *************************************************************************/
	void bleed(gameObject& obj)
	{
		Enemy* enemy = dynamic_cast<Enemy*>(&obj);
		Boss* boss = dynamic_cast<Boss*>(&obj);
		if (boss)
		{
			boss->take_damage(BLEED_DMG);
		}
		if (enemy)
		{
			enemy->take_damage(BLEED_DMG);
		}
	}
}