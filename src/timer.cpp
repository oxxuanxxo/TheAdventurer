/*!*************************************************************************
****
\file   timer.cpp
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Mar 4 2021

\brief
This file contains the implementation of the time scaling system in 
the game engine.

The functions include:
- time_load
- time_initialize
- time_update
- time_free
- set_time_multi
- get_time_multi
- temp_scale_timer

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/
#include "timer.h"
static f32 multiplier;
static f32 slow_timer;
static f32 stored_scale, dest_scale;

/*!****************************************************************************
 * @brief Loads the time, setting to initial values.
 * 
*******************************************************************************/
void time_load(void)
{
	G_DELTATIME = 0.0f;
	multiplier = 1.0f;
	stored_scale = 1.0f;
}

/*!****************************************************************************
 * @brief Initialize the timer
 * 
*******************************************************************************/
void time_initialize(void)
{
	G_DELTATIME = G_DT * multiplier;
}

/*!****************************************************************************
 * @brief Updates the timer
 * Lerps the time value if slow_timer is > 0
 * 
*******************************************************************************/
void time_update(void)
{
	if (slow_timer > 0)
	{
		AEVec2 target_scale{ dest_scale, dest_scale };
		AEVec2 curr_scale{ multiplier, multiplier };
		AEVec2Lerp(&curr_scale, &curr_scale, &target_scale, 0.14f);
		multiplier = curr_scale.x;
		slow_timer -= G_DT;
	}
	else
	{
		multiplier = 1;
	}
	G_DELTATIME = G_DT * multiplier;
}

/*!****************************************************************************
 * @brief Sets G_DELTATIME to 0
 * 
*******************************************************************************/
void time_free(void)
{
	G_DELTATIME = 0.0f;
}

/*!****************************************************************************
 * @brief Set the time multi object
 * 
 * @param multi 
*******************************************************************************/
void set_time_multi(f32 multi)
{
	multiplier = multi;
}

/*!****************************************************************************
 * @brief Get the time multi object
 * 
 * @return f32 
*******************************************************************************/
f32 get_time_multi(void)
{
	return multiplier;
}

/*!****************************************************************************
 * @brief Sets the time_scale temporarily
 * 
 * @param time_scale 
 * Time scale to set
 * @param duration 
 * How long the scale will be set for
*******************************************************************************/
void temp_scale_timer(f32 time_scale, f32 duration)
{
	stored_scale = G_DELTATIME / G_DT;
	dest_scale = time_scale;
	slow_timer = duration;
}