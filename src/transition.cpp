/*!*****************************************************************************
\file   transition.cpp
\author Mohammad Hanif Koh Teck Wee
\par    DP email: mohammadhanifkoh.t\@digipen.edu
\date   April 3 2021

\brief
This file contains the implementation of the transition function used when
transiting between pages or gamestates as outlined in transition.h

The functions include:
- load_transition_assets
- unload_transition_assets
- transition
- transition
- stop_transition
- lerp

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************************/
#include "transition.h"
#include "graphics.h"

static AEGfxVertexList* pMeshTransition;
static AEVec2 scale;
static f32 rot;
static f32 alpha;
static f32 transition_duration;
bool transitioning = true;
static u32 next_gamestate = -1;

/*!*****************************************************************************
  \brief
	Loads any assets used in the transition functions. Called once in Main.cpp
	as the assets are constantly used through the different gamestates
*******************************************************************************/
void load_transition_assets()
{
	scale = { static_cast<f32>(AEGetWindowWidth()),static_cast<f32>(AEGetWindowHeight()) };
	pMeshTransition = create_square_mesh(1.0f, 1.0f, 0xFF000000);
	alpha = 100.0f;
	transition_duration = 1.0f;
}

/*!*****************************************************************************
  \brief
	Unloads all assets loaded in the previous function. Called once at the end
	of Main.cpp before exiting the system.
*******************************************************************************/
void unload_transition_assets()
{
	AEGfxMeshFree(pMeshTransition);
}

/*!*****************************************************************************
  \brief
	Main transition function. Checks if the global bool transitioning is set to
	true. If it is, check the current alpha of the transition to see if it
	should be transitioning in or out. If transitioning out, increases the alpha
	until it is 100%. It then changes the gamestate to the next gamestate and
	decreases the alpha until it is 0%. Once it is 0%, it sets transitioning to
	false.
*******************************************************************************/
void transition()
{
	if (transitioning)
	{
		static bool transition_in;
		if (alpha >= 100) transition_in = true;
		else if (alpha <= 0) transition_in = false;
		if (transition_in)
		{
			alpha = lerp(0, alpha, 100 / transition_duration / 60);
			if (alpha > 0)
			{
				draw_non_relative(nullptr, pMeshTransition, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, alpha / 100.0f }, alpha / 100.0f, scale, rot);
			}

			else
			{
				if (next_gamestate != -1)
				{
					draw_non_relative(nullptr, pMeshTransition, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, alpha / 100.0f }, alpha / 100.0f, scale, rot);
					gAEGameStateNext = next_gamestate;
					next_gamestate = -1;
				}

				transitioning = false;
			}
		}

		else
		{
			alpha = lerp(100, alpha, 100 / transition_duration / 60);
			if (alpha < 100)
			{
				draw_non_relative(nullptr, pMeshTransition, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, alpha / 100.0f }, alpha / 100.0f, scale, rot);
			}
			else
			{
				if (next_gamestate != -1)
				{
					draw_non_relative(nullptr, pMeshTransition, { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, alpha / 100.0f }, alpha / 100.0f, scale, rot);
					gAEGameStateNext = next_gamestate;
					next_gamestate = -1;
				}
			}
		}
	}
}

/*!*****************************************************************************
  \brief
	Function to start any transition. Sets the transition duration and the
	gamestate to be set after the transition, and then sets transitioning to
	true.

  \param duration
	How long it takes to transition out of or into the next gamestate

  \param gs_next
	The next gamestate to be set
*******************************************************************************/
void transition(f32 duration, u32 gs_next)
{
	transition_duration = duration;
	next_gamestate = gs_next;
	transitioning = true;
}

/*!*****************************************************************************
  \brief
	Function to force transition to end.
*******************************************************************************/
void stop_transition()
{
	transitioning = false;
	alpha = 0.0f;
}

/*!*****************************************************************************
  \brief
	Function to linearly interpolate between two floats.

  \param flGoal
	The desired float to lerp to

  \param flCurrent
	The current float

  \param dt
	The interval for the interpolation
*******************************************************************************/
float lerp(float flGoal, float flCurrent, float dt)
{
	float flDifference = flGoal - flCurrent;
	if (flDifference > dt)
		return flCurrent + dt;
	if (flDifference < -dt)
		return flCurrent - dt;

	return flGoal;
}