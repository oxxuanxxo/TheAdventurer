/*!*************************************************************************
****
\file   button.h
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   April 4 2021

\brief
This header file outlines an implementation of the button UI in the
game engine.

The functions include:
- load_platforms
- set_text
- draw_button
- update
- draw

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/
#pragma once
#include "gameobject.h"

enum class BUTTON_TYPE
{
	BUTTON_NORMAL,
	BUTTON_HOVER,
	BUTTON_CLICKED,
	BUTTON_INACTIVE
};

struct Button : public gameObject
{
	BUTTON_TYPE button_type;
	AEGfxTexture* hover_tex;
	AEGfxTexture* normal_tex;
	std::string text;
	f32 text_offset_x;
	f32 text_offset_y;
	s8 font;
	Button();
	Button(const Button&);
	Button(AEVec2 position, AEVec2 scale, Sprite* sprite, AEGfxTexture* hover_tex, AEGfxTexture* normal_tex);
	void set_text(s8 const* text, f32 offset_x, f32 offset_y = 5.0f);
	void(*on_click)(void);
	void draw_button();
	~Button();
};

struct ButtonManager
{
	vector<Button> buttons;
	void update();
	void draw();
};