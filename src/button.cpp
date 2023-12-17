/*!*************************************************************************
****
\file   button.cpp
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   April 4 2021

\brief
This file includes the implementation of the button UI in the
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
#include "graphics.h"
#include "button.h"

/**
 * @brief Construct a new Button:: Button object
 * 
 */
Button::Button() : button_type(BUTTON_TYPE::BUTTON_NORMAL), hover_tex {nullptr}, normal_tex{nullptr}, 
					text_offset_x{ 0 }, text_offset_y{ 5 }, on_click{ nullptr }, font{ bold_font } {}

/**
 * @brief Construct a new Button:: Button object
 * 
 * @param position 
 * Position of the button
 * @param scale 
 * Scale factor of the button in x and y
 * @param sprite 
 * Pointer to the sprite
 * @param hover_tex 
 * Texture when the cursor is hovering over the button
 * @param normal_tex 
 * Default texture of the button
 */
Button::Button(AEVec2 position, AEVec2 scale, Sprite* sprite, AEGfxTexture* hover_tex, AEGfxTexture* normal_tex)
	: hover_tex{ hover_tex }, normal_tex{ normal_tex }, button_type(BUTTON_TYPE::BUTTON_NORMAL), text_offset_x{ 0 }, text_offset_y{ 5 },
	on_click{nullptr}, font{bold_font}
{
	this->curr_pos = position;
	this->scale = scale;
	this->obj_sprite = sprite;
	set_aabb();
}

/**
 * @brief Construct a new Button:: Button object
 * 
 * @param rhs 
 * The button reference to copy
 */
Button::Button(const Button& rhs) : hover_tex{ rhs.hover_tex }, normal_tex{ rhs.normal_tex }, button_type(rhs.button_type),
									on_click{rhs.on_click}, text{rhs.text}, font{rhs.font}
{
	curr_pos = rhs.curr_pos;
	scale = rhs.scale;
	obj_sprite = rhs.obj_sprite;
	bounding_box = rhs.bounding_box;
	text_offset_x = rhs.text_offset_x;
	text_offset_y = rhs.text_offset_y;
}

/**
 * @brief Destroy the Button:: Button object
 * This only sets the pointers to nullptr. The programmer has to destruct the sprite pointer and
 *  textures manually themselves.
 * 
 */
Button::~Button()
{
	hover_tex = nullptr;
	normal_tex = nullptr;
	obj_sprite = nullptr;
}

/**
 * @brief Sets the text and offset of the button respectively
 * 
 * @param text 
 * Text on the button
 * @param offset 
 * Offset of the position to print the text on the button
 */
void Button::set_text(s8 const* text, f32 offset_x, f32 offset_y)
{
	this->text = text;
	text_offset_x = offset_x;
	text_offset_y = offset_y;
}

/**
 * @brief Draws the button on the screen
 * 
 */
void Button::draw_button()
{
	switch (button_type)
	{
	case BUTTON_TYPE::BUTTON_CLICKED:
	case BUTTON_TYPE::BUTTON_INACTIVE:
		obj_sprite->tint = { 0.5f, 0.5f, 0.5f, 1.0f };
		break;
	case BUTTON_TYPE::BUTTON_HOVER:
		obj_sprite->texture = hover_tex;
		break;
	case BUTTON_TYPE::BUTTON_NORMAL:
		obj_sprite->texture = normal_tex;
		obj_sprite->tint = { 1.0f, 1.0f, 1.0f, 1.0f };
		break;
	}
	draw_non_relative(obj_sprite, curr_pos, obj_sprite->scale);
	text_print(font, text, &curr_pos, { text_offset_x, text_offset_y }, 1, obj_sprite->tint);
}

/**
 * @brief Updates the button manager
 * As the coordinate system of the mouse is not the same as the coordinate
 *  system of the engine, it will be converted via matrix multiplication to be 
 * accurate to the engine. Each buttons in the button manager will then be checked
 * with the converted mouse position.
 * 
 */
void ButtonManager::update()
{
	AEVec2 screen_mouse = { static_cast<f32>(mouse_x), static_cast<f32>(mouse_y) };
	AEMtx33 screen_matrix, trans;
	AEMtx33Identity(&screen_matrix);
	screen_matrix.m[0][1] = 0;
	screen_matrix.m[1][1] = -1;
	AEMtx33Trans(&trans, -static_cast<f32>(AEGetWindowWidth() / 2), static_cast<f32>(AEGetWindowHeight() / 2));
	AEMtx33Concat(&screen_matrix, &trans, &screen_matrix);
	AEMtx33MultVec(&screen_mouse, &screen_matrix, &screen_mouse);
	for (Button& b : buttons)
	{
		if (b.button_type != BUTTON_TYPE::BUTTON_INACTIVE)
		{
			if ((screen_mouse.x > b.bounding_box.min.x) &&
				(screen_mouse.x < b.bounding_box.max.x) &&
				(screen_mouse.y > b.bounding_box.min.y) &&
				(screen_mouse.y < b.bounding_box.max.y))
			{
				b.button_type = BUTTON_TYPE::BUTTON_HOVER;
				if (AEInputCheckTriggered(AEVK_LBUTTON))
				{
					b.button_type = BUTTON_TYPE::BUTTON_CLICKED;
				}
				if (AEInputCheckReleased(AEVK_LBUTTON))
				{
					b.button_type = BUTTON_TYPE::BUTTON_NORMAL;
					if (b.on_click)
						b.on_click();
				}
			}
			else
			{
				b.button_type = BUTTON_TYPE::BUTTON_NORMAL;
			}
		}
	}
}

/**
 * @brief Draw all the buttons stored by the button manager.
 * 
 */
void ButtonManager::draw()
{
	for (Button& b : buttons)
	{
		b.draw_button();
	}
}