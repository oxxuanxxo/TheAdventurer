/*!*****************************************************************************
\file   digipen.cpp
\author Mohammad Hanif Koh Teck Wee
\par    DP email: mohammadhanifkoh.t\@digipen.edu
\date   April 6 2021

\brief
This file contains the implementation of the game state functions of the digipen
gamestate as outlined in digipen.h

The functions include:
- digipen_load
- digipen_initialize
- digipen_update
- digipen_draw
- digipen_free
- digipen_unload

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************************/
#include "digipen.h"
#include "graphics.h"
#include "game_states.h"
#include "transition.h"
#include "animation.h"

static AEGfxVertexList* pMeshDigipen;
static AEGfxVertexList* pMeshLogo;
static AEGfxTexture* pTexDigipen;
static AEGfxTexture* pTexLogo;
static AEVec2 logo_size;
f32 timer;
f32 alpha_increment;
f32 min_duration;
bool curr_logo;
static Sprite image;
static Animation logo_anim;

/*!*****************************************************************************
  \brief
	Load function for digipen game state. Allocates memory for the resources
	used.
*******************************************************************************/
void digipen_load()
{
	pMeshDigipen = create_square_mesh(1.0f, 1.0f);
	pMeshLogo = create_square_mesh(1.0f / 13.0f, 1.0f);
	pTexDigipen = AEGfxTextureLoad("./Digipen/digiPen_logo.png");
	pTexLogo = AEGfxTextureLoad("./Digipen/logo.png");
}

/*!*****************************************************************************
  \brief
	Init function for digipen game state. Initializes the data used in the
	gamestate.
*******************************************************************************/
void digipen_initialize()
{
	image.scale = { static_cast<f32>(AEGetWindowWidth() / 2), static_cast<f32>(AEGetWindowHeight()) / 3 };
	image.texture = pTexDigipen;
	image.mesh = pMeshDigipen;
	image.set_size(read_png_size("./Digipen/logo.png"));
	image.tint = { 1.0f, 1.0f, 1.0f, 1.0f };
	logo_anim = Animation(13, { 0, 0 }, &image, 500, 0.08f);
	min_duration = 0.25f;
	timer = 0.0f;
	alpha_increment = 0.0f;
	curr_logo = false;
}

/*!*****************************************************************************
  \brief
	Update function for the digipen gamestate. Performs calculations and handles
	all events in each frame.
*******************************************************************************/
void digipen_update()
{
	if ((AEInputCheckTriggered(AEVK_RETURN) ||
		AEInputCheckTriggered(AEVK_ESCAPE) ||
		AEInputCheckTriggered(AEVK_SPACE) ||
		AEInputCheckTriggered(AEVK_LBUTTON) ||
		AEInputCheckTriggered(AEVK_RBUTTON)) &&
		min_duration <= 0.0f)
	{
		stop_transition();
		gAEGameStateNext = GS_MAINMENU;
	}

	timer += G_DELTATIME;
	min_duration -= G_DELTATIME;
	image.tint.a += alpha_increment;

	if (timer >= 2.5f && !curr_logo && image.tint.a > 0)
	{
		alpha_increment = -0.016f;
	}

	else if (image.tint.a < 0.0f)
	{
		image.texture = pTexLogo;
		image.mesh = pMeshLogo;
		alpha_increment = 0.033f;
		curr_logo = true;
	}

	else if (image.tint.a > 1.0f && curr_logo)
	{
		timer = 0.0f;
		alpha_increment = 0;
		image.tint.a = 1.0f;
	}

	if (timer >= 2.5f && curr_logo && image.tint.a == 1.0f)
	{
		transition(0.5f, GS_MAINMENU);
	}

	else if (curr_logo && image.tint.a == 1.0f)
	{
		logo_anim.play_animation(false);
	}
}

/*!*****************************************************************************
  \brief
	Draw function for the digipen gamestate. Draws all Sprites and images in
	their new positions and sizes after each update function.
*******************************************************************************/
void digipen_draw()
{
	draw_non_relative(&image, { 0, 0 }, image.scale);
	transition();
}

/*!*****************************************************************************
  \brief
	Free function for the gamestate. As there is no data to be uninitialized,
	function is left empty.
*******************************************************************************/
void digipen_free()
{
	timer = 0.0f;
	alpha_increment = 0.0f;
	curr_logo = false;
}

/*!*****************************************************************************
  \brief
	Unload function for the digipen gamestate. Deallocates all memory used for
	assets in the game state.
*******************************************************************************/
void digipen_unload()
{
	AEGfxMeshFree(pMeshDigipen);
	AEGfxMeshFree(pMeshLogo);
	AEGfxTextureUnload(pTexDigipen);
	AEGfxTextureUnload(pTexLogo);
}